# ST7796S

Lightweight and fast ST7796S / MSP4020 / MSP4021 TFT library for ESP32.

## Features

* ST7796S TFT display support
* MSP4020 and MSP4021 board support
* XPT2046 touch controller support
* UTF-8 text rendering
* Custom font support
* RGB565 image rendering
* SD card image loading
* Touchscreen calibration
* Virtual keyboard
* Optimized SPI transfers
* Hardware SPI support
* Low memory footprint

---

# INSTALLATION

Copy the library into your Arduino libraries folder.

```text
Documents/
└── Arduino/
    └── libraries/
        └── ST7796S/
```

---

# BASIC USAGE

## MSP4020

```cpp
#include <MSP4020.h>

SPIClass SPI_TFT(VSPI);
ST7796S::MSP4020* TFT = nullptr;

void setup() {
    SPI_TFT.begin();

    TFT = new ST7796S::MSP4020(
        SPI_TFT,
        15,
        2,
        320,
        480
    );
}
```

---

## MSP4021

```cpp
#include <MSP4021.h>

SPIClass SPI_TFT(VSPI);
ST7796S::MSP4021* TFT = nullptr;

void setup() {
    SPI_TFT.begin();

    TFT = new ST7796S::MSP4021(
        SPI_TFT,
        21,
        15,
        2,
        320,
        480
    );
}
```

---

# COLORS

```cpp
uint16_t red   = TFT.rgb(255, 0, 0);
uint16_t green = TFT.rgb(0, 255, 0);
uint16_t blue  = TFT.rgb(0, 0, 255);
```

---

# SCREEN

## Clear screen

Fills the entire display with a single RGB565 color.

```cpp
TFT.fillScreen(
    TFT.rgb(0, 0, 0)
);
```

---

## Rotation

Sets the display orientation.

```cpp
TFT.setRotation(0);
TFT.setRotation(1);
TFT.setRotation(2);
TFT.setRotation(3);
```

---

## Shutdown

Places the display into its shutdown state.
If a hardware reset pin is configured, the ST7796S controller is held in hardware reset.
If no reset pin is available, the display is simply cleared to black.

```cpp
TFT.shutdown();
```

---

## Startup

Restarts the display after a previous `shutdown()`.
When a hardware reset pin is available, the controller is reinitialized automatically and the previous display rotation is restored.

```cpp
TFT.startup();
```

---

## Reset

Performs a complete display reset.
If a hardware reset pin is available, this method internally performs:

```cpp
TFT.shutdown();
TFT.startup();
```

If no reset pin is configured, the display is cleared to black.

```cpp
TFT.reset();
```

---

# DRAWING

## Pixel

```cpp
TFT.pixel(
    100,
    100,
    TFT.rgb(255, 0, 0)
);
```

## Line

```cpp
TFT.line(
    10,
    10,
    200,
    100,
    TFT.rgb(255, 0, 0)
);
```

## Rectangle

```cpp
TFT.rect(
    20,
    20,
    120,
    60,
    TFT.rgb(255, 0, 0)
);
```

## Filled rectangle

```cpp
TFT.rectFill(
    20,
    20,
    120,
    60,
    TFT.rgb(255, 0, 0)
);
```

## Circle

```cpp
TFT.circle(
    160,
    120,
    50,
    TFT.rgb(255, 0, 0)
);
```

## Filled circle

```cpp
TFT.circleFill(
    160,
    120,
    50,
    TFT.rgb(255, 0, 0)
);
```

## Triangle

```cpp
TFT.triangle(
    50, 50,
    150, 50,
    100, 150,
    TFT.rgb(255, 0, 0)
);
```

## Filled triangle

```cpp
TFT.triangleFill(
    50, 50,
    150, 50,
    100, 150,
    TFT.rgb(255, 0, 0)
);
```

## Arc

```cpp
TFT.arc(
    160,
    120,
    50,
    0,
    180,
    TFT.rgb(255, 0, 0)
);
```

