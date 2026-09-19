/*
 * src/images.cpp
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

#include <pgmspace.h>
#include <Stream.h>
#include "MSP4020.h"
using namespace ST7796S;

void MSP4020::_image(int x, int y, int w, int h, const uint16_t* img) {
    if (!img || w <= 0 || h <= 0)
        { return; }
    this->_transactionBegin();

    for (int row = 0; row < h; row++) {
        this->_setAddress(x, y + row, x + w - 1, y + row);
        int remaining = w;
        int offset = row * w;

        while (remaining > 0) {
            int chunk = remaining > MSP4020::_BUFFER_SIZE
                ? MSP4020::_BUFFER_SIZE
                : remaining;

            for (int i = 0; i < chunk; i++) {
                uint16_t color = pgm_read_word(&img[offset + i]);
                this->_BUFFER_CPU[i] = color;
            }
            this->_writePixels(this->_BUFFER_CPU, chunk);

            offset += chunk;
            remaining -= chunk;
        }
    }
    this->_transactionEnd();
}

bool MSP4020::imageSD(int x, int y, Stream& file) {
    if (!this->_DRIVER || !file.available())
        { return false; }
    uint8_t lo = file.read();
    uint8_t hi = file.read();
    uint16_t w = lo | (hi << 8);

    if (w > this->_SCREEN_WIDTH * 4)
        { return false; }
    lo = file.read();
    hi = file.read();
    uint16_t h = lo | (hi << 8);

    if (h > this->_SCREEN_HEIGHT * 4)
        { return false; }
    if (w == 0 || h == 0)
        { return false; }
    this->_transactionBegin();

    for (uint16_t row = 0; row < h; row++) {
        this->_setAddress(x, y + row, x + w - 1, y + row);
        uint16_t remaining = w;

        while (remaining > 0) {
            uint16_t chunk = remaining > MSP4020::_BUFFER_SIZE
                ? MSP4020::_BUFFER_SIZE
                : remaining;
            for (int i = 0; i < chunk; i++) {
                int high = file.read();
                int low = file.read();
                if (high < 0 || low < 0) {
                    this->_transactionEnd();
                    return false;
                }
                this->_BUFFER_CPU[i] =
                    (static_cast<uint16_t>(high) << 8) |
                    static_cast<uint16_t>(low);
            }
            this->_writePixels(this->_BUFFER_CPU, chunk);
            remaining -= chunk;
        }
    }
    this->_transactionEnd();
    return true;
}
