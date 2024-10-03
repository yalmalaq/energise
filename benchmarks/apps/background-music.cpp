#define PACKAGE_NAME "com.soundcloud.android"
#define BENCHMARK_NAME "bgmusic"

#include "benchmark.inc"

void open_url(char *url)
{
	std::string cmd = "am start -n ";
	std::string c = cmd + url;
	system(c.c_str());
}

void actions() {


	// old music benchmark 
    // // To touch somewhere on the screen
    // sleep(2);
    
    // system("input touchscreen tap 40 1200");
    // sleep(2);

    // //To start playing music
    // system("input touchscreen tap 1000 600");
    // sleep(2);

    //        // To go to home screen
    // system("input keyevent KEYCODE_HOME");
    // sleep(2);
    
    // //system("input keyevent KEYCODE_POWER");
	
    sleep(2);
    
    system("input touchscreen tap 40 1200");
    sleep(2);

    //To start playing music
    system("input touchscreen tap 44 2142");
    sleep(30);
    
    system("input touchscreen swipe 100 1000 100 200");
    sleep(30);
      
    system("input touchscreen swipe 100 1000 100 200");
    sleep(30);
    
    system("input touchscreen swipe 100 1000 100 200");
    sleep(10);
    
    system("input touchscreen swipe 100 1000 100 200");

    sleep(15); 
    system("input touchscreen tap 44 2142");

    
}

void run_benchmark()
{
    system("am start -n com.soundcloud.android/.launcher.LauncherActivity");
    actions();
}

