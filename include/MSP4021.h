#ifndef _XPT2046_
#define _XPT2046_

#include "MSP4020.h"
#include <map>

namespace ST7796S {
    class MSP4021 : public MSP4020 {
        private:
            int *_PIN_CS, *_LAST_X, *_LAST_Y;
            bool *_INVERT_X, *_INVERT_Y, *_SWAP_XY;
            float *_COEFF_XA, *_COEFF_XB, *_COEFF_XC;
            float *_COEFF_YA, *_COEFF_YB, *_COEFF_YC;
            inline void CS_LOW()  { digitalWrite(*this->_PIN_CS, LOW); }
            inline void CS_HIGH() { digitalWrite(*this->_PIN_CS, HIGH); }
            uint16_t readRaw(uint8_t cmd);
            uint16_t readAverage(uint8_t cmd);
            uint16_t readPressure();
            void waitForTouch(uint16_t &outX, uint16_t &outY);
            void waitRelease();
            bool isTouched();
            void applyOrientation(uint16_t &x, uint16_t &y);
            void detectOrientation();
            bool affineCalibration(float *sx, float *sy, float *rx, float *ry, int count);
        public:
            MSP4021(SPIClass &spi, int pinCS_TOUCH, int pinCS_TFT, int pinDC_TFT, int screenWidth, int screenHeight, int pinRST_TFT = -1);
            ~MSP4021();
            inline bool touchUsed() { return digitalRead(*this->_PIN_CS) == LOW; };
            inline bool getInvertX() { return *this->_INVERT_X; }
            inline bool getInvertY() { return *this->_INVERT_Y; }
            inline bool getSwapXY() { return *this->_SWAP_XY; }
            inline float getCoeffXA() { return *this->_COEFF_XA; }
            inline float getCoeffXB() { return *this->_COEFF_XB; }
            inline float getCoeffXC() { return *this->_COEFF_XC; }
            inline float getCoeffYA() { return *this->_COEFF_YA; }
            inline float getCoeffYB() { return *this->_COEFF_YB; }
            inline float getCoeffYC() { return *this->_COEFF_YC; }
            bool TRead(int &x, int &y, bool antiSmoothing = true);
            bool TCalibrate();
    };
}
#endif

