/*
 * MSP4020.h
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

#ifndef _ST7796S_
#define _ST7796S_

#include <Arduino.h>
#include <SPI.h>
#include "../src/fonts/font.h"
#include "../src/images/image.h"

namespace ST7796S {
    class MSP4020 {
        private:
            SPISettings *_SETTINGS;
            int *_PIN_CS, *_PIN_DC, *_PIN_RST, *_RAW_WIDTH, *_RAW_HEIGHT;
            static const int _BUFFER_SIZE = 1024;
            uint16_t _BUFFER_A[_BUFFER_SIZE], _BUFFER_B[_BUFFER_SIZE];
            uint16_t *_BUFFER_CPU = _BUFFER_A, *_BUFFER_DMA = _BUFFER_B;
            const Font *_FONT;
            uint8_t *_TEXT_SCALE;
            uint16_t *_TEXT_COLOR;
            inline void _start() { digitalWrite(*this->_PIN_CS, LOW); }
            inline void _stop() { digitalWrite(*this->_PIN_CS, HIGH); }
            inline void _DC_CMD() { digitalWrite(*this->_PIN_DC, LOW); }
            inline void _DC_DATA() { digitalWrite(*this->_PIN_DC, HIGH); }
            void _init();
            void _transactionBegin();
            void _transactionEnd();
            void _writeCmd(uint8_t cmd);
            void _writeData(const uint8_t *data, int length);
            void _setAddress(int x0, int y0, int x1, int y1);
            void _swapBuffers();
            void _pixel(int x, int y, uint16_t color);
            void _line(int x0, int y0, int x1, int y1, uint16_t color);
            void _char(uint16_t x, uint16_t y, uint16_t myChar);
            void _charBounds(uint16_t c, int &w, int &h, int &yOff);
            void _image(int x, int y, int w, int h, const uint16_t* img);
        protected:
            int *_SCREEN_WIDTH, *_SCREEN_HEIGHT, *_SCREEN_ROTATION;
            SPIClass *_SPI;
        public:
            /**
             * @brief Initializes the ST7796S TFT display driver.
             * 
             * @param spi SPI bus instance.
             * @param pinCS TFT chip select pin.
             * @param pinDC TFT data/command pin.
             * @param screenWidth Display width in pixels.
             * @param screenHeight Display height in pixels.
             * @param pinRST Optional reset pin (-1 if unused).
             */
            MSP4020(SPIClass &spi, int pinCS, int pinDC, int screenWidth, int screenHeight, int pinRST = -1);

            /**
             * @brief Copy constructor disabled.
             */
            MSP4020(const MSP4020&) = delete;

            /**
             * @brief Copy assignment disabled.
             */
            MSP4020& operator=(const MSP4020&) = delete;

            /**
             * @brief Destroys the TFT driver instance.
             */
            ~MSP4020();

            /**
             * @brief Converts RGB888 color to RGB565 display format.
             * 
             * @param red Red component.
             * @param green Green component.
             * @param blue Blue component.
             * @return RGB565 encoded color.
             */
            inline uint16_t rgb(uint8_t red, uint8_t green, uint8_t blue) { return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3); }
            
            /**
             * @brief Fills the entire screen with a color.
             * 
             * @param color RGB565 color.
             */
            inline void fillScreen(uint16_t color) { this->rectFill(0, 0, *this->_SCREEN_WIDTH, *this->_SCREEN_HEIGHT, color); }

            /**
             * @brief Draws an image stored in flash memory.
             * 
             * @param x Image position X.
             * @param y Image position Y.
             * @param img Image structure.
             */
            inline void image(int x, int y, const Image& img) { this->_image(x, y, img.width, img.height, img.data); }

            /**
             * @brief Sets the active font.
             * 
             * @param font Font structure.
             */
            inline void setFont(const Font& font) { this->_FONT = &font; }

            /**
             * @brief Sets the current text color.
             * 
             * @param color RGB565 color.
             */
            inline void setTextColor(uint16_t color) { *this->_TEXT_COLOR = color; }

            /**
             * @brief Sets the current text scale factor.
             * 
             * @param scale Text scale multiplier.
             */
            inline void setTextScale(uint8_t scale) { *this->_TEXT_SCALE = (scale < 1) ? 1 : scale; }

            /**
             * @brief Sets the display rotation.
             * 
             * @param rotation Rotation value from 0 to 3.
             */
            void setRotation(int rotation = 0);

            /**
             * @brief Draws UTF-8 text.
             * 
             * @param x Text position X.
             * @param y Text position Y.
             * @param str UTF-8 string.
             */
            void text(uint16_t x, uint16_t y, const char* str);

            /**
             * @brief Draws centered UTF-8 text inside a rectangle area.
             * 
             * @param x Area position X.
             * @param y Area position Y.
             * @param w Area width.
             * @param h Area height.
             * @param str UTF-8 string.
             */
            void textCenter(int x, int y, int w, int h, const char* str);

            /**
             * @brief Draws a RGB565 image from a Stream source.
             * 
             * Stream format:
             * - uint16_t width  (little-endian)
             * - uint16_t height (little-endian)
             * - RGB565 pixel data (big-endian)
             * 
             * @param x Image position X.
             * @param y Image position Y.
             * @param file Input stream.
             * @return true if image was successfully drawn.
             */
            bool imageSD(int x, int y, Stream& file);

            /**
             * @brief Draws a horizontal line.
             * 
             * @param x Start X position.
             * @param y Y position.
             * @param width Line width.
             * @param color RGB565 color.
             */
            void lineH(int x, int y, int width, uint16_t color);

            /**
             * @brief Draws a vertical line.
             * 
             * @param x X position.
             * @param y Start Y position.
             * @param height Line height.
             * @param color RGB565 color.
             */
            void lineV(int x, int y, int height, uint16_t color);

            /**
             * @brief Draws a circle outline.
             * 
             * @param x0 Center X position.
             * @param y0 Center Y position.
             * @param r Circle radius.
             * @param color RGB565 color.
             */
            void circle(int x0, int y0, int r, uint16_t color);

            /**
             * @brief Draws a filled circle.
             * 
             * @param x0 Center X position.
             * @param y0 Center Y position.
             * @param r Circle radius.
             * @param color RGB565 color.
             */
            void circleFill(int x0, int y0, int r, uint16_t color);

            /**
             * @brief Draws a filled triangle.
             * 
             * @param x1 First point X.
             * @param y1 First point Y.
             * @param x2 Second point X.
             * @param y2 Second point Y.
             * @param x3 Third point X.
             * @param y3 Third point Y.
             * @param color RGB565 color.
             */
            void triangleFill(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color);

            /**
             * @brief Draws a rectangle outline.
             * 
             * @param x Rectangle X position.
             * @param y Rectangle Y position.
             * @param width Rectangle width.
             * @param height Rectangle height.
             * @param color RGB565 color.
             */
            void rect(int x, int y, int width, int height, uint16_t color);

            /**
             * @brief Draws a filled rectangle.
             * 
             * @param x Rectangle X position.
             * @param y Rectangle Y position.
             * @param width Rectangle width.
             * @param height Rectangle height.
             * @param color RGB565 color.
             */
            void rectFill(int x, int y, int width, int height, uint16_t color);

            /**
             * @brief Draws a rounded rectangle outline.
             * 
             * @param x Rectangle X position.
             * @param y Rectangle Y position.
             * @param width Rectangle width.
             * @param height Rectangle height.
             * @param radius Corner radius.
             * @param color RGB565 color.
             */
            void rectRound(int x, int y, int width, int height, int radius, uint16_t color);

            /**
             * @brief Draws a filled rounded rectangle.
             * 
             * @param x Rectangle X position.
             * @param y Rectangle Y position.
             * @param width Rectangle width.
             * @param height Rectangle height.
             * @param radius Corner radius.
             * @param color RGB565 color.
             */
            void rectRoundFill(int x, int y, int width, int height, int radius, uint16_t color);

            /**
             * @brief Draws an arc.
             * 
             * @param x0 Center X position.
             * @param y0 Center Y position.
             * @param r Arc radius.
             * @param startDeg Start angle in degrees.
             * @param endDeg End angle in degrees.
             * @param color RGB565 color.
             * @param clockwise Draw clockwise if true.
             */
            void arc(int x0, int y0, int r, int startDeg, int endDeg, uint16_t color, bool clockwise = true);

            /**
             * @brief Draws a progress bar.
             * 
             * @param x Position X.
             * @param y Position Y.
             * @param w Width.
             * @param h Height.
             * @param value Progress value from 0 to 100.
             * @param color RGB565 color.
             */
            void progressBar(int x, int y, int w, int h, int value, uint16_t color);

            /**
             * @brief Draws a horizontal slider.
             * 
             * @param x Position X.
             * @param y Position Y.
             * @param w Width.
             * @param h Height.
             * @param value Slider value from 0 to 100.
             * @param color RGB565 color.
             */
            void slider(int x, int y, int w, int h, int value, uint16_t color);
    };
}
#endif