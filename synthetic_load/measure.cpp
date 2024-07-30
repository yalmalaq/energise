#include "../benchmarks/common.inc"

void print_stats()
{
    system("cat /sys/bus/iio/devices/iio:device0/energy_value");
    system("cat /sys/bus/iio/devices/iio:device1/energy_value");
}


struct global_args
{
    pthread_barrier_t barrier;
    int running;
};

struct thread_args
{
    struct global_args *globals;
    int i;
    int enabled;
    double work_rate;
}; 

void work()
{
    for (int i = 0; i < 1000; i++) {
        asm volatile ("nop");
    }
}

void *work_thread(void *in)
{
    struct thread_args *args    = (struct thread_args*) in;
    struct global_args *globals = args->globals;

    pin_to_core(args->i);
    pthread_barrier_wait(&globals->barrier);

    unsigned long start_time = get_usecs();
   
    long int count = 0;
    while (globals->running) {
        work();
        count++;
    }

    unsigned long end_time = get_usecs();
    double secs = (end_time - start_time) / 1000000.0;

    args->work_rate = count / secs;
    return NULL;
}

void go_raw(int nthreads)
{

    printf("===\n");
    printf("time %ld\n", get_usecs());

    /* disable/enable cpus */
    
    struct global_args globals;
    pthread_t threads[NUM_CORES];
    struct thread_args thread_args[NUM_CORES];


    globals.running = 1;
    pthread_barrier_init(&globals.barrier, NULL, nthreads + 1);

    /* spawn threads */
    
    for (int core = 0; core < NUM_CORES; core++) {
        if (is_enabled(core)) {
            thread_args[core].i = core;
            thread_args[core].globals = &globals;
            thread_args[core].work_rate = -1;
            pthread_create(&threads[core], NULL, work_thread, &thread_args[core]);
        }
    }

    /* work */
    
    pthread_barrier_wait(&globals.barrier);
    sleep(1); /* let things settle */
    print_stats();
   
    sleep(25);
    globals.running = 0;
   
    /* report */

    print_stats();
   
    for (int core = 0; core < NUM_CORES; core++) {
        if (is_enabled(core)) {
            pthread_join(threads[core], NULL);
            printf("core_wr %d %lf\n", core, thread_args[core].work_rate);
        }
    }
}

void go(long int small, long int med, long int large, long int enabled)
{
    int enabled_count = setup_cpufreq(small, med, large, enabled);
    go_raw(enabled_count);
}


    
/*
  negative current -> battery drain
  typical current_now ~= -137500 (137mA)
  300000 574000 738000 930000 1098000 1197000 1328000 1401000 1598000 1704000 1803000
  400000 553000 696000 799000 910000 1024000 1197000 1328000 1491000 1663000 1836000 1999000 2130000 2253000 
  500000 851000 984000 1106000 1277000 1426000 1582000 1745000 1826000 2048000 2188000 2252000 2401000 2507000 2630000 2704000 2802000 

  useage: freq_small freq_med freq_large on_cores */
int main(int argc, char** argv)
{
    setvbuf(stdout, NULL, _IOLBF, 512);

    enable_rails(0, strdup("BUCK2M,BUCK3M,BUCK4M,BUCK6M,LDO2M,LDO11M,LDO12M"));
    enable_rails(1, strdup("BUCK1S,BUCK2S,BUCK4S,BUCK5S,BUCK8S,BUCK10S,LDO1S"));

    if (argc == 2 && strcmp(argv[1], "stress") == 0) {
        go_raw(NUM_CORES);
    } else if (argc == 1) {
        /* schedule */ 
        for (int c = 1; c <= 4; c++) {
            for (int fi = 0; fi < arr_size(small_freqs); fi++) {
                go(small_freqs[fi], top(med_freqs), top(large_freqs), ((1 << c) - 1) << 0);
            }
        }

        for (int c = 1; c <= 2; c++) {
            for (int fi = 0; fi < arr_size(med_freqs); fi++) {
                go(top(small_freqs), med_freqs[fi], top(large_freqs), ((1 << c) - 1) << 4);
            }
        }

        for (int c = 1; c <= 2; c++) {
            for (int fi = 0; fi < arr_size(large_freqs); fi++) {
                go(top(small_freqs), top(med_freqs), large_freqs[fi], ((1 << c) - 1) << 6);
            }
        }

        int small = 0xf;
        int med = 0x3 << 5;
        int large = 0x3 << 6;
        go(low(small_freqs), low(med_freqs), low(large_freqs), small | med);
        go(mid(small_freqs), mid(med_freqs), mid(large_freqs), small | med);
        go(top(small_freqs), top(med_freqs), top(large_freqs), small | med);
        
        go(low(small_freqs), low(med_freqs), low(large_freqs), small | large);
        go(mid(small_freqs), mid(med_freqs), mid(large_freqs), small | large);
        go(top(small_freqs), top(med_freqs), top(large_freqs), small | large);
        
        go(low(small_freqs), low(med_freqs), low(large_freqs), med | large);
        go(mid(small_freqs), mid(med_freqs), mid(large_freqs), med | large);
        go(top(small_freqs), top(med_freqs), top(large_freqs), med | large);
        
        go(low(small_freqs), low(med_freqs), low(large_freqs), small | med | large);
        go(mid(small_freqs), mid(med_freqs), mid(large_freqs), small | med | large);
        go(top(small_freqs), top(med_freqs), top(large_freqs), small | med | large);

        goto rand;

    } else if (argc == 2 && strcmp(argv[1], "rand") == 0) {
    rand:
        while (1) {
            int small = small_freqs[rand() % arr_size(small_freqs)];
            int med   = med_freqs[rand() % arr_size(med_freqs)];
            int large = large_freqs[rand() % arr_size(large_freqs)];
            int enabled = rand();
            go (small, med, large, enabled);
        }

        reset();
    } else {
        /* single shot mode */
        long int small = atol(argv[1]);
        long int med = atol(argv[2]);
        long int large = atol(argv[3]);

        long int enabled = 0;
        for (char *start = argv[4]; *start; start++) {
            int num = *start - '0';
            enabled |= 1 << num;
        }

        go(small, med, large, enabled);

        reset();
    }
    
}
