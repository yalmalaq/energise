#define BENCHMARK_NAME "videocall"
#define PACKAGE_NAME "com.microsoft.teams"

#include "benchmark.inc"

void actions() {
    
    // To touch somewhere on the screen
    sleep(2);
    
    system("input touchscreen tap 100 500");
    sleep(2);

    //To start playing music
    system("input touchscreen tap 700 200");
    sleep(2);    
}



void run_benchmark()
{
    system("am start -n com.microsoft.teams/com.microsoft.skype.teams.Launcher");
    
    for (int i = 0; i <25; i++) {
        actions();
        struct timespec ts = {1, 000 * 1000000UL};
        nanosleep(&ts, NULL);
    }
}
