#!/bin/bash
set -x
cd $(dirname $0)
ROOT_DIR=`pwd`  # The script file is here

system=`uname`
if [ "$system" == "Darwin" ]; then
    echo "This is macOS"
    export DYLD_LIBRARY_PATH=$ROOT_DIR/install/Mac/x64/Debug/bin:$DYLD_LIBRARY_PATH
else
    echo "This is Linux"
    export LD_LIBRARY_PATH=$ROOT_DIR/install/Linux/x64/Debug/bin:$LD_LIBRARY_PATH
fi

# enable openvx log level
export VX_ZONE_MASK=0x180F
#export VX_ZONE_MASK=0

#./install/Linux/x64/Debug/bin/vx_test -t <case_id>
./install/Mac/x64/Debug//bin/vx_test -t 8