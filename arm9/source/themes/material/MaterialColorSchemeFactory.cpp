#include "common.h"
#include <algorithm>
#include "material/scheme/scheme.h"
#include "MaterialColorSchemeFactory.h"

using Rgb888 = Rgb<8, 8, 8>;

// Bakes "b at t/255 alpha over a" into one opaque color.
static Rgb888 Mix(const Rgb888& a, const Rgb888& b, int t)
{
    return Rgb888(
        (a.r * (255 - t) + b.r * t + 127) / 255,
        (a.g * (255 - t) + b.g * t + 127) / 255,
        (a.b * (255 - t) + b.b * t + 127) / 255);
}

static int Luma(const Rgb888& c) { return (c.r * 54 + c.g * 183 + c.b * 19) >> 8; }

void MaterialColorSchemeFactory::FromPrimaryColor(const Rgb<8, 8, 8>& primaryColor,
    bool darkTheme, MaterialColorScheme& s)
{
    const Rgb888 white(255, 255, 255), black(0, 0, 0);
    const Rgb888 ink = darkTheme ? Rgb888(0xEB, 0xEB, 0xF5) : Rgb888(0x3C, 0x3C, 0x43);

    // dark mode needs a brighter accent to stay readable
    const Rgb888 accent = darkTheme ? Mix(primaryColor, white, 38) : primaryColor;

    // surfaces: neutral base, ~3% accent tint
    const Rgb888 bg   = darkTheme ? Mix(black, accent, 6)
                                  : Mix(Rgb888(0xF2, 0xF2, 0xF7), accent, 8);
    const Rgb888 card = darkTheme ? Mix(Rgb888(0x1C, 0x1C, 0x1E), accent, 8)
                                  : Mix(white, accent, 4);
    const Rgb888 label = darkTheme ? white : black;

    s.inverseOnSurface = bg;                    // page
    s.surfaceBright    = card;                  // rows / cells
    s.onSurface        = label;
    s.onSurfaceVariant = Mix(card, ink, 170);   // secondary text, ~67% alpha baked
    s.outline          = Mix(bg, ink, darkTheme ? 70 : 74);   // separators
    s.scrim            = Mix(bg, label, 110);

    s.secondaryContainer      = Mix(bg, darkTheme ? white : ink, darkTheme ? 44 : 18);
    s.onSecondaryContainer    = label;
    s.surfaceContainerHighest = Mix(bg, darkTheme ? white : ink, darkTheme ? 44 : 30); // placeholders

    // focus/pressed fill: accent at ~31% (light) / ~43% (dark) over the card
    s.mainIconBg = Mix(card, accent, darkTheme ? 110 : 80);

    s.primary   = accent;
    s.onPrimary = Luma(accent) > 150 ? black : white;
    s.tertiary  = accent;
    s.onTertiary = s.onPrimary;

    // folder icon colour: lightened accent
    s.tertiaryContainer   = darkTheme ? accent : Mix(accent, white, 89);
    s.onTertiaryContainer = Luma(s.tertiaryContainer) > 150 ? black : white;
}
