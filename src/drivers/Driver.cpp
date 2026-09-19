/*
 * src/drivers/Driver.cpp
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

#include "Driver.h"
#include "MSP4020.h"

namespace ST7796S {
namespace drivers {
    DetectionBus::DetectionBus(MSP4020& display)
        : _DISPLAY(display) {}

    void DetectionBus::readDirect(
        uint8_t command,
        uint8_t* data,
        int length,
        uint8_t dummyClockBits
    ) {
        this->_DISPLAY._readRegisterDirect(
            command,
            data,
            length,
            dummyClockBits
        );
    }

    uint8_t DetectionBus::readSelected(
        uint8_t command,
        uint8_t selectorCommand,
        uint8_t selectorValue,
        uint8_t resetValue
    ) {
        return this->_DISPLAY._readRegisterSelected(
            command,
            selectorCommand,
            selectorValue,
            resetValue
        );
    }

    DriverBus::DriverBus(MSP4020& display)
        : _DISPLAY(display) {}

    void DriverBus::begin() {
        this->_DISPLAY._transactionBegin();
    }

    void DriverBus::end() {
        this->_DISPLAY._transactionEnd();
    }

    void DriverBus::writeCommand(uint8_t command) {
        this->_DISPLAY._writeCmd(command);
    }

    void DriverBus::writeData(const uint8_t* data, int length) {
        this->_DISPLAY._writeData(data, length);
    }

    void DisplayDriver::setRotation(
        DriverBus& bus,
        uint8_t rotation
    ) const {
        static const uint8_t madctl[] = {0x48, 0x28, 0x88, 0xE8};
        bus.begin();
        bus.writeCommand(0x36);
        bus.writeData(&madctl[rotation], 1);
        bus.end();
    }

    void DisplayDriver::setAddress(
        DriverBus& bus,
        int x0,
        int y0,
        int x1,
        int y1
    ) const {
        uint8_t data[] = {
            static_cast<uint8_t>(x0 >> 8),
            static_cast<uint8_t>(x0),
            static_cast<uint8_t>(x1 >> 8),
            static_cast<uint8_t>(x1)
        };
        bus.writeCommand(0x2A);
        bus.writeData(data, sizeof(data));

        data[0] = y0 >> 8;
        data[1] = y0;
        data[2] = y1 >> 8;
        data[3] = y1;
        bus.writeCommand(0x2B);
        bus.writeData(data, sizeof(data));
        bus.writeCommand(0x2C);
    }

    uint32_t DisplayDriver::writeFrequency() const {
        return 4000000u;
    }

    uint8_t DisplayDriver::writeBitOrder() const {
        return MSBFIRST;
    }

    uint8_t DisplayDriver::writeDataMode() const {
        return SPI_MODE0;
    }

    uint8_t DisplayDriver::bytesPerPixel() const {
        return 2;
    }

    void DisplayDriver::encodePixels(
        const uint16_t* colors,
        int count,
        uint8_t* output
    ) const {
        for (int i = 0; i < count; i++) {
            output[i * 2] = colors[i] >> 8;
            output[i * 2 + 1] = colors[i] & 0xFF;
        }
    }
}
}
