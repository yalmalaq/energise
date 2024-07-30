#include "common.inc"

void run_benchmark();

void print_stats()
{
#ifdef EXTRA_STATS
    EXTRA_STATS();
#endif

#ifdef PACKAGE_NAME    
    const char *package_name = PACKAGE_NAME;
    if (package_name) {    
        char cmd[255];
        int len = snprintf(cmd, 255, "dumpsys gfxinfo %s | grep -i jank -C 5", package_name);
        cmd[len] = 0;
        system(cmd);
    }
#endif

    /* output the counters */
    system("cat /sys/bus/iio/devices/iio:device0/energy_value");
    system("cat /sys/bus/iio/devices/iio:device1/energy_value");

    system("dumpsys SurfaceFlinger | grep -A 2 fps");
}


void *benchmark_thread(void *_dat)
{
    run_benchmark();
    return NULL;
}

int main(int argc, char **argv)
{
    setvbuf(stdout, NULL, _IOLBF, 512);
    //system("echo test > /sys/power/wake_lock");
    
    printf("===\n");
#ifdef BENCHMARK_NAME
    puts("name " BENCHMARK_NAME);
#endif
    
    wait_for_charge();
    setup_from_env();

    pthread_t t;
    pthread_create(&t, NULL, benchmark_thread, NULL);
    
    sleep(2); /* allow things to settle */

    int time = 120;
    if (getenv("TIME"))
        time = atoi(getenv("TIME"));

    if (getenv("MEASURE_BAT")) {
        measure_energy_from_charge_counter(10, print_stats);
        
    } else {
        print_stats();
        long int start_time = get_usecs();
        sleep(time);
        puts("**DONE**\n");
        print_stats();
        long int end_time = get_usecs();

        printf("Time %lf\n", (end_time - start_time) / 1000000.0);
    }
    
    exit(0);
}
