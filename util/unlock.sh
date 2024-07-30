if dumpsys activity activities | grep -i mAwake=false ; then
    input keyevent KEYCODE_POWER    
fi

if dumpsys activity activities | grep isKeyguardShowing=true ; then
    input touchscreen swipe 540 1720 540 1720 2000
fi
