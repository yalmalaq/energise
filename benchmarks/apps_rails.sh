#!/bin/bash


# This script will run Android Benchmarks multiple times, and after each time, it will close all the opened apps and pages.

# Also, it will measure energy from rails, and measure QoS.

##### This func is to run diff benchmarks ###
run_benchmark() {
    benchmark=$1
    
 CFLAGS=-static-libstdc++
 /Users/yazeedyousefalmalaq/android-ndk/toolchains/llvm/prebuilt/darwin-x86_64/bin/armv7a-linux-androideabi21-clang++ $CFLAGS "$benchmark"
 ./adb push a.out /mnt/ryan
./adb shell chmod u+x /mnt/ryan/a.out
./adb shell /mnt/ryan/a.out
}


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

# energy value from CH3= S2M_VDD_CPUCL2 (energy of large cores)
rails_energy_ch3() {
 ./adb shell cat /sys/bus/iio/devices/iio:device0/energy_value | grep 'CH3' | awk '{print $2}'
}
# energy value from CH4= S3M_VDD_CPUCL1 (energy of med cores)
rails_energy_ch4() {
 ./adb shell cat /sys/bus/iio/devices/iio:device0/energy_value | grep 'CH4' | awk '{print $2}'
}
# energy value from CH5= S4M_VDD_CPUCL0 (energy of small cores)
rails_energy_ch5() {
 ./adb shell cat /sys/bus/iio/devices/iio:device0/energy_value | grep 'CH5' | awk '{print $2}'
}

# to get the time from rails
rails_read_time() {
 ./adb shell cat /sys/bus/iio/devices/iio:device0/energy_value | grep 't' | sed 's/t=//'
}
# to get janky frames
janky_frames() {
    package=$1
./adb shell dumpsys gfxinfo "$package" | grep 'Janky frames:' | awk '{print $3}'
}
# to get total frames rendered
total_frames() {
    package=$1
./adb shell dumpsys gfxinfo "$package" | grep 'Total frames' | awk '{print $4}'
}


# ./adb shell settings put system screen_brightness 100


####################### Web benchmark #############
for i in {1..1}; do

# Lunch the app:
   ./adb shell am start -n org.lineageos.jelly/.MainActivity
   sleep 1
   
# Start measuremnets of energy and time from rails:
    start_energy1_value=$(rails_energy_ch3)
    start_energy2_value=$(rails_energy_ch4)
    start_energy3_value=$(rails_energy_ch5)
    start_time=$(rails_read_time)
    
# Start QoS measuremnets:
    start_janky_frames=$(janky_frames "org.lineageos.jelly")
    start_total_frames=$(total_frames "org.lineageos.jelly")
    sleep 1
    
# Run the benchmark:
   run_benchmark web-task.cpp #>> web_data.txt

# End QoS measuremnets:
   end_janky_frames=$(janky_frames "org.lineageos.jelly")
   end_total_frames=$(total_frames "org.lineageos.jelly")

# End measuremnets of energy and time from rails:
  end_energy1_value=$(rails_energy_ch3)
  end_energy2_value=$(rails_energy_ch4)
  end_energy3_value=$(rails_energy_ch5)
  end_time=$(rails_read_time)

# Calculate the energy (Joule), time (second) and power (Joule/second):
  energy1=$(echo "scale=2; ($end_energy1_value - $start_energy1_value) / 100000" | bc)
  energy2=$(echo "scale=2; ($end_energy2_value - $start_energy2_value) / 100000" | bc)
  energy3=$(echo "scale=2; ($end_energy3_value - $start_energy3_value) / 100000" | bc)
  energy_rails=$(awk "BEGIN { printf \"%.2f\", ($energy1 + $energy2 + $energy3) / 3 }")
  time_rails=$(echo "scale=2; ($end_time - $start_time) / 1000" | bc)
  power_rails=$(echo "scale=2;  $energy_rails/ $time_rails" | bc)
  echo "Power_rails: $power_rails" #>> web_data.txt
  echo "Energy_rails: $energy_rails" #>> web_data.txt
  echo "Time_rails: $time_rails"  #>> web_data.txt

