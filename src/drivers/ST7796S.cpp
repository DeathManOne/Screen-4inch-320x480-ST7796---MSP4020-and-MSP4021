/*
 * src/drivers/ST7796S.cpp
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
#include "drivers/Detection.h"
#include "ST7796S.h"

using namespace ST7796S;

namespace ST7796S {
namespace drivers {
    class ST7796SDriver final : public DisplayDriver {
        public:
            const char* name() const override {
                return "ST7796S";
            }

            bool detect(
                DetectionBus& bus,
                DetectionReport& report
            ) const override {
                uint8_t* indexedD3 = report.add("Indexed 0xD3 :", 4);
                if (!indexedD3)
                    { return false; }
                for (uint8_t i = 0; i < 4; i++) {
                    indexedD3[i] = bus.readSelected(
                        0xD3,
                        0xFB,
                        0x10 | i,
                        0x00
                    );
                }
                return
                    indexedD3[2] == 0x77 &&
                    indexedD3[3] == 0x96;
            }

            void init(DriverBus& bus) const override;

    };

    const DisplayDriver& st7796s() {
        static const ST7796SDriver driver;
        return driver;
    }
}
}

void drivers::ST7796SDriver::init(drivers::DriverBus& bus) const {
    uint8_t d;
    bus.begin();
    bus.writeCommand(0x01);
    delay(120);
    bus.writeCommand(0x11);
    delay(120);
    d = 0xC3;
    bus.writeCommand(0xF0);
    bus.writeData(&d, 1);
    d = 0x96;
    bus.writeCommand(0xF0);
    bus.writeData(&d, 1);
    d = 0x48;
    bus.writeCommand(0x36);
    bus.writeData(&d, 1);
    d = 0x05;
    bus.writeCommand(0x3A);
    bus.writeData(&d, 1);
    d = 0x01;
    bus.writeCommand(0xB4);
    bus.writeData(&d, 1);
    uint8_t b6[] = {0x80,0x02,0x3B};
    bus.writeCommand(0xB6);
    bus.writeData(b6, 3);
    uint8_t e8[] = {0x40,0x8A,0x00,0x00,0x29,0x19,0xA5,0x33};
    bus.writeCommand(0xE8);
    bus.writeData(e8, 8);
    d = 0x06;
    bus.writeCommand(0xC1);
    bus.writeData(&d, 1);
    d = 0xA7;
    bus.writeCommand(0xC2);
    bus.writeData(&d, 1);
    d = 0x18;
    bus.writeCommand(0xC5);
    bus.writeData(&d, 1);
    delay(120);
    uint8_t gammaPos[] = {
        0xF0,0x09,0x0b,0x06,0x04,0x15,0x2F,
        0x54,0x42,0x3C,0x17,0x14,0x18,0x1B
    };
    bus.writeCommand(0xE0);
    bus.writeData(gammaPos, 14);
    uint8_t gammaNeg[] = {
        0xE0,0x09,0x0B,0x06,0x04,0x03,0x2B,
        0x43,0x42,0x3B,0x16,0x14,0x17,0x1B
    };
    bus.writeCommand(0xE1);
    bus.writeData(gammaNeg, 14);
    delay(120);
    d = 0x3C;
    bus.writeCommand(0xF0);
    bus.writeData(&d, 1);
    d = 0x69;
    bus.writeCommand(0xF0);
    bus.writeData(&d, 1);
    bus.writeCommand(0x29);
    bus.end();
}
