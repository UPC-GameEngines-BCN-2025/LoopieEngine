#include "FileDialog.h"
#include <nfd.h>

namespace Loopie {
	DialogResult FileDialog::SelectFile(const std::vector<FileFilter>& filters, const std::filesystem::path& defaultPath)
	{
        NFD_Init();

        nfdchar_t* outPath;
        DialogResult result;

        std::vector<nfdfilteritem_t> filterItems;
        filterItems.reserve(filters.size());

        for (const FileFilter& filter : filters) {
            filterItems.push_back(nfdfilteritem_t{ filter.Name.c_str(),filter.Extension.c_str() });
        }

        nfdresult_t nfdResult = NFD_OpenDialog(&outPath, filterItems.data(), (nfdfiltersize_t)filterItems.size(), defaultPath.empty() ? NULL : defaultPath.string().c_str());
        if (nfdResult == NFD_OKAY) {

            result.Status = DialogResultType::SUCCESS;
            result.Paths.reserve(1);
            result.Paths.push_back(outPath);
            NFD_FreePath(outPath);
        }
        else if (nfdResult == NFD_ERROR) {
            result.Status = DialogResultType::ERROR;
            result.Error = NFD_GetError();
        }
        NFD_Quit();

        return result;
	}

	DialogResult FileDialog::SelectFiles(const std::vector<FileFilter>& filters, const std::filesystem::path& defaultPath)
	{
        NFD_Init();

        const nfdpathset_t* pathSet;
        DialogResult result;

        std::vector<nfdfilteritem_t> filterItems;
        filterItems.reserve(filters.size());

        for (const FileFilter& filter : filters) {
            filterItems.push_back(nfdfilteritem_t{ filter.Name.c_str(),filter.Extension.c_str() });
        }

        nfdresult_t nfdResult = NFD_OpenDialogMultiple(&pathSet, filterItems.data(), (nfdfiltersize_t)filterItems.size(), defaultPath.empty() ? NULL : defaultPath.string().c_str());
        if (nfdResult == NFD_OKAY) {

            result.Status = DialogResultType::SUCCESS;

            nfdpathsetsize_t count;
            NFD_PathSet_GetCount(pathSet,&count);

            result.Paths.reserve(count);
            
            nfdpathsetsize_t i;
            for (i = 0; i < count; ++i) {
                nfdchar_t* path;
                NFD_PathSet_GetPath(pathSet, i, &path);
                result.Paths.push_back(path);
                NFD_PathSet_FreePath(path);
            }

            NFD_PathSet_Free(pathSet);
        }
        else if (nfdResult == NFD_ERROR) {
            result.Status = DialogResultType::ERROR;
            result.Error = NFD_GetError();
        }
        NFD_Quit();

        return result;
	}

	DialogResult FileDialog::SelectFolder(const std::filesystem::path& defaultPath)
	{
        NFD_Init();

        nfdchar_t* outPath;
        DialogResult result;

        nfdresult_t nfdResult = NFD_PickFolder(&outPath, defaultPath.empty() ? NULL : defaultPath.string().c_str());
        if (nfdResult == NFD_OKAY) {
            result.Status = DialogResultType::SUCCESS;
            result.Paths.reserve(1);
            result.Paths.push_back(outPath);
            NFD_FreePath(outPath);
        }
        else if (nfdResult == NFD_ERROR) {
            result.Status = DialogResultType::ERROR;
            result.Error = NFD_GetError();
        }
        NFD_Quit();

        return result;
	}

    DialogResult FileDialog::SelectFolders(const std::filesystem::path& defaultPath)
    {
        NFD_Init();

        const nfdpathset_t* pathSet;
        DialogResult result;

        nfdresult_t nfdResult = NFD_PickFolderMultiple(&pathSet, defaultPath.empty() ? NULL : defaultPath.string().c_str());
        if (nfdResult == NFD_OKAY) {
            result.Status = DialogResultType::SUCCESS;

            nfdpathsetsize_t count;
            NFD_PathSet_GetCount(pathSet, &count);

            result.Paths.reserve(count);

            nfdpathsetsize_t i;
            for (i = 0; i < count; ++i) {
                nfdchar_t* path;
                NFD_PathSet_GetPath(pathSet, i, &path);
                result.Paths.push_back(path);
                NFD_PathSet_FreePath(path);
            }

            NFD_PathSet_Free(pathSet);
        }
        else if (nfdResult == NFD_ERROR) {
            result.Status = DialogResultType::ERROR;
            result.Error = NFD_GetError();
        }
        NFD_Quit();

        return result;
    }
}