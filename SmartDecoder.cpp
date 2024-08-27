#include "SmartDecoder.hpp"
#define MemBasicInfo MEMORY_BASIC_INFORMATION
namespace {
	
	bool IsMemoryAddressValid(HANDLE Handle, uintptr_t Address) {
		MemBasicInfo Info = RBX::MemorySecurity::QueryMemoryAddress(Handle, Address);

		return (Info.State == MEM_COMMIT);
	}

}

namespace RBX {
	namespace SmartDecoder {
		std::string DecodeString(HANDLE Handle, uintptr_t Address) { // rewrote this like 3 times now
			uintptr_t Addy = Address;
			while (true)
			{
				
				std::string PTRDecoded = std::string(DecodePointer<RBX::Types::InstanceName>(Handle, Addy).data());
				uintptr_t StringUint = PointerLoop(Handle, Addy);
				std::string NextIndex = std::string(DecodePointer<RBX::Types::InstanceName>(Handle, StringUint).data());
				bool CanDecode = IsMemoryAddressValid(Handle, StringUint);

				/*std::cout << "Smart Decoder Log: Next index::  " << NextIndex << std::endl;
				std::cout << "Smart Decoder Log: Next Address:: " << StringUint << std::endl;
				std::cout << "Smart Decoder Log: Is Address Valid:: " << IsMemoryAddressValid(Handle, StringUint) << std::endl;
				std::cout << "Smart Decoder Log: " << PTRDecoded << std::endl;*/
				if (CanDecode == false) {
					return PTRDecoded;
				}
				else {
					Addy = StringUint;
				}
			}
		}
	}
}