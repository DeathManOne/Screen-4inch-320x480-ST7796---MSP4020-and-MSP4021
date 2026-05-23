# Changelog

All notable changes to this project will be documented in this file.

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

---

## Improved

### Text rendering engine

* Added private `_textBounds()` engine.
* Refactored `_textAlign()` internals.
* Reduced duplicated UTF-8 parsing logic.
* Improved text positioning consistency.
* Improved text alignment precision.
* Improved API consistency.

---

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

---

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
