#!/bin/bash

SDKS=( iPhoneOS10.2.sdk.zip iPhoneOS10.3.sdk.zip iPhoneOS11.0.sdk.zip  iPhoneOS11.1.sdk.zip  iPhoneOS11.2.sdk.zip  )


SDK_ROOT=$(dirname $(realpath $0))
SDK_ROOT=$(realpath $SDK_ROOT/../SDKs)


for i in ${SDKS[@]}; do
    echo Download $i
    wget --user "sskaje.me" --password "sskaje.me" -q https://dl.sskaje.me/ios-sdks/$i -O $i;
    unzip -d $SDK_ROOT $i;
    rm -f $i;
done