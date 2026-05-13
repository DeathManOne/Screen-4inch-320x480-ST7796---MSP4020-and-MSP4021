#
# image2h.py
#
# Copyright (c) 2026 DeathManOne
# https://github.com/DeathManOne
#
# This file is part of the ST7796S library.
#
# ST7796S is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# ST7796S is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with ST7796S.
# If not, see <https://www.gnu.org/licenses/>.
#

import sys, os
from PIL import Image

MAX_SIZE = 300

def clean_name(path):
    name = os.path.splitext(os.path.basename(path))[0]
    return "".join(c if c.isalnum() else "_" for c in name)

def rgb888_to_rgb565(r, g, b):
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

def generate_image_header(image_path):
    output_dir = os.path.join(os.path.dirname(__file__), "images")
    os.makedirs(output_dir, exist_ok=True)

    image_name = clean_name(image_path)
    output_h = os.path.join(output_dir, f"{image_name}.h")
    output_rgb565 = os.path.join(output_dir, f"{image_name}.rgb565")

    img = Image.open(image_path).convert("RGB")
    w, h = img.size
    largest = max(w, h)

    if largest > MAX_SIZE:
        if w >= h:
            new_w = MAX_SIZE
            new_h = int(h * (MAX_SIZE / w))
        else:
            new_h = MAX_SIZE
            new_w = int(w * (MAX_SIZE / h))
        img = img.resize((new_w, new_h), Image.LANCZOS)
    w, h = img.size

    guard = f"_ST7796S_IMAGE_{image_name.upper()}_"
    pixels = []

    for y in range(h):
        for x in range(w):
            pixels.append(img.getpixel((x, y)))

    with open(output_h, "w", encoding="utf-8") as f:
        f.write(f"#ifndef {guard}\n")
        f.write(f"#define {guard}\n\n")
        f.write("#include <Arduino.h>\n")
        f.write("#include <images/image.h>\n\n")
        f.write("namespace ST7796S {\n")
        f.write(f"    const uint16_t {image_name}_width  = {w};\n")
        f.write(f"    const uint16_t {image_name}_height = {h};\n")
        f.write(f"    const uint16_t {image_name}_data[] PROGMEM = {{\n")

        for i, (r, g, b) in enumerate(pixels):
            color = rgb888_to_rgb565(r, g, b)
            if i % 12 == 0:
                f.write("        ")
            f.write(f"0x{color:04X}")
            if i != len(pixels) - 1:
                f.write(", ")
            if i % 12 == 11:
                f.write("\n")

        if len(pixels) % 12 != 0:
            f.write("\n")
        f.write("    };\n")
        f.write(f"    const Image {image_name} = {{{w}, {h}, {image_name}_data, 0x0000, false}};\n")
        f.write("}\n")
        f.write("#endif\n")
    
    with open(output_rgb565, "wb") as f:
        f.write(w.to_bytes(2, "little"))
        f.write(h.to_bytes(2, "little"))

        for y in range(h):
            for x in range(w):
                r, g, b = img.getpixel((x, y))
                color = rgb888_to_rgb565(r, g, b)
                f.write(color.to_bytes(2, "big"))
    
    print(f"✔ Generated: {output_h}")
    print(f"✔ Generated: {output_rgb565}")
    print(f"  Size: {w} x {h}")
    print(f"  Flash/RAW: {w * h * 2} bytes + 4 bytes header\n")

def main():
    if len(sys.argv) < 2:
        print("👉 Drag & drop image files here")
        input()
        return

    for image_path in sys.argv[1:]:
        try:
            generate_image_header(image_path)
        except Exception as e:
            print(f"Error with {image_path}")
            print(e)

    print("DONE")
    input()

if __name__ == "__main__":
    main()