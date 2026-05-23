# ST7796S - MSP4020 & MSP4021

Lightweight and optimized ST7796S TFT display library for ESP32 with optional XPT2046 touchscreen support.

Designed for:

* Arduino Nano ESP32
* ST7796S 320x480 SPI displays
* MSP4020 / MSP4021 modules
* High speed SPI rendering
* UTF-8 text rendering
* RGB565 image rendering
* Virtual keyboard support
* Touch calibration
* Simple and lightweight API

---

# VERSION

Current version: `1.3.0`

See:

```text
CHANGELOG.md
```

for release history.

---

# WHY THIS LIBRARY

This library focuses on:

* lightweight rendering
* simple API
* UTF-8 support
* touch calibration
* virtual keyboard
* RGB565 performance
* low memory usage
* ESP32 optimization

Unlike generic TFT libraries, this project is specifically optimized for MSP4020/MSP4021 hardware.

---

# FEATURES

## MSP4020

Display only:

* Hardware SPI
* Rotation support
* RGB565 rendering
* UTF-8 text rendering
* Custom fonts
* Text alignment helpers
* Text size metrics
* Images from flash memory
* Images from SD card / Stream
* Graphics primitives
* Rounded rectangles
* Progress bars
* Sliders
* Arcs

---

## MSP4021

MSP4020 + touchscreen:

* XPT2046 support
* Touch calibration
* Affine correction
* Smoothing filter
* Virtual keyboard
* SPI pause/resume support

---

# INSTALLATION

## PlatformIO

Add to `platformio.ini`:

```ini
lib_deps =
    https://github.com/DeathManOne/Screen-4inch-320x480-ST7796---MSP4020-and-MSP4021.git
```

---

# HARDWARE CONNECTIONS

## Arduino Nano ESP32

---

## TFT Display (ST7796S)

| TFT Pin | Arduino Nano ESP32 |
| :------ | :----------------- |
| VCC     | 3.3v or 5v         |
| GND     | GND                |
| CS      | 10                 |
| RESET   | 3.3v or GPIO       |
| DC/RS   | 5                  |
| SDI     | 11 (MOSI)          |
| SCK     | 13 (CLK)           |
| LED     | 3.3v               |
| SDO     | not connected      |

---

## Touchscreen (XPT2046)

| TOUCH Pin | Arduino Nano ESP32 |
| :-------- | :----------------- |
| T_CLK     | 13 (CLK)           |
| T_CS      | 4                  |
| T_DIN     | 11 (MOSI)          |
| T_DO      | 12 (MISO)          |
| T_IRQ     | not connected      |

---

# QUICK START

## MSP4020 (display only)

```cpp
#include <Arduino.h>
#include <SPI.h>
#include <MSP4020.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480

SPIClass SPI_TFT(FSPI);
ST7796S::MSP4020* TFT;

void setup() {
    SPI_TFT.begin(13, -1, 11);

    TFT = new ST7796S::MSP4020(
        SPI_TFT,
        10,
        5,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        -1
    );

    TFT->setRotation(3);
    TFT->fillScreen(TFT->rgb(0, 0, 0));

    TFT->rectFill(
        20,
        20,
        100,
        50,
        TFT->rgb(255, 0, 0)
    );
}

void loop() {
}
```

---

## MSP4021 (display + touch)

```cpp
#include <Arduino.h>
#include <SPI.h>
#include <MSP4021.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480

SPIClass SPI_TFT(FSPI);
ST7796S::MSP4021* TFT;

void setup() {
    SPI_TFT.begin(13, 12, 11);

    TFT = new ST7796S::MSP4021(
        SPI_TFT,
        4,
        10,
        5,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        -1
    );

    TFT->setRotation(3);
    TFT->fillScreen(TFT->rgb(0, 0, 0));
}

void loop() {
    int x = 0;
    int y = 0;

    if (TFT->TRead(x, y)) {
        TFT->circleFill(
            x,
            y,
            5,
            TFT->rgb(255, 0, 0)
        );
    }
}
```

---

