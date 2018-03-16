#!/bin/bash

CWD=$(pwd)

BUILD_ROOT=$(dirname $(realpath $0))
BUILD_ROOT=$BUILD_ROOT/ios-arm64-electra
SDK_ROOT=$(realpath $BUILD_ROOT/../../SDKs)

rm -rf "$BUILD_ROOT"

mkdir $BUILD_ROOT

cd $BUILD_ROOT

cmake -D IOS_JAILBREAK_ELECTRA=1 -D BUILD_ARCH=arm64 -D LOCAL_SDK_ROOT="${SDK_ROOT}"  ../../

make -j 8

cd $CWD

#codesign -s - --entitlements $BUILD_ROOT/../../ent.xml -f $BUILD_ROOT/tests/test-*
#codesign -s - --entitlements $BUILD_ROOT/../../ent.xml -f $BUILD_ROOT/mh_cli

#$BUILD_ROOT/../../tools/ldid2 -S$BUILD_ROOT/../../ent.xml $BUILD_ROOT/mh_cli

ARCH=arm64 $BUILD_ROOT/../../tools/jtool --sign --ent $BUILD_ROOT/../../ent.xml --inplace $BUILD_ROOT/mh_cli
