#!/bin/bash
# This script was done because vcpkg's dbus has incompatibility with Linux
# You want to use your system's dbus, but unfortunately vpckg overrides 
# that option no matter what I tried, thus I have to export the library's 
# env variable manually like below. 
# Would love to see if another Linux worked with this workflow.
export LD_LIBRARY_PATH=/usr/lib64:/lib64:$LD_LIBRARY_PATH
cd "$(dirname "$0")/build/Release/LoopieEditor"
./LoopieEditor "$@"
