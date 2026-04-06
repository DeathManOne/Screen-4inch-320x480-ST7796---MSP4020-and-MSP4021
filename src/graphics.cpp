#include "../include/MSP4020.h"
using namespace ST7796S;

void MSP4020::setFont(const Font& font) {
    this->_FONT = &font;
}

void MSP4020::setTextColor(uint16_t color) {
    uint8_t r = (color >> 11) & 0x1F;
    uint8_t g = (color >> 5) & 0x3F;
    uint8_t b = color & 0x1F;
    *this->_TEXT_COLOR = (g << 11) | (b << 5) | r;
}

void MSP4020::setTextScale(uint8_t scale) {
    *this->_TEXT_SCALE = (scale < 1) ? 1 : scale;
}

const uint32_t* MSP4020::_getCharBitmap(uint8_t myChar) {
    if (!this->_FONT)
        { return nullptr; }
    if (myChar < this->_FONT->firstChar || myChar > this->_FONT->lastChar)
        { return nullptr; }
    uint16_t index = (myChar - this->_FONT->firstChar) * this->_FONT->width;
    return &((const uint32_t*)this->_FONT->data)[index];
}

void MSP4020::_drawChar(uint16_t x, uint16_t y, char myChar) {
    if (!this->_FONT) return;

    const uint32_t *bitmap = _getCharBitmap((uint8_t)myChar);
    if (!bitmap) return;

    uint8_t scale = *this->_TEXT_SCALE;

    this->_transactionBegin();
    for (uint8_t i = 0; i < this->_FONT->width; i++) {
        uint32_t col = pgm_read_dword(&bitmap[i]);
        for (uint8_t j = 0; j < this->_FONT->height; j++) {
            if (col & (1UL << j)) {
                uint16_t px = x + i * scale;
                uint16_t py = y + j * scale;
                this->_setAddress(px, py, px + scale - 1, py + scale - 1);
                uint8_t data[2] = {
                    uint8_t((*this->_TEXT_COLOR) >> 8),
                    uint8_t((*this->_TEXT_COLOR) & 0xFF)
                };
                for (uint8_t sx = 0; sx < scale * scale; sx++) {
                    this->_writeData(data, 2);
                }
            }
        }
    }
    this->_transactionEnd();
}

void MSP4020::drawString(uint16_t x, uint16_t y, const char* str) {
    if (!this->_FONT || !str) return;

    const uint32_t* fontData = this->_FONT->data;
    const uint16_t* fontOffsets = this->_FONT->offsets; // tableau offset par char
    const uint8_t* fontWidths = this->_FONT->widths;     // tableau largeur par char
    uint8_t scale = *this->_TEXT_SCALE;

    while (*str) {
        uint8_t myChar = (uint8_t)*str;
        if (myChar < this->_FONT->firstChar || myChar > this->_FONT->lastChar)
            myChar = '?';

        uint16_t charIndex = myChar - this->_FONT->firstChar;
        uint16_t charWidth = fontWidths[charIndex];
        uint16_t charOffset = fontOffsets[charIndex];
        
        // === DRAW CHAR ===
        this->_transactionBegin();
        for (uint16_t i = 0; i < charWidth; i++) {
            uint32_t col = pgm_read_dword(&fontData[charOffset + i]);
            for (uint16_t j = 0; j < this->_FONT->height; j++) {
                if (col & (1UL << j)) {
                    uint16_t px = x + i * scale;
                    uint16_t py = y + j * scale;
                    this->_setAddress(px, py, px + scale - 1, py + scale - 1);
                    uint8_t data[2] = {
                        uint8_t((*this->_TEXT_COLOR) >> 8),
                        uint8_t((*this->_TEXT_COLOR) & 0xFF)
                    };
                    for (uint16_t sx = 0; sx < scale * scale; sx++)
                        this->_writeData(data, 2);
                }
            }
        }
        this->_transactionEnd();

        // avancer x selon largeur réelle du caractère + 1px d’espacement
        x += (charWidth + 1) * scale;
        str++;
    }
}

