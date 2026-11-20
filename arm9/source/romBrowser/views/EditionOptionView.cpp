#include "common.h"
#include "themes/material/MaterialColorScheme.h"
#include "themes/IFontRepository.h"
#include "core/math/RgbMixer.h"
#include "gui/palette/GradientPalette.h"
#include "gui/GraphicsContext.h"
#include "gui/OamBuilder.h"
#include "gui/input/InputProvider.h"
#include "EditionOptionView.h"

#define LABEL_X     16
#define LABEL_Y     5

EditionOptionView::EditionOptionView(const char16_t* text,
    const MaterialColorScheme* materialColorScheme, const IFontRepository* fontRepository)
    : _label(Label2DView::CreateShared(196, 16, 32, fontRepository->GetFont(FontType::Regular10)))
    , _materialColorScheme(materialColorScheme)
{
    _label->SetText(text);
    _label->SetEllipsisStyle(LabelView::EllipsisStyle::Ellipsis);
    AddChildTail(_label.GetPointer());
}

void EditionOptionView::Update()
{
    _label->SetPosition(_position.x + LABEL_X, _position.y + LABEL_Y);
    ViewContainer::Update();
}

void EditionOptionView::Draw(GraphicsContext& graphicsContext)
{
    if (!graphicsContext.IsVisible(GetBounds()))
    {
        return;
    }

    auto backColor = _materialColorScheme->GetColor(md::sys::color::surfaceContainerLow);
    if (IsFocused())
    {
        auto selectorFullColor = _materialColorScheme->GetColor(md::sys::color::onSurface);
        backColor = RgbMixer::Lerp(backColor, selectorFullColor, 10, 100);
    }

    _label->SetBackgroundColor(backColor);
    _label->SetForegroundColor(_materialColorScheme->onSurface);

    if (IsFocused())
    {
        u32 selectorPaletteRow = graphicsContext.GetPaletteManager().AllocRow(
            GradientPalette(backColor, backColor),
            _position.y, _position.y + 24);
        auto selectorOam = graphicsContext.GetOamManager().AllocOams(4);
        OamBuilder::OamWithSize<64, 32>(_position.x, _position.y, _selectorVramOffset >> 7)
            .WithPalette16(selectorPaletteRow)
            .WithPriority(graphicsContext.GetPriority())
            .Build(selectorOam[0]);
        OamBuilder::OamWithSize<64, 32>(_position.x + 64, _position.y, _selectorVramOffset >> 7)
            .WithPalette16(selectorPaletteRow)
            .WithPriority(graphicsContext.GetPriority())
            .Build(selectorOam[1]);
        OamBuilder::OamWithSize<64, 32>(_position.x + 2 * 64, _position.y, _selectorVramOffset >> 7)
            .WithPalette16(selectorPaletteRow)
            .WithPriority(graphicsContext.GetPriority())
            .Build(selectorOam[2]);
        OamBuilder::OamWithSize<64, 32>(_position.x + 2 * 64 + 32, _position.y, _selectorVramOffset >> 7)
            .WithPalette16(selectorPaletteRow)
            .WithPriority(graphicsContext.GetPriority())
            .Build(selectorOam[3]);
    }

    ViewContainer::Draw(graphicsContext);
}

bool EditionOptionView::HandleInput(const InputProvider& inputProvider, FocusManager& focusManager)
{
    if (inputProvider.Triggered(InputKey::A))
    {
        Activate();
        return true;
    }

    return ViewContainer::HandleInput(inputProvider, focusManager);
}

void EditionOptionView::HandlePenDown(const Point& touchPoint, FocusManager& focusManager)
{
    if (GetBounds().Contains(touchPoint))
    {
        _penDown = true;
    }
}

void EditionOptionView::HandlePenMove(const Point& touchPoint, FocusManager& focusManager)
{
    if (!GetBounds().Contains(touchPoint))
    {
        _penDown = false;
    }
}

void EditionOptionView::HandlePenUp(const Point& lastTouchPoint, FocusManager& focusManager)
{
    if (_penDown && GetBounds().Contains(lastTouchPoint))
    {
        focusManager.Focus(SharedFromThis());
        Activate();
    }

    _penDown = false;
}

void EditionOptionView::Activate()
{
    if (_action)
    {
        _action(this, _actionArg);
    }
}
