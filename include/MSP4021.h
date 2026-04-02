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
            uint16_t readRaw(uint8_t cmd);
            uint16_t readAverage(uint8_t cmd);
            uint16_t readPressure();
            void waitForTouch(uint16_t &x, uint16_t &y);
            void waitRelease();
            bool isTouched();
            void drawCalibrationPoint(int x, int y, uint16_t &rx, uint16_t &ry);
            void detectOrientation();
            void applyOrientation(uint16_t &x, uint16_t &y);
            bool affineCalibration(float *sx, float *sy, float *rx, float *ry, int count);
            void transform(float &x, float &y);
        public:
            MSP4021(SPIClass &spi, int pinCS_TOUCH, int pinCS_TFT, int pinDC_TFT, int screenWidth, int screenHeight, int pinRST_TFT = -1);
            ~MSP4021();
            inline bool TGetSwapXY() { return *this->_SWAP_XY; }
            inline bool TGetInvertX() { return *this->_INVERT_X; }
            inline bool TGetInvertY() { return *this->_INVERT_Y; }
            inline float TGetCoeffXA() { return *this->_COEFF_XA; }
            inline float TGetCoeffXB() { return *this->_COEFF_XB; }
            inline float TGetCoeffXC() { return *this->_COEFF_XC; }
            inline float TGetCoeffYA() { return *this->_COEFF_YA; }
            inline float TGetCoeffYB() { return *this->_COEFF_YB; }
            inline float TGetCoeffYC() { return *this->_COEFF_YC; }
            inline bool TUsed() { return digitalRead(*this->_PIN_CS) == LOW; };
            inline void TStart()  { digitalWrite(*this->_PIN_CS, LOW); }
            inline void TStop() { digitalWrite(*this->_PIN_CS, HIGH); }
            bool TRead(int &x, int &y, bool antiSmoothing = true);
            bool TCalibrate();
            void TCalibrate(bool swapXY, bool invertX, bool invertY, float CXA, float CXB, float CXC, float CYA, float CYB, float CYC);
    };
}
#endif

