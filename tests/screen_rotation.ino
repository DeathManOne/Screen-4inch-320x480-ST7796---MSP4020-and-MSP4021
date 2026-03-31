#include <Arduino.h>
#include <SPI.h>
#include <vector>
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
  delay(1000);
}

void loop() {
    int rotation = 1;
    std::vector<uint16_t> colors = {
        _TFT->rgb(0, 0, 0),
        _TFT->rgb(255, 0, 0),
        _TFT->rgb(0, 255, 0),
        _TFT->rgb(0, 0, 255)
    };

    while (true) {
        int x = 300;
        int y = 460;
        if (rotation % 2 != 0) {
            x = 460;
            y = 300;
        }
        _TFT->setRotation(rotation);
        _TFT->fillScreen(colors.at(rotation));
        _TFT->rect(0, 0, 20, 20, _TFT->rgb(255, 0, 149));
        _TFT->rect(x, y, 20, 20, _TFT->rgb(252, 252, 251));

        rotation++;
        if (rotation > 3)
            { rotation = 0; }
        delay(1000);
    }
}