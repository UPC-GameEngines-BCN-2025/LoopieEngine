#pragma once

#include <vector>
#include <filesystem>

namespace Loopie {

    enum class DialogResultType {
        SUCCESS,
        CANCEL,
        ERROR
    };

    struct FileFilter {
        /// <summary> Example: "Image Files" </summary>
        std::string Name;
        /// <summary> Example: "jpg,png,jpeg" </summary>
        std::string Extension;
    };

    struct DialogResult {

        DialogResultType Status = DialogResultType::CANCEL;
        std::vector<std::filesystem::path> Paths;
        std::string Error;
    };

	class FileDialog {
    public:
        static DialogResult SelectFile(const std::vector<FileFilter>& filters, const std::filesystem::path& defaultPath = "");
        static DialogResult SelectFiles(const std::vector<FileFilter>& filters, const std::filesystem::path& defaultPath = "");

        static DialogResult SelectFolder(const std::filesystem::path& defaultPath = "");
        static DialogResult SelectFolders(const std::filesystem::path& defaultPath = "");
	};
}