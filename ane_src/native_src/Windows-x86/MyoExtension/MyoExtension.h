#include <Windows.h>
#include "FlashRuntimeExtensions.h"


extern "C"
{
	__declspec(dllexport) void MyoExtInitializer(void** extDataToSet, FREContextInitializer* ctxInitializerToSet, FREContextFinalizer* ctxFinalizerToSet);
	__declspec(dllexport) void MyoExtFinalizer(void* extData);

}