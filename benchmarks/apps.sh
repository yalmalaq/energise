#!/bin/bash

#This script will run android apps five times and after each times will cloes all the opened apps and pages, the will store the outputs result in txt file
 unlock_screen() {
    ./adb shell input keyevent KEYCODE_WAKEUP
    sleep 2
    ./adb shell input swipe 500 1200 500 500
    sleep 2
}


    ##### close all apps func ###
close_all_apps() {
    ./adb shell input keyevent KEYCODE_APP_SWITCH
    sleep 2
    ./adb shell input swipe 100 500 1000 500
    sleep 2
    ./adb shell input swipe 100 500 1000 500
    sleep 2
    ./adb shell input swipe 100 500 1000 500
    sleep 2
    ./adb shell input touchscreen tap 200 1200
    sleep 2
}

## ./adb shell settings put system screen_brightness 100
#Lock Screen:
#./adb shell input keyevent KEYCODE_POWER

keep_screen_on() {
    ./adb shell svc power stayon true
    sleep 120
    ./adb shell svc power stayon false
}

for i in {1..10}; do
  ./adb shell am start -n org.lineageos.jelly/.MainActivity
        sleep 1
  echo "Timestamp: $(date)" >> qos_web.txt; ./adb shell dumpsys gfxinfo org.lineageos.jelly %s | grep -i jank -C 5 >> qos_web.txt 2>&1
    sleep 1
    ./run.sh  >> qos_web.txt
    sleep 1
  echo "Timestamp: $(date)" >> qos_web.txt; ./adb shell dumpsys gfxinfo org.lineageos.jelly %s | grep -i jank -C 5 >> qos_web.txt 2>&1
    sleep 2
    unlock_screen
    sleep 2
    close_all_apps
    sleep 2
done
#
#
