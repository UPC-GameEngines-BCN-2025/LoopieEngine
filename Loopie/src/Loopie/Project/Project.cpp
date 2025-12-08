#include "Project.h"

#include "Loopie/Core/Application.h"
#include "Loopie/Files/DirectoryManager.h"

namespace Loopie {
	bool Project::Create(const std::filesystem::path& projectPath, const std::string& name) {

		if (projectPath.empty() || DirectoryManager::Contains(projectPath / name)) {
			return false;
		}

		m_projectPath = DirectoryManager::CreateFolder(projectPath, name);
		m_congifPath = DirectoryManager::CreateFile(m_projectPath, "project", ".config");
		CreateDefaultPaths();

		/// Maybe some config Files???? Once Scene Exists a default One
		Application::GetInstance().m_notifier.Notify(EngineNotification::OnProjectChange);
		return true;
	}

	bool Project::Open(const std::filesystem::path& projectPath) {
		if (projectPath.empty()|| !DirectoryManager::Contains(projectPath) || !DirectoryManager::Contains(projectPath / "project.config")) {
			return false;
		}

		m_projectPath = projectPath;
		m_congifPath = projectPath / "project.config";
		CreateDefaultPaths();

		/// Maybe read/save config Files????

		Application::GetInstance().m_notifier.Notify(EngineNotification::OnProjectChange);
		return true;
	}
	const void Project::CreateDefaultPaths()
	{
		m_assetsPath = DirectoryManager::CreateFolder(m_projectPath, "Assets");

		m_cachePath = DirectoryManager::CreateFolder(m_projectPath, "Library");
		DirectoryManager::CreateFolder(m_cachePath, "Textures");
		DirectoryManager::CreateFolder(m_cachePath, "Meshes");
		DirectoryManager::CreateFolder(m_cachePath, "Materials");
		DirectoryManager::CreateFolder(m_cachePath, "Shaders");

	}
}