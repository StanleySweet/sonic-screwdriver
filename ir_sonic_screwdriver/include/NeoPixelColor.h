#ifndef NEOPIXEL_COLOR_H
#define NEOPIXEL_COLOR_H

#include <cstdint>

namespace NeoPixel
{
    struct Color
    {
        uint8_t R;
        uint8_t G;
        uint8_t B;
    };

    namespace Colors
    {
        extern const Color Yellow;
        extern const Color Red;
        extern const Color Green;
        extern const Color Blue;
        extern const Color Purple;
        extern const Color Aqua;
        extern const Color Mint;
        extern const Color White;

        namespace Freya
        {
            extern const Color DarkBlue;
            extern const Color DarkGreen;
            extern const Color DarkRed;
            extern const Color LightRed;
            extern const Color LightBlue;
            extern const Color LightGreen;
        }
    }
}

#endif // NEOPIXEL_COLOR_H
