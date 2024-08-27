#include "DllCore.h"

#include "RBX.h"
#include "RbxInstance.h"
#include "ClassDescriptor.h"
#include "Properties.h"
#include "TTypes.h"
// No properties yet :/


EXPORT ClassLinkingData MakeRbxInstance(uintptr_t Address, HANDLE RobloxHandle)
{
	RBX::Instance rbxInstance = RBX::Instance(Address, RobloxHandle);

	ClassLinkingData LinkData = ClassLinkingData("RbxInstance");
	LinkData.AddFunction("GetChildren", [&rbxInstance]()
	                     {
		                     return rbxInstance.GetChildren();
	                     }
	);
	LinkData.AddFunction("Name", [&rbxInstance]()
	                     {
		                     return rbxInstance.Name();
	                     }
	);
	LinkData.AddFunction("ClassName", [&rbxInstance]()
	                     {
		                     return rbxInstance.ClassName();
	                     }
	);
	LinkData.AddFunction("Parent", [&rbxInstance]()
	                     {
		                     return rbxInstance.Parent();
	                     }
	);
	LinkData.AddProperty("InstanceAddress", reinterpret_cast<uintptr_t*>(&rbxInstance.InstancePointerInt));

	return LinkData;
}
