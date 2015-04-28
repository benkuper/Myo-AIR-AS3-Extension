call acompc -source-path as3_src -include-classes benkuper.nativeExtensions.MyoController -swf-version=14 -output MyoController.swc

unzip -o MyoController.swc -x catalog.xml

call adt -package -target ane MyoController.ane extension-descriptor_win_mac.xml -swc MyoController.swc -platform Windows-x86 -C native_src\Windows-x86 MyoExtension.dll -C ./ library.swf -platform MacOS-x86  -C native_src\MacOS-x86 . -C ./ library.swf
del library.swf
del MyoController.swc

echo "ok"
pause