#define BENCHMARK_NAME "videocamera"
#define PACKAGE_NAME "org.lineageos.snap"

#include "benchmark.inc"


void run_benchmark()
{
    // system("am start -n com.JindoBlu.OfflineGames/com.unity3d.player.UnityPlayerActivity");
    // system("am start -n org.lineageos.snap/org.lineageos.snap.snap");
	system("input touchscreen tap 970 2195");
    sleep(10);
	system("input touchscreen tap 551 2079");
	sleep(110);
	system("input touchscreen tap 551 2079");
}