# Calculate the QoSs:
  janky=$(echo "scale=2; ($end_janky_frames - $start_janky_frames)" | bc)
  frames=$(echo "scale=2; ($end_total_frames - $start_total_frames)" | bc)
  janky_sec=$(echo "scale=2; ($janky / $time_rails)" | bc)
  frames_sec=$(echo "scale=2; ($frames / $time_rails)" | bc)
  janky_totalframes=$(echo "scale=3; ($janky / $frames) * 100" | bc)
  echo "Janky: $janky"  #>> web_data.txt
  echo "Frames: $frames"  #>> web_data.txt
  echo "janky_per_second: $janky_sec"  #>> web_data.txt
  echo "frames per second: $frames_sec"  #>> web_data.txt
  echo "Janky_/_totalframes: $janky_totalframes %"  #>> web_data.txt

# Close all apps in backgrounds:
  unlock_screen
  sleep 2
  close_all_apps
  sleep 2
done

# All benchmarks are below, just remove the # comment symbol to run ... :
#
######################### Game playing Benchmark #############
#for i in {1..1}; do
#
## Lunch the app:
#     ./adb shell am start -n com.JindoBlu.OfflineGames/com.unity3d.player.UnityPlayerActivity
#   
## Start measuremnets of energy and time from rails:
#    start_energy1_value=$(rails_energy_ch3)
#    start_energy2_value=$(rails_energy_ch4)
#    start_energy3_value=$(rails_energy_ch5)
#    start_time=$(rails_read_time)
#    
## Start QoS measuremnets:
#    start_janky_frames=$(janky_frames "com.JindoBlu.OfflineGames")
#    start_total_frames=$(total_frames "com.JindoBlu.OfflineGames")
#    sleep 1
#    
## Run the benchmark:
#   run_benchmark games.cpp #>> games_data.txt
#
## End QoS measuremnets:
#   end_janky_frames=$(janky_frames "com.JindoBlu.OfflineGames")
#   end_total_frames=$(total_frames "com.JindoBlu.OfflineGames")
#
## End measuremnets of energy and time from rails:
#  end_energy1_value=$(rails_energy_ch3)
#  end_energy2_value=$(rails_energy_ch4)
#  end_energy3_value=$(rails_energy_ch5)
#  end_time=$(rails_read_time)
#
## Calculate the energy (Joule), time (second) and power (Joule/second):
#  energy1=$(echo "scale=2; ($end_energy1_value - $start_energy1_value) / 100000" | bc)
#  energy2=$(echo "scale=2; ($end_energy2_value - $start_energy2_value) / 100000" | bc)
#  energy3=$(echo "scale=2; ($end_energy3_value - $start_energy3_value) / 100000" | bc)
#  energy_rails=$(awk "BEGIN { printf \"%.2f\", ($energy1 + $energy2 + $energy3) / 3 }")
#  time_rails=$(echo "scale=2; ($end_time - $start_time) / 1000" | bc)
#  power_rails=$(echo "scale=2;  $energy_rails/ $time_rails" | bc)
#  echo "Power_rails: $power_rails" #>> games_data.txt
#  echo "Energy_rails: $energy_rails" #>> games_data.txt
#  echo "Time_rails: $time_rails"  #>> games_data.txt
#
## Calculate the QoSs:
#  janky=$(echo "scale=2; ($end_janky_frames - $start_janky_frames)" | bc)
#  frames=$(echo "scale=2; ($end_total_frames - $start_total_frames)" | bc)
#  janky_sec=$(echo "scale=2; ($janky / $time_rails)" | bc)
#  frames_sec=$(echo "scale=2; ($frames / $time_rails)" | bc)
#  janky_totalframes=$(echo "scale=3; ($janky / $frames) * 100" | bc)
#  echo "Janky: $janky"  #>> games_data.txt
#  echo "Frames: $frames"  #>> games_data.txt
#  echo "janky_per_second: $janky_sec" #>> games_data.txt
#  echo "frames per second: $frames_sec" #>> games_data.txt
#  echo "Janky_/_totalframes: $janky_totalframes %"  #>> games_data.txt
#
## Close all apps in backgrounds:
#  unlock_screen
#  sleep 2
#  close_all_apps
#  sleep 2
#done
#
#
######################### Background music Benchmark  ##########
#for i in {1..1}; do
#
## Lunch the app:
#   ./adb shell am start -n com.soundcloud.android/.launcher.LauncherActivity
#
#   
## Start measuremnets of energy and time from rails:
#    start_energy1_value=$(rails_energy_ch3)
#    start_energy2_value=$(rails_energy_ch4)
#    start_energy3_value=$(rails_energy_ch5)
#    start_time=$(rails_read_time)
#    
## Start QoS measuremnets:
#    start_janky_frames=$(janky_frames "com.soundcloud.android")
#    start_total_frames=$(total_frames "com.soundcloud.android")
#    sleep 1
#    
## Run the benchmark:
#   run_benchmark background-music.cpp #>> background_music_data.txt
#
## End QoS measuremnets:
#   end_janky_frames=$(janky_frames "com.soundcloud.android")
#   end_total_frames=$(total_frames "com.soundcloud.android")
#
## End measuremnets of energy and time from rails:
#  end_energy1_value=$(rails_energy_ch3)
#  end_energy2_value=$(rails_energy_ch4)
#  end_energy3_value=$(rails_energy_ch5)
#  end_time=$(rails_read_time)
#
## Calculate the energy (Joule), time (second) and power (Joule/second):
#  energy1=$(echo "scale=2; ($end_energy1_value - $start_energy1_value) / 100000" | bc)
#  energy2=$(echo "scale=2; ($end_energy2_value - $start_energy2_value) / 100000" | bc)
#  energy3=$(echo "scale=2; ($end_energy3_value - $start_energy3_value) / 100000" | bc)
#  energy_rails=$(awk "BEGIN { printf \"%.2f\", ($energy1 + $energy2 + $energy3) / 3 }")
#  time_rails=$(echo "scale=2; ($end_time - $start_time) / 1000" | bc)
#  power_rails=$(echo "scale=2;  $energy_rails/ $time_rails" | bc)
#  echo "Power_rails: $power_rails" #>> background_music_data.txt
#  echo "Energy_rails: $energy_rails" #>> background_music_data.txt
#  echo "Time_rails: $time_rails"  #>> background_music_data.txt
#
## Calculate the QoSs:
#  janky=$(echo "scale=2; ($end_janky_frames - $start_janky_frames)" | bc)
#  frames=$(echo "scale=2; ($end_total_frames - $start_total_frames)" | bc)
#  janky_sec=$(echo "scale=2; ($janky / $time_rails)" | bc)
#  frames_sec=$(echo "scale=2; ($frames / $time_rails)" | bc)
#  janky_totalframes=$(echo "scale=3; ($janky / $frames) * 100" | bc)
#  echo "Janky: $janky"  #>> background_music_data.txt
#  echo "Frames: $frames" #>> background_music_data.txt
#  echo "janky_per_second: $janky_sec"  #>> background_music_data.txt
#  echo "frames per second: $frames_sec"  #>> background_music_data.txt
#  echo "Janky_/_totalframes: $janky_totalframes %" #>> background_music_data.txt
#
## Close all apps in backgrounds:
#  unlock_screen
#  sleep 2
#  close_all_apps
#  sleep 2
#done
#
################# Video call Benchmark ################
#
#for i in {1..1}; do
#
## Lunch the app:
#   ./adb shell am start -n com.microsoft.teams/com.microsoft.skype.teams.Launcher
#
#   
## Start measuremnets of energy and time from rails:
#    start_energy1_value=$(rails_energy_ch3)
#    start_energy2_value=$(rails_energy_ch4)
#    start_energy3_value=$(rails_energy_ch5)
#    start_time=$(rails_read_time)
#    
## Start QoS measuremnets:
#    start_janky_frames=$(janky_frames "com.microsoft.teams")
#    start_total_frames=$(total_frames "com.microsoft.teams")
#    sleep 1
#    
## Run the benchmark:
#   run_benchmark video-call.cpp #>> video_call_data.txt
#
## End QoS measuremnets:
#   end_janky_frames=$(janky_frames "com.microsoft.teams")
#   end_total_frames=$(total_frames "com.microsoft.teams")
#
## End measuremnets of energy and time from rails:
#  end_energy1_value=$(rails_energy_ch3)
#  end_energy2_value=$(rails_energy_ch4)
#  end_energy3_value=$(rails_energy_ch5)
#  end_time=$(rails_read_time)
#
## Calculate the energy (Joule), time (second) and power (Joule/second):
#  energy1=$(echo "scale=2; ($end_energy1_value - $start_energy1_value) / 100000" | bc)
#  energy2=$(echo "scale=2; ($end_energy2_value - $start_energy2_value) / 100000" | bc)
#  energy3=$(echo "scale=2; ($end_energy3_value - $start_energy3_value) / 100000" | bc)
#  energy_rails=$(awk "BEGIN { printf \"%.2f\", ($energy1 + $energy2 + $energy3) / 3 }")
#  time_rails=$(echo "scale=2; ($end_time - $start_time) / 1000" | bc)
#  power_rails=$(echo "scale=2;  $energy_rails/ $time_rails" | bc)
#  echo "Power_rails: $power_rails" #>> video_call_data.txt
#  echo "Energy_rails: $energy_rails" #>> video_call_data.txt
#  echo "Time_rails: $time_rails"  #>> video_call_data.txt
#
## Calculate the QoSs:
#  janky=$(echo "scale=2; ($end_janky_frames - $start_janky_frames)" | bc)
#  frames=$(echo "scale=2; ($end_total_frames - $start_total_frames)" | bc)
#  janky_sec=$(echo "scale=2; ($janky / $time_rails)" | bc)
#  frames_sec=$(echo "scale=2; ($frames / $time_rails)" | bc)
#  janky_totalframes=$(echo "scale=3; ($janky / $frames) * 100" | bc)
#  echo "Janky: $janky"  #>> video_call_data.txt
#  echo "Frames: $frames"  #>> video_call_data.txt
#  echo "janky_per_second: $janky_sec"  #>> video_call_data.txt
#  echo "frames per second: $frames_sec"  #>> video_call_data.txt
#  echo "Janky_/_totalframes: $janky_totalframes %"  #>> video_call_data.txt
#
## Close all apps in backgrounds:
#  unlock_screen
#  sleep 2
#  close_all_apps
#  sleep 2
#done
#
######################### Social media benchmark ##########
#for i in {1..1}; do
#
## Lunch the app:
#    ./adb shell am start -n com.twitter.android/com.twitter.android.StartActivity
#    sleep 1
#    ./adb shell am start -n com.zhiliaoapp.musically/com.ss.android.ugc.aweme.splash.SplashActivity
#    sleep 1
# 
#   
#   
## Start measuremnets of energy and time from rails:
#    start_energy1_value=$(rails_energy_ch3)
#    start_energy2_value=$(rails_energy_ch4)
#    start_energy3_value=$(rails_energy_ch5)
#    start_time=$(rails_read_time)
#    
## Start QoS measuremnets:
#    start_janky_frames=$(janky_frames "com.twitter.android")
#    start_total_frames=$(total_frames "com.twitter.android")
#    sleep 1
#    start_janky_frames2=$(janky_frames "com.zhiliaoapp.musically")
#    start_total_frames2=$(total_frames "com.zhiliaoapp.musically")
#    sleep 1
#    
#    
## Run the benchmark:
#   run_benchmark social-media.cpp #>> social_media_data.txt
#
## End QoS measuremnets:
#    end_janky_frames=$(janky_frames "com.twitter.android")
#    end_total_frames=$(total_frames "com.twitter.android")
#    sleep 1
#    end_janky_frames2=$(janky_frames "com.zhiliaoapp.musically")
#    end_total_frames2=$(total_frames "com.zhiliaoapp.musically")
#    sleep 1
#    
#
## End measuremnets of energy and time from rails:
#  end_energy1_value=$(rails_energy_ch3)
#  end_energy2_value=$(rails_energy_ch4)
#  end_energy3_value=$(rails_energy_ch5)
#  end_time=$(rails_read_time)
#
## Calculate the energy (Joule), time (second) and power (Joule/second):
#  energy1=$(echo "scale=2; ($end_energy1_value - $start_energy1_value) / 100000" | bc)
#  energy2=$(echo "scale=2; ($end_energy2_value - $start_energy2_value) / 100000" | bc)
#  energy3=$(echo "scale=2; ($end_energy3_value - $start_energy3_value) / 100000" | bc)
#  energy_rails=$(awk "BEGIN { printf \"%.2f\", ($energy1 + $energy2 + $energy3) / 3 }")
#  time_rails=$(echo "scale=2; ($end_time - $start_time) / 1000" | bc)
#  power_rails=$(echo "scale=2;  $energy_rails/ $time_rails" | bc)
#  echo "Power_rails: $power_rails" #>> social_media_data.txt
#  echo "Energy_rails: $energy_rails" #>> social_media_data.txt
#  echo "Time_rails: $time_rails"  #>> social_media_data.txt
#
## Calculate the QoSs for twitter:
#  janky=$(echo "scale=2; ($end_janky_frames - $start_janky_frames)" | bc)
#  frames=$(echo "scale=2; ($end_total_frames - $start_total_frames)" | bc)
#  janky_sec=$(echo "scale=2; ($janky / $time_rails)" | bc)
#  frames_sec=$(echo "scale=2; ($frames / $time_rails)" | bc)
#  janky_totalframes=$(echo "scale=3; ($janky / $frames) * 100" | bc)
#  echo "Janky: $janky"  #>> social_media_data.txt
#  echo "Frames: $frames" #>> social_media_data.txt
#  echo "janky_per_second: $janky_sec"  #>> social_media_data.txt
#  echo "frames per second: $frames_sec"  #>> social_media_data.txt
#  echo "Janky_/_totalframes: $janky_totalframes %"  #>> social_media_data.txt
#  
## Calculate the QoSs for Tiktok:
#  janky2=$(echo "scale=2; ($end_janky_frames2 - $start_janky_frames2)" | bc)
#  frames2=$(echo "scale=2; ($end_total_frames2 - $start_total_frames2)" | bc)
#  janky_sec2=$(echo "scale=2; ($janky2 / $time_rails)" | bc)
#  frames_sec2=$(echo "scale=2; ($frames2 / $time_rails)" | bc)
#  janky_totalframes2=$(echo "scale=3; ($janky2 / $frames2) * 100" | bc)
#  echo "Janky2: $janky2"  #>> social_media_data.txt
#  echo "Frames2: $frames2"  #>> social_media_data.txt
#  echo "janky_per_second2: $janky_sec2"  #>> social_media_data.txt
#  echo "frames per second2: $frames_sec2" #>> social_media_data.txt
#  echo "Janky_/_totalframes2: $janky_totalframes2 %"  #>> social_media_data.txt
#
## Close all apps in backgrounds:
#  unlock_screen
#  sleep 2
#  close_all_apps
#  sleep 2
#done
