#!/bin/bash
tizen clean
tizen build-native -a arm
#tizen build-native
tizen package -t tpk -s ABS -- ./Debug
sdb push ./Debug/org.tizen.* /tmp
sdb root on
sdb shell tpk-backend -i /tmp/org.tizen.scope* --preload
