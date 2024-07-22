#pragma once

namespace HookEngine {
  class HookEngine;

  HookEngine* instance();
}

#ifdef _WIN64
  #include "private/x64/HookEngine64.h"
#else
  #include <HookEngine/private/x86/HookEngine86.h>
#endif

#include "Hook.h"
#include "AutoLink.h"
