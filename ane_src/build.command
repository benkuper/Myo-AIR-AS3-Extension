#!/bin/bash

# set for debug
# set -xv

TARGET=MyoExtension

#rm -f $TARGET

FLEX_SDK=/Users/benkuper/Documents/SDKs/AIRFlexSDK/flex_sdk_4.6
ADT=$FLEX_SDK/bin/adt
ACOMPC=$FLEX_SDK/bin/acompc

echo $FLEX_SDK
echo $ADT

rm -f $TARGET.swc
rm -f $TARGET.ane
rm -rf build
mkdir -p build

cd /Users/benkuper/Documents/Dev/NativeExtensions/Myo-AIR-AS3-Extension-master/ane_src
"$ACOMPC" -source-path as3_src -include-classes benkuper.nativeExtensions.MyoController -swf-version=14 -output $TARGET.swc


cp -r "native_src/MacOS-x86/MyoExtension/Build/Products/Debug/$TARGET.framework" build
#cp extension-descriptor.xml build
#cp $TARGET.swc build
unzip -o -q $TARGET.swc library.swf
mv library.swf build

"$ADT" -package \
	-target ane MyoController.ane extension-descriptor.xml \
	-swc $TARGET.swc  \
    -platform Windows-x86 \
    -C native_src/Windows-x86/MyoExtension/Debug . \
    -C build library.swf \
    -platform MacOS-x86 \
    -C build . \
    -C /Users/benkuper/Documents/SDKs/myo-mac-sdk-alpha6a/myo.framework \

if [ -f ./$TARGET.ane ];
then
    echo "SUCCESS"
#rm -rf build
else
    echo "FAILED"
fi

