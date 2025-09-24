#pragma once

#include <filesystem>
#include <string>

namespace Loopie {
	class DirectoryManager {
	public:
		static const std::filesystem::path CreateFolder(const std::filesystem::path& path, const std::string& folderName);
		static const std::filesystem::path CreateFile(const std::filesystem::path& path, const std::string& fileName, const std::string& fileExtension);

		static bool Contains(const std::filesystem::path& path, const std::string& nameToFind);
		static bool Contains(const std::filesystem::path& path);
	};

}