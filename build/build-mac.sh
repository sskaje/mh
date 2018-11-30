#!/bin/bash

CWD=$(pwd)

BUILD_ROOT=$(dirname $(realpath $0))
BUILD_ROOT=$BUILD_ROOT/mac

rm -rf "$BUILD_ROOT"

mkdir $BUILD_ROOT

cd $BUILD_ROOT

cmake ../../

make -j 8

cd $CWD


