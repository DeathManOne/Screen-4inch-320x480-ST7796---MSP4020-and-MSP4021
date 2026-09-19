/*
 * src/drivers/ILI9488.cpp
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
#include "ILI9488.h"

using namespace ST7796S;

namespace ST7796S {
namespace drivers {
    class ILI9488Driver final : public DisplayDriver {
        public:
            const char* name() const override {
                return "ILI9488";
            }

            bool detect(
                DetectionBus& bus,
                DetectionReport& report
            ) const override {
                uint8_t* direct04 = report.add("Direct  0x04 :", 4);
                if (!direct04)
                    { return false; }
                bus.readDirect(
                    0x04,
                    direct04,
                    4,
                    1
                );
                const uint8_t* id = direct04;
                bool shifted =
                    id[0] == 0x2A &&
                    id[1] == 0x40 &&
                    id[2] == 0x33;
                bool native =
                    (id[0] == 0x54 && id[1] == 0x80 && id[2] == 0x66) ||
                    (id[1] == 0x54 && id[2] == 0x80 && id[3] == 0x66);
                return shifted || native;
            }

            void init(DriverBus& bus) const override;

            uint8_t bytesPerPixel() const override {
                return 3;
            }

            void encodePixels(
                const uint16_t* colors,
                int count,
                uint8_t* output
            ) const override {
                for (int i = 0; i < count; i++) {
                    uint16_t color = colors[i];
                    output[i * 3] = ((color >> 11) & 0x1F) << 3;
                    output[i * 3 + 1] = ((color >> 5) & 0x3F) << 2;
                    output[i * 3 + 2] = (color & 0x1F) << 3;
                }
            }
    };

    const DisplayDriver& ili9488() {
        static const ILI9488Driver driver;
        return driver;
    }
}
}

void drivers::ILI9488Driver::init(drivers::DriverBus& bus) const {
    bus.begin();
    bus.writeCommand(0x01);
    delay(120);

    uint8_t gammaPos[] = {
        0x00,0x03,0x09,0x08,0x16,0x0A,0x3F,0x78,
        0x4C,0x09,0x0A,0x08,0x16,0x1A,0x0F
    };
    bus.writeCommand(0xE0);
    bus.writeData(gammaPos, sizeof(gammaPos));

    uint8_t gammaNeg[] = {
        0x00,0x16,0x19,0x03,0x0F,0x05,0x32,0x45,
        0x46,0x04,0x0E,0x0D,0x35,0x37,0x0F
    };
    bus.writeCommand(0xE1);
    bus.writeData(gammaNeg, sizeof(gammaNeg));

    uint8_t d[] = {0x17,0x15};
    bus.writeCommand(0xC0);
    bus.writeData(d, 2);

    d[0] = 0x41;
    bus.writeCommand(0xC1);
    bus.writeData(d, 1);

    uint8_t c5[] = {0x00,0x12,0x80};
    bus.writeCommand(0xC5);
    bus.writeData(c5, sizeof(c5));

    d[0] = 0x48;
    bus.writeCommand(0x36);
    bus.writeData(d, 1);

    d[0] = 0x66;
    bus.writeCommand(0x3A);
    bus.writeData(d, 1);

    d[0] = 0x00;
    bus.writeCommand(0xB0);
    bus.writeData(d, 1);

    d[0] = 0xA0;
    bus.writeCommand(0xB1);
    bus.writeData(d, 1);

    d[0] = 0x02;
    bus.writeCommand(0xB4);
    bus.writeData(d, 1);

    uint8_t b6[] = {0x02,0x02,0x3B};
    bus.writeCommand(0xB6);
    bus.writeData(b6, sizeof(b6));

    d[0] = 0xC6;
    bus.writeCommand(0xB7);
    bus.writeData(d, 1);

    uint8_t f7[] = {0xA9,0x51,0x2C,0x82};
    bus.writeCommand(0xF7);
    bus.writeData(f7, sizeof(f7));

    bus.writeCommand(0x11);
    delay(120);
    bus.writeCommand(0x29);
    delay(25);
    bus.end();
}
