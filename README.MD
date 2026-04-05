## LINKS
- [LCDWiki](https://www.lcdwiki.com/4.0inch_SPI_Module_ST7796)
- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)
- [DataSheet](https://www.lcdwiki.com/res/MSP4021/ST7796S-Sitronix.pdf)
- [UserManual](https://www.lcdwiki.com/res/MSP4021/4.0inch_SPI_Module_MSP4020&MSP4021_User_Manual_EN.pdf)
## NEXT STEP
- make calibration more accurate
- add fonts
## WHAT IS WHAT
- **MSP4020** -> `screen` only
- **MSP4021** -> `MSP4020` + `TouchScreen`
## PINS CONNECTIONS
- pins numbers are from `Arduino nano ESP32`
### Screen
| Name  | Pin on board      | Description                                 |
| :---- | :---------------- | :------------------------------------------ |
| VCC   | `3.3v`            | ***works for me, if not for you, then 5v*** |
| GND   | `ground`          |                                             |
| CS    | `10`              | or an other available pin                   |
| RESET | `3.3v`            | or an available pin                         |
| DC/RS | `5`               | or an other available pin                   |
| SDI   | `11`              | ***`SPI_MOSI`***                            |
| SCK   | `13`              | ***`SPI_CLK`***                             |
| LED   | `3.3v`            |                                             |
| SDO   | **not connected** |                                             |
### Touch
| Name  | Pin on board      | Description               |
| :-----| :---------------- | :------------------------ |
| T_CLK | `13`              | ***`SPI_CLK`***           |
| T_CS  | `4`               | or an other available pin |
| T_DIN | `11`              | ***`SPI_MOSI`***          |
| T_DO  | `12`              | ***`SPI_MISO`***          |
| T_IRQ | **not connected** |                           |
## EXAMPLES
| Test     | File                        | Description                                                  |
| :------- | :-------------------------- | :----------------------------------------------------------- |
| Screen   | `tests/screen.ino`          | check whether your screen works and well connected           |
| rotation | `tests/screen_rotation.ino` | check rotation with point 0-0 and point maxX-maxY            |
| Touch    | `tests/touch.ino`           | check whether your Touch and screen works and well connected |
## TOUCH
### CALIBRATE
#### FROM SAVED FILE
```c++
_TFT->TCalibrate(swapXY, invertX, invertY, coeffXA, coeffXB, coeffXC, coeffYA, coeffYB, coeffYC);
```
#### CALIBRATE (with save)
- Do `setRotation` **BEFORE** a `TCalibrate`
```c++
int rotation = 3 // between 0 and 3
_TFT->setRotation(3);
while (!_TFT->TCalibrate());
/** IN ORDER TO SAVE CALIBRATION
  * _TFT->TGetInvertX(); // bool
  * _TFT->TGetInvertY(); // bool
  * _TFT->TGetSwapXY();  // bool
  *
  * _TFT->TGetCoeffXA(); // float
  * _TFT->TGetCoeffXB(); // float
  * _TFT->TGetCoeffXC(); // float
  * _TFT->TGetCoeffYA(); // float
  * _TFT->TGetCoeffYB(); // float
  * _TFT->TGetCoeffYC(); // float
  */
```
### STOP SPI (for a time if needed)
```c++
bool TUsed = _TFT->TUsed();
if (TUsed)
  { _TFT->TStop(); }
//
// YOUR CODE
//
if (TUsed)
  { _TFT->TStart(); }
```
