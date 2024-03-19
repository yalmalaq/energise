#!/bin/bash

#This script will run android apps five times and after each times will cloes all the opened apps and pages, the will store the outputs result in txt file

    ##### Turn on the screen func ###
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

# This func read the energy value from CH0 and take the last number after the space
rails_energy_ch0() {
 ./adb shell cat /sys/bus/iio/devices/iio:device0/energy_value | grep 'CH0' | awk '{print $2}'
}

# This func the time from rails
rails_read_time() {
 ./adb shell cat /sys/bus/iio/devices/iio:device0/energy_value | grep 't' | sed 's/t=//'
}

## ./adb shell settings put system screen_brightness 100
#Lock Screen:
#./adb shell input keyevent KEYCODE_POWER

#keep_screen_on() {
#    ./adb shell svc power stayon true
#    sleep 120
#    ./adb shell svc power stayon false
#}


################ Web browser Benchmark ################
for i in {1..1}; do

# Start reading measuremnet of energy and time from rails:
    start_energy_value=$(rails_energy_ch0)
    start_time=$(rails_read_time)
    sleep 1
    
# Lunch the app:
   ./adb shell am start -n org.lineageos.jelly/.MainActivity
   sleep 1

# Start QoS measuremnets:
    echo "Timestamp: $(date)" >> web_data.txt; ./adb shell dumpsys gfxinfo org.lineageos.jelly %s | grep -i jank -C 5 >> web_data.txt 2>&1
    sleep 1

# Run the benchmark:
   ./runweb.sh  >> web_data.txt
   sleep 1

# End QoS measuremnets:
   echo "Timestamp: $(date)" >> web_data.txt; ./adb shell dumpsys gfxinfo org.lineageos.jelly %s | grep -i jank -C 5 >> web_data.txt 2>&1
   sleep 2

# End reading measuremnet of energy and time from rails:
  end_energy_value=$(rails_energy_ch0)
  end_time=$(rails_read_time)
  
# Calculate the energy (Joule), time (second) and power (Joule/second):
  energy_rails=$(echo "scale=2; ($end_energy_value - $start_energy_value) / 10000" | bc)
  time_rails=$(echo "scale=2; ($end_time - $start_time) / 1000" | bc)
  power_rails=$(echo "scale=2;  $energy_rails/ $time_rails" | bc)
  echo "Power_rails: $power_rails"
  echo "Energy_rails: $energy_rails"
  echo "Time_rails: $time_rails"
  
# Close all apps in backgrounds:
    unlock_screen
   sleep 2
    close_all_apps
    sleep 2
done

################### Game Benchmark ##################
#for i in {1..1}; do
#
## Start reading measuremnet of energy and time from rails:
#    start_energy_value=$(rails_energy_ch0)
#    start_time=$(rails_read_time)
#    sleep 1
#    
## Lunch the app:
#    ./adb shell am start -n com.JindoBlu.OfflineGames/com.unity3d.player.UnityPlayerActivity
#    
## Start QoS measuremnets:
#      echo "Timestamp: $(date)" >> game_test.txt; ./adb shell dumpsys gfxinfo com.JindoBlu.OfflineGames %s | grep -i jank -C 5 >> game_test.txt 2>&1
#         
## Run the benchmark:
#    ./rungame.sh >> game_test.txt 2>&1
#    
## End QoS measuremnets:
#      echo "Timestamp: $(date)" >> game_test.txt; ./adb shell dumpsys gfxinfo com.JindoBlu.OfflineGames %s | grep -i jank -C 5 >> game_test.txt 2>&1
#      
## End reading measuremnet of energy and time from rails:
#  end_energy_value=$(rails_energy_ch0)
#  end_time=$(rails_read_time)
#  
## Calculate the energy (Joule), time (second) and power (Joule/second):
#  energy_rails=$(echo "scale=2; ($end_energy_value - $start_energy_value) / 10000" | bc)
#  time_rails=$(echo "scale=2; ($end_time - $start_time) / 1000" | bc)
#  power_rails=$(echo "scale=2;  $energy_rails/ $time_rails" | bc)
#  echo "Power_rails: $power_rails"
#  echo "Energy_rails: $energy_rails"
#  echo "Time_rails: $time_rails"
## Close all apps in backgrounds:
#    unlock_screen
#    sleep 2
#    close_all_apps
#    sleep 2
# 
#done


