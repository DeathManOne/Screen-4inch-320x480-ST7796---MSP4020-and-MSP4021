#include <Arduino.h>
#include <SPI.h>
#include <MSP4021.h>

#define TFT_CS 10
#define TFT_DC 5
#define TFT_RST -1 // 3.3v or an available pin you want

#define TOUCH_CS 4

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480

ST7796S::MSP4021 *_TFT;

void setup() {
  SPI.begin();

  _TFT = new ST7796S::MSP4021(SPI, TOUCH_CS, TFT_CS, TFT_DC, SCREEN_WIDTH, SCREEN_HEIGHT, TFT_RST);
  while (!_TFT->TCalibrate());
  _TFT->fillScreen(_TFT->rgb(0, 0, 0));
}

void loop() {
  delay(500);

  int x=0, y=0;
  bool antiSmoothing = true; // optional | default = true
  if (_TFT->TRead(x, y, antiSmoothing)) {
    _TFT->rect(x, y, 20, 20, _TFT->rgb(0, 255, 0));
    delay(1500);
    _TFT->rect(x, y, 20, 20, _TFT->rgb(0, 0, 0));
  }
}
