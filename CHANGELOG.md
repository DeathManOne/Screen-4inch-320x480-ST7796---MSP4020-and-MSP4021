# Changelog

All notable changes to this project will be documented in this file.

---

# [1.5.0] - 2026-09-19

## Added

* Added automatic ST7796S and ILI9488 controller detection without fallback.
* Added a registry-based display-driver architecture for future controllers.
* Added dedicated TFT SDO/MISO support for controller detection on shared SPI buses.
* Added controller detection status, name and reusable diagnostic reports.
* Added `Print`-compatible detection report output.
* Added automatic RGB565-to-RGB666 pixel conversion for ILI9488 displays.
* Added raw XPT2046 diagnostic readings through `TReadRaw()`.

## Changed

* Updated all examples for separate TFT and touch MISO wiring.
* Unified touch-pressure detection and added release debouncing during calibration.
* Improved continuous touch reporting when coordinate smoothing is enabled.

## Fixed

* Fixed XPT2046 pressure calculation and unsigned underflow.
* Fixed valid touches near the calibrated top-left corner being rejected.
* Prevented calibration from blocking while touch access is paused or display detection failed.
* Preserved the previous touch orientation when a new affine calibration fails.
* Kept touch chip select inactive outside SPI transactions during pause and resume.
* Prevented calibrated touch coordinates from being returned before calibration succeeds.

---

# [1.4.0] - 2026-07-26

## Added

* Added `shutdown()` to place the display into its shutdown state.
* Added `startup()` to reinitialize the display after shutdown.
* Added `reset()` for a complete display restart.
* Display rotation is now automatically restored after `startup()` and `reset()`.

## Notes

* `shutdown()` holds the hardware reset pin LOW when available.
* If no reset pin is configured, `shutdown()` and `reset()` clear the display to black instead.

---

# [1.3.8] - 2026-06-07

## Changed

* Replaced legacy header guards with `#pragma once`.
* Cleaned and minimized includes across headers and source files.
* Removed unnecessary `Arduino.h` includes where not required.
* Updated generated font and image headers to use `#pragma once`.
* Updated generated font headers to include `<font.h>`.
* Updated generated image headers to include `<image.h>`.
* Replaced virtual keyboard `std::string` buffer with fixed `char[65]` buffer.
* Changed `KRead()` return type from `std::string` to `const char*`.
* Updated keyboard buffer handling to avoid dynamic allocation.

## Improved

* Reduced memory usage in the virtual keyboard.
* Improved generated asset portability for user projects.
* Improved include hygiene and build clarity.

---

# [1.3.0] - 2026-05-24

## Added

### Text metrics helpers

Added:

* `textWidth()`
* `textHeight()`

These functions return rendered UTF-8 text dimensions using the current font and text scale.

Useful for:

* dynamic layouts
* responsive UI
* manual centering
* clipping
* custom widgets

## Improved

### Text rendering engine

* Added private `_textBounds()` engine.
* Refactored `_textAlign()` internals.
* Reduced duplicated UTF-8 parsing logic.
* Improved text positioning consistency.
* Improved text alignment precision.
* Improved API consistency.

### Documentation

* README fully updated.
* Added alignment examples.
* Added text metrics examples.
* Improved API documentation.
* Added project changelog.

---

# [1.2.0] - 2026-05-21

## Added

### Text alignment helpers

Added new text alignment functions:

* `textTopLeft()`
* `textTopCenter()`
* `textTopRight()`
* `textCenterLeft()`
* `textCenterRight()`
* `textBottomLeft()`
* `textBottomCenter()`
* `textBottomRight()`

These functions simplify text positioning inside rectangular areas.

### Screen size helpers

Added:

* `width()`
* `height()`

These values automatically update with display rotation.

## Improved

### Text rendering system

* Refactored text alignment internals.
* Added private `_textAlign()` engine.
* Reduced duplicated alignment calculations.
* Improved API consistency.

---

# [1.1.0] - 2026-05-11

## Added

* UTF-8 text rendering
* Virtual keyboard
* Touch calibration
* RGB565 image rendering
* SD card image loading
* Rounded rectangles
* Progress bars
* Sliders
* Arc rendering

Initial public release.
