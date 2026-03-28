#include "../include/MSP4020.h"
using namespace ST7796S;

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

    this->transactionBegin();
    this->setAddress(x, y, x + width - 1, y + height - 1);

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

        this->writeData((uint8_t*)this->_BUFFER_CPU, chunk * 2);
        total -= chunk;
        if (total > 0)
            { this->swapBuffers(); }
    }
    this->transactionEnd();
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

    this->transactionBegin();
    this->setAddress(x, y, x + width - 1, y);

    int remaining = width;
    while (remaining > 0) {
        int chunk = (remaining > MSP4020::_BUFFER_SIZE) ? MSP4020::_BUFFER_SIZE : remaining;
        this->writeData((uint8_t*)this->_BUFFER_CPU, chunk * 2);
        remaining -= chunk;
    }
    this->transactionEnd();
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

    this->transactionBegin();
    this->setAddress(x, y, x, y + height - 1);

    int remaining = height;
    while (remaining > 0) {
        int chunk = (remaining > MSP4020::_BUFFER_SIZE) ? MSP4020::_BUFFER_SIZE : remaining;
        this->writeData((uint8_t*)this->_BUFFER_CPU, chunk * 2);
        remaining -= chunk;
    }
    this->transactionEnd();
}

void MSP4020::rectf(int x, int y, int width, int height, uint16_t color) {
    this->lineH(x, y, width, color);
    this->lineH(x + 1, y + height - 1, width - 1, color);
    this->lineV(x, y + 1, height - 1, color);
    this->lineV(x + width - 1, y, height - 1, color);
}

/*
void MSP4021::drawText(int x,int y,const char* txt, uint16_t color) {
    while(*txt) {
        this->drawPixel(x,y,color);
        x += 6;
        txt++;
    }
}
*/

void MSP4020::drawButton(int x, int y, int width, int height, uint16_t color) {
    this->rect(x, y, width, height, color);
}