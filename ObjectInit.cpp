#include "ObjectInit.h"

#include "TypeRegisteryTTypes.h"


#include <iostream>

namespace RBX {
	namespace Objects {
		void Init() {
			RBX::TypeRegistery::InitTypes();
			std::cout << "TTypes Loaded." << std::endl;
		}
	}
}