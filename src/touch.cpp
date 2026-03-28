#include "../include/MSP4021.h"
using namespace ST7796S;

MSP4021::MSP4021(SPIClass &spi, int pinCS_TOUCH, int pinCS_TFT, int pinDC_TFT, int screenWidth, int screenHeight, int pinRST_TFT)
    : MSP4020(spi, pinCS_TFT, pinDC_TFT, screenWidth, screenHeight, pinRST_TFT) {
    this->_PIN_CS = new int(pinCS_TOUCH);
    this->_LAST_X = new int(-1);
    this->_LAST_Y = new int(-1);

    this->_INVERT_X = new bool(false);
    this->_INVERT_Y = new bool(false);
    this->_SWAP_XY = new bool(false);

    this->_COEFF_XA = new float(1);
    this->_COEFF_XB = new float(0);
    this->_COEFF_XC = new float(0);
    this->_COEFF_YA = new float(0);
    this->_COEFF_YB = new float(1);
    this->_COEFF_YC = new float(0);

    pinMode(pinCS_TOUCH, OUTPUT);
    this->CS_HIGH();
}

MSP4021::~MSP4021() {
    delete this->_PIN_CS;
    delete this->_LAST_X;
    delete this->_LAST_Y;
    delete this->_INVERT_X;
    delete this->_INVERT_Y;
    delete this->_SWAP_XY;
    delete this->_COEFF_XA;
    delete this->_COEFF_XB;
    delete this->_COEFF_XC;
    delete this->_COEFF_YA;
    delete this->_COEFF_YB;
    delete this->_COEFF_YC;
}

uint16_t MSP4021::readRaw(uint8_t cmd) {
    this->CS_LOW();
    this->_SPI->transfer(cmd);
    uint8_t high = this->_SPI->transfer(0x00);
    uint8_t low = this->_SPI->transfer(0x00);
    this->CS_HIGH();
    return ((high << 8) | low) >> 3;
}

uint16_t MSP4021::readAverage(uint8_t cmd) {
    uint16_t v[9];
    for (int i = 0; i < 9; i++) {
        v[i] = this->readRaw(cmd);
        delayMicroseconds(10);
    }
    for (int i = 0; i < 8; i++) {
        for (int j = i + 1; j < 9; j++) {
            if (v[j] < v[i]) {
                uint16_t tmp = v[i];
                v[i] = v[j];
                v[j] = tmp;
            }
        }
    }
    return v[4];
}

uint16_t MSP4021::readPressure() {
    uint16_t z1 = this->readRaw(0xB0);
    uint16_t z2 = this->readRaw(0xC0);

    if (z1 == 0)
        { return 0; }
    return z2 - z1;
}

void MSP4021::waitForTouch(uint16_t &outX, uint16_t &outY) {
    while (true) {
        uint16_t samplesX[5], samplesY[5];

        for (int i = 0; i < 5; i++) {
            samplesX[i] = this->readAverage(0x90);
            samplesY[i] = this->readAverage(0xD0);
            delay(10);
        }

        uint32_t sumX = 0, sumY = 0;
        for (int i = 0; i < 5; i++) {
            sumX += samplesX[i];
            sumY += samplesY[i];
        }

        uint16_t avgX = sumX / 5;
        uint16_t avgY = sumY / 5;

        if (avgX < 200 || avgX > 3900 || avgY < 200 || avgY > 3900)
            { continue; }

        bool stable = true;
        for (int i = 0; i < 5; i++) {
            if (abs((int)samplesX[i] - avgX) > 25 || abs((int)samplesY[i] - avgY) > 25) {
                stable = false;
                break;
            }
        }
        if (!stable || !this->isTouched())
            { continue; }
        outX = avgX;
        outY = avgY;
        break;
    }
}

void MSP4021::waitRelease() {
    while (true) {
        uint16_t x1 = this->readAverage(0x90);
        uint16_t y1 = this->readAverage(0xD0);

        delay(10);

        uint16_t x2 = this->readAverage(0x90);
        uint16_t y2 = this->readAverage(0xD0);

        if (abs((int)x1 - (int)x2) > 40 || abs((int)y1 - (int)y2) > 40)
            { return; }
    }
}

bool MSP4021::isTouched() {
    return this->readPressure() > 30;
}

void MSP4021::applyOrientation(uint16_t &x, uint16_t &y) {
    if (*this->_SWAP_XY)
        { std::swap(x,y); }
    if (*this->_INVERT_X)
        { x = 4095 - x; }
    if (*this->_INVERT_Y)
        { y = 4095 - y; }
}

