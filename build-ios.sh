#!/bin/bash

mkdir build/ios-armv7
bash build/build-ios-armv7.sh

mkdir build/ios-armv7s
bash build/build-ios-armv7s.sh

mkdir build/ios-arm64
bash build/build-ios-arm64.sh

lipo -create build/ios-armv7/mh_cli build/ios-armv7s/mh_cli build/ios-arm64/mh_cli -output build/mh_cli
