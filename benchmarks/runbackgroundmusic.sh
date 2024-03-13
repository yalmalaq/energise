set -eu
# Do the following after a reboot:
#    adb root (to enable root shell)
# in adb shell
#    mkdir /mnt/ryan
#    mount -t tmpfs none /mnt/ryan

CFLAGS=-static-libstdc++
/Users/yazeedyousefalmalaq/android-ndk/toolchains/llvm/prebuilt/darwin-x86_64/bin/armv7a-linux-androideabi21-clang++ $CFLAGS background-music.cpp
./adb push a.out /mnt/ryan
./adb shell chmod u+x /mnt/ryan/a.out
./adb shell /mnt/ryan/a.out

#./adb push sensor.py /data/data/com.termux/files/home
#./adb shell chmod +x /data/data/com.termux/files/home/power_energy4.sh
#cp /mnt/yazeed/mob23  /Users/yazeedyousefalmalaq/Desktop/Android/energy/
# ssh from mbile to laptop: ssh username@ippadd
#  ./adb pair 10.201.65.155:30391
#  ./adb connect 10.201.65.155:40191

# solve the problem of shell :
#export PATH=/data/data/com.termux/files/usr/bin:${PATH}
#Also, don't run su directly under Termux shell. Use tsu instead which can be installed by pkg install tsu.


#cat /sys/class/power_supply/battery/current_now
# off the charging
#echo 0 > /sys/class/power_supply/main-charger/online

# on the charging
#echo 1 > /sys/class/power_supply/main-charger/online

# adb: more than one device/emulator
# ./adb -s 192.168.0.30 shell


#./adb push web-task.cpp /data/data/com.termux/files/home

#./adb push w1 /sdcard

# cp /data/data/com.termux/files/home/web-task.cpp /home/yazeedalmalaq/Desktop/energy
