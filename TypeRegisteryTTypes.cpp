
#include "TypeRegistery.cpp"

namespace RBX {
	namespace TypeRegistery {


		void InitTypes() {
			TypeRegistry::RegisterType<int>("int");
			TypeRegistry::RegisterType<float>("float");
			TypeRegistry::RegisterType<double>("double");
			TypeRegistry::RegisterType<bool>("bool");
			TypeRegistry::RegisterType<int64_t>("int64");
		}
	}
}