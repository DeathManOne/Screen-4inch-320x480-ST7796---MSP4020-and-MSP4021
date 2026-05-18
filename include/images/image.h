/*
 * image.h
 *
 * Copyright (c) 2026 DeathManOne
 * https://github.com/DeathManOne
 * 
 * This file is part of the ST7796S library.
 *
 * ST7796S is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ST7796S is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ST7796S.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _ST7796S_IMAGE_
#define _ST7796S_IMAGE_

#include <Arduino.h>

namespace ST7796S {
    struct Image {
        const uint16_t width;
        const uint16_t height;
        const uint16_t* data;
        // futur :
        const uint16_t transparent;
        const bool hasTransparency;
    };
}
#endif