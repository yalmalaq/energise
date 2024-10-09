// TODO: simple on-off loop
// TODO: print task switch events
// TODO: limit rate of switching

#include <linux/percpu-defs.h>
//#include "sched.h"
#include <linux/topology.h>
#include <linux/idle_inject.h>
#include <linux/workqueue.h>

//#include <linux/spinlock_api.h>
#include <linux/timer.h>

static void arm_timer(unsigned long);

/* data structures */

struct sleepy_pcpu {
	//    int capacity;
	int initalised;
	struct freq_qos_request qos_req_min, qos_req_max;
};

struct cpu_op {
	u32 freq_min, freq_max; // in kHz
	u16 capacity;
	u16 should_be_on;
};

struct sys_op {
	u16 window_size_us;
	u16 ncpus;
	struct cpu_op *cpu_ops;
};

struct sys_op_table {
	u32 sleep;
	u32 up_time;
	u32 down_time;
	u32 util_scale;
	u32 num_util;
	struct sys_op *sys_ops;
};

static DEFINE_PER_CPU(struct sleepy_pcpu, sleepy_pcpu) = { .initalised = 0 };

static raw_spinlock_t              tick_lock = __RAW_SPIN_LOCK_UNLOCKED(sleepy_lock);
static struct sys_op              *curr_sys_op   = NULL;
static struct sys_op_table        *autoset_table = NULL;
static struct idle_inject_device  *idle_dev      = NULL; 
static int state = 0;
static int printing = 0;

/* userspace API */

/*
 * TABLE FORMAT:
 *
 *  TBL  := scale num_util SYS_POINT+
 *  SYS_POINT := window_us ncpus CPU_POINT+
 *  CPU_POINT := freq_min freq_max idle_us
 *
 * each is an unsigned 4 byte int (in native byte order)
 */


struct buf {
	const char *data;
	int err;
	int offset;
	int size;
};

static u32 read_int(struct buf* buf)
{
	u32 ret;

	if (buf->offset + 4 > buf->size) {
		buf->err = 1;
		return 0;
	}
	ret = *(u32*)(buf->data + buf->offset);
	buf->offset += 4;
	return ret;
}

struct sys_op_table *parse_op_table(struct buf *buf)
{
	struct sys_op_table *ret;
	int entries, op, cpu;

	ret = kcalloc(sizeof(*ret), 1, GFP_ATOMIC);

	ret->sleep= read_int(buf);
	ret->up_time= read_int(buf);
	ret->down_time= read_int(buf);

	ret->util_scale = read_int(buf);
	ret->num_util   = read_int(buf);
	entries         = ret->num_util * ret->num_util;
	/* TODO: not an atomic context anymore */
	ret->sys_ops    = kcalloc(sizeof(struct sys_op), entries, GFP_ATOMIC);

	BUG_ON(!ret->sys_ops);

	for (op = 0; op < entries; op++) {
		struct sys_op *sys_op = &ret->sys_ops[op];

		sys_op->window_size_us = read_int(buf);
		sys_op->ncpus          = read_int(buf);
		sys_op->cpu_ops        = kcalloc(sizeof(struct cpu_op), sys_op->ncpus, GFP_ATOMIC);

		BUG_ON(!sys_op->cpu_ops);

		for (cpu = 0; cpu < sys_op->ncpus; cpu++) {
			sys_op->cpu_ops[cpu].freq_min     = read_int(buf);
			sys_op->cpu_ops[cpu].freq_max     = read_int(buf);
			sys_op->cpu_ops[cpu].should_be_on = read_int(buf);
		}

		BUG_ON(buf->err);
	}

	return ret; 
}

static void free_op_table(struct sys_op_table *table)
{
	int entry;
	int entries = table->num_util * table->num_util;

	for (entry = 0; entry < entries; entry++) {
		kfree(table->sys_ops[entry].cpu_ops);
	}

	kfree(table->sys_ops);
	kfree(table);
}

#define MAX_BUFFER_SIZE 1024 * 1024 * 4

