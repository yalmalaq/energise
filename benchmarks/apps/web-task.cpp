#define PACKAGE_NAME "org.lineageos.jelly"
#define BENCHMARK_NAME "web"

#include "benchmark.inc"

void open_url(const char *url)
{
	std::string cmd = "am start -a android.intent.action.VIEW -d ";
	std::string c = cmd + url;
	system(c.c_str());
}

void scroll(bool up)
{
	if (up) {
		 system("input touchscreen swipe 100 500 100 1100 300");
	} else {
		 system("input touchscreen swipe 100 1100 100 500 300");
	}
}

#define S(x) (1 << (x))
#define M(x) (1 << ((x) + 4))
#define L(x) (1 << ((x) + 6))

void run_benchmark()
{
    while (1) {
	std::vector<const char*> urls = {"https://youtube.com", "https://reddit.com", "https://bbc.co.uk", "https://www.theguardian.com/uk"};
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