## Progress bar

```cpp
TFT.progressBar(
    20,
    20,
    200,
    30,
    75,
    TFT.rgb(0, 255, 0)
);
```

## Slider

```cpp
TFT.slider(
    20,
    20,
    200,
    30,
    40,
    TFT.rgb(255, 0, 0)
);
```

---

# TEXT

## Built-in font

```cpp
TFT.setTextScale(2);
TFT.setTextColor(
    TFT.rgb(255, 255, 255)
);

TFT.text(
    10,
    10,
    "Hello world"
);
```

## Custom font

```cpp
#include "DejaVuSans_Bold_18.h"

TFT.setFont(
    DejaVuSans_Bold_18
);
```

## Centered text

```cpp
TFT.textCenter(
    0,
    0,
    320,
    40,
    "Centered"
);
```

---

# IMAGES

## Flash memory image

```cpp
#include "logo.h"

TFT.image(
    0,
    0,
    logo
);
```

## SD card image

```cpp
File file = SD.open(
    "/logo.rgb565"
);

TFT.imageSD(
    0,
    0,
    file
);
```

---

# TOUCHSCREEN

## Read touch

```cpp
int x;
int y;

if (TFT.TRead(x, y)) {
    Serial.println(x);
    Serial.println(y);
}
```

---

# TOUCH CALIBRATION

Always call `setRotation()` before calibration.

## Automatic calibration

```cpp
while (!TFT.TCalibrate()) {
}
```

## Save calibration

```cpp
bool swapXY;
bool invertX;
bool invertY;

float xa, xb, xc;
float ya, yb, yc;

TFT.TCalibrateInfo(
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

Store these values in EEPROM, Preferences or SD card.

## Restore calibration

```cpp
TFT.TCalibrate(
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
TFT.TPause();

/* SPI code */

TFT.TResume();
```

---

# VIRTUAL KEYBOARD

## Draw keyboard

```cpp
TFT.KDraw(
    "Enter your name"
);
```

## Update keyboard

```cpp
int tx;
int ty;

if (TFT.TRead(tx, ty)) {
    if (TFT.KUpdate(tx, ty)) {
        Serial.println(
            TFT.KRead()
        );
    }
}
```

## Keyboard buffer

```cpp
TFT.KSetText(
    "Default text"
);
```

```cpp
TFT.KClear();
```

```cpp
const char* value =
    TFT.KRead();
```

---

# FONT GENERATOR

`font2h.py` converts TTF fonts into ST7796S compatible headers.

## Features

* UTF-8
* Accents
* Symbols
* Multiple sizes

## Usage

```text
Drag & drop .ttf files onto dragYourFileOverMe.bat
```

## Generated files

```text
/fonts/*.h
```

---

# IMAGE GENERATOR

`image2h.py` converts images into:

* C++ headers
* RGB565 files

## Supported formats

* PNG
* JPG
* JPEG
* BMP
* GIF
* WEBP

## Usage

```text
Drag & drop image files onto dragYourFileOverMe.bat
```

## Generated files

```text
/images/*.h
/images/*.rgb565
```

---

# PERFORMANCE NOTES

* Write-only TFT display
* Hardware SPI optimized
* RGB565 rendering
* Buffered SPI transfers
* ESP32 focused
* Low memory footprint

---

# EXAMPLES

Examples are available inside:

```text
/tests
```

---

# LICENSE

GNU GPL v3 or later.

See:

```text
LICENSE
```

for details.

---

# LINKS

LCDWiki
https://www.lcdwiki.com/4.0inch_SPI_Module_ST7796

TFT_eSPI
https://github.com/Bodmer/TFT_eSPI

ST7796S Datasheet
https://www.lcdwiki.com/res/MSP4021/ST7796S-Sitronix.pdf

MSP4020 / MSP4021 User Manual
https://www.lcdwiki.com/res/MSP4021/4.0inch_SPI_Module_MSP4020&MSP4021_User_Manual_EN.pdf
