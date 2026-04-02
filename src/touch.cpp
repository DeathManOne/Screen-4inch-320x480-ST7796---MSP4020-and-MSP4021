#include "../include/MSP4021.h"
using namespace ST7796S;

MSP4021::MSP4021(SPIClass &spi, int pinCS_TOUCH, int pinCS_TFT, int pinDC_TFT, int screenWidth, int screenHeight, int pinRST_TFT)
    : MSP4020(spi, pinCS_TFT, pinDC_TFT, screenWidth, screenHeight, pinRST_TFT) {
    this->_PIN_CS = new int(pinCS_TOUCH);
    this->_LAST_X = new int(-1);
    this->_LAST_Y = new int(-1);

    this->_SWAP_XY = new bool(false);
    this->_INVERT_X = new bool(false);
    this->_INVERT_Y = new bool(false);

    this->_COEFF_XA = new float(1);
    this->_COEFF_XB = new float(0);
    this->_COEFF_XC = new float(0);

    this->_COEFF_YA = new float(0);
    this->_COEFF_YB = new float(1);
    this->_COEFF_YC = new float(0);

    pinMode(pinCS_TOUCH, OUTPUT);
    this->TStop();
}

MSP4021::~MSP4021() {
    delete this->_PIN_CS;
    delete this->_LAST_X;
    delete this->_LAST_Y;
    delete this->_SWAP_XY;
    delete this->_INVERT_X;
    delete this->_INVERT_Y;
    delete this->_COEFF_XA;
    delete this->_COEFF_XB;
    delete this->_COEFF_XC;
    delete this->_COEFF_YA;
    delete this->_COEFF_YB;
    delete this->_COEFF_YC;
}

uint16_t MSP4021::readRaw(uint8_t cmd) {
    this->TStart();
    this->_SPI->transfer(cmd);
    uint8_t high = this->_SPI->transfer(0x00);
    uint8_t low = this->_SPI->transfer(0x00);
    this->TStop();
    return ((high << 8) | low) >> 3;
}

uint16_t MSP4021::readAverage(uint8_t cmd) {
    const int N = 15;
    uint16_t v[N];

    for (int i = 0; i < N; i++) {
        v[i] = this->readRaw(cmd);
        delayMicroseconds(15);
    }

    for (int i = 0; i < N - 1; i++) {
        for (int j = i + 1; j < N; j++) {
            if (v[j] < v[i]) {
                uint16_t tmp = v[i];
                v[i] = v[j];
                v[j] = tmp;
            }
        }
    }

    uint32_t sum = 0;
    for (int i = 4; i < 11; i++)
        { sum += v[i]; }
    return sum / 7;
}

uint16_t MSP4021::readPressure() {
    uint16_t z1 = this->readRaw(0xB0);
    uint16_t z2 = this->readRaw(0xC0);

    if (z1 == 0)
        { return 0; }
    return z2 - z1;
}

