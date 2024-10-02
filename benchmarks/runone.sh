first() {
    echo $1
}

: ${NDK:=$(first /opt/android-sdk/ndk/*)}
#CLANG=$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android33-clang++
CLANG=$NDK/toolchains/llvm/prebuilt/darwin-x86_64/bin/aarch64-linux-android33-clang++

if ! test -d $NDK ; then
    echo could not find NDK, set the NDK environment variable
    exit 1
fi

set -eux

$CLANG -I . $1 -o benchmark -static-libstdc++
remote_adb push benchmark /data/ryan

if ! test -v RAILS ; then
    RAILS="RAILS0=BUCK2M,BUCK3M,BUCK4M,BUCK6M,LDO2M,LDO11M,LDO12M RAILS1=BUCK1S,BUCK2S,BUCK4S,BUCK5S,BUCK8S,BUCK10S,LDO1S"
fi

if ! test -v TIME ; then
    TIME=TIME=120
fi

echo started $(date)
remote_adb shell sh /data/ryan/clear.sh "&&" env $RAILS $TIME /data/ryan/benchmark "$@"
echo stopped $(date)
