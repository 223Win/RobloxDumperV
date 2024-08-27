#include "TypeRegistery.cpp"
#include "RBX.h"
#include "RbxMem.hpp"
#include "Getter.h"


namespace RBX {
	namespace Getter {
		auto GetProperty(uintptr_t GetterAddress, auto Base)
		{
			std::cout << "TYPE BASE: " << typeid(Base).name() << std::endl;
		}
	}
}