#!/bin/bash

DEP_DIR=$(dirname $(dirname $(realpath $0)))/deps/duktape

WORK_DIR=duktape-tmp-checkout

VERSION=2.3.0

echo "Downloading https://github.com/svaarala/duktape/releases/download/v${VERSION}/duktape-${VERSION}.tar.xz ..."
curl --silent -L https://github.com/svaarala/duktape/releases/download/v${VERSION}/duktape-${VERSION}.tar.xz | tar x

echo "Building..."
cd duktape-${VERSION}
rm -rf src-custom

python tools/configure.py \
      --source-directory src-input \
      --output-directory src-custom \
      --config-metadata config \
      -DDUK_USE_FASTINT

mkdir -p $DEP_DIR
cp src-custom/* $DEP_DIR
cp -r extras $DEP_DIR
cp -r licenses $DEP_DIR
cp LICENSE.txt $DEP_DIR

echo "Cleaning..."
cd ..
rm -rf duktape-${VERSION} v${VERSION}.tar.gz
