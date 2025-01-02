#!/bin/bash
set -x
cd $(dirname $0)

#git submodule update --init --recursive
rm -rf build
system=`uname`
if [ "$system" == "Darwin" ]; then
    echo "This is macOS"
    python3 Build.py --os=Mac --arch=32 --conf=Debug --opencl
else
    echo "This is Linux"
    python3 Build.py --os=Linux --arch=32 --conf=Debug --opencl
fi

#make TARGET_FAMILY=arm64 HOST_FAMILY=arm64