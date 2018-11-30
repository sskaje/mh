#!/bin/bash

CWD=$(pwd)

BUILD_ROOT=$(dirname $(realpath $0))
BUILD_ROOT=$BUILD_ROOT/ios-arm64

rm -rf "$BUILD_ROOT"
mkdir $BUILD_ROOT

SDK_ROOT=$(realpath $BUILD_ROOT/../../SDKs)

cd $BUILD_ROOT

cmake -D BUILD_ARCH=arm64 -D LOCAL_SDK_ROOT="${SDK_ROOT}" -D IOS_JAILBREAK_ELECTRA=1 ../../

make -j 8

cd $CWD

codesign -s - --entitlements $BUILD_ROOT/../../ent.xml -f $BUILD_ROOT/tests/test-*
codesign -s - --entitlements $BUILD_ROOT/../../ent.xml -f $BUILD_ROOT/mh_cli

