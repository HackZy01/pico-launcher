#pragma once
#include "../IRomBrowserController.h"

/// @brief View model for the edition select popup (see GameEdition.h).
class EditionSelectViewModel
{
public:
    explicit EditionSelectViewModel(IRomBrowserController* romBrowserController)
        : _romBrowserController(romBrowserController) { }

    /// @brief Launches the original edition.
    void SelectOriginal()
    {
        _romBrowserController->LaunchFile(_romBrowserController->GetTriggerFileInfo());
    }

    /// @brief Launches the variant edition.
    void SelectVariant()
    {
        _romBrowserController->LaunchFile(_romBrowserController->GetEditionVariantFileInfo());
    }

    /// @brief Closes the popup without launching anything.
    void Cancel()
    {
        _romBrowserController->HideEditionSelect();
    }

private:
    IRomBrowserController* _romBrowserController;
};
