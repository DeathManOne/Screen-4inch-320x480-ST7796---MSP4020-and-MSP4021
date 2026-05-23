/*
 * texts.cpp
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

#include "../include/MSP4020.h"
using namespace ST7796S;

void MSP4020::_char(uint16_t x, uint16_t y, uint16_t myChar) {
    if (!this->_FONT)
        { return; }
    uint16_t iChar = 0xFFFF;

    for (uint16_t i = 0; ; i++) {
        uint16_t code = pgm_read_word(&this->_FONT->unicode[i]);
        if (code == 0xFFFF)
            { break; }
        if (code == myChar) {
            iChar = i;
            break;
        }
    }

    if (iChar == 0xFFFF) {
        for (uint16_t i = 0;; i++) {
            uint16_t code = pgm_read_word(&this->_FONT->unicode[i]);
            if (code == 0xFFFF)
                { return; }
            if (code == '?') {
                iChar = i;
                break;
            }
        }
    }

    uint8_t scale = this->_TEXT_SCALE;
    uint16_t w = pgm_read_byte(&this->_FONT->widths[iChar]);
    uint16_t h = pgm_read_byte(&this->_FONT->heights[iChar]);
    int8_t yOff = pgm_read_byte(&this->_FONT->yOffsets[iChar]);
    uint16_t offset = pgm_read_word(&this->_FONT->offsets[iChar]);
    const uint32_t* data = this->_FONT->data;

    this->_transactionBegin();
    for (uint16_t i = 0; i < w; i++) {
        uint32_t col = pgm_read_dword(&data[offset + i]);
        for (uint16_t j = 0; j < h; j++) {
            if (col & (1UL << j)) {
                uint16_t px = x + i*scale;
                uint16_t py = y + (j + yOff)*scale;
                this->_setAddress(px, py, px+scale-1, py+scale-1);

                uint8_t d[2] = {
                    uint8_t((this->_TEXT_COLOR) >> 8),
                    uint8_t((this->_TEXT_COLOR)&0xFF)
                };
                for (uint16_t s=0; s<scale*scale; s++)
                    { this->_writeData(d, 2); }
            }
        }
    }
    this->_transactionEnd();
}

void MSP4020::_charBounds(uint16_t c, int &w, int &h, int &yOff) {
    if (!this->_FONT) {
        w = h = yOff = 0;
        return;
    }

    uint16_t iChar = 0xFFFF;
    for (uint16_t i = 0;; i++) {
        uint16_t code = pgm_read_word(&this->_FONT->unicode[i]);
        if (code == 0xFFFF)
            { break; }
        if (code == c) {
            iChar = i;
            break;
        }
    }

    if (iChar == 0xFFFF) {
        for (uint16_t i = 0;; i++) {
            uint16_t code = pgm_read_word(&this->_FONT->unicode[i]);
            if (code == 0xFFFF)
                { return; }
            if (code == '?') {
                iChar = i;
                break;
            }
        }
    }

    w = pgm_read_byte(&this->_FONT->widths[iChar]);
    h = pgm_read_byte(&this->_FONT->heights[iChar]);
    yOff = pgm_read_byte(&this->_FONT->yOffsets[iChar]);
}

void MSP4020::_textAlign(int x, int y, int w, int h, const char* str, uint8_t alignH, uint8_t alignV) {
    if (!this->_FONT || !str || !*str)
        { return; }

    int totalWidth;
    int textHeight;
    int minYOffset;

    this->_textBounds(str, totalWidth, textHeight, minYOffset);

    int textX = x;
    int textY = y - (minYOffset * this->_TEXT_SCALE);

    if (alignH == 1) { textX = x + (w - totalWidth) / 2; }
    else if (alignH == 2) { textX = x + w - totalWidth; }

    if (alignV == 1) { textY = y + (h - textHeight) / 2 - (minYOffset * this->_TEXT_SCALE); }
    else if (alignV == 2) { textY = y + h - textHeight - (minYOffset * this->_TEXT_SCALE); }

    this->text(textX, textY, str);
}

void MSP4020::_textBounds(const char* str, int &width, int &height, int &minYOffset) {
    width = 0;
    height = 0;
    minYOffset = 0;

    if (!this->_FONT || !str || !*str)
        { return; }
    uint8_t scale = this->_TEXT_SCALE;

    int maxHeight = 0;
    int maxYOffset = 0;

    const char* ptr = str;
    while (*ptr) {
        uint16_t c;

        if ((*ptr & 0x80) == 0) { c = *ptr++; }
        else if ((*ptr & 0xE0) == 0xC0) {
            c = ((*ptr & 0x1F) << 6) | (ptr[1] & 0x3F);
            ptr += 2;
        } else if ((*ptr & 0xF0) == 0xE0) {
            c = ((*ptr & 0x0F) << 12) | ((ptr[1] & 0x3F) << 6) | (ptr[2] & 0x3F);
            ptr += 3;
        } else {
            ptr++;
            continue;
        }

        int cw, ch, yOff;
        this->_charBounds(c, cw, ch, yOff);
        width += (cw + 1) * scale;

        if (ch > maxHeight)
            { maxHeight = ch; }
        if (yOff < minYOffset)
            { minYOffset = yOff; }
        if (yOff > maxYOffset)
            { maxYOffset = yOff; }
    }

    if (width > 0)
        { width -= scale; }
    height = (maxHeight + maxYOffset - minYOffset) * scale;
}

void MSP4020::text(uint16_t x, uint16_t y, const char* str) {
    if (!this->_FONT || !str)
        { return; }
    uint8_t scale = this->_TEXT_SCALE;

    while (*str) {
        uint16_t c;
        if ((*str & 0x80) == 0) { c = *str++; }
        else if ((*str & 0xE0) == 0xC0) {
            c = ((*str & 0x1F) << 6) | (str[1] & 0x3F);
            str += 2;
        } else if ((*str & 0xF0) == 0xE0) {
            c = ((*str & 0x0F) << 12) | ((str[1] & 0x3F) << 6) | (str[2] & 0x3F);
            str += 3;
        } else if ((*str & 0xF8) == 0xF0) {
            str += 4;
            continue;
        } else {
            str++;
            continue;
        }

        this->_char(x, y, c);
        uint16_t iChar = 0xFFFF;
        for (uint16_t i = 0;; i++) {
            uint16_t code = pgm_read_word(&this->_FONT->unicode[i]);
            if (code == 0xFFFF)
                { break; }
            if (code == c) {
                iChar = i;
                break;
            }
        }

        if (iChar == 0xFFFF) {
            for (uint16_t i = 0;; i++) {
                uint16_t code = pgm_read_word(&this->_FONT->unicode[i]);
                if (code == 0xFFFF)
                    { return; }
                if (code == '?') {
                    iChar = i;
                    break;
                }
            }
        }
        uint16_t w = pgm_read_byte(&this->_FONT->widths[iChar]);
        x += (w + 1) * scale;
    }
}

int MSP4020::textWidth(const char* str) {
    int w, h, y;
    this->_textBounds(str, w, h, y);
    return w;
}

int MSP4020::textHeight(const char* str) {
    int w, h, y;
    this->_textBounds(str, w, h, y);
    return h;
}
