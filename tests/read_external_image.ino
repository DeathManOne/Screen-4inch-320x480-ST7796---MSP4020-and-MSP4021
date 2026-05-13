/*
 * read_external_image.ino
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
#include <SD.h>
#include <MSP4020.h>

#define TFT_MISO -1
#define TFT_MOSI 4
#define TFT_SCK 5

#define TFT_CS 6
#define TFT_DC 7
#define TFT_RST -1 // 3.3v or an available pin you want

#define SD_CS 10
#define SD_MOSI 11
#define SD_MISO 12
#define SD_SCK 13

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480

SPIClass SPI_SD(FSPI);
SPIClass SPI_TFT(HSPI);

ST7796S::MSP4020 *_TFT;

bool drawImageFromSD(int x, int y, const char* path) {
    if (!path)
        { return false; }
    if (!String(path).endsWith(".rgb565"))
        { return false; }

    File file = SD.open(path, FILE_READ);
    if (!file)
        { return false; }
    bool ok = _TFT->imageSD(x, y, file);
    file.close();
    return ok;
}

void setup() {
    Serial.begin(9600);

    SPI_TFT.begin(TFT_SCK, TFT_MISO, TFT_MOSI);
    _TFT = new ST7796S::MSP4020(SPI_TFT, TFT_CS, TFT_DC, SCREEN_WIDTH, SCREEN_HEIGHT, -1);
    _TFT->setRotation(3);
    _TFT->fillScreen(_TFT->rgb(0, 0, 0));

    SPI_SD.begin(SD_SCK, SD_MISO, SD_MOSI);
    bool sdEnable = false;
    do { sdEnable = SD.begin(SD_CS, SPI_SD); }
    while (!sdEnable);

    drawImageFromSD(90 , 10, "/images/imgTest.rgb565");
}

void loop() {}