#pragma once
#include <string>

namespace ExUtils {
	namespace Encoding {
		std::string DecodeString(std::string EncodedVal);
		std::string EncodeString(std::string BaseString);
	}
}