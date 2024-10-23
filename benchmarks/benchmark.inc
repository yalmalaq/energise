#include "common.inc"

void run_benchmark();

void enable_cores(long mask)
{
	for (int core = 1; core < 8; core++) {
		char path[500];
	snprintf(path, sizeof(path), "/sys/devices/system/cpu/cpu%d/online", core);
			int fd = open(path, O_WRONLY);
			if (fd < 0) {
				printf("%s\n", path);
				perror("no!");
				exit(-1);
			}
			char buf[] = "0\n";
			buf[0] = (mask & (1UL << core)) ? '1' : '0';
			int size = write(fd, buf, sizeof(buf));
			if (size < sizeof(buf)) {
				perror("no!");
				exit(-1);
			}
			close(fd);
	}
}

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

#define S(x) (1 << (x))
#define M(x) (1 << ((x) + 4))
#define L(x) (1 << ((x) + 6))

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

    // enable_cores(S(0) | S(1) | S(2) | S(3) | M(0) | M(1) | L(0) | L(1));

    enable_cores(M(0) | M(1));


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
