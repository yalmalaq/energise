#define PACKAGE_NAME "org.lineageos.jelly"
#define BENCHMARK_NAME "youtube"

#include "benchmark.inc"

void open_url(const char *url)
{
	std::string cmd = "am start -a android.intent.action.VIEW -d ";
	std::string c = cmd + url;
	system(c.c_str());
}

void scroll(bool up)
{
    sleep(3);
    //        touch somehwere to unmute
    system("input tap 540 400");
    
    sleep(30);
    system("input tap 540 300");
    
    
}

void run_benchmark()
{
    while (1) {
	std::vector<const char*> urls = {"https://www.youtube.com/watch?v=VMV0O7cQRuU"};
	for (const char *s : urls) {
            open_url(s);
            for (int i = 0; i < 25; i++) {
                scroll(false);
                struct timespec ts = {1, 000 * 1000000UL};
                nanosleep(&ts, NULL);
            }
	}
    }
}


