set -eu
# Do the following after a reboot:
#    adb root (to enable root shell)
# in adb shell
#    mkdir /mnt/ryan
#    mount -t tmpfs none /mnt/ryan

CFLAGS=-static-libstdc++
/Users/yazeedyousefalmalaq/android-ndk/toolchains/llvm/prebuilt/darwin-x86_64/bin/armv7a-linux-androideabi21-clang++ $CFLAGS web-task.cpp #benchmarks_name.cpp ex: web-task.cpp"
./adb push a.out /mnt/ryan
./adb shell chmod u+x /mnt/ryan/a.out
./adb shell /mnt/ryan/a.out
