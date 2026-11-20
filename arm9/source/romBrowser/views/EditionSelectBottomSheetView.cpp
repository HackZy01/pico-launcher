#include "common.h"
#include <libtwl/dma/dmaNitro.h>
#include "gui/GraphicsContext.h"
#include "gui/VramContext.h"
#include "gui/IVramManager.h"
#include "gui/input/InputProvider.h"
#include "themes/material/MaterialColorScheme.h"
#include "themes/IFontRepository.h"
#include "cheatSelector.h"
#include "EditionSelectBottomSheetView.h"

#define TITLE_LABEL_X       20
#define TITLE_LABEL_Y       16

#define OPTIONS_X           16
#define OPTIONS_Y           40
#define OPTION_HEIGHT       24

#define OPTION_ORIGINAL     0
#define OPTION_VARIANT      1
#define OPTION_CANCEL       2

EditionSelectBottomSheetView::EditionSelectBottomSheetView(
    SharedPtr<EditionSelectViewModel> viewModel, const MaterialColorScheme* materialColorScheme,
    const IFontRepository* fontRepository)
    : _viewModel(std::move(viewModel))
    , _titleLabel(Label2DView::CreateShared(224, 16, 40, fontRepository->GetFont(FontType::Medium11)))
    , _materialColorScheme(materialColorScheme)
{
    _titleLabel->SetText(u"W którą wersję chciałbyś zagrać?");
    AddChildTail(_titleLabel.GetPointer());

    static const char16_t* const sOptionTexts[3] =
    {
        [OPTION_ORIGINAL] = u"Angielska",
        [OPTION_VARIANT] = u"Polskie tłumaczenie",
        [OPTION_CANCEL] = u"Anuluj"
    };

    for (u32 i = 0; i < _options.size(); i++)
    {
        _options[i] = EditionOptionView::CreateShared(sOptionTexts[i], materialColorScheme, fontRepository);
        _options[i]->SetAction([] (EditionOptionView* sender, void* arg)
        {
            auto self = reinterpret_cast<EditionSelectBottomSheetView*>(arg);
            if (sender == self->_options[OPTION_ORIGINAL].GetPointer())
                self->_viewModel->SelectOriginal();
            else if (sender == self->_options[OPTION_VARIANT].GetPointer())
                self->_viewModel->SelectVariant();
            else
                self->_viewModel->Cancel();
        }, this);
        AddChildTail(_options[i].GetPointer());
    }
}

void EditionSelectBottomSheetView::InitVram(const VramContext& vramContext)
{
    BottomSheetView::InitVram(vramContext);

    const auto objVramManager = vramContext.GetObjVramManager();
    if (objVramManager)
    {
        u32 selectorVramOffset = objVramManager->Alloc(cheatSelectorTilesLen);
        dma_ntrCopy32(3, cheatSelectorTiles, objVramManager->GetVramAddress(selectorVramOffset), cheatSelectorTilesLen);
        for (auto& option : _options)
        {
            option->SetSelectorVramOffset(selectorVramOffset);
        }
    }
}

void EditionSelectBottomSheetView::Update()
{
    _titleLabel->SetPosition(TITLE_LABEL_X, _position.y + TITLE_LABEL_Y);
    for (u32 i = 0; i < _options.size(); i++)
    {
        _options[i]->SetPosition(OPTIONS_X, _position.y + OPTIONS_Y + (int)i * OPTION_HEIGHT);
    }
    BottomSheetView::Update();
}

void EditionSelectBottomSheetView::Draw(GraphicsContext& graphicsContext)
{
    graphicsContext.SetClipArea(GetBounds());
    u32 oldPrio = graphicsContext.SetPriority(1);
    {
        _titleLabel->SetBackgroundColor(_materialColorScheme->GetColor(md::sys::color::surfaceContainerLow));
        _titleLabel->SetForegroundColor(_materialColorScheme->onSurface);
        BottomSheetView::Draw(graphicsContext);
    }
    graphicsContext.SetPriority(oldPrio);
    graphicsContext.ResetClipArea();
}

bool EditionSelectBottomSheetView::HandleInput(
    const InputProvider& inputProvider, FocusManager& focusManager)
{
    if (inputProvider.Triggered(InputKey::B))
    {
        _viewModel->Cancel();
        return true;
    }
    return false;
}

SharedPtr<View> EditionSelectBottomSheetView::MoveFocus(const SharedPtr<View>& currentFocus,
    FocusMoveDirection direction, View* source)
{
    const int count = _options.size();
    for (int i = 0; i < count; i++)
    {
        if (currentFocus.GetPointer() == _options[i].GetPointer())
        {
            if (direction == FocusMoveDirection::Up)
                return _options[(i + count - 1) % count];
            else if (direction == FocusMoveDirection::Down)
                return _options[(i + 1) % count];
            else
                return nullptr;
        }
    }
    return nullptr;
}

void EditionSelectBottomSheetView::Close()
{
    _viewModel->Cancel();
}
