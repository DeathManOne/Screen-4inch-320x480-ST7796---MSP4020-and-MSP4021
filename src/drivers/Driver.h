/*
 * src/drivers/Driver.h
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

#pragma once

#include <cstdint>

namespace ST7796S {
    class MSP4020;
    struct DetectionReport;
}

namespace ST7796S {
namespace drivers {
    /**
     * @brief Restricted transport used by controller-detection routines.
     */
    class DetectionBus {
        private:
            MSP4020& _DISPLAY;

        public:
            /** @brief Binds the detection transport to a display instance. */
            explicit DetectionBus(MSP4020& display);

            /**
             * @brief Reads bytes directly from a controller register.
             *
             * @param command Register read command.
             * @param data Destination buffer.
             * @param length Number of bytes to read.
             * @param dummyClockBits Clock bits discarded before the response.
             */
            void readDirect(
                uint8_t command,
                uint8_t* data,
                int length,
                uint8_t dummyClockBits = 0
            );

            /**
             * @brief Reads one byte after selecting a register index.
             *
             * @param command Register read command.
             * @param selectorCommand Index-selector command.
             * @param selectorValue Index-selector value.
             * @param resetValue Value restoring the selector after the read.
             * @return Byte returned by the controller.
             */
            uint8_t readSelected(
                uint8_t command,
                uint8_t selectorCommand,
                uint8_t selectorValue,
                uint8_t resetValue
            );
    };

    /**
     * @brief Restricted write transport exposed to display drivers.
     */
    class DriverBus {
        private:
            MSP4020& _DISPLAY;

        public:
            /** @brief Binds the write transport to a display instance. */
            explicit DriverBus(MSP4020& display);

            /** @brief Starts a display SPI transaction. */
            void begin();

            /** @brief Ends the current display SPI transaction. */
            void end();

            /** @brief Sends one controller command. */
            void writeCommand(uint8_t command);

            /** @brief Sends a data buffer to the controller. */
            void writeData(const uint8_t* data, int length);
    };

    /**
     * @brief Common interface implemented by every display controller driver.
     */
    class DisplayDriver {
        public:
            /** Maximum encoded size supported by the shared pixel buffers. */
            static constexpr uint8_t MAX_BYTES_PER_PIXEL = 3;

            virtual ~DisplayDriver() = default;

            /** @return Stable human-readable controller name. */
            virtual const char* name() const = 0;

            /**
             * @brief Probes the controller and appends diagnostic results.
             * @param bus Detection transport.
             * @param report Report receiving the probe results.
             * @return true when the controller signature matches this driver.
             */
            virtual bool detect(
                DetectionBus& bus,
                DetectionReport& report
            ) const = 0;

            /** @brief Initializes the detected controller. @param bus Write transport. */
            virtual void init(DriverBus& bus) const = 0;

            /** @brief Applies a rotation value from 0 to 3. @param bus Write transport. @param rotation Rotation value. */
            virtual void setRotation(DriverBus& bus, uint8_t rotation) const;

            /**
             * @brief Selects the inclusive pixel-write window.
             * @param bus Write transport.
             * @param x0 First column.
             * @param y0 First row.
             * @param x1 Last column.
             * @param y1 Last row.
             */
            virtual void setAddress(
                DriverBus& bus,
                int x0,
                int y0,
                int x1,
                int y1
            ) const;

            /** @return SPI write frequency in hertz. */
            virtual uint32_t writeFrequency() const;

            /** @return SPI bit-order constant. */
            virtual uint8_t writeBitOrder() const;

            /** @return SPI data-mode constant. */
            virtual uint8_t writeDataMode() const;

            /** @return Number of encoded bytes written per pixel. */
            virtual uint8_t bytesPerPixel() const;

            /**
             * @brief Converts RGB565 source pixels to the controller format.
             * @param colors RGB565 source buffer.
             * @param count Number of pixels to convert.
             * @param output Encoded destination buffer.
             */
            virtual void encodePixels(
                const uint16_t* colors,
                int count,
                uint8_t* output
            ) const;
    };
}
}
