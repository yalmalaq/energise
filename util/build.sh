set -eux 
/usr/lib/jvm/java-11-openjdk/bin/javac \
	-cp framework.jar \
	Screen.java
/opt/android-sdk/build-tools/34.0.0/d8 --classpath framework.jar  *.class
scp classes.dex corr:
ssh corr adb push classes.dex /data/ryan/screen.dex
#adb push classes.dex /data/ryan/wakeup.dex
