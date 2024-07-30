#define BENCHMARK_NAME "bgmusic"

#include "benchmark.inc"

void open_url(char *url)
{
	std::string cmd = "am start -n ";
	std::string c = cmd + url;
	system(c.c_str());
}

void actions() {
    
    // To touch somewhere on the screen
    sleep(2);
    
    system("input touchscreen tap 40 1200");
    sleep(2);

    //To start playing music
    system("input touchscreen tap 1000 600");
    sleep(2);

           // To go to home screen
    system("input keyevent KEYCODE_HOME");
    sleep(2);
    
    //system("input keyevent KEYCODE_POWER");
}

void run_benchmark()
{
    system("am start -n com.soundcloud.android/.launcher.LauncherActivity");
    actions();
}

