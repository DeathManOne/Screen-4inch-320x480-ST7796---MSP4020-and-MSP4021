# MSP402x TFT (ST7796S / ILI9488)

Lightweight and fast MSP4020 / MSP4021 TFT library for ESP32, with automatic
ST7796S and ILI9488 controller detection.

## Features

* ST7796S and ILI9488 TFT display support
* Automatic controller detection without fallback
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

constexpr int TFT_CS = 10;
constexpr int TFT_DC = 5;
constexpr int TFT_RST = -1;
constexpr int TFT_MISO = A3;
constexpr int TFT_MOSI = 11;
constexpr int TFT_CLK = 13;

void setup() {
    SPI_TFT.begin(TFT_CLK, TFT_MISO, TFT_MOSI);

    TFT = new ST7796S::MSP4020(
        SPI_TFT,
        TFT_CS,
        TFT_DC,
        320,
        480,
        TFT_RST,
        TFT_MISO,
        TFT_MISO,
        TFT_MOSI,
        TFT_CLK
    );
}
```

---

## MSP4021

```cpp
#include <MSP4021.h>

SPIClass SPI_TFT(VSPI);
ST7796S::MSP4021* TFT = nullptr;

constexpr int TOUCH_CS = 4;
constexpr int TFT_CS = 10;
constexpr int TFT_DC = 5;
constexpr int TFT_RST = -1;
constexpr int TFT_MISO = A3;
constexpr int TOUCH_MISO = 12;
constexpr int TFT_MOSI = 11;
constexpr int TFT_CLK = 13;

void setup() {
    SPI_TFT.begin(TFT_CLK, TOUCH_MISO, TFT_MOSI);

    TFT = new ST7796S::MSP4021(
        SPI_TFT,
        TOUCH_CS,
        TFT_CS,
        TFT_DC,
        320,
        480,
        TFT_RST,
        TFT_MISO,
        TOUCH_MISO,
        TFT_MOSI,
        TFT_CLK
    );
}
```

---

## SPI MISO wiring

The display and touch controller expose two independent MISO signals:

* `TFT SDO (MISO)` returns display-controller registers and is required for
  automatic controller detection.
* `TOUCH T_DO` returns XPT2046 touch measurements.

They may use the same hardware SPI bus only when the module correctly releases
each MISO line while its chip select is HIGH. When this cannot be guaranteed,
use separate readable pins and pass the TFT and touch pins explicitly as shown
above.

---

## Controller detection

The TFT `SDO (MISO)` line must be connected to a readable SPI MISO pin.
When the TFT and touch controller use different MISO pins, pass the dedicated
TFT MISO pin to the constructor through `pinMISO_TFT`. The dedicated
software-SPI detection path is enabled only when `pinMISO_TFT`, `pinMOSI` and
`pinCLK` are all provided.

The library probes every registered driver and does not select a fallback when
no signature matches. The display then remains inactive, while the collected
diagnostic values remain available:

```cpp
if (TFT->detected()) {
    Serial.println(TFT->controllerName());
} else {
    TFT->printDetection(Serial);
}
```

`printDetection()` accepts any Arduino `Print` destination, including a serial
port, file or network client. The same results can be accessed without
formatting through `detectionReport()`.

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
If a hardware reset pin is configured, the detected TFT controller is held in hardware reset.
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

if (TFT->TRead(x, y)) {
    Serial.println(x);
    Serial.println(y);
}
```

`TRead(x, y)` disables coordinate smoothing by default. Pass `false` as the
third argument to enable smoothing while preserving continuous contact reports:

```cpp
if (TFT->TRead(x, y, false)) {
    // Smoothed calibrated coordinates
}
```

`TRead()` returns `false` until `TCalibrate()` has completed successfully or
saved calibration values have been restored with the manual overload.

For XPT2046 diagnostics, raw X, Y, Z1 and Z2 values are also available:

```cpp
uint16_t rawX, rawY, z1, z2;

if (TFT->TReadRaw(rawX, rawY, z1, z2)) {
    // Raw 12-bit ADC measurements; this does not mean the panel is touched.
}
```

---

# TOUCH CALIBRATION

Always call `setRotation()` before calibration. Calibration values belong to
the selected rotation and the individual touch panel.

## Automatic calibration

```cpp
if (!TFT->TCalibrate()) {
    Serial.println("Touch calibration failed");
}
```

`TCalibrate()` returns `false` immediately when touch access is paused, when no
display controller was detected, or when the affine calculation fails.

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

Store these values in EEPROM, Preferences or SD card.

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

/* SPI code */

TFT->TResume();
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

if (TFT->TRead(tx, ty)) {
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

`font2h.py` converts TTF fonts into headers compatible with this library.

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

* TFT register reads used only for controller detection
* Hardware SPI optimized
* RGB565 public color and image format
* Automatic RGB565-to-RGB666 conversion for ILI9488
* Buffered SPI transfers
* ESP32 focused
* Low memory footprint

---

# ADDING A DISPLAY DRIVER

Display-controller implementations are isolated in `src/drivers`. To add a
controller:

1. Create a class derived from `drivers::DisplayDriver` in its own `.cpp` file.
2. Implement `name()`, `detect()` and `init()`.
3. Override `setRotation()`, `setAddress()` or the SPI settings only when the
   controller differs from the shared defaults.
4. Keep RGB565 as the public color format. Override `bytesPerPixel()` and
   `encodePixels()` when the controller requires another wire format, such as
   the ILI9488 RGB666 transfer format.
5. Expose one stateless driver accessor in a matching header.
6. Include that header in `Registry.cpp` and add the accessor to `REGISTRY`.

`detect()` must record the useful register response in `DetectionReport` and
return `true` only for a verified signature. Registry order matters when two
controllers expose overlapping signatures. There is deliberately no fallback
driver: an unknown signature remains available through `printDetection()` for
supporting a future library update.

---

# EXAMPLES

Examples are available inside:

```text
/examples
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
