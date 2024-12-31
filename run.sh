#!/bin/bash
set -x

cd $(dirname $0)
# enable openvx log level
#export VX_ZONE_MASK=0x180F
export VX_ZONE_MASK=0

#./install/Linux/x64/Debug/bin/vx_test -t <case_id>
./install/Linux/x64/Debug/bin/vx_test -t 11