################# Video call Benchmark ################
#for i in {1..1}; do
#
## Start reading measuremnet of energy and time from rails:
#    start_energy_value=$(rails_energy_ch0)
#    start_time=$(rails_read_time)
#    sleep 1
#
## Lunch the app:
#   ./adb shell am start -n com.microsoft.teams/com.microsoft.skype.teams.Launcher
#   sleep 2
#
## Start QoS measuremnets:
#    echo "Timestamp: $(date)" >> video_data.txt; ./adb shell dumpsys gfxinfo com.microsoft.teams %s | grep -i jank -C 5 >> video_data.txt 2>&1
#    sleep 1
#
## Run the benchmark:
#    ./runvideocall.sh >> video_data.txt 2>&1
#
## End QoS measuremnets:
#    echo "Timestamp: $(date)" >> video_data.txt; ./adb shell dumpsys gfxinfo com.microsoft.teams %s | grep -i jank -C 5 >> video_data.txt 2>&1
#
## End reading measuremnet of energy and time from rails:
#    end_energy_value=$(rails_energy_ch0)
#    end_time=$(rails_read_time)
#
## Calculate the energy (Joule), time (second) and power (Joule/second):
#    energy_rails=$(echo "scale=2; ($end_energy_value - $start_energy_value) / 10000" | bc)
#    time_rails=$(echo "scale=2; ($end_time - $start_time) / 1000" | bc)
#    power_rails=$(echo "scale=2;  $energy_rails/ $time_rails" | bc)
#    echo "Power_rails: $power_rails"
#    echo "Energy_rails: $energy_rails"
#    echo "Time_rails: $time_rails"
#
## Close all apps in backgrounds:
#    sleep 2
#    unlock_screen
#    sleep 2
#    close_all_apps
#    sleep 2
#done

###
#for i in {1..10}; do

## Start reading measuremnet of energy and time from rails:
#    start_energy_value=$(rails_energy_ch0)
#    start_time=$(rails_read_time)
#    sleep 1

## Lunch the app:
#  ./adb shell am start -n com.soundcloud.android/.launcher.LauncherActivity
#    sleep 2

## Start QoS measuremnets:
#     echo "Timestamp: $(date)" >> qos_music.txt; ./adb shell dumpsys gfxinfo com.soundcloud.android %s | grep -i jank -C 5 >> qos_music.txt 2>&1
#    sleep 1

## Run the benchmark:
#    ./runbackgroundmusic.sh >> qos_music.txt 2>&1

## End QoS measuremnets:
#     echo "Timestamp: $(date)" >> qos_music.txt; ./adb shell dumpsys gfxinfo com.soundcloud.android %s | grep -i jank -C 5 >> qos_music.txt 2>&1
#    sleep 2

## End reading measuremnet of energy and time from rails:
#    end_energy_value=$(rails_energy_ch0)
#    end_time=$(rails_read_time)
#
## Calculate the energy (Joule), time (second) and power (Joule/second):
#    energy_rails=$(echo "scale=2; ($end_energy_value - $start_energy_value) / 10000" | bc)
#    time_rails=$(echo "scale=2; ($end_time - $start_time) / 1000" | bc)
#    power_rails=$(echo "scale=2;  $energy_rails/ $time_rails" | bc)
#    echo "Power_rails: $power_rails"
#    echo "Energy_rails: $energy_rails"
#    echo "Time_rails: $time_rails"
#
## Close all apps in backgrounds:
#    unlock_screen
#    sleep 2
#    close_all_apps
#    sleep 2
#done
#

################# Social media Benchmark ################
#for i in {1..10}; do

## Start reading measuremnet of energy and time from rails:
#    start_energy_value=$(rails_energy_ch0)
#    start_time=$(rails_read_time)
#    sleep 1

## Lunch the app:
#    ./adb shell am start -n com.zhiliaoapp.musically/com.ss.android.ugc.aweme.splash.SplashActivity
#        sleep 2

## Start QoS measuremnets:
#    ./adb shell am start -n com.twitter.android/com.twitter.android.StartActivity
#    sleep 2
#    echo "Timestamp: $(date)" >> qos_tiktok.txt; ./adb shell dumpsys gfxinfo com.zhiliaoapp.musically %s | grep -i jank -C 5 >> qos_tiktok.txt  2>&1
#        sleep 2
#       echo "Timestamp: $(date)" >> qos_twitter.txt;  ./adb shell dumpsys gfxinfo com.twitter.android %s | grep -i jank -C 5 >>  qos_twitter.txt  2>&1
#    sleep 2

## Run the benchmark:
#    ./runsocialmedia.sh >> qos_twitter.txt
#        sleep 2

## End QoS measuremnets:
#     echo "Timestamp: $(date)" >> qos_tiktok.txt; ./adb shell dumpsys gfxinfo com.zhiliaoapp.musically %s | grep -i jank -C 5 >> qos_tiktok.txt  2>&1
#        sleep 2
#       echo "Timestamp: $(date)" >> qos_twitter.txt;  ./adb shell dumpsys gfxinfo com.twitter.android %s | grep -i jank -C 5 >>  qos_twitter.txt  2>&1
#    sleep 2

