## LINKS
- [LCDWiki](https://www.lcdwiki.com/4.0inch_SPI_Module_ST7796)
- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)
- [DataSheet](https://www.lcdwiki.com/res/MSP4021/ST7796S-Sitronix.pdf)
- [UserManual](https://www.lcdwiki.com/res/MSP4021/4.0inch_SPI_Module_MSP4020&MSP4021_User_Manual_EN.pdf)
## NEXT STEP
- change rotation without a new calibration (again)
- add font
## WHAT IS WHAT
- MSP4020 -> screen only
- MSP4021 -> MSP4020 + touchscreen
## PINS CONNECTIONS
`pins numbers are from Arduino nano ESP32`
### Screen
| Name    | Pin on board | Description             |
| :------ | :----------- | :---------------------- |
| `VCC`   | `3.3v`   | ***`works for me, if not for you, then 5v`*** |
| `GND`   | `ground`     | |
| `CS`    | `10`         | `or an other available pin` |
| `RESET` | `3.3v`       | `or an available pin` |
| `DC/RS` | `5`          | `or an other available pin` |
| `SDI`   | `11`         | ***`SPI_MOSI`*** |
| `SCK`   | `13`         | ***`SPI_CLK`*** |
| `LED`   | `3.3v`       |  |
| `SDO`   | **`not connected`** |  |
### Touch
| Name    | Pin on board | Description               |
| :-------| :----------- | :------------------------ |
| `T_CLK` | `13`         | ***`SPI_CLK`*** |
| `T_CS`  | `4`          | `or an other available pin` |
| `T_DIN` | `11`         | ***`SPI_MOSI`*** |
| `T_DO`  | `12`         | ***`SPI_MISO`*** |
| `T_IRQ` | **`not connected`** |  |
## EXAMPLES (for tests)
### MSP4020 and MSP4021
- also work for MSP4021 if you don't want to use TouchScreen
```c++
#include <Arduino.h>
#include <SPI.h>
#include <MSP4020.h>

#define TFT_CS 10
#define TFT_DC 5
#define TFT_RST -1

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480

ST7796S::MSP4021 *_TFT;

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
```
### MSP4021
```c++
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
  while (!_TFT->TCalibrate()) {}

  /** IN ORDER TO SAVE CALIBRATION
   * _TFT->getInvertX(); // bool
   * _TFT->getInvertY(); // bool
   * _TFT->getSwapXY(); // bool
   * _TFT->getCoeffXA(); // float
   * _TFT->getCoeffXB(); // float
   * _TFT->getCoeffXC(); // float
   * _TFT->getCoeffYA(); // float
   * _TFT->getCoeffYB(); // float
   * _TFT->getCoeffYC(); // float
   */
  _TFT->fillScreen(_TFT->rgb(0, 0, 0));
}

void loop() {
  delay(500);

  int x, y;
  bool antiSmoothing = true; // optional | default = true
  if (_TFT->TRead(x, y, antiSmoothing)) {
    _TFT->rect(x, y, 20, 20, _TFT->rgb(0, 255, 0));
    delay(3000);
    _TFT->rect(x, y, 20, 20, _TFT->rgb(0, 0, 0));
  }
}
```