static long procs_unlocked_ioctl(
                                 struct file * _file,
                                 unsigned int size, 
                                 unsigned long addr)
{
	struct buf buf;
	void *dat;
	int copied_size;
	struct sys_op_table *table, *last_table;

	printk(KERN_NOTICE "SleepySched: iotcl %x %lx \n", size, addr);

	if (size < 0 || size > MAX_BUFFER_SIZE)
		return -EINVAL;

	/* buffer may get split by write? maybe just pass the address? */

	dat = kmalloc(size, GFP_KERNEL);
	BUG_ON(!dat);
	copied_size = copy_from_user(dat, (const char* __user) addr, size);

	printk(KERN_NOTICE "SleepySched: copied %d bytes of %d\n",
	       copied_size, size);

	if (copied_size != 0) {
		kfree(dat);
		return -EINVAL;
	}

	buf.data   = dat;
	buf.err    = 0;
	buf.offset = 0;
	buf.size   = size;
	table       = parse_op_table(&buf);
	printk(KERN_NOTICE "SleepySched: parsed table\n");

	raw_spin_lock(&tick_lock);
	last_table    = autoset_table;
	autoset_table = table;
	raw_spin_unlock(&tick_lock);

	printk(KERN_NOTICE "SleepySched: replaced table\n");
	if (last_table)
		free_op_table(last_table);

	printk(KERN_NOTICE "SleepySched: installed new table\n");
	return 0;
}

static struct proc_ops file_ops;


/* util functions */

static int warned = 0;
static void write_to_attr(struct kobject *kobj, const char *attr_name, const char *attr_value)
{
	struct kernfs_open_file of = {0};
	struct kernfs_node *node;
	ssize_t ret;
       
	node = kernfs_find_and_get_ns(kobj->sd, attr_name, NULL);

	if (!node && !warned) {
		panic("Could  not get %s", attr_name);
	}

	of.kn = node;
	ret = node->attr.ops->write(&of, (char*) attr_value, strlen(attr_value), 0);
	if (ret != strlen(attr_value)) {
		//panic("Expected %d got %d", sizeof(attr_value), ret);
		printk(KERN_NOTICE "Could not write to sysfs file\n");
	}
}

/*
static void read_attr(struct kobject *kobj, const char *attr_name, char* buf)
{
	const struct attribute_group **group;
	struct attribute **attr;

	for (group = kobj->ktype->default_groups; *group != NULL; group++) {
		for (attr = (**group).attrs; *attr != NULL; attr++) {
			if (strcmp((**attr).name, attr_name) != 0)
				continue;

			kobj->ktype->sysfs_ops->show(kobj, *attr, buf);
		}
	}
}
*/


/* internals */
static struct sys_op *select_operating_point(unsigned long total_util, unsigned long max_util, struct sys_op_table *tbl)
{
	int i1, i2, index;

	i1 = (total_util / tbl->util_scale);
	if (i1 >= tbl->num_util)
		i1 = tbl->num_util - 1;
	
	i2 = (max_util / tbl->util_scale);
	if (i2 >= tbl->num_util)
		i2 = tbl->num_util - 1;

	index = i1 + tbl->num_util * i2;

	if (printing)
		printk(KERN_NOTICE " SleepySched: index=%d Util (%d|%d) / %d\n",
				index, i1, i2, tbl->util_scale);
	return &tbl->sys_ops[index];
}

/*
static bool idle_inject_cb(void)
{
	return true; 
}
*/


//[  328.740992] SleepySched: switched OP 0 = 300000\x0a1 = 300000\x0a2 = 300000\x0a3 = 300000\x0a4 = 1024000\x0a5 = off\x0a6 = off\x0a7 = off\x0a
char buf[512];
char *log_op(struct sys_op *op)
{
	int off = 0;
	int cpu;
	for (cpu = 0; cpu < op->ncpus; cpu++) {
		if (op->cpu_ops[cpu].should_be_on) {
			off += snprintf(buf + off, sizeof(buf) - off, "%d = %ld ", cpu, op->cpu_ops[cpu].freq_max);
		} else {
			off += snprintf(buf + off, sizeof(buf) - off, "%d = off ", cpu);
		}
	}
	return buf;
}

