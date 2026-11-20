#pragma once
#include "gui/views/ViewContainer.h"
#include "gui/views/Label2DView.h"

class MaterialColorScheme;
class IFontRepository;

/// @brief A single text row of the edition select popup.
class EditionOptionView : public ViewContainer
{
    SHARED_ONLY(EditionOptionView)

public:
    typedef void (*action_t)(EditionOptionView* sender, void* arg);

    void Update() override;
    void Draw(GraphicsContext& graphicsContext) override;
    bool HandleInput(const InputProvider& inputProvider, FocusManager& focusManager) override;
    void HandlePenDown(const Point& touchPoint, FocusManager& focusManager) override;
    void HandlePenMove(const Point& touchPoint, FocusManager& focusManager) override;
    void HandlePenUp(const Point& lastTouchPoint, FocusManager& focusManager) override;

    Rectangle GetBounds() const override
    {
        return Rectangle(_position.x, _position.y, 224, 24);
    }

    void SetAction(action_t action, void* arg)
    {
        _action = action;
        _actionArg = arg;
    }

    void SetSelectorVramOffset(u32 vramOffset) { _selectorVramOffset = vramOffset; }

private:
    SharedPtr<Label2DView> _label;
    const MaterialColorScheme* _materialColorScheme;
    action_t _action = nullptr;
    void* _actionArg = nullptr;
    u32 _selectorVramOffset = 0;
    bool _penDown = false;

    EditionOptionView(const char16_t* text, const MaterialColorScheme* materialColorScheme,
        const IFontRepository* fontRepository);

    void Activate();
};
