#ifndef _ST7796S_
#define _ST7796S_

#include <Arduino.h>
#include <SPI.h>

namespace ST7796S {
    class MSP4020 {
        private:
            int *_PIN_CS;
            static const int _BUFFER_SIZE = 1024;
            uint16_t _BUFFER_A[_BUFFER_SIZE], _BUFFER_B[_BUFFER_SIZE];
            uint16_t *_BUFFER_CPU = _BUFFER_A, *_BUFFER_DMA = _BUFFER_B;
            int *_PIN_DC, *_PIN_RST;
            SPISettings *_SETTINGS;
            inline void CS_LOW()  { digitalWrite(*this->_PIN_CS, LOW); }
            inline void CS_HIGH() { digitalWrite(*this->_PIN_CS, HIGH); }
            inline void DC_CMD()  { digitalWrite(*this->_PIN_DC, LOW); }
            inline void DC_DATA() { digitalWrite(*this->_PIN_DC, HIGH); }
            void init();
            void transactionBegin();
            void transactionEnd();
            void writeCmd(uint8_t cmd);
            void writeData(const uint8_t *data, int length);
            void setAddress(int x0, int y0, int x1, int y1);
            void swapBuffers();
        protected:
            int *_SCREEN_WIDTH, *_SCREEN_HEIGHT;
            SPIClass *_SPI;
        public:
            MSP4020(SPIClass &spi, int pinCS, int pinDC, int screenWidth, int screenHeight, int pinRST = -1);
            ~MSP4020();
            inline bool TFTUsed() { return digitalRead(*this->_PIN_CS) == LOW; }
            inline uint16_t rgb(uint8_t red, uint8_t green, uint8_t blue) { return ((blue & 0xF8) << 8) | ((red & 0xFC) << 3) | (green >> 3); }
            inline void fillScreen(uint16_t color) { this->rect(0, 0, *this->_SCREEN_WIDTH, *this->_SCREEN_HEIGHT, color); }
            void rect(int x, int y, int width, int height, uint16_t color);
            void lineH(int x, int y, int width, uint16_t color);
            void lineV(int x, int y, int height, uint16_t color);
            void rectf(int x, int y, int width, int height, uint16_t color);
            //void drawText(int x, int y, const char* txt, uint16_t color);
            //void drawPixel(uint16_t x, uint16_t y, uint16_t color);
            void drawButton(int x, int y, int width, int height, uint16_t color);
    };
}
#endif