/*
 * src/lowLevel.cpp
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

#include <Arduino.h>
#include <algorithm>
#include <cstring>
#include "MSP4020.h"
#include "drivers/Driver.h"
#include "drivers/Registry.h"
using namespace ST7796S;

MSP4020::MSP4020(SPIClass &spi, int pinCS, int pinDC, int screenWidth, int screenHeight, int pinRST, int pinMISO_TFT, int pinMISO, int pinMOSI, int pinCLK, int pinSharedCS) {
    this->_PIN_CS = pinCS;
    this->_PIN_DC = pinDC;
    this->_PIN_RST = pinRST;
    this->_PIN_MISO_TFT = pinMISO_TFT;
    this->_PIN_MISO = pinMISO;
    this->_PIN_MOSI = pinMOSI;
    this->_PIN_CLK = pinCLK;
    this->_PIN_SHARED_CS = pinSharedCS;

    if (screenWidth > screenHeight) {
        this->_RAW_WIDTH = screenHeight;
        this->_RAW_HEIGHT = screenWidth;
        this->_SCREEN_WIDTH = screenHeight;
        this->_SCREEN_HEIGHT = screenWidth;
    } else {
        this->_RAW_WIDTH = screenWidth;
        this->_RAW_HEIGHT = screenHeight;
        this->_SCREEN_WIDTH = screenWidth;
        this->_SCREEN_HEIGHT = screenHeight;
    }

    this->_SPI = &spi;
    if (this->_PIN_RST != -1)
        { pinMode(this->_PIN_RST, OUTPUT); }
    pinMode(this->_PIN_CS, OUTPUT);
    pinMode(this->_PIN_DC, OUTPUT);
    digitalWrite(this->_PIN_CS, HIGH);
    if (this->_PIN_SHARED_CS != -1) {
        pinMode(this->_PIN_SHARED_CS, OUTPUT);
        digitalWrite(this->_PIN_SHARED_CS, HIGH);
    }
    if (this->_PIN_MISO_TFT != -1 && this->_PIN_MOSI != -1 && this->_PIN_CLK != -1) {
        pinMode(this->_PIN_MISO_TFT, INPUT);
        pinMode(this->_PIN_MOSI, OUTPUT);
        pinMode(this->_PIN_CLK, OUTPUT);
        digitalWrite(this->_PIN_CLK, LOW);
    }
    this->_init();
}

void MSP4020::_init() {
    if (this->_PIN_MISO_TFT != -1 && this->_PIN_MOSI != -1 && this->_PIN_CLK != -1) {
        pinMode(this->_PIN_MISO_TFT, INPUT);
        pinMode(this->_PIN_MOSI, OUTPUT);
        pinMode(this->_PIN_CLK, OUTPUT);
        digitalWrite(this->_PIN_CLK, LOW);
    }

    if (this->_PIN_RST != -1) {
        digitalWrite(this->_PIN_RST, HIGH);
        delay(5);
        digitalWrite(this->_PIN_RST, LOW);
        delay(15);
        digitalWrite(this->_PIN_RST, HIGH);
        delay(15);
    }

    this->_DRIVER = this->_detectDriver();
    if (this->_PIN_MISO_TFT != -1 && this->_PIN_MOSI != -1 && this->_PIN_CLK != -1) {
        this->_SPI->end();
        this->_SPI->begin(this->_PIN_CLK, this->_PIN_MISO, this->_PIN_MOSI);
    }
    if (this->_DRIVER) {
        this->_SETTINGS = SPISettings(
            this->_DRIVER->writeFrequency(),
            this->_DRIVER->writeBitOrder(),
            this->_DRIVER->writeDataMode()
        );
        drivers::DriverBus bus(*this);
        this->_DRIVER->init(bus);
    }
}

const char* MSP4020::controllerName() const {
    return this->_DRIVER ? this->_DRIVER->name() : "UNKNOWN";
}

const drivers::DisplayDriver* MSP4020::_detectDriver() {
    drivers::DetectionBus bus(*this);
    return drivers::detect(bus, this->_DETECTION_REPORT);
}

void MSP4020::printDetection(Print& output) const {
    for (uint8_t lineIndex = 0;
         lineIndex < this->_DETECTION_REPORT.count;
         lineIndex++) {
        const DetectionLine& line =
            this->_DETECTION_REPORT.lines[lineIndex];
        output.print(line.label);
        for (uint8_t i = 0; i < line.length; i++) {
            output.print(' ');
            if (line.data[i] < 0x10)
                { output.print('0'); }
            output.print(line.data[i], HEX);
        }
        output.println();
    }
}

void MSP4020::_transactionBegin() {
    this->_SPI->beginTransaction(this->_SETTINGS);
    this->_start();
}

void MSP4020::_transactionEnd() {
    this->_stop();
    this->_SPI->endTransaction();
}

void MSP4020::_writeCmd(uint8_t cmd) {
    this->_DC_CMD();
    this->_SPI->transfer(cmd);
}

void MSP4020::_writeData(const uint8_t *data, int length) {
    this->_DC_DATA();
    this->_SPI->writeBytes(data, length);
}

void MSP4020::_writeSoftwareSPI(uint8_t value) {
    for (int bit = 7; bit >= 0; bit--) {
        digitalWrite(this->_PIN_MOSI, (value >> bit) & 0x01);
        digitalWrite(this->_PIN_CLK, HIGH);
        digitalWrite(this->_PIN_CLK, LOW);
    }
}

uint8_t MSP4020::_readSoftwareSPI() {
    uint8_t value = 0;
    for (int bit = 7; bit >= 0; bit--) {
        if (digitalRead(this->_PIN_MISO_TFT))
            { value |= 1u << bit; }
        digitalWrite(this->_PIN_CLK, HIGH);
        digitalWrite(this->_PIN_CLK, LOW);
    }
    return value;
}

void MSP4020::_writeColor(uint16_t color, int count) {
    if (!this->_DRIVER || count <= 0)
        { return; }

    constexpr int PIXELS_PER_CHUNK = 64;
    int bytesPerPixel = this->_DRIVER->bytesPerPixel();
    if (
        bytesPerPixel <= 0 ||
        bytesPerPixel > drivers::DisplayDriver::MAX_BYTES_PER_PIXEL
    ) {
        return;
    }
    uint8_t data[
        PIXELS_PER_CHUNK * drivers::DisplayDriver::MAX_BYTES_PER_PIXEL
    ];
    uint8_t encoded[drivers::DisplayDriver::MAX_BYTES_PER_PIXEL];
    this->_DRIVER->encodePixels(&color, 1, encoded);

    for (int i = 0; i < PIXELS_PER_CHUNK; i++)
        { memcpy(&data[i * bytesPerPixel], encoded, bytesPerPixel); }

    while (count > 0) {
        int chunk = count > PIXELS_PER_CHUNK
            ? PIXELS_PER_CHUNK
            : count;
        this->_writeData(data, chunk * bytesPerPixel);
        count -= chunk;
    }
}

void MSP4020::_writePixels(const uint16_t *colors, int count) {
    if (!this->_DRIVER || !colors || count <= 0)
        { return; }

    constexpr int PIXELS_PER_CHUNK = 64;
    int bytesPerPixel = this->_DRIVER->bytesPerPixel();
    if (
        bytesPerPixel <= 0 ||
        bytesPerPixel > drivers::DisplayDriver::MAX_BYTES_PER_PIXEL
    ) {
        return;
    }
    uint8_t data[
        PIXELS_PER_CHUNK * drivers::DisplayDriver::MAX_BYTES_PER_PIXEL
    ];
    int offset = 0;

    while (count > 0) {
        int chunk = count > PIXELS_PER_CHUNK
            ? PIXELS_PER_CHUNK
            : count;

        this->_DRIVER->encodePixels(&colors[offset], chunk, data);
        this->_writeData(data, chunk * bytesPerPixel);

        offset += chunk;
        count -= chunk;
    }
}

void MSP4020::_readRegisterDirect(
    uint8_t cmd,
    uint8_t *data,
    int length,
    uint8_t dummyClockBits
) {
    if (this->_PIN_MISO_TFT != -1 && this->_PIN_MOSI != -1 && this->_PIN_CLK != -1) {
        this->_start();
        this->_DC_CMD();
        this->_writeSoftwareSPI(cmd);
        this->_DC_DATA();

        for (uint8_t i = 0; i < dummyClockBits; i++) {
            digitalWrite(this->_PIN_CLK, HIGH);
            digitalWrite(this->_PIN_CLK, LOW);
        }

        for (int i = 0; i < length; i++)
            { data[i] = this->_readSoftwareSPI(); }
        this->_stop();
        return;
    }

    this->_SPI->beginTransaction(this->_READ_SETTINGS);
    this->_start();
    this->_writeCmd(cmd);
    this->_DC_DATA();
    for (int i = 0; i < length; i++)
        { data[i] = this->_SPI->transfer(0x00); }
    this->_stop();
    this->_SPI->endTransaction();
}

uint8_t MSP4020::_readRegisterSelected(
    uint8_t cmd,
    uint8_t selectorCommand,
    uint8_t selectorValue,
    uint8_t resetValue
) {
    if (this->_PIN_MISO_TFT != -1 && this->_PIN_MOSI != -1 && this->_PIN_CLK != -1) {
        this->_start();
        this->_DC_CMD();
        this->_writeSoftwareSPI(selectorCommand);
        this->_DC_DATA();
        this->_writeSoftwareSPI(selectorValue);
        this->_stop();

        this->_start();
        this->_DC_CMD();
        this->_writeSoftwareSPI(cmd);
        this->_DC_DATA();
        uint8_t value = this->_readSoftwareSPI();
        this->_stop();

        this->_start();
        this->_DC_CMD();
        this->_writeSoftwareSPI(selectorCommand);
        this->_DC_DATA();
        this->_writeSoftwareSPI(resetValue);
        this->_stop();
        return value;
    }

    this->_SPI->beginTransaction(this->_READ_SETTINGS);

    this->_start();
    this->_writeCmd(selectorCommand);
    this->_writeData(&selectorValue, 1);
    this->_stop();

    this->_start();
    this->_writeCmd(cmd);
    this->_DC_DATA();
    uint8_t value = this->_SPI->transfer(0x00);
    this->_stop();

    this->_start();
    this->_writeCmd(selectorCommand);
    this->_writeData(&resetValue, 1);
    this->_stop();

    this->_SPI->endTransaction();
    return value;
}

void MSP4020::_setAddress(int x0, int y0, int x1, int y1) {
    if (x0 > x1)
        { std::swap(x0, x1); }
    if (y0 > y1)
        { std::swap(y0, y1); }
    if (!this->_DRIVER)
        { return; }
    drivers::DriverBus bus(*this);
    this->_DRIVER->setAddress(bus, x0, y0, x1, y1);
}

void MSP4020::_swapBuffers() {
    uint16_t *tmp = this->_BUFFER_CPU;
    this->_BUFFER_CPU = this->_BUFFER_DMA;
    this->_BUFFER_DMA = tmp;
}

void MSP4020::shutdown() {
    if (this->_PIN_RST != -1) {
        digitalWrite(this->_PIN_RST, LOW);
        return;
    }
    this->fillScreen(this->rgb(0, 0, 0));
}

void MSP4020::startup() {
    if (this->_PIN_RST == -1)
        { return; }
    int rotation = this->_SCREEN_ROTATION;

    this->_SCREEN_ROTATION = -1;
    this->_init();
    this->setRotation(rotation);
}

void MSP4020::reset() {
    if (this->_PIN_RST == -1) {
        this->fillScreen(this->rgb(0, 0, 0));
        return;
    }
    this->shutdown();
    this->startup();
}

void MSP4020::setRotation(int rotation) {
    rotation = (rotation % 4 + 4) % 4;
    if (this->_SCREEN_ROTATION == rotation)
        { return; }
    this->_SCREEN_ROTATION = rotation;

    if (this->_SCREEN_ROTATION % 2 == 0) {
        this->_SCREEN_WIDTH = this->_RAW_WIDTH;
        this->_SCREEN_HEIGHT = this->_RAW_HEIGHT;
    } else {
        this->_SCREEN_WIDTH = this->_RAW_HEIGHT;
        this->_SCREEN_HEIGHT = this->_RAW_WIDTH;
    }
    if (this->_DRIVER) {
        drivers::DriverBus bus(*this);
        this->_DRIVER->setRotation(bus, this->_SCREEN_ROTATION);
    }
}
