#include "Roblox.h"
#include "RBX.h"
#include <format>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <shlobj.h>
#include <combaseapi.h>



using namespace std::filesystem;
using namespace std;

static std::optional<std::string> ParseDir(const std::string& NameToGet, std::filesystem::directory_iterator Iteration) {
	for (const auto& entry : Iteration) {
		if (entry.is_directory()) {
			auto Result = ParseDir(NameToGet, std::filesystem::directory_iterator(entry.path()));
			if (Result.has_value()) {
				return Result;
			}
		}
		else if (entry.is_regular_file()) {
			if (entry.path().filename() == NameToGet) {
				return entry.path().string();
			}
		}
	}
	return std::nullopt;
}

namespace RBX {
	namespace Roblox {
		std::string GetGG() {
			PWSTR path = nullptr;
			if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &path))) {
				std::wstring ws(path);
				std::string localAppDataPath(ws.begin(), ws.end());
				CoTaskMemFree(path);
				return localAppDataPath;
			}
			else {
				throw std::runtime_error("Failed to get Local AppData folder path.");
			}
		}

		void GetRobloxPath()
		{
			std::string VersionsPath = GetGG() + "\\Roblox\\Versions";
			auto Result = ParseDir("RobloxPlayerBeta", std::filesystem::directory_iterator(VersionsPath));
			if (Result.has_value()) {
				std::cout << Result.value() << std::endl;
			}
			else {
				std::cout << "RobloxPlayerBeta not found." << std::endl;
			}
		}
	}
}
