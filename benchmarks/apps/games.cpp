void extra_stats();

#define BENCHMARK_NAME "Game"
#define PACKAGE_NAME "com.JindoBlu.OfflineGames"
#define EXTRA_STATS extra_stats

#include "benchmark.inc"

void open_url(char *url)
{
	std::string cmd = "am start -n ";
	std::string c = cmd + url;
	system(c.c_str());
}

void extra_stats()
{
    puts("<frame-times>");
    system("dumpsys SurfaceFlinger --latency $(dumpsys SurfaceFlinger --list | grep SurfaceView | grep JindoBlu | tail -n 1)");
    puts("</frame-times>");
}


void actions() {
    // To touch somewhere on the screen
    system("input touchscreen tap 40 960");
    sleep(2);

    // To touch PLAY button
    system("input touchscreen tap 540 1900");
    sleep(2);

    // To choose restart again
    system("input touchscreen tap 500 1700");
    sleep(2);

    // Fake playing for 15 seconds
    while (1) {
        system("input touchscreen tap 540 960");
        sleep(1);
    }
}

void run_benchmark()
{
    system("am start -n com.JindoBlu.OfflineGames/com.unity3d.player.UnityPlayerActivity");
    sleep(10);
    actions();
}