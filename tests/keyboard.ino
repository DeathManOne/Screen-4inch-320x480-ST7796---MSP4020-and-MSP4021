/*
 * keyboard.ino
 *
 * Copyright (c) 2026 DeathManOne
 * https://github.com/DeathManOne
 *
 * This file is part of the ST7796S library.
 *
 * Licensed under the GNU GPL v3 or later.
 * See LICENSE file for details.
 */

#include <Arduino.h>
#include <SPI.h>
#include <MSP4021.h>

#define TFT_CS 10
#define TFT_DC 5
#define TFT_RST -1 // 3.3v or an available pin you want

#define TOUCH_CS 4

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480

ST7796S::MSP4021 *_TFT;

void setup() {
    SPI.begin();

    _TFT = new ST7796S::MSP4021(SPI, TOUCH_CS, TFT_CS, TFT_DC, SCREEN_WIDTH, SCREEN_HEIGHT, TFT_RST);
    _TFT->setRotation(3);
    while (!_TFT->TCalibrate());
    _TFT->KDraw("TEST your KeyBoard");
}

void loop() {
    int x, y;
    if (_TFT->TRead(x, y)) {
        if (_TFT->KUpdate(x, y)) {
            Serial.println(_TFT->KRead().c_str());
            _TFT->KClear();
            _TFT->KDraw("TEST your KeyBoard");
            delay(2000);
        }
    }
}