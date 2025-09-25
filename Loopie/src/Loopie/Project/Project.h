#pragma once

#include <filesystem>

namespace Loopie {
	class Project
	{
	public:
		Project() = default;
		~Project() = default;

		bool Create(std::filesystem::path projectPath, const std::string& name);
		bool Open(std::filesystem::path pathPath);

		const std::string GetProjectName() { return ProjectPath.filename().string(); }
		bool IsEmpty() { return ProjectPath.empty() || !std::filesystem::exists(ProjectPath); }
		
		const std::filesystem::path& GetProjectPath() { return ProjectPath; }
		const std::filesystem::path& GetAssetsPath() { return AssetsPath; }

	private:
		std::filesystem::path ProjectPath;
		std::filesystem::path AssetsPath;
	};
}