#ifndef _ST7796S_FONT_
#define _ST7796S_FONT_

#include <Arduino.h>

namespace ST7796S {
    struct Font {
        const uint8_t width;        // largeur max du font (utile pour calcul global)
        const uint8_t height;       // hauteur des caractères
        const uint8_t firstChar;    // premier caractère ASCII
        const uint8_t lastChar;     // dernier caractère ASCII
        const uint32_t* data;       // bitmap des colonnes (uint32_t)
        const uint16_t* offsets;    // index de départ de chaque caractère dans data[]
        const uint8_t* widths;      // largeur réelle de chaque caractère
    };
}
#endif