# DISPLAY FUNCTIONS

# Colors

```cpp
uint16_t color = TFT->rgb(255, 0, 0);
```

---

# Screen

```cpp
TFT->fillScreen(color);
```

---

# Screen size

```cpp
int width = TFT->width();
int height = TFT->height();
```

Values automatically update with rotation.

---

# Text

```cpp
TFT->setFont(MyFont);
TFT->setTextColor(
    TFT->rgb(255, 255, 255)
);
TFT->setTextScale(2);

TFT->text(20, 20, "Hello");
```

UTF-8 supported:

```cpp
TFT->text(
    20,
    20,
    "Bonjour été àèù"
);
```

---

# Text metrics

```cpp
int w = TFT->textWidth("Hello");
int h = TFT->textHeight("Hello");
```

Uses current:

* font
* UTF-8 rendering
* text scale

Useful for:

* dynamic layouts
* custom widgets
* clipping
* manual centering
* responsive UI

---

# Dynamic layout example

```cpp
const char* title = "SETTINGS";

int w = TFT->textWidth(title);
int h = TFT->textHeight(title);

TFT->text(
    (TFT->width() - w) / 2,
    20,
    title
);
```

---

# Text alignment

## Top left

```cpp
TFT->textTopLeft(
    0,
    0,
    320,
    40,
    "Top Left"
);
```

## Top center

```cpp
TFT->textTopCenter(
    0,
    0,
    320,
    40,
    "Top Center"
);
```

## Top right

```cpp
TFT->textTopRight(
    0,
    0,
    320,
    40,
    "Top Right"
);
```

## Center left

```cpp
TFT->textCenterLeft(
    0,
    0,
    320,
    40,
    "Center Left"
);
```

## Center

```cpp
TFT->textCenter(
    0,
    0,
    320,
    40,
    "Centered"
);
```

## Center right

```cpp
TFT->textCenterRight(
    0,
    0,
    320,
    40,
    "Center Right"
);
```

## Bottom left

```cpp
TFT->textBottomLeft(
    0,
    0,
    320,
    40,
    "Bottom Left"
);
```

## Bottom center

```cpp
TFT->textBottomCenter(
    0,
    0,
    320,
    40,
    "Bottom Center"
);
```

## Bottom right

```cpp
TFT->textBottomRight(
    0,
    0,
    320,
    40,
    "Bottom Right"
);
```

---

# Images from Flash

```cpp
#include "images/myImage.h"

TFT->image(20, 20, myImage);
```

---

# Images from SD Card

```cpp
File file = SD.open(
    "/images/test.rgb565",
    FILE_READ
);

if (file) {
    TFT->imageSD(0, 0, file);
    file.close();
}
```

RGB565 file format:

* uint16_t width (little-endian)
* uint16_t height (little-endian)
* RGB565 pixel data (big-endian)

---

# GRAPHICS

## Horizontal line

```cpp
TFT->lineH(
    10,
    10,
    100,
    TFT->rgb(255, 0, 0)
);
```

---

## Vertical line

```cpp
TFT->lineV(
    10,
    10,
    100,
    TFT->rgb(255, 0, 0)
);
```

---

## Rectangle

```cpp
TFT->rect(
    20,
    20,
    100,
    50,
    TFT->rgb(255, 0, 0)
);
```

---

## Filled rectangle

```cpp
TFT->rectFill(
    20,
    20,
    100,
    50,
    TFT->rgb(255, 0, 0)
);
```

---

## Rounded rectangle

```cpp
TFT->rectRound(
    20,
    20,
    100,
    50,
    10,
    TFT->rgb(255, 0, 0)
);
```

---

## Filled rounded rectangle

```cpp
TFT->rectRoundFill(
    20,
    20,
    100,
    50,
    10,
    TFT->rgb(255, 0, 0)
);
```

---

## Circle

```cpp
TFT->circle(
    100,
    100,
    30,
    TFT->rgb(255, 0, 0)
);
```

---

## Filled circle

```cpp
TFT->circleFill(
    100,
    100,
    30,
    TFT->rgb(255, 0, 0)
);
```

