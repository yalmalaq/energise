#!/bin/bash

#This script will run android apps ten times and after each times will cloes all the opened apps and pages, the will store the outputs result in txt file

    ##### to weak up and unlcock the screen ###########
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


for i in {1..10}; do
    ./run.sh >> social_media.txt
    
    ## measure the fps for TikTok app:
    ./adb shell dumpsys gfxinfo com.zhiliaoapp.musically >> social_media.txt
    sleep 2
    
     unlock_screen
     sleep 1
    

    close_all_apps
    sleep 1
done
