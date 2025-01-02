#!/bin/bash
set -x
cd $(dirname $0)
ROOT_DIR=`pwd`  # The script file is here

system=`uname`
if [ "$system" == "Darwin" ]; then
    echo "This is macOS"
    export DYLD_LIBRARY_PATH=$ROOT_DIR/install/Mac/x32/Debug/bin:$DYLD_LIBRARY_PATH
    export OPENVX_INSTALL_DIR=$ROOT_DIR/install/Mac/x32/Debug
else
    echo "This is Linux"
    export LD_LIBRARY_PATH=$ROOT_DIR/install/Linux/x32/Debug/bin:$LD_LIBRARY_PATH
    export OPENVX_INSTALL_DIR=$ROOT_DIR/install/Linux/x32/Debug
fi
# if opencl is enabled
export VX_CL_INCLUDE_DIR=$ROOT_DIR/include
export VX_CL_SOURCE_DIR=$ROOT_DIR/kernels/opencl

# enable openvx log level
export VX_ZONE_MASK=0xFFF3F
export VX_ZONE_MASK=0xF

# run test
#vx_test -t <case_id>
$OPENVX_INSTALL_DIR/bin/vx_test -t $1