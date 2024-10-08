#define PACKAGE_NAME "org.lineageos.jelly"
#define BENCHMARK_NAME "book"

#include "benchmark.inc"

void open_url(const char *url)
{
	std::string cmd = "am start -a android.intent.action.VIEW -d ";
	std::string c = cmd + url;
	system(c.c_str());
}

void scroll(bool up)
{
    for (int i = 0; i < 8; ++i) {
        system("input touchscreen swipe 100 1500 100 200");
        sleep(15);
    }
    
}

void run_benchmark()
{
    while (1) {
	std::vector<const char*> urls = {"https://milton.host.dartmouth.edu/reading_room/pl/book_9/text.shtml"};
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