void MSP4021::detectOrientation() {
    uint16_t x[3], y[3];

    this->rect(0, 0, 20, 20, this->rgb(255, 0, 0));
    this->waitForTouch(x[0], y[0]);
    this->waitRelease();
    this->rect(0, 0, 20, 20, this->rgb(0, 0, 0));

    this->rect(*this->_SCREEN_WIDTH - 20, 0, 20, 20, this->rgb(255, 0, 0));
    this->waitForTouch(x[1], y[1]);
    this->waitRelease();
    this->rect(*this->_SCREEN_WIDTH - 20, 0, 20, 20, this->rgb(0, 0, 0));

    this->rect(0, *this->_SCREEN_HEIGHT - 20, 20, 20, this->rgb(255, 0, 0));
    this->waitForTouch(x[2], y[2]);
    this->waitRelease();
    this->rect(0, *this->_SCREEN_HEIGHT - 20, 20, 20, this->rgb(0, 0, 0));

    int dx_h = abs((int)x[1] - (int)x[0]);
    int dy_h = abs((int)y[1] - (int)y[0]);
    int dx_v = abs((int)x[2] - (int)x[0]);
    int dy_v = abs((int)y[2] - (int)y[0]);

    *this->_SWAP_XY = (dx_h < dy_h);
    if (*this->_SWAP_XY) {
        std::swap(x[0], y[0]);
        std::swap(x[1], y[1]);
        std::swap(x[2], y[2]);
    }
    *this->_INVERT_X = x[1] < x[0];
    *this->_INVERT_Y = y[2] < y[0];
}

bool MSP4021::affineCalibration(float *sx, float *sy, float *rx, float *ry, int count) {
    float x1 = rx[0], y1 = ry[0];
    float x2 = rx[1], y2 = ry[1];
    float x3 = rx[2], y3 = ry[2];

    float X1 = sx[0], Y1 = sy[0];
    float X2 = sx[1], Y2 = sy[1];
    float X3 = sx[2], Y3 = sy[2];

    float det = (x1*(y2 - y3) + x2*(y3 - y1) + x3*(y1 - y2));

    if (fabs(det) < 1e-6)
        { return false; }

    *this->_COEFF_XA = (X1*(y2 - y3) + X2*(y3 - y1) + X3*(y1 - y2)) / det;
    *this->_COEFF_XB = (X1*(x3 - x2) + X2*(x1 - x3) + X3*(x2 - x1)) / det;
    *this->_COEFF_XC = (X1*(x2*y3 - x3*y2) + X2*(x3*y1 - x1*y3) + X3*(x1*y2 - x2*y1)) / det;

    *this->_COEFF_YA = (Y1*(y2 - y3) + Y2*(y3 - y1) + Y3*(y1 - y2)) / det;
    *this->_COEFF_YB = (Y1*(x3 - x2) + Y2*(x1 - x3) + Y3*(x2 - x1)) / det;
    *this->_COEFF_YC = (Y1*(x2*y3 - x3*y2) + Y2*(x3*y1 - x1*y3) + Y3*(x1*y2 - x2*y1)) / det;

    return true;
}

bool MSP4021::TRead(int &x, int &y, bool antiSmoothing) {
    if (antiSmoothing) {
        *this->_LAST_X = -1;
        *this->_LAST_Y = -1;
    }

    uint16_t rawX = this->readAverage(0x90);
    uint16_t rawY = this->readAverage(0xD0);
    if (!this->isTouched()) {
        *this->_LAST_X = -1;
        *this->_LAST_Y = -1;
        return false;
    }
    this->applyOrientation(rawX, rawY);

    float tx = *this->_COEFF_XA * rawX + *this->_COEFF_XB * rawY + *this->_COEFF_XC;
    float ty = *this->_COEFF_YA * rawX + *this->_COEFF_YB * rawY + *this->_COEFF_YC;
    if (*this->_LAST_X != -1) {
        tx = 0.7f * *this->_LAST_X + 0.3f * tx;
        ty = 0.7f * *this->_LAST_Y + 0.3f * ty;
    }

    int newX = constrain((int)(tx + 0.5f), 0, *_SCREEN_WIDTH);
    int newY = constrain((int)(ty + 0.5f), 0, *_SCREEN_HEIGHT);
    if (newX <= 1 && newY <= 1)
        { return false; }

    const int threshold = (*this->_LAST_X == -1) ? 0 : 3;
    if (abs(newX - *this->_LAST_X) < threshold && abs(newY - *this->_LAST_Y) < threshold) {
        x = *this->_LAST_X;
        y = *this->_LAST_Y;
        return false;
    }
    *this->_LAST_X = newX;
    *this->_LAST_Y = newY;

    x = newX;
    y = newY;
    return true;
}

bool MSP4021::TCalibrate() {
    *this->_LAST_X = -1;
    *this->_LAST_Y = -1;

    this->fillScreen(this->rgb(0, 0, 0));
    this->detectOrientation();

    const int count = 5;
    float sx[count] = {20, 300, 300, 20, 160};
    float sy[count] = {20, 20, 460, 460, 240};
    float rx[count], ry[count];

    for (int i = 0; i < count; i++) {
        this->rect(sx[i], sy[i], 20, 20, this->rgb(255, 0, 0));
        delay(800);

        uint16_t tx, ty;
        this->waitForTouch(tx, ty);
        this->waitRelease();

        this->applyOrientation(tx, ty);
        rx[i] = (float)tx;
        ry[i] = (float)ty;

        this->rect(sx[i], sy[i], 20, 20, this->rgb(0, 0, 0));
    }
    for (int i = 0; i < count; i++) {
        if (isnan(rx[i]) || isnan(ry[i]))
            { return false; }
    }
    return this->affineCalibration(sx, sy, rx, ry, count);
}
