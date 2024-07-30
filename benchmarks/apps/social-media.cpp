#define BENCHMARK_NAME "socialmedia"
#define PACKAGE_NAME "com.twitter.android"

#include "benchmark.inc"

void open_activity(char *url)
{
	std::string cmd = "am start -n ";
	std::string c = cmd + url;
	system(c.c_str());
}

void actions() {
    sleep(3);
    
    for (int i = 0; i < 15; ++i) {
        system("input touchscreen swipe 100 1500 100 200");
        sleep(2);
    }
}

void run_benchmark()
{
    while (1) {
	std::vector<char*> urls = {
            (char*) "com.twitter.android/com.twitter.android.StartActivity"
            //, (char*) "com.zhiliaoapp.musically/com.ss.android.ugc.aweme.splash.SplashActivity"
        };
        
	for (char *s : urls) {
            open_activity(s);
            for (int i = 0; i <1; i++) {
                actions();
                struct timespec ts = {1, 000 * 1000000UL};
                nanosleep(&ts, NULL);
            }
	}
    }
}
