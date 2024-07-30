if test -n "$(lsof -t /data/ryan/bat.lck)" ; then
    echo already running
    exit 1
fi
exec 5<>/data/ryan/bat.lck


while :; do
    BAT=$(( 100 * $(cat /sys/class/power_supply/battery/charge_counter) / $(cat /sys/class/power_supply/battery/charge_full_design) ))
    
    if [ $BAT -lt 30 ] ; then
        killall -9 benchmark
        echo 0 > /sys/class/power_supply/usb/device/usb_limit_sink_enable
    fi
    sleep 1
done
