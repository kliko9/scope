tizen build-native -a arm
tizen package -t tpk -s kalim -- ./Debug
sdb push ./Debug/org.tizen.* /tmp
sdb root on
sdb shell tpk-backend -i /tmp/org.tizen.scope* --preload
