dumpsys activity recents | grep -oE 'taskId=[0-9]+' | cut -d = -f 2 | xargs -n 1 am stack remove
