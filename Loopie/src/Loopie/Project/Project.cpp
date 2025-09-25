#include "Project.h"

#include "Loopie/Files/DirectoryManager.h"

namespace Loopie {
	bool Project::Create(std::filesystem::path projectPath, const std::string& name) {

		if (DirectoryManager::Contains(projectPath / name)) {
			return false;
		}

		ProjectPath = DirectoryManager::CreateFolder(projectPath, name);
		AssetsPath = DirectoryManager::CreateFolder(ProjectPath, "Assets");

		/// Maybe some config Files???? Once Scene Exists a default One
		return true;
	}

	bool Project::Open(std::filesystem::path projectPath) {
		if (!DirectoryManager::Contains(projectPath)) {
			return false;
		}

		ProjectPath = projectPath;
		AssetsPath = DirectoryManager::CreateFolder(ProjectPath, "Assets");

		/// Maybe read/save config Files????
		return true;
	}
}