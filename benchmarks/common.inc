#include <string>
#include <stdlib.h>
#include <vector>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>



#define NUM_CORES 8

long int small_freqs[] = {300000, 574000, 738000, 930000, 1098000, 1197000, 1328000, 1401000, 1598000, 1704000, 1803000};
long int med_freqs[]   = {400000, 553000, 696000, 799000, 910000, 1024000, 1197000, 1328000, 1491000, 1663000, 1836000, 1999000, 2130000, 2253000};
long int large_freqs[] = {500000, 851000, 984000, 1106000, 1277000, 1426000, 1582000, 1745000, 1826000, 2048000, 2188000, 2252000, 2401000, 2507000, 2630000, 2704000, 2802000};

#define arr_size(arr) (sizeof(arr) / sizeof(arr[0]))
#define low(arr) arr[0]
#define mid(arr) arr[arr_size(arr) / 2]
#define top(arr) arr[arr_size(arr) - 1]

#define CHARGE_COUNTER "/sys/class/power_supply/battery/charge_counter"
#define MAX_CHARGE "/sys/class/power_supply/battery/charge_full_design"

long read_counter(const char *c)
{
    int fd = open(c, O_RDONLY);
    char buf[500] = {0};
    read(fd, buf, sizeof(buf));        
    long ret = atol(buf);
    close(fd);
    return ret;
}


void set_charging(bool on)
{
    int fd = open("/sys/class/power_supply/usb/device/usb_limit_sink_enable", O_WRONLY);
    if (fd < 0) {
        perror("no!");
        exit(-1);
    }
    char buf[] = "0\n";
    buf[0] = on ? '0' : '1';
    int size = write(fd, buf, sizeof(buf));
    if (size < sizeof(buf)) {
        perror("no!");
        exit(-1);
    }
    close(fd);
}


unsigned long get_usecs()
{
    struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);
    return tv.tv_sec * 1000000UL + tv.tv_nsec / 1000UL;
}

struct measured_data
{
    double seconds;
    double energy;
    double power;
};

struct measured_data measure_energy_from_charge_counter(int min_sleep, void (*print_stats)(void))
{
    struct timespec small = {0, 500 * 1000UL};
    long start_value, mid_value, end_value;
    long int start_time, end_time;

    /* --- startup --- */
    
    printf("wait for edge\n");
    fflush(stdout);
    set_charging(false);

    start_value = read_counter(CHARGE_COUNTER);
    
    while (read_counter(CHARGE_COUNTER) == start_value) {
        nanosleep(&small, NULL);
    }
    start_value = read_counter(CHARGE_COUNTER);
    start_time = get_usecs();

    /* --- running --- */

    printf("start counter = %ld\n", start_value);
    if (print_stats)
        print_stats();
    fflush(stdout);
    
    sleep(min_sleep);

    /* --- shutdown --- */
    
    printf("wait for edge\n");
    fflush(stdout);

    while (read_counter(CHARGE_COUNTER) == start_value) {
        nanosleep(&small, NULL);
    }
    mid_value = read_counter(CHARGE_COUNTER);
    
    while (read_counter(CHARGE_COUNTER) == mid_value) {
        nanosleep(&small, NULL);
    }
    end_value = read_counter(CHARGE_COUNTER);
    
    end_time = get_usecs();
    printf("end counter = %ld\n", end_value);
    if (print_stats)
        print_stats();
    fflush(stdout);

    /* --- done --- */

    double secs   = (end_time - start_time) / 1000000.0;
    double energy = 0.0138 * (start_value - end_value); /* should always be a postive number */
    double power  = energy / secs;

    printf("Power %lf\nEnergy %lf\nTime %lf\n", power, energy, secs);
    fflush(stdout);
    set_charging(true);

    return (struct measured_data) {secs, energy, power};
}

char *numstr(long int arg)
{
    static char buf[64];
    snprintf(buf, sizeof(buf), "%ld\n", arg);
    return buf;
}

void write_file(const char *pat1, long int arg1, const char *val)
{
    int tries = 0;
 retry:
    
    char path[255];
    char dat [255];
    
    int path_bytes = snprintf(path, sizeof(path), pat1, arg1);
    path[path_bytes] = 0;
    
    int dat_bytes  = snprintf(dat , sizeof(dat) , "%s", val);

    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        fprintf(stderr, "Error opening %s\n", path);
        perror("errno");
        
        if (tries++ < 5) {
            sleep(1);
            goto retry;
        } else {
            exit(-1);
        }
    }

    if (write(fd, dat, dat_bytes) != dat_bytes) {
        fprintf(stderr, "Error writing %d bytes to %s\n", dat_bytes, path);
        perror("errno");
        close(fd);
        
        if (tries++ < 5) {
            sleep(1);
            goto retry;
        } else {
            exit(-1);
        }
    }
}

