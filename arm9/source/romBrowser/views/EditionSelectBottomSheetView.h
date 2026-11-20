#pragma once
#include <array>
#include "core/SharedPtr.h"
#include "BottomSheetView.h"
#include "EditionOptionView.h"
#include "gui/views/Label2DView.h"
#include "../viewModels/EditionSelectViewModel.h"

class MaterialColorScheme;
class IFontRepository;

/// @brief Bottom sheet that lets the user choose which edition of a title to launch (see GameEdition.h).
class EditionSelectBottomSheetView : public BottomSheetView
{
    SHARED_ONLY(EditionSelectBottomSheetView)

public:
    void InitVram(const VramContext& vramContext) override;
    void Update() override;
    void Draw(GraphicsContext& graphicsContext) override;
    bool HandleInput(const InputProvider& inputProvider, FocusManager& focusManager) override;
    SharedPtr<View> MoveFocus(const SharedPtr<View>& currentFocus,
        FocusMoveDirection direction, View* source) override;

    void Focus(FocusManager& focusManager) override
    {
        focusManager.Focus(_options[0]);
    }

protected:
    void Close() override;

private:
    SharedPtr<EditionSelectViewModel> _viewModel;
    SharedPtr<Label2DView> _titleLabel;
    std::array<SharedPtr<EditionOptionView>, 3> _options;
    const MaterialColorScheme* _materialColorScheme;

    EditionSelectBottomSheetView(SharedPtr<EditionSelectViewModel> viewModel,
        const MaterialColorScheme* materialColorScheme, const IFontRepository* fontRepository);
};
