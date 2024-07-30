bg() {
	"$@" >/dev/null 2>/dev/null < /dev/null &
}

if test -z $(lsof -t /data/ryan/screen.dex) ; then
	echo starting
	bg env CLASSPATH=/data/ryan/screen.dex app_process64 /data/ryan connectivity.Screen &
fi
