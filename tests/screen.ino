#include <Arduino.h>
#include <SPI.h>
#include <MSP4020.h>

#define TFT_CS 10
#define TFT_DC 5
#define TFT_RST -1 // 3.3v or an available pin you want

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480

ST7796S::MSP4020 *_TFT;

void setup() {
  SPI.begin();

  _TFT = new ST7796S::MSP4020(SPI, TFT_CS, TFT_DC, SCREEN_WIDTH, SCREEN_HEIGHT, TFT_RST);
  _TFT->fillScreen(_TFT->rgb(0,0,0));
}

void loop() {
  delay(1000);
  _TFT->fillScreen(_TFT->rgb(255, 0, 0));
  delay(1000);
  _TFT->fillScreen(_TFT->rgb(0, 255, 0));
  delay(1000);
  _TFT->fillScreen(_TFT->rgb(0, 0, 255));
}