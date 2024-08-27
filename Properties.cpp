#include "Properties.h"
#include "RBX.h"
#include "RbxMem.hpp"
#include "SmartDecoder.hpp"

namespace RBX {

	PropertyCore::PropertyCore(uintptr_t PropPointer, HANDLE RobloxHandle) :
		PropertyPointer(PropPointer),
		InheritedRobloxHandle(RobloxHandle),
		TTypeObjectRef(new TTypeObject(this, RobloxHandle))
	{
	}
	TTypeObject PropertyCore::GetTType() const
	{
		return *TTypeObjectRef;
	}
	std::string PropertyCore::Name()
	{
		return RBX::SmartDecoder::DecodeString(InheritedRobloxHandle, PropertyPointer + 0x8);
	}
	std::string PropertyCore::Category()
	{
		return RBX::SmartDecoder::DecodeString(InheritedRobloxHandle, PropertyPointer + 0x28);
	}

}
