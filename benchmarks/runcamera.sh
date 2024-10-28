OUTPUT_FILE=output/sleepy_run/output_$(date '+%y-%m-%d_%H-%M')_camera.txt

echo "**** run starting $(date)" >> $OUTPUT_FILE


for i in $(seq 5); do
    remote_adb shell sh /data/ryan/unlock.sh
    remote_adb shell sh /data/ryan/prevent_sleep.sh

    sh runone.sh apps/camera-task.cpp | tee -a $OUTPUT_FILE
    #sh runone.sh apps/background-music.cpp | tee -a $OUTPUT_FILE
    #sh runone.sh apps/games.cpp | tee -a $OUTPUT_FILE
    #sh runone.sh apps/idle.cpp | tee -a $OUTPUT_FILE
    #sh runone.sh apps/social-media.cpp | tee -a $OUTPUT_FILE
    
    #sh runone.sh video-call.cpp | tee -a output
    #video playing?


    remote_adb shell sh /data/ryan/allow_sleep.sh
    remote_adb shell sh /data/ryan/clear.sh

    sleep 10 
done


echo "done"