## End reading measuremnet of energy and time from rails:
#    end_energy_value=$(rails_energy_ch0)
#    end_time=$(rails_read_time)
#
## Calculate the energy (Joule), time (second) and power (Joule/second):
#    energy_rails=$(echo "scale=2; ($end_energy_value - $start_energy_value) / 10000" | bc)
#    time_rails=$(echo "scale=2; ($end_time - $start_time) / 1000" | bc)
#    power_rails=$(echo "scale=2;  $energy_rails/ $time_rails" | bc)
#    echo "Power_rails: $power_rails"
#    echo "Energy_rails: $energy_rails"
#    echo "Time_rails: $time_rails"
#
## Close all apps in backgrounds:
#    unlock_screen
#    sleep 2
#    close_all_apps
#    sleep 2
#done
#



################################# The remaining is for testing web browser benchmarks #########
#for i in {1..10}; do
#  ./adb shell am start -n org.lineageos.jelly/.MainActivity
#        sleep 1
#  echo "Timestamp: $(date)" >> web_1s.txt; ./adb shell dumpsys gfxinfo org.lineageos.jelly %s | grep -i jank -C 5 >> web_1s.txt 2>&1
#    sleep 1
#    ./runweb1s.sh  >> web_1s.txt
#    sleep 1
#  echo "Timestamp: $(date)" >> web_1s.txt; ./adb shell dumpsys gfxinfo org.lineageos.jelly %s | grep -i jank -C 5 >> web_1s.txt 2>&1
#    sleep 2
#    unlock_screen
#    sleep 2
#    close_all_apps
#    sleep 2
#done
##
#
#for i in {1..10}; do
#  ./adb shell am start -n org.lineageos.jelly/.MainActivity
#        sleep 1
#  echo "Timestamp: $(date)" >> web_2s.txt; ./adb shell dumpsys gfxinfo org.lineageos.jelly %s | grep -i jank -C 5 >> web_2s.txt 2>&1
#    sleep 1
#    ./runweb2s.sh  >> web_2s.txt
#    sleep 1
#  echo "Timestamp: $(date)" >> web_2s.txt; ./adb shell dumpsys gfxinfo org.lineageos.jelly %s | grep -i jank -C 5 >> web_2s.txt 2>&1
#    sleep 2
#    unlock_screen
#    sleep 2
#    close_all_apps
#    sleep 2
#done

#
#for i in {1..10}; do
#  ./adb shell am start -n org.lineageos.jelly/.MainActivity
#        sleep 1
#  echo "Timestamp: $(date)" >> web_4s.txt; ./adb shell dumpsys gfxinfo org.lineageos.jelly %s | grep -i jank -C 5 >> web_4s.txt 2>&1
#    sleep 1
#    ./runweb4s.sh  >> web_4s.txt
#    sleep 1
#  echo "Timestamp: $(date)" >> web_4s.txt; ./adb shell dumpsys gfxinfo org.lineageos.jelly %s | grep -i jank -C 5 >> web_4s.txt 2>&1
#    sleep 2
#    unlock_screen
#    sleep 2
#    close_all_apps
#    sleep 2
#done
#
#
#for i in {1..10}; do
#  ./adb shell am start -n org.lineageos.jelly/.MainActivity
#        sleep 1
#  echo "Timestamp: $(date)" >> web_4s2m.txt; ./adb shell dumpsys gfxinfo org.lineageos.jelly %s | grep -i jank -C 5 >> web_4s2m.txt 2>&1
#    sleep 1
#    ./runweb4s2m.sh  >> web_4s2m.txt
#    sleep 1
#  echo "Timestamp: $(date)" >> web_4s2m.txt; ./adb shell dumpsys gfxinfo org.lineageos.jelly %s | grep -i jank -C 5 >> web_4s2m.txt 2>&1
#    sleep 2
#    unlock_screen
#    sleep 2
#    close_all_apps
#    sleep 2
#done
#
#
#for i in {1..10}; do
#  ./adb shell am start -n org.lineageos.jelly/.MainActivity
#        sleep 1
#  echo "Timestamp: $(date)" >> web_4s2l.txt; ./adb shell dumpsys gfxinfo org.lineageos.jelly %s | grep -i jank -C 5 >> web_4s2l.txt 2>&1
#    sleep 1
#    ./runweb4s2l.sh  >> web_4s2l.txt
#    sleep 1
#  echo "Timestamp: $(date)" >> web_4s2l.txt; ./adb shell dumpsys gfxinfo org.lineageos.jelly %s | grep -i jank -C 5 >> web_4s2l.txt 2>&1
#    sleep 2
#    unlock_screen
#    sleep 2
#    close_all_apps
#    sleep 2
#done
##
##
