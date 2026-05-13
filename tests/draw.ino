/*
 * draw.ino
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

  _TFT->circle(50, 200, 30, _TFT->rgb(255, 0, 0)); // GBR
  _TFT->circleFill(50, 50, 30, _TFT->rgb(255, 0, 0));

  _TFT->triangleFill(100, 50, 100, 100, 150, 50, _TFT->rgb(255, 0, 0));

  _TFT->slider(20, 120, 280, 40, 75, _TFT->rgb(255, 0, 0));

  _TFT->arc(160, 240, 50, 0, 60, _TFT->rgb(255, 0, 0)); // GBR
  _TFT->arc(160, 240, 50, 61, 180, _TFT->rgb(0, 255, 0)); // GBR
  _TFT->arc(300, 240, 50, 0, 90, _TFT->rgb(255, 0, 0), false); // GBR
  _TFT->arc(300, 240, 50, 91, 180, _TFT->rgb(0, 0, 255), false); // GBR
  
  _TFT->progressBar(240, 30, 200, 30, 0, _TFT->rgb(255, 0, 0));

  _TFT->rect(240, 65, 50, 20, _TFT->rgb(255, 0, 0));
  _TFT->rectFill(240, 90, 50, 20, _TFT->rgb(255, 0, 0));
}

void loop() {
  _TFT->rectFill(240, 30, 200, 30, _TFT->rgb(45, 45, 45));
  for (int i = 0; i <= 100; i++) {
    _TFT->progressBar(240, 30, 200, 30, i, _TFT->rgb(255, 0, 0));
    delay(50);
  }
}