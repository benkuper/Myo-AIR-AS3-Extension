## MyoController

AIR Native Extension for Myo

ane_src contains c++ and as3 sources for the ANE, swcCompiler.bat is a script to automate the creation of the .ane
as3_demo is a FlashDevelop project to show the library usage. 


### Installation and troubleshooting

 - Windows

/!\ You may need to add myo32.dll from the SDK in a path that the AIR SDK can find ! 

You can do this either by copying the 2 files in the [FlashDevelopInstall]/tools/flexsdk/bin folder or by adding the path of the folder containing these 2 dlls in the PATH environment variable.

 - Mac
 
For OSX users, you may need to copy myo.framework from the Myo SDK into /Library/Frameworks, otherwise you may get the error "The extension context does not have a method with the name init"