static void set_sys_op(struct sys_op* sys_op)
{
	static int itercount = 0;
	static struct sys_op* last_op = NULL;

	int cpu;
	int all_ready = 1;

	curr_sys_op = sys_op;

	if (sys_op == last_op) {
		/* every so often reset all the params in case someone stepped on them */
		itercount++;
		if (itercount % 100 != 0)
			return;
	}

	printk(KERN_NOTICE "SleepySched: switched OP %s\n", log_op(sys_op));

	last_op = sys_op;

	if (state == 0) {
		state++;
	}

	//idle_inject_set_duration(idle_dev, sys_op->window_size_us, 0);


	for (cpu = 0; cpu < curr_sys_op->ncpus; cpu++) {
		struct cpu_op *cpu_op    = &curr_sys_op->cpu_ops[cpu];
		struct sleepy_pcpu *pcpu = per_cpu_ptr(&sleepy_pcpu, cpu);
		struct cpufreq_policy *pol;
		struct device *cpu_device = get_cpu_device(cpu);

		//pcpu->capacity = cpu_op->capacity;
	


		if (cpu_op->should_be_on && cpu_device->offline && cpu != 0) {
			printk(KERN_EMERG "SleepySched: Bringing %d Online (running on %d state 0x%x)\n", cpu, smp_processor_id(), preempt_count());
			add_cpu(cpu);
			printk(KERN_EMERG "SleepSched: Done");
		} else if (!cpu_op->should_be_on && !cpu_device->offline && cpu != 0) {
			printk(KERN_EMERG "SleepySched: Taking %d Offline (running on %d state 0x%x)\n", cpu, smp_processor_id(), preempt_count());
			remove_cpu(cpu);
			printk(KERN_EMERG "SleepSched: Done");
		}

		pol = cpufreq_cpu_get(cpu);
		if (pol == NULL) {
			if (cpu_op->should_be_on) {
				printk(KERN_NOTICE "SleepySched: CPU %d Policy not ready\n", cpu);
				all_ready = 0;
			}
			continue;
		}

		if (cpumask_first(pol->cpus) != cpu) {
			/* Groups of CPUs that must be controlled together, only apply the policy of the first one */
			continue;
		}

		if (!pcpu->initalised) {
                    
			/* use "userspace" governor since it respects min and max freq requests,
			   intel_pstate will return -EINVAL since it doesn't support governors,
			   which is fine since pstate will respect the min and max qos anyway */
                    
			freq_qos_add_request(&pol->constraints,
					     &pcpu->qos_req_min,
					     FREQ_QOS_MIN,
					     cpu_op->freq_min);
                    
			freq_qos_add_request(&pol->constraints,
					     &pcpu->qos_req_max,
					     FREQ_QOS_MAX,
					     cpu_op->freq_max);
                    
			pcpu->initalised = 1;
		} else  {
			freq_qos_update_request(&pcpu->qos_req_min,
						cpu_op->freq_min);
                        
			freq_qos_update_request(&pcpu->qos_req_max,
						cpu_op->freq_max);
		}



		/* TODO: Performance instead? */

		write_to_attr(&pol->kobj, "scaling_governor", "userspace");
		//pol->policy = CPUFREQ_POLICY_POWERSAVE;
		refresh_frequency_limits(pol);
		cpufreq_cpu_put(pol);

		//idle_inject_set_percpu_duration(idle_dev, cpu, cpu_op->should_be_on);
	}
	printk(KERN_EMERG "SleepSched: Done SETOP");

	if (state == 1 && all_ready) {
		printk(KERN_NOTICE "SleepySched: starting iidev\n");
		//idle_inject_start(idle_dev);
		state++;
	}
}


int last_print  = 0;

