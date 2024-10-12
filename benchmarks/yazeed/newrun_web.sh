#!/bin/bash
# Do the following after a reboot:
#    adb root (to enable root shell)
# in adb shell
#    mkdir /mnt/ryan
#    mount -t tmpfs none /mnt/ryan


# This script will run Android Benchmarks multiple times, and after each time, it will close all the opened apps and pages.

# Also, it will measure energy from rails, and measure QoS.

OUTPUT_FILE=output/output_$(date '+%y-%m-%d_%H-%M')_web.txt
#OUTPUT_FILE7=output/output_$(date '+%y-%m-%d_%H-%M')_youtube.txt


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

# energy value from CH0= S10M_VDD_TPU
rails_energy_ch0() {
 ./adb shell cat /sys/bus/iio/devices/iio:device0/energy_value | grep 'CH0' | awk '{print $2}'
}

# energy value from CH1= VSYS_PWR_DISPLAY
rails_energy_ch1() {
 ./adb shell cat /sys/bus/iio/devices/iio:device0/energy_value | grep 'CH1' | awk '{print $2}'
}

# energy value from CH2= L15M_VDD_SLC_M
rails_energy_ch2() {
 ./adb shell cat /sys/bus/iio/devices/iio:device0/energy_value | grep 'CH2' | awk '{print $2}'
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
# energy value from CH6= S5M_VDD_INT
rails_energy_ch6() {
 ./adb shell cat /sys/bus/iio/devices/iio:device0/energy_value | grep 'CH6' | awk '{print $2}'
}

# energy value from CH7= S1M_VDD_MIF
rails_energy_ch7() {
 ./adb shell cat /sys/bus/iio/devices/iio:device0/energy_value | grep 'CH7' | awk '{print $2}'
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



####################### Web benchmark  #############
for i in {1..1}; do

# Lunch the app:
   ./adb shell am start -n org.lineageos.jelly/.MainActivity
   sleep 1

# Start measuremnets of energy and time from rails:
    start_energy0_value=$(rails_energy_ch0)
    start_energy1_value=$(rails_energy_ch1)
    start_energy2_value=$(rails_energy_ch2)
    start_energy3_value=$(rails_energy_ch3)
    start_energy4_value=$(rails_energy_ch4)
    start_energy5_value=$(rails_energy_ch5)
    start_energy6_value=$(rails_energy_ch6)
    start_energy7_value=$(rails_energy_ch7)
    start_time=$(rails_read_time)

# Start QoS measuremnets:
    start_janky_frames=$(janky_frames "org.lineageos.jelly")
    start_total_frames=$(total_frames "org.lineageos.jelly")
    sleep 1

# Run the benchmark:
   run_benchmark web-task.cpp >> $OUTPUT_FILE

# End QoS measuremnets:
   end_janky_frames=$(janky_frames "org.lineageos.jelly")
   end_total_frames=$(total_frames "org.lineageos.jelly")

# End measuremnets of energy and time from rails:
    end_energy0_value=$(rails_energy_ch0)
    end_energy1_value=$(rails_energy_ch1)
    end_energy2_value=$(rails_energy_ch2)
    end_energy3_value=$(rails_energy_ch3)
    end_energy4_value=$(rails_energy_ch4)
    end_energy5_value=$(rails_energy_ch5)
    end_energy6_value=$(rails_energy_ch6)
    end_energy7_value=$(rails_energy_ch7)
    end_time=$(rails_read_time)

# Calculate the energy (Joule), time (second) and power (Joule/second):
  energy0=$(echo "scale=2; ($end_energy0_value - $start_energy0_value)" | bc)
  energy1=$(echo "scale=2; ($end_energy1_value - $start_energy1_value)" | bc)
  energy2=$(echo "scale=2; ($end_energy2_value - $start_energy2_value)" | bc)
  energy3=$(echo "scale=2; ($end_energy3_value - $start_energy3_value)" | bc)
  energy4=$(echo "scale=2; ($end_energy4_value - $start_energy4_value)" | bc)
  energy5=$(echo "scale=2; ($end_energy5_value - $start_energy5_value)" | bc)
  energy6=$(echo "scale=2; ($end_energy6_value - $start_energy6_value)" | bc)
  energy7=$(echo "scale=2; ($end_energy7_value - $start_energy7_value)" | bc)
  energy_rails_CPUs_only=$(awk "BEGIN { printf \"%.2f\", ($energy3 + $energy4 + $energy5) / 1000000 }")
  energy_rails_all=$(awk "BEGIN { printf \"%.2f\", ($energy0 + $energy1 + $energy2+ $energy3 + $energy4 + $energy5 + $energy6 + $energy7) / 1000000 }")
  
  time_rails=$(echo "scale=2; ($end_time - $start_time) / 1000" | bc)
  power_rails_CPUs_only=$(echo "scale=2;  $energy_rails_CPUs_only/ $time_rails" | bc)
  power_rails_all=$(echo "scale=2;  $energy_rails_all/ $time_rails" | bc)
  
  echo "Power_rails_CPUs_only: $power_rails_CPUs_only" >> $OUTPUT_FILE
  echo "Power_rails_all_components: $power_rails_all" >> $OUTPUT_FILE
  echo "Energy_rails_CPUs_only: $energy_rails_CPUs_only" >> $OUTPUT_FILE
  echo "Energy_rails_all_components: $energy_rails_all" >> $OUTPUT_FILE
  echo "Time_rails: $time_rails"  >> $OUTPUT_FILE

# Calculate the QoSs:
  janky=$(echo "scale=2; ($end_janky_frames - $start_janky_frames)" | bc)
  frames=$(echo "scale=2; ($end_total_frames - $start_total_frames)" | bc)
  janky_sec=$(echo "scale=2; ($janky / $time_rails)" | bc)
  frames_sec=$(echo "scale=2; ($frames / $time_rails)" | bc)
  janky_totalframes=$(echo "scale=3; ($janky / $frames) * 100" | bc)
  echo "Janky: $janky"  >> $OUTPUT_FILE
  echo "Frames: $frames"  >> $OUTPUT_FILE
  echo "janky_per_second: $janky_sec"  >> $OUTPUT_FILE
  echo "frames per second: $frames_sec"  >> $OUTPUT_FILE
  echo "Janky_/_totalframes: $janky_totalframes %"  >> $OUTPUT_FILE

# Close all apps in backgrounds:
  unlock_screen
  sleep 2
  close_all_apps
  sleep 2
done

