#include "Loopie/Files/DirectoryManager.h"
#include "DirectoryManager.h"

#include <fstream>

namespace Loopie {
    const std::filesystem::path DirectoryManager::CreateFolder(const std::filesystem::path& path, const std::string& folderName)
    {
        std::filesystem::path directorypath = path / folderName;
        if (Contains(directorypath)) {
            return directorypath;
        }

        if (std::filesystem::create_directories(directorypath)) {
            return directorypath;
        }
        return std::filesystem::path();
    }

    const std::filesystem::path DirectoryManager::CreateFile(const std::filesystem::path& path, const std::string& fileName, const std::string& fileExtension)
    {
        std::filesystem::path filePath = path / (fileName + fileExtension);
        if (Contains(filePath)) {
            return filePath;
        }

        std::ofstream ofs(filePath);
        if (ofs.good()) {
            return  filePath;
        }
        return std::filesystem::path();
    }

    bool DirectoryManager::Contains(const std::filesystem::path& path)
    {
        return std::filesystem::exists(path);
    }

    bool DirectoryManager::Contains(const std::filesystem::path& path, const std::string& nameToFind)
    {
        return Contains(path/nameToFind);
    }
    
}