int last_change = -1;
unsigned long last_util = -1;
void sleepy_sched_work(struct work_struct *work)
{
	/* only called on a single cpu */
	int cpu;
	unsigned long total_system_util, max_system_util;
	struct sys_op_table *tbl;

	/* TODO: workqueue will do the locking for us, so we could convert to RCU for updating autoset_table */
	raw_spin_lock(&tick_lock);
	tbl = autoset_table;
	raw_spin_unlock(&tick_lock);

	last_print++;
	printing = (last_print % 20) == 0;
	if (0) {
restart_with_printing:
		printing = 1;
	}

	total_system_util = 0;
	max_system_util   = 0;

	for_each_online_cpu(cpu) {
		/*
		 * note the idle injection threads runs in the FIFO class, 
		 * so this value is not affected by any ongoing idle injection.
		 */
		struct rq *runqueue = cpu_rq(cpu);
		unsigned long cpu_util = cpu_util_cfs(runqueue);
		unsigned long cpu_thermal = runqueue->avg_thermal.util_avg;
		unsigned long cpu_total = cpu_util + cpu_thermal;
		

		if (printing)
			printk(KERN_NOTICE "SleepySched: CPU %d %d/%d (%d normal, %d thermal)\n", cpu, cpu_total, arch_scale_cpu_capacity(cpu), cpu_util, cpu_thermal);

		total_system_util += cpu_total;

		// TODO: ideally this would be per-task not per-cpu
		if (cpu_total > max_system_util)
			max_system_util = cpu_total;
	}

	if (printing)
		printk(KERN_NOTICE "SleepySched: table %p total %lu max %lu\n", tbl, total_system_util, max_system_util);

	if (tbl) {
		struct sys_op *sys_op = select_operating_point(total_system_util, max_system_util, tbl);
		if ((total_system_util > last_util && last_change >= tbl->up_time) || last_change >= tbl->down_time) {
			if (!printing)
				goto restart_with_printing;
			last_util = total_system_util;
			set_sys_op(sys_op);
			last_change = 0;
		} else {
			last_change++;
		}
	} else {
		last_change++;
	}

	arm_timer(tbl ? tbl->sleep : 100); /* refresh every 100ms */
}
static DECLARE_WORK(sleepy_sched_task, sleepy_sched_work);


/* timer */
static void sleepy_sched_tick(struct timer_list *data) {
	schedule_work(&sleepy_sched_task);
}
static DEFINE_TIMER(sched_tick_timer, sleepy_sched_tick);

static void arm_timer(unsigned long after)
{
	mod_timer(&sched_tick_timer, jiffies + msecs_to_jiffies(after));
}

/*
static void reset_capacity(void)
{
	int cpu;
	
	for_each_possible_cpu(cpu) {
		struct sleepy_pcpu *pcpu = per_cpu_ptr(&sleepy_pcpu, cpu);
		//pcpu->capacity = SCHED_CAPACITY_SCALE;
	}
}


static void init_table(void)
{
	struct buf buf;
	u32 default_table[] = {
		1, 1,
			2000, 1,
				2200000, 2200000, 1000
	};

	buf.data = (const char*) default_table;
	buf.offset = 0;
	buf.size = sizeof(default_table);
	buf.err = 0;
	autoset_table = parse_op_table(&buf);
}
*/

void sleepy_sched_init(void)
{   
	struct proc_dir_entry *entry;
	struct cpumask mask;
	int cpu;

	printk(KERN_NOTICE "SleepySched: Initalizing sleepy scheduler\n");
	//init_table();

	for_each_online_cpu(cpu) {
		printk(KERN_NOTICE "SleepySched: CPU %d capacity %d\n", cpu, arch_scale_cpu_capacity(cpu));
	}
	
	printk(KERN_NOTICE "SleepySched: create iidev\n");
	cpumask_copy(&mask, cpu_online_mask);
	//idle_dev = idle_inject_register_full(&mask, idle_inject_cb);

	printk(KERN_NOTICE "SleepySched: Enabling Timer\n");
	arm_timer(100);

	file_ops.proc_ioctl = procs_unlocked_ioctl;
	entry = proc_create("sleep_sched", 0, NULL, &file_ops);
	BUG_ON(!entry);
}
