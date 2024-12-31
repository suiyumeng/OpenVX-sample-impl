#!/bin/bash
set -x
cd $(dirname $0)


#linux os
python3 Build.py --os=Linux --arch=64 --conf=Debug
