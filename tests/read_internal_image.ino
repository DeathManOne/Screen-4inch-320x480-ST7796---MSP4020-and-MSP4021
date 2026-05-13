/*
 * read_internal_image.ino
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
#include <MSP4020.h>
#include <images/imgTest.h>

#define TFT_CS 10
#define TFT_DC 5
#define TFT_RST -1 // 3.3v or an available pin you want

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480

ST7796S::MSP4020 *_TFT;

void setup() {
  SPI.begin();

  _TFT = new ST7796S::MSP4020(SPI, TFT_CS, TFT_DC, SCREEN_WIDTH, SCREEN_HEIGHT, TFT_RST);
  _TFT->setRotation(3);
  _TFT->fillScreen(_TFT->rgb(0, 0, 0));
  _TFT->image(90, 10, ST7796S::imgTest);
}

void loop() {}