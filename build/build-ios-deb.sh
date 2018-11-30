#!/bin/bash

WD=$(dirname $(realpath $0))
LAYOUT=$WD/layout

bash $WD/build-ios-armv7.sh
bash $WD/build-ios-armv7s.sh
bash $WD/build-ios-arm64.sh

rm -rf $LAYOUT/bin $LAYOUT/usr
mkdir -p $LAYOUT/bin $LAYOUT/usr/share/mh/scripts


# Prepare layout
echo "Creating Mach-O Universal Binary"
lipo -create $WD/ios-armv7/mh_cli    $WD/ios-armv7s/mh_cli    $WD/ios-arm64/mh_cli    -output $LAYOUT/bin/mh_cli
lipo -create $WD/ios-armv7/mh_script $WD/ios-armv7s/mh_script $WD/ios-arm64/mh_script -output $LAYOUT/bin/mh_script

cp $WD/../scripts/* $LAYOUT/usr/share/mh/scripts/

echo "Creating debian package"
dpkg-deb -Zgzip -b $WD/layout $WD/me.sskaje.mh_0.2-1_iphoneos-arm.deb