void set_cpu_online(int cpu, int online)
{
    write_file("/sys/devices/system/cpu/cpu%d/online", cpu, online ? "1" : "0");
}

void reset()
{
    /* re-enable all cores */
    for (int core = 1; core < NUM_CORES; core++)
        set_cpu_online(core, 1);

    write_file("/sys/devices/system/cpu/cpu%d/cpufreq/scaling_max_freq", 0, numstr(top(small_freqs)));
    write_file("/sys/devices/system/cpu/cpu%d/cpufreq/scaling_min_freq", 0, numstr(low(small_freqs)));
    write_file("/sys/devices/system/cpu/cpu%d/cpufreq/scaling_max_freq", 4, numstr(top(med_freqs)));
    write_file("/sys/devices/system/cpu/cpu%d/cpufreq/scaling_min_freq", 4, numstr(low(med_freqs)));
    write_file("/sys/devices/system/cpu/cpu%d/cpufreq/scaling_max_freq", 6, numstr(top(large_freqs)));
    write_file("/sys/devices/system/cpu/cpu%d/cpufreq/scaling_min_freq", 6, numstr(low(large_freqs)));
}

int global_cores_enabled_mask = 0xFFFF;
inline int is_enabled(int core) {
    return (global_cores_enabled_mask >> core) & 1;
}

void pin_to_core(int core)
{
   cpu_set_t cpuset;
   CPU_ZERO(&cpuset);
   CPU_SET(core, &cpuset);
   sched_setaffinity(gettid(), sizeof(cpu_set_t), &cpuset);
}

int setup_cpufreq(long int small, long int med, long int large, long int enabled)
{
    global_cores_enabled_mask = enabled;
    printf("smallf %ld\nmedf %ld\nlargef %ld\nenabled %ld\n", small, med, large, enabled);
    
    reset();
    
    /* parse args */
    if (small != -1) {
        write_file("/sys/devices/system/cpu/cpu%d/cpufreq/scaling_min_freq", 0, numstr(small));
        write_file("/sys/devices/system/cpu/cpu%d/cpufreq/scaling_max_freq", 0, numstr(small));
    }

    if (med != -1) {
        write_file("/sys/devices/system/cpu/cpu%d/cpufreq/scaling_min_freq", 4, numstr(med));
        write_file("/sys/devices/system/cpu/cpu%d/cpufreq/scaling_max_freq", 4, numstr(med));
    }

    if (large != -1) {
        write_file("/sys/devices/system/cpu/cpu%d/cpufreq/scaling_min_freq", 6, numstr(large));
        write_file("/sys/devices/system/cpu/cpu%d/cpufreq/scaling_max_freq", 6, numstr(large));
    }

    int enabled_count = 0;
    for (int core = 0; core < NUM_CORES; core++) {
        if (is_enabled(core))
            enabled_count++;
        
        if (core != 0)
            set_cpu_online(core, is_enabled(core));            
    }

    return enabled_count;
}

void enable_rails(int dev, char *rails)
{
    int chan = 0;
    char *el = strtok(rails, ",");
    while (el != NULL && chan < 8) {
        char buf[128];
        snprintf(buf, sizeof(buf), "CH%d=%s\n", chan++, el);
        write_file("/sys/bus/iio/devices/iio:device%d/enabled_rails", dev, buf);
        
        el = strtok(NULL, ",");
    }
}

void setup_from_env()
{
    long int small = -1;
    if (getenv("FREQ_S"))
        small = atol(getenv("FREQ_S"));
    
    long int med   = -1;
    if (getenv("FREQ_M"))
        med = atol(getenv("FREQ_M"));
    
    long int large = -1;
    if (getenv("FREQ_L"))
        large = atol(getenv("FREQ_L"));

    long int enabled = 0xFF;
    if (getenv("CORES")) {
        enabled = 0;
        for (char *start = getenv("CORES"); *start; start++) {
            int num = *start - '0';
            enabled |= 1 << num;
        }
    }
    
    setup_cpufreq(small, med, large, enabled);

    enable_rails(0, getenv("RAILS0"));
    enable_rails(1, getenv("RAILS1"));
}

int needs_charge()
{
    long int charge = read_counter(CHARGE_COUNTER);
    long int max    = read_counter(    MAX_CHARGE);
    double d        = 100.0 * charge / max;
    printf("Charge: %.1lf\n", d);
    return d < 30;
}

void clear_tasks()
{
    system("dumpsys activity recents | grep -oE 'taskId=[0-9]+' | cut -d = -f 2 | xargs -n 1 am stack remove");
}

void wait_for_charge()
{
    while (needs_charge()) {
        set_charging(true);
        printf("Waiting for charge\n");
        sleep(120);
    }
}
