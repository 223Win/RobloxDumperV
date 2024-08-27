#pragma once

#include "Predefs.h"
#include <Windows.h>

#define EXPORT  __declspec(dllexport)
#define External extern "C++"


EXPORT ClassLinkingData MakeRbxInstance(uintptr_t,HANDLE);
