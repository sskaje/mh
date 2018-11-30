#!/bin/bash

CWD=$(pwd)

BUILD_ROOT=$(dirname $(realpath $0))
BUILD_ROOT=$BUILD_ROOT/ios-armv7s

rm -rf "$BUILD_ROOT"
mkdir $BUILD_ROOT

SDK_ROOT=$(realpath $BUILD_ROOT/../../SDKs)

cd $BUILD_ROOT

cmake -D BUILD_ARCH=armv7s -D LOCAL_SDK_ROOT="${SDK_ROOT}" ../../

make -j 8

cd $CWD

codesign -s - --entitlements $BUILD_ROOT/../../ent.xml -f $BUILD_ROOT/tests/test-*
codesign -s - --entitlements $BUILD_ROOT/../../ent.xml -f $BUILD_ROOT/mh_cli
