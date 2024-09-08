#include "DllCore.h"

#include "RBX.h"
#include "RbxInstance.h"

// No properties yet :/

#define str4 std::string
#define CStr const char*

EXPORT str4 VERSION = "1.0.0";
EXPORT str4 CREATOR = "Shadow";
EXPORT str4 DISCORD = "";
EXPORT str4 NAME = "RobloxDumperV";

static uintptr_t EncodeRbxInstance(RBX::Instance Obj)
{
	return reinterpret_cast<uintptr_t>(&Obj);
}

static RBX::Instance DecodeRbxInstance(uintptr_t Obj)
{
	return *reinterpret_cast<RBX::Instance*>(Obj);
}

//class InstanceR : public BaseInstance {
//public:
//	InstanceR(RBX::Instance Instance): SavedInstance(Instance) {}
//	vec<BaseInstance*> GetChildren() override
//	{
//		vec<BaseInstance*>  Children;
//
//		for (auto Child: SavedInstance.GetChildren())
//		{
//			InstanceR* NewObject = new InstanceR(Child);
//			Children.emplace_back(NewObject);
//		}
//		return Children;
//	}
//	str4 Name() override
//	{
//		return SavedInstance.Name();
//	}
//	str4 ClassName() override
//	{
//		return SavedInstance.ClassName();
//	}
//	BaseInstance* Parent() override
//	{
//		return new InstanceR(SavedInstance.Parent());
//	}
//	uintptr_t GetInstanceAddress() override
//	{
//		return SavedInstance.InstancePointerInt;
//	}
//private:
//	RBX::Instance SavedInstance;
//};



ClassLinkingData* CreateLinkingDataFromInstance(RBX::Instance instance)
{
	ClassLinkingData* LinkData = new ClassLinkingData("RbxInstance");
	LinkData->AddFunction<std::vector<ClassLinkingData*>()>("GetChildren", [&instance]()
		{
			auto Children = instance.GetChildren();
			vec<ClassLinkingData*> LinkObjectVect;
			for (auto Child : Children)
			{
				LinkObjectVect.emplace_back(CreateLinkingDataFromInstance(Child));
			}

			return LinkObjectVect;
		}
	);
	LinkData->AddFunction<std::string()>("Name", [&instance]()
		{
			return instance.Name();
		}
	);
	LinkData->AddFunction<std::string()>("ClassName", [&instance]()
		{
			return instance.ClassName();
		}
	);
	LinkData->AddFunction<ClassLinkingData*()>("Parent", [&instance]()
		{
			return CreateLinkingDataFromInstance(instance.Parent());
		}
	);
	LinkData->AddProperty<RBX::Types::RbxInt>("InstanceAddress", instance.InstancePointerInt);

	return LinkData;
}

External EXPORT CStr RbxGetName(uintptr_t Obj)
{
	RBX::Instance* Inst = new RBX::Instance(DecodeRbxInstance(Obj));
	std::string SName = Inst->Name();
	char* buffer = new char[SName.length() + 1];
	strcpy_s(buffer, SName.length() + 1, SName.c_str());
	return buffer;
}

External EXPORT CStr RbxGetClassName(uintptr_t Obj)
{
	RBX::Instance* Inst = new RBX::Instance(DecodeRbxInstance(Obj));
	std::string SName = Inst->ClassName();
	char* buffer = new char[SName.length() + 1];
	strcpy_s(buffer, SName.length() + 1, SName.c_str());
	return buffer;
}

External EXPORT uintptr_t RbxGetParent(uintptr_t Obj)
{
	RBX::Instance* Inst = new RBX::Instance(DecodeRbxInstance(Obj));
	return Inst->Parent().InstancePointerInt;
}

External EXPORT uintptr_t RbxGetAddress(uintptr_t Obj)
{
	RBX::Instance* Inst = new RBX::Instance(DecodeRbxInstance(Obj));
	return Inst->InstancePointerInt;
}

External EXPORT vec<uintptr_t>* RbxGetChildren(uintptr_t Obj)
{
	RBX::Instance* Inst = new RBX::Instance(DecodeRbxInstance(Obj));
	static auto Ch = Inst->GetChildren();

	vec<uintptr_t> Children;
	

	for (auto Child: Ch)
	{
		auto Encoded = Child.InstancePointerInt;
		Children.emplace_back(Encoded);
	}
	auto Size = Children.size();
	std::vector<uintptr_t>* RealValue = new std::vector(Children); // Have to make a pointer here or other-side won't decode correctly

	return RealValue;
}

External EXPORT ClassLinkingData* MakeRbxInstance(uintptr_t Address, HANDLE RobloxHandle)
{
	RBX::Instance rbxInstance = RBX::Instance(Address, RobloxHandle);

	return CreateLinkingDataFromInstance(rbxInstance);
}

External EXPORT RBX::Instance* MakeRbxInstanceV2(uintptr_t Address, HANDLE RobloxHandle)
{
	return new RBX::Instance(Address, RobloxHandle);
}

External EXPORT uintptr_t GetDatamodelFromRenderView(uintptr_t Address, HANDLE RobloxHandle)
{
	return RBX::Memory::Read<uintptr_t>(RobloxHandle, RBX::Memory::Read<uintptr_t>(RobloxHandle, Address + 0x118) + 0x198);
}