---

## Filled triangle

```cpp
TFT->triangleFill(
    50,
    50,
    20,
    100,
    80,
    100,
    TFT->rgb(255, 0, 0)
);
```

---

## Arc

```cpp
TFT->arc(
    160,
    120,
    50,
    0,
    180,
    TFT->rgb(255, 0, 0)
);
```

---

## Progress bar

```cpp
TFT->progressBar(
    20,
    20,
    200,
    30,
    75,
    TFT->rgb(0, 255, 0)
);
```

---

## Slider

```cpp
TFT->slider(
    20,
    20,
    200,
    30,
    40,
    TFT->rgb(255, 0, 0)
);
```

---

# TOUCHSCREEN

# Read touch

```cpp
int x = 0;
int y = 0;

if (TFT->TRead(x, y)) {
    Serial.println(x);
    Serial.println(y);
}
```

---

# Touch Calibration

## Automatic calibration

Always call `setRotation()` BEFORE calibration.

```cpp
TFT->setRotation(3);

while (!TFT->TCalibrate()) {
}
```

---

## Save calibration

```cpp
bool swapXY;
bool invertX;
bool invertY;

float xa, xb, xc;
float ya, yb, yc;

TFT->TCalibrateInfo(
    swapXY,
    invertX,
    invertY,
    xa,
    xb,
    xc,
    ya,
    yb,
    yc
);
```

Save these values in EEPROM, Preferences or SD card.

---

## Restore calibration

```cpp
TFT->TCalibrate(
    swapXY,
    invertX,
    invertY,
    xa,
    xb,
    xc,
    ya,
    yb,
    yc
);
```

---

# TOUCH PAUSE / RESUME

Useful when another SPI device shares the same bus.

```cpp
TFT->TPause();

// Your SPI code

TFT->TResume();
```

Used internally by the virtual keyboard.

---

# VIRTUAL KEYBOARD

## Draw keyboard

```cpp
TFT->KDraw("Enter your name");
```

---

## Update keyboard

```cpp
int tx = 0;
int ty = 0;

if (TFT->TRead(tx, ty)) {
    if (TFT->KUpdate(tx, ty)) {
        Serial.println(
            TFT->KRead().c_str()
        );
    }
}
```

---

## Keyboard buffer

```cpp
TFT->KSetText("Default text");
```

```cpp
TFT->KClear();
```

```cpp
std::string value = TFT->KRead();
```

---

# FONT GENERATOR

`font2h.py` converts TTF fonts into ST7796S compatible headers.

Supported:

* UTF-8
* Accents
* Symbols
* Custom sizes

Usage:

```text
Drag & drop .ttf file onto dragYourFileOverMe.bat
```

Generated files are created in:

```text
/fonts
```

---

# IMAGE GENERATOR

`image2h.py` converts images into:

* C++ headers
* RGB565 raw files

Usage:

```text
Drag & drop image onto dragYourFileOverMe.bat
```

Generated:

```text
/images/myImage.h
/images/myImage.rgb565
```

---

# PERFORMANCE NOTES

* TFT display is write-only.
* Touchscreen uses MISO.
* Optimized RGB565 rendering.
* Buffered SPI transfers.
* Hardware SPI recommended.
* Designed for ESP32.

---

# EXAMPLES

Examples are available inside:

```text
/tests
```

---

# LICENSE

This project is licensed under the GNU GPL v3 or later.

See:

```text
LICENSE
```

for full license information.

---

# LINKS

* LCDWiki:
  https://www.lcdwiki.com/4.0inch_SPI_Module_ST7796

* TFT_eSPI:
  https://github.com/Bodmer/TFT_eSPI

* ST7796S Datasheet:
  https://www.lcdwiki.com/res/MSP4021/ST7796S-Sitronix.pdf

* MSP4020/MSP4021 User Manual:
  https://www.lcdwiki.com/res/MSP4021/4.0inch_SPI_Module_MSP4020&MSP4021_User_Manual_EN.pdf