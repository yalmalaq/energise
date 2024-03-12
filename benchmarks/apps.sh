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

#keep_screen_on() {
#    ./adb shell svc power stayon true
#    sleep 120
#    ./adb shell svc power stayon false
#}

for i in {1..1}; do
   ./adb shell am start -n com.microsoft.teams/com.microsoft.skype.teams.Launcher
        sleep 2
      echo "Timestamp: $(date)" >> qos_video.txt; ./adb shell dumpsys gfxinfo com.microsoft.teams %s | grep -i jank -C 5 >> qos_video.txt 2>&1
    sleep 1
    ./runvideocall.sh >> qos_video.txt 2>&1
         echo "Timestamp: $(date)" >> qos_video.txt; ./adb shell dumpsys gfxinfo com.microsoft.teams %s | grep -i jank -C 5 >> qos_video.txt 2>&1
    sleep 2
    unlock_screen
    sleep 2
    close_all_apps
    sleep 2
done
#
for i in {1..10}; do
  ./adb shell am start -n com.soundcloud.android/.launcher.LauncherActivity
    sleep 2
     echo "Timestamp: $(date)" >> qos_music.txt; ./adb shell dumpsys gfxinfo com.soundcloud.android %s | grep -i jank -C 5 >> qos_music.txt 2>&1
    sleep 1
    ./runbackgroundmusic.sh >> qos_music.txt 2>&1
     echo "Timestamp: $(date)" >> qos_music.txt; ./adb shell dumpsys gfxinfo com.soundcloud.android %s | grep -i jank -C 5 >> qos_music.txt 2>&1
    sleep 2
    unlock_screen
    sleep 2
    close_all_apps
    sleep 2
done

for i in {1..10}; do
    ./adb shell am start -n com.zhiliaoapp.musically/com.ss.android.ugc.aweme.splash.SplashActivity
        sleep 2
    ./adb shell am start -n com.twitter.android/com.twitter.android.StartActivity
    sleep 2
    echo "Timestamp: $(date)" >> qos_tiktok.txt; ./adb shell dumpsys gfxinfo com.zhiliaoapp.musically %s | grep -i jank -C 5 >> qos_tiktok.txt  2>&1
        sleep 2
       echo "Timestamp: $(date)" >> qos_twitter.txt;  ./adb shell dumpsys gfxinfo com.twitter.android %s | grep -i jank -C 5 >>  qos_twitter.txt  2>&1
    sleep 2
    ./runsocialmedia.sh >> qos_twitter.txt
        sleep 2
     echo "Timestamp: $(date)" >> qos_tiktok.txt; ./adb shell dumpsys gfxinfo com.zhiliaoapp.musically %s | grep -i jank -C 5 >> qos_tiktok.txt  2>&1
        sleep 2
       echo "Timestamp: $(date)" >> qos_twitter.txt;  ./adb shell dumpsys gfxinfo com.twitter.android %s | grep -i jank -C 5 >>  qos_twitter.txt  2>&1
    sleep 2
    unlock_screen
    sleep 2
    close_all_apps
    sleep 2
done

for i in {1..10}; do
   ./adb shell am start -n com.JindoBlu.OfflineGames/com.unity3d.player.UnityPlayerActivity
        sleep 2
      echo "Timestamp: $(date)" >> qos_game.txt; ./adb shell dumpsys gfxinfo com.JindoBlu.OfflineGames %s | grep -i jank -C 5 >> qos_game.txt 2>&1
        sleep 1
    ./rungame.sh >> qos_game.txt 2>&1
      echo "Timestamp: $(date)" >> qos_game.txt; ./adb shell dumpsys gfxinfo com.JindoBlu.OfflineGames %s | grep -i jank -C 5 >> qos_game.txt 2>&1
    sleep 2
    unlock_screen
    sleep 2
    close_all_apps
    sleep 2
 
done

for i in {1..10}; do
  ./adb shell am start -n org.lineageos.jelly/.MainActivity
        sleep 1
  echo "Timestamp: $(date)" >> qos_web.txt; ./adb shell dumpsys gfxinfo org.lineageos.jelly %s | grep -i jank -C 5 >> qos_web.txt 2>&1
    sleep 1
    ./runweb.sh  >> qos_web.txt
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
