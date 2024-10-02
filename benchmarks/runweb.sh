OUTPUT_FILE=output/base/output_$(date '+%y-%m-%d_%H-%M')_web.txt

echo "**** run starting $(date)" >> $OUTPUT_FILE

remote_adb shell sh /data/ryan/unlock.sh
remote_adb shell sh /data/ryan/prevent_sleep.sh

for i in $(seq 5); do
    sh runone.sh apps/web-task.cpp | tee -a $OUTPUT_FILE
    #sh runone.sh apps/background-music.cpp | tee -a $OUTPUT_FILE
    #sh runone.sh apps/games.cpp | tee -a $OUTPUT_FILE
    #sh runone.sh apps/idle.cpp | tee -a $OUTPUT_FILE
    #sh runone.sh apps/social-media.cpp | tee -a $OUTPUT_FILE
    
    #sh runone.sh video-call.cpp | tee -a output
    #video playing?
done

remote_adb shell sh /data/ryan/allow_sleep.sh
remote_adb shell sh /data/ryan/clear.sh

echo "done"


