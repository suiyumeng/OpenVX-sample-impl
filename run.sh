#!/bin/bash
set -x
cd $(dirname $0)
ROOT_DIR=`pwd`  # The script file is here

ARCH=x64 # x32 or x64
BUILD_TYPE=Debug # Debug or Release

system=`uname`
if [ "$system" == "Darwin" ]; then
    echo "This is macOS"
    OS=Mac # Mac or Linux
    export DYLD_LIBRARY_PATH=$ROOT_DIR/install/$OS/$ARCH/$BUILD_TYPE/bin:$DYLD_LIBRARY_PATH
    export OPENVX_INSTALL_DIR=$ROOT_DIR/install/$OS/$ARCH/$BUILD_TYPE
else
    echo "This is Linux"
    OS=Linux  # Mac or Linux
    export LD_LIBRARY_PATH=$ROOT_DIR/install/$OS/$ARCH/$BUILD_TYPE/bin:$LD_LIBRARY_PATH
    export OPENVX_INSTALL_DIR=$ROOT_DIR/install/$OS/$ARCH/$BUILD_TYPE
fi
# if opencl is enabled
export VX_CL_INCLUDE_DIR=$ROOT_DIR/include
export VX_CL_SOURCE_DIR=$ROOT_DIR/kernels/opencl

# enable openvx log level
export VX_ZONE_MASK=0xFFF3F
export VX_ZONE_MASK=0x11023
export VX_ZONE_MASK=0x1182B

# run test
#vx_test -t <case_id>
#$OPENVX_INSTALL_DIR/bin/vx_test -t 0
#$OPENVX_INSTALL_DIR/bin/vx_test $1 $2 $3 $4 $5 $6 $7 $8 $9

#$OPENVX_INSTALL_DIR/bin/openvx_test $1 $2 $3 $4 $5 $6 $7 $8 $9
$OPENVX_INSTALL_DIR/bin/openvx_test -t 0 test/1920x1080_car.png