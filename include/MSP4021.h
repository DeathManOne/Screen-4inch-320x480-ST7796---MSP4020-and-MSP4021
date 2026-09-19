/*
 * include/MSP4021.h
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
#include "MSP4020.h"

namespace ST7796S {
    class MSP4021 : public MSP4020 {
        private:
            SPISettings _SETTINGS = SPISettings(100000u, MSBFIRST, SPI_MODE0);
            char _BUFFER[65]        = {};
            bool _PAUSED            = false;
            int _PIN_CS             = -1;
            int _LAST_X             = -1;
            int _LAST_Y             = -1;
            int _KEYBOARD_W         = 40;
            int _KEYBOARD_H         = 40;
            int _KEYBOARD_MODE      = 1;
            int _KEYBOARD_SPACING   = 5;
            bool _INVERT_X          = false;
            bool _INVERT_Y          = false;
            bool _SWAP_XY           = false;
            bool _CALIBRATED        = false;
            float _COEFF_XA         = 1;
            float _COEFF_XB         = 0;
            float _COEFF_XC         = 0;
            float _COEFF_YA         = 0;
            float _COEFF_YB         = 1;
            float _COEFF_YC         = 0;
            static constexpr uint16_t TOUCH_PRESSURE_THRESHOLD = 120;
            inline bool _isTouched() { return this->_readPressure() >= TOUCH_PRESSURE_THRESHOLD; }
            uint16_t _readRaw(uint8_t cmd);
            uint16_t _readAverage(uint8_t cmd);
            uint16_t _readPressure();
            void _waitForTouch(uint16_t &x, uint16_t &y);
            void _waitRelease();
            void _drawCalibrationPoint(int x, int y, uint16_t &rx, uint16_t &ry, bool circle);
            void _detectOrientation();
            void _applyOrientation(uint16_t &x, uint16_t &y);
            bool _affineCalibration(float *sx, float *sy, float *rx, float *ry, int count);
            void _KDraw();
            void _KTextDraw();
            const char** _KRows();
            int _KRowsCount();
            int _KKeyWidth(char c);
            uint16_t _KKeyColor(char c, bool pressed = false);
            const char* _KKeyLabel(char c, char* str);
            void _KDrawKey(int x, int y, int w, char c, bool pressed = false);
            void _KLayoutMetrics(int& rowsCount, int& keyboardHeight, int& startY);
        public:
            /**
             * @brief Initializes an auto-detected TFT display with an XPT2046 touch controller.
             * 
             * @param spi SPI bus instance.
             * @param pinCS_TOUCH Touch controller chip select pin.
             * @param pinCS_TFT TFT display chip select pin.
             * @param pinDC_TFT TFT display data/command pin.
             * @param screenWidth Display width in pixels.
             * @param screenHeight Display height in pixels.
             * @param pinRST_TFT Optional TFT reset pin (-1 if unused).
             * @param pinMISO_TFT Optional dedicated TFT MISO pin used for
             *        controller detection when pinMOSI and pinCLK are also
             *        provided (-1 to use the hardware SPI read path).
             * @param pinMISO_TOUCH Optional touch controller MISO pin (-1 to use the SPI bus default).
             * @param pinMOSI Optional shared SPI MOSI pin (-1 to use the SPI bus default).
             * @param pinCLK Optional shared SPI clock pin (-1 to use the SPI bus default).
             */
            MSP4021(SPIClass &spi, int pinCS_TOUCH, int pinCS_TFT, int pinDC_TFT, int screenWidth, int screenHeight, int pinRST_TFT = -1, int pinMISO_TFT = -1, int pinMISO_TOUCH = -1, int pinMOSI = -1, int pinCLK = -1);
            
            /**
             * @brief Copy constructor disabled.
             */
            MSP4021(const MSP4021&) = delete;
            
            /**
             * @brief Copy assignment disabled.
             */
            MSP4021& operator=(const MSP4021&) = delete;
            
            /**
             * @brief Destroys the touch controller instance.
             */
            ~MSP4021() {}
            
            /**
             * @brief Temporarily disables the touch controller SPI access.
             * 
             * Useful before using another SPI device sharing the same bus.
             * Safe to call multiple times.
             */
            void TPause();

            /**
             * @brief Restores the touch controller SPI access after TPause().
             * 
             * Safe to call multiple times.
             */
            void TResume();
            
            /**
             * @brief Sets the keyboard input text buffer.
             * 
             * @param text UTF-8 text to set.
             */
            void KSetText(const char* text);
            
            /**
             * @brief Clears the keyboard text buffer.
             */
            inline void KClear() { this->_BUFFER[0] = '\0'; }
            
            /**
             * @brief Reads calibrated touch coordinates.
             * 
             * @param x Output X coordinate.
             * @param y Output Y coordinate.
             * @param disableSmoothing Disables the coordinate smoothing filter
             *        when true. The filter is disabled by default.
             * @return true while a valid touch is detected, including when the
             *         calibrated coordinates have not changed. Returns false
             *         until interactive or manual calibration has succeeded.
             */
            bool TRead(int &x, int &y, bool disableSmoothing = true);

            /**
             * @brief Reads the four uncalibrated XPT2046 measurements.
             *
             * This diagnostic function does not test whether the panel is
             * currently touched. Use Z1 and Z2 to inspect pressure data, or
             * use TRead() for normal calibrated touch input.
             *
             * @param x Output raw X measurement in the 12-bit ADC range.
             * @param y Output raw Y measurement in the 12-bit ADC range.
             * @param z1 Output raw Z1 pressure measurement.
             * @param z2 Output raw Z2 pressure measurement.
             * @return false when touch SPI access is paused. In that case all
             *         output values are set to zero; otherwise returns true.
             */
            bool TReadRaw(uint16_t &x, uint16_t &y, uint16_t &z1, uint16_t &z2);
            
            /**
             * @brief Starts the interactive touch calibration process.
             * 
             * @return true if calibration succeeded. Returns false when touch
             *         access is paused, no display controller was detected, or
             *         the affine calibration could not be calculated.
             */
            bool TCalibrate();
            
            /**
             * @brief Applies manual touch calibration values.
             * 
             * @param swapXY Swap X/Y axes.
             * @param invertX Invert X axis.
             * @param invertY Invert Y axis.
             * @param CXA Affine matrix coefficient XA.
             * @param CXB Affine matrix coefficient XB.
             * @param CXC Affine matrix coefficient XC.
             * @param CYA Affine matrix coefficient YA.
             * @param CYB Affine matrix coefficient YB.
             * @param CYC Affine matrix coefficient YC.
             */
            void TCalibrate(bool swapXY, bool invertX, bool invertY, float CXA, float CXB, float CXC, float CYA, float CYB, float CYC);
            
            /**
             * @brief Retrieves current touch calibration parameters.
             * 
             * @param swapXY Output swap XY flag.
             * @param invertX Output invert X flag.
             * @param invertY Output invert Y flag.
             * @param CXA Output affine matrix coefficient XA.
             * @param CXB Output affine matrix coefficient XB.
             * @param CXC Output affine matrix coefficient XC.
             * @param CYA Output affine matrix coefficient YA.
             * @param CYB Output affine matrix coefficient YB.
             * @param CYC Output affine matrix coefficient YC.
             */
            void TCalibrateInfo(bool &swapXY, bool &invertX, bool &invertY, float &CXA, float &CXB, float &CXC, float &CYA, float &CYB, float &CYC);
            
            /**
             * @brief Draws the virtual keyboard interface.
             * 
             * Displays the full keyboard, input field and optional title.
             * 
             * @param title Optional title displayed at the top of the keyboard.
             */
            void KDraw(const char* title = "");

            /**
             * @brief Updates the virtual keyboard state from touch coordinates.
             * 
             * @param tx Touch X coordinate.
             * @param ty Touch Y coordinate.
             * @return true if keyboard validation ("OK") was triggered.
             */
            bool KUpdate(int tx, int ty);
            
            /**
             * @brief Returns the current keyboard text buffer.
             * 
             * @return Current typed text.
             */
            const char* KRead();
    };
}
