#!/bin/bash
export ANDROID_NDK=/home/cli/code/android-openssl/android-ndk-r21
rm -rf build
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
		-DANDROID_ABI="armeabi-v7a" \
		-DANDROID_NDK=$ANDROID_NDK \
		-DANDROID_PLATFORM=android-22 \
		-DANDROID_ARM_NEON=TRUE  \
		-DANDROID_STL=c++_shared \
		..
make -j8 && make install
cd ..