void MSP4020::rect(int x, int y, int width, int height, uint16_t color) {
    if (width <= 0 || height <= 0)
        { return; }

    if (x < 0)
        { x = 0; }
    if (width > *this->_SCREEN_WIDTH)
        { width = *this->_SCREEN_WIDTH; }
    if ((width + x) > *this->_SCREEN_WIDTH)
        { width = width - x; }

    if (y < 0)
        { y = 0; }
    if (height > *this->_SCREEN_HEIGHT)
        { height = *this->_SCREEN_HEIGHT; }
    if ((height + y) > *this->_SCREEN_HEIGHT)
        { height = height - y; }

    this->_transactionBegin();
    this->_setAddress(x, y, x + width - 1, y + height - 1);

    uint32_t c = (color << 16) | color;
    for (int i = 0; i < MSP4020::_BUFFER_SIZE / 2; i++)
        { ((uint32_t*)this->_BUFFER_CPU)[i] = c; }

    int total = width * height;
    while (total > 0) {
        int chunk = (total > MSP4020::_BUFFER_SIZE) ? MSP4020::_BUFFER_SIZE : total;
        if (total > chunk) {
            for (int i = 0; i < MSP4020::_BUFFER_SIZE; i++)
                { this->_BUFFER_CPU[i] = color; }
        }

        this->_writeData((uint8_t*)this->_BUFFER_CPU, chunk * 2);
        total -= chunk;
        if (total > 0)
            { this->_swapBuffers(); }
    }
    this->_transactionEnd();
}

void MSP4020::lineH(int x, int y, int width, uint16_t color) {
    if (width <= 0)
        { return; }

    if (x < 0)
        { x = 0; }
    if (width > *this->_SCREEN_WIDTH)
        { width = *this->_SCREEN_WIDTH; }
    if ((width + x) > *this->_SCREEN_WIDTH)
        { width = width - x; }

    uint32_t c = (color << 16) | color;
    for (int i = 0; i < width / 2; i++)
        { ((uint32_t*)this->_BUFFER_CPU)[i] = c; }

    this->_transactionBegin();
    this->_setAddress(x, y, x + width - 1, y);

    int remaining = width;
    while (remaining > 0) {
        int chunk = (remaining > MSP4020::_BUFFER_SIZE) ? MSP4020::_BUFFER_SIZE : remaining;
        this->_writeData((uint8_t*)this->_BUFFER_CPU, chunk * 2);
        remaining -= chunk;
    }
    this->_transactionEnd();
}

void MSP4020::lineV(int x, int y, int height, uint16_t color) {
    if (height <= 0)
        { return; }

    if (y < 0)
        { y = 0; }
    if (height > *this->_SCREEN_HEIGHT)
        { height = *this->_SCREEN_HEIGHT; }
    if ((height + y) > *this->_SCREEN_HEIGHT)
        { height = height - y; }

    uint32_t c = (color << 16) | color;
    for (int i = 0; i < MSP4020::_BUFFER_SIZE / 2; i++)
        { ((uint32_t*)this->_BUFFER_CPU)[i] = c; }

    this->_transactionBegin();
    this->_setAddress(x, y, x, y + height - 1);

    int remaining = height;
    while (remaining > 0) {
        int chunk = (remaining > MSP4020::_BUFFER_SIZE) ? MSP4020::_BUFFER_SIZE : remaining;
        this->_writeData((uint8_t*)this->_BUFFER_CPU, chunk * 2);
        remaining -= chunk;
    }
    this->_transactionEnd();
}

void MSP4020::rectf(int x, int y, int width, int height, uint16_t color) {
    this->lineH(x, y, width, color);
    this->lineH(x + 1, y + height - 1, width - 1, color);
    this->lineV(x, y + 1, height - 1, color);
    this->lineV(x + width - 1, y, height - 1, color);
}

void MSP4020::drawButton(int x, int y, int width, int height, uint16_t color) {
    this->rect(x, y, width, height, color);
}