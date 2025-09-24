#pragma once

#include <filesystem>

namespace Loopie {
	struct Project
	{
		std::filesystem::path ProjectPath;
		std::filesystem::path AssetsPath;

		const std::string GetProjectName() {
			return ProjectPath.filename().string();
		}

		bool IsEmpty() {
			return std::filesystem::exists(ProjectPath);
		}
	};
}