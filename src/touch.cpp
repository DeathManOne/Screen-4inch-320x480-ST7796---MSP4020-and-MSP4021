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

uint16_t MSP4021::_readRaw(uint8_t cmd) {
    this->TStart();
    this->_SPI->transfer(cmd);
    uint8_t high = this->_SPI->transfer(0x00);
    uint8_t low = this->_SPI->transfer(0x00);
    this->TStop();
    return ((high << 8) | low) >> 3;
}

uint16_t MSP4021::_readAverage(uint8_t cmd) {
    const int N = 15;
    uint16_t v[N];

    for (int i = 0; i < N; i++) {
        v[i] = this->_readRaw(cmd);
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

uint16_t MSP4021::_readPressure() {
    uint16_t z1 = this->_readRaw(0xB0);
    uint16_t z2 = this->_readRaw(0xC0);

    if (z1 == 0)
        { return 0; }
    return z2 - z1;
}

void MSP4021::_waitForTouch(uint16_t &x, uint16_t &y) {
    const int stabilityThreshold = 15;

    while (true) {
        uint16_t samplesX[5], samplesY[5];

        for (int i = 0; i < 5; i++) {
            samplesX[i] = this->_readAverage(0x90);
            samplesY[i] = this->_readAverage(0xD0);
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
        if (this->_readPressure() < 50)
            { continue; }

        bool stable = true;
        for (int i = 0; i < 5; i++) {
            if (abs((int)samplesX[i] - avgX) > stabilityThreshold ||
                abs((int)samplesY[i] - avgY) > stabilityThreshold) {
                stable = false;
                break;
            }
        }

        if (!stable || !this->_isTouched())
            { continue;} 
        x = avgX;
        y = avgY;
        break;
    }
}

void MSP4021::_waitRelease() {
    while (true) {
        uint16_t x1 = this->_readAverage(0x90);
        uint16_t y1 = this->_readAverage(0xD0);

        delay(10);

        uint16_t x2 = this->_readAverage(0x90);
        uint16_t y2 = this->_readAverage(0xD0);

        if (abs((int)x1 - (int)x2) > 40 || abs((int)y1 - (int)y2) > 40)
            { return; }
    }
}

bool MSP4021::_isTouched() {
    return this->_readPressure() > 30;
}

void MSP4021::_drawCalibrationPoint(int x, int y, uint16_t &rx, uint16_t &ry) {
    const int size = 20;

    this->rect(x, y, size, size, this->rgb(255, 0, 0));
    delay(80);
    this->_waitForTouch(rx, ry);
    this->_waitRelease();

    this->rect(x, y, size, size, this->rgb(0, 255, 0));
    delay(250);
    this->rect(x, y, size, size, this->rgb(0, 0, 0));
}

void MSP4021::_detectOrientation() {
    uint16_t x[3], y[3];
    this->_drawCalibrationPoint(0, 0, x[0], y[0]);
    this->_drawCalibrationPoint(*this->_SCREEN_WIDTH - 20, 0, x[1], y[1]);
    this->_drawCalibrationPoint(0, *this->_SCREEN_HEIGHT - 20, x[2], y[2]);

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

void MSP4021::_applyOrientation(uint16_t &x, uint16_t &y) {
    if (*this->_SWAP_XY)
        { std::swap(x,y); }
    if (*this->_INVERT_X)
        { x = 4095 - x; }
    if (*this->_INVERT_Y)
        { y = 4095 - y; }
}

bool MSP4021::_affineCalibration(float *sx, float *sy, float *rx, float *ry, int count) {
    float sum_x = 0;
    float sum_y = 0;
    float sum_1 = 0;
    float sum_xx = 0;
    float sum_yy = 0;
    float sum_xy = 0;
    float sum_X = 0;
    float sum_Y = 0;
    float sum_xX = 0;
    float sum_yX = 0;
    float sum_xY = 0;
    float sum_yY = 0;

    float cx = 0, cy = 0;
    int valid = 0;
    for (int i = 0; i < count; i++) {
        float x = rx[i];
        float y = ry[i];

        if (x < 150 || x > 3950 || y < 150 || y > 3950)
            { continue; }
        cx += x;
        cy += y;
        valid++;
    }

    if (valid < 3)
        { return false; }
    cx /= valid;
    cy /= valid;

    for (int i = 0; i < count; i++) {
        float x = rx[i];
        float y = ry[i];
        float X = sx[i];
        float Y = sy[i];
        if (x < 150 || x > 3950 || y < 150 || y > 3950)
            { continue; }

        float weight = 1.0f;
        if (i == 4)
            { weight = 3.0f; }
        else if (i == 0 || i == 2 || i == 6 || i == 8)
            { weight = 1.5f; }
        else
             { weight = 2.0f; }
        float dx = x - cx;
        float dy = y - cy;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist < 1.0f)
            { dist = 1.0f; }

        weight *= 1.0f + (dist / (5000.0f + dist));
        if (weight > 4.0f)
            { weight = 4.0f; }
        sum_1 += weight;

        sum_x  += weight * x;
        sum_y  += weight * y;
        sum_xx += weight * x * x;
        sum_yy += weight * y * y;
        sum_xy += weight * x * y;

        sum_X  += weight * X;
        sum_Y  += weight * Y;

        sum_xX += weight * x * X;
        sum_yX += weight * y * X;

        sum_xY += weight * x * Y;
        sum_yY += weight * y * Y;
    }

    if (sum_1 < 1e-6f)
        { return false; }
    float inv = 1.0f / sum_1;
    sum_x  *= inv;
    sum_y  *= inv;
    sum_xx *= inv;
    sum_yy *= inv;
    sum_xy *= inv;
    sum_X  *= inv;
    sum_Y  *= inv;
    sum_xX *= inv;
    sum_yX *= inv;
    sum_xY *= inv;
    sum_yY *= inv;
    sum_1 = 1.0f;
    float det = sum_xx * (sum_yy * sum_1 - sum_y * sum_y)
              - sum_xy * (sum_xy * sum_1 - sum_y * sum_x)
              + sum_x  * (sum_xy * sum_y - sum_yy * sum_x);

    if (fabs(det) < 1e-6)
        { return false; }

    *this->_COEFF_XA = (
        sum_xX * (sum_yy * sum_1 - sum_y * sum_y)
        - sum_yX * (sum_xy * sum_1 - sum_y * sum_x)
        + sum_X  * (sum_xy * sum_y - sum_yy * sum_x))
        / det;
    *this->_COEFF_XB = (
        sum_xx * (sum_yX * sum_1 - sum_y * sum_X)
        - sum_xy * (sum_xX * sum_1 - sum_x * sum_X)
        + sum_x  * (sum_xX * sum_y - sum_yX * sum_x))
        / det;
    *this->_COEFF_XC = (
        sum_xx * (sum_yy * sum_X - sum_y * sum_yX)
        - sum_xy * (sum_xy * sum_X - sum_y * sum_xX)
        + sum_x  * (sum_xy * sum_yX - sum_yy * sum_xX))
        / det;
    *this->_COEFF_YA = (
        sum_xY * (sum_yy * sum_1 - sum_y * sum_y)
        - sum_yY * (sum_xy * sum_1 - sum_y * sum_x)
        + sum_Y  * (sum_xy * sum_y - sum_yy * sum_x))
        / det;
    *this->_COEFF_YB = (
        sum_xx * (sum_yY * sum_1 - sum_y * sum_Y)
        - sum_xy * (sum_xY * sum_1 - sum_x * sum_Y)
        + sum_x  * (sum_xY * sum_y - sum_yY * sum_x))
        / det;
    *this->_COEFF_YC = (
        sum_xx * (sum_yy * sum_Y - sum_y * sum_yY)
        - sum_xy * (sum_xy * sum_Y - sum_y * sum_xY)
        + sum_x  * (sum_xy * sum_yY - sum_yy * sum_xY))
        / det;
    return true;
}

void MSP4021::_transform(float &x, float &y) {
  float tmp;
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

    uint16_t pressure = this->_readPressure();
    if (pressure < 60) {
        *this->_LAST_X = -1;
        *this->_LAST_Y = -1;
        return false;
    }

    uint16_t rawX = this->_readAverage(0x90);
    uint16_t rawY = this->_readAverage(0xD0);
    if (rawX < 50 || rawY < 50)
        { return false; }
    this->_applyOrientation(rawX, rawY);

    float tx = *this->_COEFF_XA * rawX + *this->_COEFF_XB * rawY + *this->_COEFF_XC;
    float ty = *this->_COEFF_YA * rawX + *this->_COEFF_YB * rawY + *this->_COEFF_YC;
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
    this->fillScreen(this->rgb(0, 0, 0));
    this->setRotation();
    this->_detectOrientation();
    this->setRotation(userRotation);

    int count = 9;
    float width = *this->_SCREEN_WIDTH;
    float height = *this->_SCREEN_HEIGHT;
    float sx[count] = {
        20, width / 2, width - 20,
        20, width / 2, width - 20,
        20, width / 2, width - 20
    };
    float sy[count] = {
        20, 20, 20,
        height / 2,  height / 2,  height / 2,
        height - 20, height - 20, height - 20
    };

    float rx[count], ry[count];
    for (int i = 0; i < count; i++) {
        uint16_t tx, ty;
        this->_drawCalibrationPoint(sx[i], sy[i], tx, ty);
        this->_applyOrientation(tx, ty);
        rx[i] = (float)tx;
        ry[i] = (float)ty;
    }
    for (int i = 0; i < count; i++) {
        if (isnan(rx[i]) || isnan(ry[i]))
            { return false; }
    }
    return this->_affineCalibration(sx, sy, rx, ry, count);
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