void MSP4021::waitForTouch(uint16_t &x, uint16_t &y) {
    const int stabilityThreshold = 15;

    while (true) {
        uint16_t samplesX[5], samplesY[5];

        for (int i = 0; i < 5; i++) {
            samplesX[i] = this->readAverage(0x90);
            samplesY[i] = this->readAverage(0xD0);
            delay(5);
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
        if (this->readPressure() < 50)
            { continue; }

        bool stable = true;
        for (int i = 0; i < 5; i++) {
            if (abs((int)samplesX[i] - avgX) > stabilityThreshold ||
                abs((int)samplesY[i] - avgY) > stabilityThreshold) {
                stable = false;
                break;
            }
        }

        if (!stable || !this->isTouched())
            { continue;} 
        x = avgX;
        y = avgY;
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

void MSP4021::drawCalibrationPoint(int x, int y, uint16_t &rx, uint16_t &ry) {
    const int size = 20;

    this->rect(x, y, size, size, this->rgb(255, 0, 0));
    this->waitForTouch(rx, ry);
    this->waitRelease();

    this->rect(x, y, size, size, this->rgb(0, 255, 0));
    delay(250);
    this->rect(x, y, size, size, this->rgb(0, 0, 0));
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
    this->drawCalibrationPoint(0, 0, x[0], y[0]);
    this->drawCalibrationPoint(*this->_SCREEN_WIDTH - 20, 0, x[1], y[1]);
    this->drawCalibrationPoint(0, *this->_SCREEN_HEIGHT - 20, x[2], y[2]);

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

void MSP4021::transform(float &x, float &y) {
  int tmp;
  switch (*this->_SCREEN_ROTATION) {
    case 1:
        tmp = x;
        x = y;
        y = *this->_SCREEN_HEIGHT - 1 - tmp;
        break;
    case 2:
        x = *this->_SCREEN_WIDTH - 1 - x;
        y = *this->_SCREEN_HEIGHT - 1 - y;
        break;
    case 3:
        tmp = x;
        x = *this->_SCREEN_WIDTH - 1 - y;
        y = tmp;
        break;
    default:
        break;
  }
}

bool MSP4021::TRead(int &x, int &y, bool antiSmoothing) {
    if (antiSmoothing) {
        *this->_LAST_X = -1;
        *this->_LAST_Y = -1;
    }

    uint16_t pressure = this->readPressure();
    if (pressure < 60) {
        *this->_LAST_X = -1;
        *this->_LAST_Y = -1;
        return false;
    }

    uint16_t rawX = this->readAverage(0x90);
    uint16_t rawY = this->readAverage(0xD0);
    if (rawX < 50 || rawY < 50)
        { return false; }
    this->applyOrientation(rawX, rawY);

    float tx = *this->_COEFF_XA * rawX + *this->_COEFF_XB * rawY + *this->_COEFF_XC;
    float ty = *this->_COEFF_YA * rawX + *this->_COEFF_YB * rawY + *this->_COEFF_YC;
    this->transform(tx, ty);

    if (*this->_LAST_X != -1) {
        float dx = fabs(tx - *this->_LAST_X);
        float dy = fabs(ty - *this->_LAST_Y);
        float alpha = (dx > 10 || dy > 10) ? 0.5f : 0.2f;
        tx = (1 - alpha) * *this->_LAST_X + alpha * tx;
        ty = (1 - alpha) * *this->_LAST_Y + alpha * ty;
    }

    int newX = constrain((int)(tx + 0.5f), 0, *this->_SCREEN_WIDTH - 1);
    int newY = constrain((int)(ty + 0.5f), 0, *this->_SCREEN_HEIGHT - 1);
    if (newX <= 1 && newY <= 1)
        { return false; }

    const int threshold = (*this->_LAST_X == -1) ? 0 : 2;
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

    int userRotation = *this->_SCREEN_ROTATION;
    this->setRotation();

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

        this->rect(sx[i], sy[i], 20, 20, this->rgb(0, 255, 0));
        delay(250);
        this->rect(sx[i], sy[i], 20, 20, this->rgb(0, 0, 0));
    }
    for (int i = 0; i < count; i++) {
        if (isnan(rx[i]) || isnan(ry[i]))
            { return false; }
    }

    this->setRotation();
    bool toReturn = this->affineCalibration(sx, sy, rx, ry, count);
    this->setRotation(userRotation);
    return toReturn;
}

void MSP4021::TCalibrate(bool swapXY, bool invertX, bool invertY, float CXA, float CXB, float CXC, float CYA, float CYB, float CYC) {
    *this->_SWAP_XY = swapXY;
    *this->_INVERT_X = invertX;
    *this->_INVERT_Y = invertY;
    *this->_COEFF_XA = CXA;
    *this->_COEFF_XB = CXB;
    *this->_COEFF_XC = CXC;
    *this->_COEFF_YA = CYA;
    *this->_COEFF_YB = CYB;
    *this->_COEFF_YC = CYC;
}