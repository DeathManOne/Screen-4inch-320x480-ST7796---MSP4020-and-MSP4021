/*
 * examples/fonts.ino
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

#include <fonts/DejaVuSans_12.h>
#include <fonts/DejaVuSans_16.h>
#include <fonts/DejaVuSans_Bold_12.h>
#include <fonts/DejaVuSans_Bold_16.h>
#include <fonts/DejaVuSans_Bold_18.h>
#include <fonts/DejaVuSans_Bold_24.h>

#define TFT_CS 10
#define TFT_DC 5
#define TFT_RST -1 // 3.3v or an available pin you want
#define TFT_MISO A3
#define TFT_MOSI 11
#define TFT_CLK 13

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480

ST7796S::MSP4020 *_TFT;

void setup() {
  SPI.begin(TFT_CLK, TFT_MISO, TFT_MOSI);

  _TFT = new ST7796S::MSP4020(
    SPI, TFT_CS, TFT_DC, SCREEN_WIDTH, SCREEN_HEIGHT, TFT_RST,
    TFT_MISO, -1, TFT_MOSI, TFT_CLK
  );
  _TFT->setRotation(3);
  _TFT->fillScreen(_TFT->rgb(0, 0, 0));

  _TFT->setTextColor(_TFT->rgb(0, 0, 255));
  _TFT->setFont(ST7796S::DejaVuSans_12);
  _TFT->text(10, 20, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

  _TFT->setFont(ST7796S::DejaVuSans_16);
  _TFT->text(10, 40, "abcdefghijklmnopqrstuvwxyz");

  _TFT->setTextColor(_TFT->rgb(255, 0, 0));
  _TFT->setFont(ST7796S::DejaVuSans_Bold_12);
  _TFT->text(10, 60, "0123456789");

  _TFT->setFont(ST7796S::DejaVuSans_Bold_16);
  _TFT->text(10, 80, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

  _TFT->setTextColor(_TFT->rgb(0, 255, 0));
  _TFT->setFont(ST7796S::DejaVuSans_Bold_18);
  _TFT->text(10, 100, "abcdefghijklmnopqrstuvwxyz");

  _TFT->setFont(ST7796S::DejaVuSans_Bold_24);
  _TFT->text(10, 120, "0123456789");
}

void loop() {}
