void extra_stats();

#define BENCHMARK_NAME "Game"
#define PACKAGE_NAME "com.bigduckgames.flow"
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
    sleep(5);
    
    system("input touchscreen tap 540 600");
    sleep(3);
    system("input touchscreen tap 200 700");
    sleep(3);
    system("input touchscreen tap 200 900");
    
    
    sleep(3);
//    system("input touchscreen tap 540 600");
//
//    sleep(3);
//    system("input touchscreen tap 200 900");
//    
//    system("input touchscreen tap 200 800");

    system("input touchscreen swipe 540 960 800 400 1000");
    
    sleep(3);
    system("input touchscreen swipe 540 960 800 400 1000");
    
    //green
    system("input touchscreen swipe 540 800 200 1600 1000");
    sleep(2);
    
    sleep(3);
    system("input touchscreen swipe 540 960 540 1600 1000");
    
    sleep(3);
    system("input touchscreen swipe 540 960 540 1600 1000");
    
    system("input touchscreen swipe 540 960 800 400 1000");
    
    sleep(3);
    system("input touchscreen swipe 540 960 800 400 1000");
    
    
    //blue
    sleep(3);
    system("input touchscreen swipe 540 960 540 1600 1000");
    
    sleep(2);

    system("input touchscreen swipe 540 800 200 1600 1000");
    sleep(2);
    
    //yellow
    system("input touchscreen swipe 950 800 500 1800 1000");
    sleep(2);
    
    
    //orange
    system("input touchscreen swipe 950 950 500 1800 1000");
    sleep(2);
    
    //yellow again crash
    system("input touchscreen swipe 950 800 500 1800 1000");
    sleep(2);
    
    //yellow
    system("input touchscreen swipe 950 950 800 1600 1000");
    sleep(2);
    
    //orange
    system("input touchscreen swipe 540 960 800 400 1000");
    
    sleep(3);
    system("input touchscreen swipe 540 960 540 1600 1000");
    

    system("input touchscreen swipe 540 800 200 1600 1000");
    sleep(2);

    //yellow
    system("input touchscreen swipe 950 800 500 1800 1000");
    sleep(2);

    //orange
    system("input touchscreen swipe 950 950 500 1800 1000");
    sleep(2);

    //yellow again crash
    system("input touchscreen swipe 950 800 500 1800 1000");
    sleep(2);

    //yellow
    system("input touchscreen swipe 950 950 800 1600 1000");
    
    system("input touchscreen swipe 200 960 200 1600 1000");
    
}

void run_benchmark()
{
    // system("am start -n com.JindoBlu.OfflineGames/com.unity3d.player.UnityPlayerActivity");
    system("am start -n com.bigduckgames.flow/com.bigduckgames.flow.flow");
    sleep(10);
    actions();
}
