#pragma once

#include "Predefs.h"
#include <Windows.h>

#define EXPORT  __declspec(dllexport)
#define External extern "C"


External EXPORT ClassLinkingData* MakeRbxInstance(uintptr_t, HANDLE);
External EXPORT uintptr_t GetDatamodelFromRenderView(uintptr_t, HANDLE);