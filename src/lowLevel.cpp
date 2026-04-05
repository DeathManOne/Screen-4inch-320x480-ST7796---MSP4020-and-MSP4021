#include "../include/MSP4020.h"
using namespace ST7796S;

MSP4020::MSP4020(SPIClass &spi, int pinCS, int pinDC, int screenWidth, int screenHeight, int pinRST) {
  this->_PIN_CS = new int(pinCS);
  this->_PIN_DC = new int(pinDC);
  this->_PIN_RST = new int(pinRST);

  if (screenWidth > screenHeight) {
    this->_RAW_WIDTH = new int(screenHeight);
    this->_RAW_HEIGHT = new int(screenWidth);
    this->_SCREEN_WIDTH = new int(screenHeight);
    this->_SCREEN_HEIGHT = new int(screenWidth);
  } else {
    this->_RAW_WIDTH = new int(screenWidth);
    this->_RAW_HEIGHT = new int(screenHeight);
    this->_SCREEN_WIDTH = new int(screenWidth);
    this->_SCREEN_HEIGHT = new int(screenHeight);
  }
  this->_SCREEN_ROTATION = new int(0);

  this->_SPI = &spi;
  this->_SETTINGS = new SPISettings(4000000u, MSBFIRST, SPI_MODE0);
  
  if (pinRST != -1)
    { pinMode(pinRST, OUTPUT); }
  pinMode(pinCS, OUTPUT);
  pinMode(pinDC, OUTPUT);

  this->_init();
}

MSP4020::~MSP4020() {
  delete this->_BUFFER_CPU;
  delete this->_BUFFER_DMA;
  delete this->_PIN_CS;
  delete this->_PIN_DC;
  delete this->_PIN_RST;
  delete this->_RAW_WIDTH;
  delete this->_RAW_HEIGHT;
  delete this->_SCREEN_WIDTH;
  delete this->_SCREEN_HEIGHT;
  delete this->_SCREEN_ROTATION;
  delete this->_SPI;
  delete this->_SETTINGS;
}

void MSP4020::_init() {
  if (*this->_PIN_RST != -1) {
    digitalWrite(*this->_PIN_RST, HIGH);
    delay(5);
    digitalWrite(*this->_PIN_RST, LOW);
    delay(15);
    digitalWrite(*this->_PIN_RST, HIGH);
    delay(15);
  }

  uint8_t d;
  this->_transactionBegin();
  this->_writeCmd(0x01);
  delay(120);
  this->_writeCmd(0x11);
  delay(120);
  d = 0xC3;
  this->_writeCmd(0xF0);
  this->_writeData(&d, 1);
  d = 0x96;
  this->_writeCmd(0xF0);
  this->_writeData(&d, 1);
  d = 0x48;
  this->_writeCmd(0x36);
  this->_writeData(&d, 1);
  d = 0x05;
  this->_writeCmd(0x3A);
  this->_writeData(&d, 1);
  d = 0x01;
  this->_writeCmd(0xB4);
  this->_writeData(&d, 1);
  uint8_t b6[] = {0x80,0x02,0x3B};
  this->_writeCmd(0xB6);
  this->_writeData(b6, 3);
  uint8_t e8[] = {0x40,0x8A,0x00,0x00,0x29,0x19,0xA5,0x33};
  this->_writeCmd(0xE8);
  this->_writeData(e8, 8);
  d = 0x06;
  this->_writeCmd(0xC1);
  this->_writeData(&d, 1);
  d = 0xA7;
  this->_writeCmd(0xC2);
  this->_writeData(&d, 1);
  d = 0x18;
  this->_writeCmd(0xC5);
  this->_writeData(&d, 1);
  delay(120);
  uint8_t gammaPos[] = {
    0xF0,0x09,0x0b,0x06,0x04,0x15,0x2F,
    0x54,0x42,0x3C,0x17,0x14,0x18,0x1B
  };
  this->_writeCmd(0xE0);
  this->_writeData(gammaPos, 14);
  uint8_t gammaNeg[] = {
    0xE0,0x09,0x0B,0x06,0x04,0x03,0x2B,
    0x43,0x42,0x3B,0x16,0x14,0x17,0x1B
  };
  this->_writeCmd(0xE1);
  this->_writeData(gammaNeg, 14);
  delay(120);
  d = 0x3C;
  this->_writeCmd(0xF0);
  this->_writeData(&d, 1);
  d = 0x69;
  this->_writeCmd(0xF0);
  this->_writeData(&d, 1);
  this->_writeCmd(0x29);
  this->_transactionEnd();
}

void MSP4020::_transactionBegin() {
    this->_SPI->beginTransaction(*this->_SETTINGS);
    this->_start();
}

void MSP4020::_transactionEnd() {
    this->_stop();
    this->_SPI->endTransaction();
}

void MSP4020::_writeCmd(uint8_t cmd) {
    this->_DC_CMD();
    this->_SPI->transfer(cmd);
}

void MSP4020::_writeData(const uint8_t *data, int length) {
    this->_DC_DATA();
    this->_SPI->writeBytes(data, length);
}

void MSP4020::_setAddress(int x0, int y0, int x1, int y1) {
  if (x0 > x1)
    { std::swap(x0, x1); }
  if (y0 > y1)
    { std::swap(y0, y1); }

  uint8_t d[4];

  this->_writeCmd(0x2A);
  d[0] = x0 >> 8;
  d[1] = x0;
  d[2] = x1 >> 8;
  d[3] = x1;
  this->_writeData(d,4);

  this->_writeCmd(0x2B);
  d[0] = y0 >> 8;
  d[1] = y0;
  d[2] = y1 >> 8;
  d[3] = y1;
  this->_writeData(d,4);

  this->_writeCmd(0x2C);
}

void MSP4020::_swapBuffers() {
    uint16_t *tmp = this->_BUFFER_CPU;
    this->_BUFFER_CPU = this->_BUFFER_DMA;
    this->_BUFFER_DMA = tmp;
}

void MSP4020::setRotation(int rotation) {
  rotation = (rotation % 4 + 4) % 4;
  if (*this->_SCREEN_ROTATION == rotation)
    { return; }
  *this->_SCREEN_ROTATION = rotation;

  uint8_t d;
  switch (*this->_SCREEN_ROTATION) {
    case 1:
      d = 0x28;
      break;
    case 2:
      d = 0x88;
      break;
    case 3:
      d = 0xE8;
      break;
    default:
      d = 0x48;
      break;
  }

  if (*this->_SCREEN_ROTATION % 2 == 0) {
    *this->_SCREEN_WIDTH = *this->_RAW_WIDTH;
    *this->_SCREEN_HEIGHT = *this->_RAW_HEIGHT;
  } else {
    *this->_SCREEN_WIDTH = *this->_RAW_HEIGHT;
    *this->_SCREEN_HEIGHT = *this->_RAW_WIDTH;
  }

  this->_transactionBegin();
  this->_writeCmd(0x36);
  this->_writeData(&d, 1);
  this->_transactionEnd();
}