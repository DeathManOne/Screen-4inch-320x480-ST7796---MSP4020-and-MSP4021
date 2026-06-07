/*
 * keyboard.cpp
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

#include <cstring>
#include "MSP4021.h"
#include "fonts/DejaVuSans_Bold_16.h"
#include "fonts/DejaVuSans_Bold_18.h"
using namespace ST7796S;

static const char* rowsUpperLandscape[] = {
    "QWERTYUIOP",
    "ASDFGHJKLZ",
    "^XCVBNM<",
    "_"
};

static const char* rowsLowerLandscape[] = {
    "qwertyuiop",
    "asdfghjklz",
    "^xcvbnm<",
    "_"
};

static const char* rowsNumLandscape[] = {
    "1234567890",
    "+-*/=.,:;",
    "^()[]{}<",
    "_"
};

static const char* rowsUpperPortrait[] = {
    "QWERTY",
    "UIOPAS",
    "DFGHJK",
    "LZXCVB",
    "^NM<",
    "_"
};

static const char* rowsLowerPortrait[] = {
    "qwerty",
    "uiopas",
    "dfghjk",
    "lzxcvb",
    "^nm<",
    "_"
};

static const char* rowsNumPortrait[] = {
    "12345",
    "67890",
    "+-*/=",
    ".,:;(",
    ")[]{}",
    "^ <",
    "_"
};

static constexpr int rowsUpperLandscapeCount = sizeof(rowsUpperLandscape) / sizeof(rowsUpperLandscape[0]);
static constexpr int rowsLowerLandscapeCount = sizeof(rowsLowerLandscape) / sizeof(rowsLowerLandscape[0]);
static constexpr int rowsNumLandscapeCount = sizeof(rowsNumLandscape) / sizeof(rowsNumLandscape[0]);
static constexpr int rowsUpperPortraitCount = sizeof(rowsUpperPortrait) / sizeof(rowsUpperPortrait[0]);
static constexpr int rowsLowerPortraitCount = sizeof(rowsLowerPortrait) / sizeof(rowsLowerPortrait[0]);
static constexpr int rowsNumPortraitCount = sizeof(rowsNumPortrait) / sizeof(rowsNumPortrait[0]);

void MSP4021::_KDraw() {
    const char** rows = this->_KRows();
    int rowsCount, keyboardHeight, startY;

    this->_KLayoutMetrics(rowsCount, keyboardHeight, startY);
    this->rectFill( 0, 120, this->_SCREEN_WIDTH, this->_SCREEN_HEIGHT - 120, this->rgb(0, 0, 0));

    for (int r = 0; r < rowsCount; r++) {
        int len = strlen(rows[r]);
        int rowWidth = 0;
        for (int i = 0; i < len; i++) {
            rowWidth += this->_KKeyWidth(rows[r][i]);
            if (i < len - 1)
                { rowWidth += this->_KEYBOARD_SPACING; }
        }

        int startX = (this->_SCREEN_WIDTH - rowWidth) / 2;
        int x = startX;
        int y = startY + r * (this->_KEYBOARD_H + this->_KEYBOARD_SPACING);
        for (int i = 0; i < len; i++) {
            char c = rows[r][i];
            if (c == ' ') {
                x += this->_KEYBOARD_W + this->_KEYBOARD_SPACING;
                continue;
            }
            int keyW = this->_KKeyWidth(c);
            this->_KDrawKey(x, y, keyW, c);
            x += keyW + this->_KEYBOARD_SPACING;
        }
    }
}

void MSP4021::_KTextDraw() {
    int textW = this->_SCREEN_WIDTH - 20;
    this->rectFill(10, 45, textW, 40, this->rgb(0, 0, 0));
    this->rect(10, 46, textW, 38, this->rgb(255, 255, 255));
    this->textCenter(15, 47, textW, 40, this->_BUFFER);
}

int MSP4021::_KRowsCount() {
    bool portrait = this->_SCREEN_HEIGHT > this->_SCREEN_WIDTH;
    if (portrait) {
        return (this->_KEYBOARD_MODE == 0)
            ? rowsNumPortraitCount
            : (this->_KEYBOARD_MODE == 1)
                ? rowsUpperPortraitCount
                : rowsLowerPortraitCount;
    }
    return (this->_KEYBOARD_MODE == 0)
        ? rowsNumLandscapeCount
        : (this->_KEYBOARD_MODE == 1)
            ? rowsUpperLandscapeCount
            : rowsLowerLandscapeCount;
}

const char** MSP4021::_KRows() {
    bool portrait = this->_SCREEN_HEIGHT > this->_SCREEN_WIDTH;
    if (portrait) {
        return (this->_KEYBOARD_MODE == 0)
            ? rowsNumPortrait
            : (this->_KEYBOARD_MODE == 1)
                ? rowsUpperPortrait
                : rowsLowerPortrait;
    }
    return (this->_KEYBOARD_MODE == 0)
        ? rowsNumLandscape
        : (this->_KEYBOARD_MODE == 1)
            ? rowsUpperLandscape
            : rowsLowerLandscape;
}

int MSP4021::_KKeyWidth(char c) {
    bool portrait = this->_SCREEN_HEIGHT > this->_SCREEN_WIDTH;
    if (c == '_') {
        if (portrait)
            { return (this->_KEYBOARD_W * 5) + (this->_KEYBOARD_SPACING * 4); }
        return (this->_KEYBOARD_W * 4) + (this->_KEYBOARD_SPACING * 3);
    }
    if (c == '^' || c == '<')
        { return (this->_KEYBOARD_W * 2) + this->_KEYBOARD_SPACING; }
    return this->_KEYBOARD_W;
}

