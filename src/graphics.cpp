/*
 * graphics.cpp
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

void MSP4020::_pixel(int x, int y, uint16_t color) {
    if (x < 0 || y < 0 || x >= this->_SCREEN_WIDTH || y >= this->_SCREEN_HEIGHT)
        { return; }
    this->_transactionBegin();
    this->_setAddress(x, y, x, y);

    uint8_t d[2] = { uint8_t(color >> 8), uint8_t(color) };
    this->_writeData(d, 2);

    this->_transactionEnd();
}

void MSP4020::_line(int x0, int y0, int x1, int y1, uint16_t color) {
    if (y0 == y1) {
        if (x1 < x0)
            { std::swap(x0, x1); }
        this->lineH(x0, y0, x1 - x0 + 1, color);
        return;
    }
    if (x0 == x1) {
        if (y1 < y0)
            { std::swap(y0, y1); }
        this->lineV(x0, y0, y1 - y0 + 1, color);
        return;
    }

    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        _pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1)
            { break; }

        int e2 = err << 1;
        if (e2 > -dy)
            { err -= dy; x0 += sx; }
        if (e2 < dx) 
            { err += dx; y0 += sy; }
    }
}

void MSP4020::lineH(int x, int y, int width, uint16_t color) {
    if (width <= 0 || y < 0 || y >= this->_SCREEN_HEIGHT)
        { return; }
    if (x < 0)
        { x = 0; }
    if (width > this->_SCREEN_WIDTH)
        { width = this->_SCREEN_WIDTH; }
    if ((width + x) > this->_SCREEN_WIDTH)
        { width = this->_SCREEN_WIDTH - x; }
    color = (color >> 8) | (color << 8);

    uint32_t c = ((uint32_t)color << 16) | color;
    for (int i = 0; i < MSP4020::_BUFFER_SIZE / 2; i++)
        { ((uint32_t*)this->_BUFFER_CPU)[i] = c; }

    this->_transactionBegin();
    this->_setAddress(x, y, x + width - 1, y);

    int remaining = width;
    while (remaining > 0) {
        int chunk = (remaining > MSP4020::_BUFFER_SIZE)
            ? MSP4020::_BUFFER_SIZE
            : remaining;
        this->_writeData((uint8_t*)this->_BUFFER_CPU, chunk * 2);
        remaining -= chunk;
    }
    this->_transactionEnd();
}

void MSP4020::lineV(int x, int y, int height, uint16_t color) {
    if (height <= 0 || x < 0 || x >= this->_SCREEN_WIDTH)
        { return; }
    if (y < 0)
        { y = 0; }
    if (height > this->_SCREEN_HEIGHT)
        { height = this->_SCREEN_HEIGHT; }
    if ((height + y) > this->_SCREEN_HEIGHT)
        { height = this->_SCREEN_HEIGHT - y; }
    color = (color >> 8) | (color << 8);

    uint32_t c = ((uint32_t)color << 16) | color;
    for (int i = 0; i < MSP4020::_BUFFER_SIZE / 2; i++)
        { ((uint32_t*)this->_BUFFER_CPU)[i] = c; }

    this->_transactionBegin();
    this->_setAddress(x, y, x, y + height - 1);

    int remaining = height;
    while (remaining > 0) {
        int chunk = (remaining > MSP4020::_BUFFER_SIZE)
            ? MSP4020::_BUFFER_SIZE
            : remaining;
        this->_writeData((uint8_t*)this->_BUFFER_CPU, chunk * 2);
        remaining -= chunk;
    }
    this->_transactionEnd();
}

void MSP4020::circle(int x0, int y0, int r, uint16_t color) {
    int x = r;
    int y = 0;
    int err = 1 - r;

    while (x >= y) {
        _pixel(x0 + x, y0 + y, color);
        _pixel(x0 + y, y0 + x, color);
        _pixel(x0 - y, y0 + x, color);
        _pixel(x0 - x, y0 + y, color);
        _pixel(x0 - x, y0 - y, color);
        _pixel(x0 - y, y0 - x, color);
        _pixel(x0 + y, y0 - x, color);
        _pixel(x0 + x, y0 - y, color);

        y++;
        if (err < 0) { err += 2*y + 1; }
        else {
            x--;
            err += 2*(y - x) + 1;
        }
    }
}

void MSP4020::circleFill(int x0, int y0, int r, uint16_t color) {
    int x = r;
    int y = 0;
    int err = 1 - r;

    while (x >= y) {
        lineH(x0 - x, y0 + y, 2*x + 1, color);
        lineH(x0 - x, y0 - y, 2*x + 1, color);
        lineH(x0 - y, y0 + x, 2*y + 1, color);
        lineH(x0 - y, y0 - x, 2*y + 1, color);

        y++;
        if (err < 0) { err += 2*y + 1; }
        else {
            x--;
            err += 2*(y - x) + 1;
        }
    }
}

void MSP4020::triangleFill(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color) {
    if (y1 > y2) {
        std::swap(y1,y2);
        std::swap(x1,x2);
    }
    if (y2 > y3) {
        std::swap(y2,y3);
        std::swap(x2,x3);
    }
    if (y1 > y2) {
        std::swap(y1,y2);
        std::swap(x1,x2);
    }

    auto interp = [](int y0, int x0, int y1, int x1, int y){
        if (y1 == y0)
            { return x0; }
        return x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    };

    for (int y = y1; y <= y3; y++) {
        if (y < 0 || y >= this->_SCREEN_HEIGHT)
            { continue; }

        int xa = (y < y2)
            ? interp(y1, x1, y2, x2, y)
            : interp(y2, x2, y3, x3, y);
        int xb = interp(y1, x1, y3, x3, y);

        if (xa > xb)
            { std::swap(xa, xb); }
        lineH(xa, y, xb - xa + 1, color);
    }
}

void MSP4020::rect(int x, int y, int width, int height, uint16_t color) {
    this->lineH(x, y, width, color);
    this->lineH(x, y + height - 1, width, color);
    this->lineV(x, y, height, color);
    this->lineV(x + width - 1, y, height, color);
}

void MSP4020::rectFill(int x, int y, int width, int height, uint16_t color) {
    if (width <= 0 || height <= 0)
        { return; }
    if (x < 0)
        { x = 0; }
    if (width > this->_SCREEN_WIDTH)
        { width = this->_SCREEN_WIDTH; }
    if ((width + x) > this->_SCREEN_WIDTH)
        { width = this->_SCREEN_WIDTH - x; }

    if (y < 0)
        { y = 0; }
    if (height > this->_SCREEN_HEIGHT)
        { height = this->_SCREEN_HEIGHT; }
    if ((height + y) > this->_SCREEN_HEIGHT)
        { height = this->_SCREEN_HEIGHT - y; }
    color = (color >> 8) | (color << 8);

    this->_transactionBegin();
    this->_setAddress(x, y, x + width - 1, y + height - 1);

    int total = width * height;
    while (total > 0) {
        int chunk = (total > MSP4020::_BUFFER_SIZE)
            ? MSP4020::_BUFFER_SIZE
            : total;
        for (int i = 0; i < chunk; i++)
            { this->_BUFFER_CPU[i] = color; }
        this->_writeData((uint8_t*)this->_BUFFER_CPU, chunk * 2);

        total -= chunk;
        if (total > 0)
            { this->_swapBuffers(); }
    }
    this->_transactionEnd();
}

void MSP4020::rectRound(int x, int y, int width, int height, int radius, uint16_t color) {
    if (width <= 0 || height <= 0)
        { return; }
    if (radius < 0)
        { radius = 0; }
    if (radius == 0) {
        this->rect(x, y, width, height, color);
        return;
    }

    int maxRadius = min(width, height) / 2;
    if (radius > maxRadius)
        { radius = maxRadius; }

    this->lineH(x + radius, y, width - 2 * radius, color);
    this->lineH(x + radius, y + height - 1, width - 2 * radius, color);
    this->lineV(x, y + radius, height - 2 * radius, color);
    this->lineV(x + width - 1, y + radius, height - 2 * radius, color);

    int cx1 = x + radius;
    int cx2 = x + width - 1 - radius;
    int cy1 = y + radius;
    int cy2 = y + height - 1 - radius;

    int px = radius;
    int py = 0;
    int err = 1 - radius;

    while (px >= py) {
        this->_pixel(cx1 - px, cy1 - py, color);
        this->_pixel(cx1 - py, cy1 - px, color);
        this->_pixel(cx2 + px, cy1 - py, color);
        this->_pixel(cx2 + py, cy1 - px, color);
        this->_pixel(cx1 - px, cy2 + py, color);
        this->_pixel(cx1 - py, cy2 + px, color);
        this->_pixel(cx2 + px, cy2 + py, color);
        this->_pixel(cx2 + py, cy2 + px, color);

        py++;
        if (err < 0) { err += 2 * py + 1; }
        else {
            px--;
            err += 2 * (py - px) + 1;
        }
    }
}

void MSP4020::rectRoundFill(int x, int y, int width, int height, int radius, uint16_t color) {
    if (width <= 0 || height <= 0)
        { return; }
    if (radius < 0)
        { radius = 0; }
    if (radius == 0) {
        this->rectFill(x, y, width, height, color);
        return;
    }

    int maxRadius = min(width, height) / 2;
    if (radius > maxRadius)
        { radius = maxRadius; }

    this->rectFill(x + radius, y, width - 2 * radius, height, color);
    this->rectFill(x, y + radius, radius, height - 2 * radius, color);
    this->rectFill(x + width - radius, y + radius, radius, height - 2 * radius, color);

    int cx1 = x + radius;
    int cx2 = x + width - 1 - radius;
    int cy1 = y + radius;
    int cy2 = y + height - 1 - radius;

    int px = radius;
    int py = 0;
    int err = 1 - radius;

    while (px >= py) {
        this->lineH(cx1 - px, cy1 - py, 2 * px + 1, color);
        this->lineH(cx1 - px, cy1 + py, 2 * px + 1, color);
        this->lineH(cx1 - py, cy1 - px, 2 * py + 1, color);
        this->lineH(cx1 - py, cy1 + px, 2 * py + 1, color);
        this->lineH(cx2 - px, cy1 - py, 2 * px + 1, color);
        this->lineH(cx2 - px, cy1 + py, 2 * px + 1, color);
        this->lineH(cx2 - py, cy1 - px, 2 * py + 1, color);
        this->lineH(cx2 - py, cy1 + px, 2 * py + 1, color);
        this->lineH(cx1 - px, cy2 - py, 2 * px + 1, color);
        this->lineH(cx1 - px, cy2 + py, 2 * px + 1, color);
        this->lineH(cx1 - py, cy2 - px, 2 * py + 1, color);
        this->lineH(cx1 - py, cy2 + px, 2 * py + 1, color);
        this->lineH(cx2 - px, cy2 - py, 2 * px + 1, color);
        this->lineH(cx2 - px, cy2 + py, 2 * px + 1, color);
        this->lineH(cx2 - py, cy2 - px, 2 * py + 1, color);
        this->lineH(cx2 - py, cy2 + px, 2 * py + 1, color);

        py++;
        if (err < 0) { err += 2 * py + 1; }
        else {
            px--;
            err += 2 * (py - px) + 1;
        }
    }
}

void MSP4020::arc(int x0, int y0, int r, int startDeg, int endDeg, uint16_t color, bool clockwise) {
    if(clockwise) {
        for(int a = startDeg; a <= endDeg; a++) {
            float rad = a * 0.0174533f;
            int x = x0 - r * cos(rad);
            int y = y0 - r * sin(rad);
            _pixel(x, y, color);
        }
    } else {
        for(int a = endDeg; a >= startDeg; a--) {
            float rad = a * 0.0174533f;
            int x = x0 - r * cos(rad);
            int y = y0 + r * sin(rad);
            _pixel(x, y, color);
        }
    }
}

void MSP4020::progressBar(int x, int y, int w, int h, int value, uint16_t color) {
    if (value < 0)
        { value = 0; }
    if (value > 100)
        { value = 100; }
    int fill = (w-2) * value / 100;

    rect(x, y, w, h, color);
    rectFill(x + 1, y + 1, fill, h - 2, color);
}

void MSP4020::slider(int x, int y, int w, int h, int value, uint16_t color) {
    if (value < 0)
        { value = 0; }
    if (value > 100)
        { value = 100; }
    int knob = x + (w * value / 100);

    lineH(x, y + h / 2, w, color);
    rectFill(knob - 3, y, 7, h, color);
}
