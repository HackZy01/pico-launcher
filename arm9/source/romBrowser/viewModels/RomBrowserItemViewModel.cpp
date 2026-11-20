#include "common.h"
#include "romBrowser/IRomBrowserController.h"
#include "RomBrowserViewModel.h"
#include "romBrowser/FileType/Nds/NdsFileType.h"
#include "RomBrowserItemViewModel.h"

void RomBrowserItemViewModel::Activate()
{
    if (_index >= 0)
    {
        const auto& item = _romBrowserController->GetRomBrowserViewModel()->GetFileInfoManager().GetItem(_index);
        if (item.GetFileType()->GetClassification() == FileTypeClassification::Folder)
        {
            _romBrowserController->NavigateToPath(item.GetFileName());
        }
        else
        {
            // launches the variant edition when that is the selected one
            _romBrowserController->LaunchFile(item.GetActiveFile());
        }
    }
}

void RomBrowserItemViewModel::ShowGameInfo()
{
    if (_index >= 0)
    {
        const auto& item = _romBrowserController->GetRomBrowserViewModel()->GetFileInfoManager().GetItem(_index);
        if (item.GetFileType() == &NdsFileType::sInstance)
        {
            _romBrowserController->ShowGameInfo(item.GetActiveFile());
        }
    }
}

bool RomBrowserItemViewModel::SelectEdition(bool useVariant)
{
    if (_index < 0)
        return false;

    const auto& item = _romBrowserController->GetRomBrowserViewModel()->GetFileInfoManager().GetItem(_index);
    if (!item.HasEditionVariant())
        return false;

    item.SelectEditionVariant(useVariant);
    return true;
}