void MSP4021::_KDrawKey(int x, int y, int w, char c, bool pressed) {
    char str[2];
    const char* label = this->_KKeyLabel(c, str);
    uint16_t keyColor = this->_KKeyColor(c, pressed);

    this->rectFill(x, y, w, this->_KEYBOARD_H, keyColor);
    this->setTextColor(this->rgb(255, 255, 255));
    this->textCenter(x, y - 3, w, this->_KEYBOARD_H, label);
}

uint16_t MSP4021::_KKeyColor(char c, bool pressed) {
    if (pressed) { return this->rgb(0, 90, 200); }
    if (c == '^') { return this->rgb(0, 120, 255); }
    if (c == '<') { return this->rgb(255, 140, 0); }
    return this->rgb(45, 45, 45);
}

const char* MSP4021::_KKeyLabel(char c, char* str) {
    if (c == '^') { return "Shift"; }
    if (c == '<') { return "Del"; }
    if (c == '_') { return "SPACE"; }

    str[0] = c;
    str[1] = '\0';
    return str;
}

void MSP4021::_KLayoutMetrics(int& rowsCount, int& keyboardHeight, int& startY) {
    rowsCount = this->_KRowsCount();
    keyboardHeight = rowsCount * (this->_KEYBOARD_H + this->_KEYBOARD_SPACING) - this->_KEYBOARD_SPACING;
    startY = this->_SCREEN_HEIGHT - keyboardHeight - 10;
}

void MSP4021::KSetText(const char* text) {
    if (!text) {
        this->_BUFFER[0] = '\0';
        return;
    }
    strncpy(this->_BUFFER, text, sizeof(this->_BUFFER) - 1);
    this->_BUFFER[sizeof(this->_BUFFER) - 1] = '\0';
}

void MSP4021::KDraw(const char* title) {
    this->TPause();
    this->fillScreen(this->rgb(0,0,0));

    if (title && *title) {
        this->setFont(DejaVuSans_Bold_18);
        this->setTextColor(this->rgb(255, 255, 255));
        this->setTextScale(1);
        this->textCenter(10, 90, this->_SCREEN_WIDTH - 20, 20, title);
    }
    
    this->rectFill(0, 0, this->_SCREEN_WIDTH, 34, this->rgb(0, 180, 80));
    this->setFont(DejaVuSans_Bold_16);
    this->setTextColor(this->rgb(255, 255, 255));
    this->setTextScale(2);
    this->textCenter(0, 0, this->_SCREEN_WIDTH, 34, "OK");

    this->_KTextDraw();
    this->_KDraw();
    this->TResume();
}

bool MSP4021::KUpdate(int tx, int ty) {
    this->TPause();
    if (ty >= 10 && ty <= 34 && tx >= 10 && tx <= this->_SCREEN_WIDTH - 20) {
        this->TResume();
        this->_KEYBOARD_MODE = 1;
        return true;
    }

    const char** rows = this->_KRows();
    int rowsCount, keyboardHeight, startY;
    this->_KLayoutMetrics(rowsCount, keyboardHeight, startY);

    for (int r = 0; r < rowsCount; r++) {
        int len = strlen(rows[r]);
        int rowWidth = 0;

        for (int i = 0; i < len; i++) {
            rowWidth += this->_KKeyWidth(rows[r][i]);
            if (i < len - 1)
                { rowWidth += this->_KEYBOARD_SPACING; }
        }

        int startX = (this->_SCREEN_WIDTH - rowWidth) / 2;
        int x = startX;
        int y = startY + r * (this->_KEYBOARD_H + this->_KEYBOARD_SPACING);

        for (int i = 0; i < len; i++) {
            char c = rows[r][i];
            if (c == ' ') {
                x += this->_KEYBOARD_W + this->_KEYBOARD_SPACING;
                continue;
            }
            int keyW = this->_KKeyWidth(c);

            if (tx >= x && tx <= x + keyW && ty >= y && ty <= y + this->_KEYBOARD_H) {
                this->_KDrawKey(x, y, keyW, c, true);
                if (c == '^') {
                    (this->_KEYBOARD_MODE)++;
                    if (this->_KEYBOARD_MODE > 2)
                        { this->_KEYBOARD_MODE = 0; }

                    delay(150);
                    this->_KDraw();
                    this->TResume();
                    return false;
                }
                size_t len = strlen(this->_BUFFER);
                if (c == '_') {
                    if (len < 64) {
                        this->_BUFFER[len] = ' ';
                        this->_BUFFER[len + 1] = '\0';
                    }
                } else if (c == '<') {
                    if (len > 0)
                        { this->_BUFFER[len - 1] = '\0'; }
                } else {
                    if (len < 64) {
                        this->_BUFFER[len] = c;
                        this->_BUFFER[len + 1] = '\0';
                    }
                }

                this->_KTextDraw();
                delay(150);
                this->_KDrawKey(x, y, keyW, c);
                this->TResume();
                return false;
            }
            x += keyW + this->_KEYBOARD_SPACING;
        }
    }
    this->TResume();
    return false;
}

const char* MSP4021::KRead() {
    while (this->_BUFFER[0] == ' ')
        { memmove(this->_BUFFER, this->_BUFFER + 1, strlen(this->_BUFFER)); }
    size_t len = strlen(this->_BUFFER);

    while (len > 0 && this->_BUFFER[len - 1] == ' ') {
        this->_BUFFER[len - 1] = '\0';
        len--;
    }
    return this->_BUFFER;
}
