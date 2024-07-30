set -eux
/opt/android-sdk/ndk/26.1.10909125/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android33-clang++ -static-libstdc++ test-cores.cpp -o test-cores
remote_adb push test-cores /data/ryan
