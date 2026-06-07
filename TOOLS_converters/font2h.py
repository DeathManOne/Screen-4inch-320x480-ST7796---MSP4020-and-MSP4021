#
# font2h.py
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
from PIL import Image, ImageFont, ImageDraw
####################################
####  ----- FONTS SIZES! -----  ####
###         MIN SIZE: 10         ###
##          MAX SIZE: 24          ##
#   COMMENT WHAT YOU DO NOT NEED   #
####################################
SIZES = [
    10,
    12,
    14,
    16,
    18,
    20,
    24
]
######################################
##        DO    NOT    TOUCH        ##
######################################
CHARSET = [
    "à", "â", "ä",
    "é", "è", "ê", "ë",
    "î", "ï",
    "ô", "ö",
    "ù", "û", "ü",
    "À", "Â", "Ä",
    "É", "È", "Ê", "Ë",
    "Î", "Ï",
    "Ô", "Ö",
    "Ù", "Û", "Ü",
    "ç", "Ç",
    "¤", "£", "§", "°", "€"
    
]

CHARSET += [
    *[chr(i) for i in range(32, 127)]
]

def trim_x(img):
    bbox = img.getbbox()
    if bbox:
        return img.crop((bbox[0], 0, bbox[2], img.height))
    return img

def generate_font(font_path, size):
    output_dir = os.path.join(os.path.dirname(__file__), "fonts")
    os.makedirs(output_dir, exist_ok=True)

    font_name = "".join(c if c.isalnum() else "_" for c in os.path.splitext(os.path.basename(font_path))[0])
    output_file = os.path.join(output_dir, f"{font_name}_{size}.h")

    font = ImageFont.truetype(font_path, size)
    ascent, descent = font.getmetrics()
    full_height = ascent + descent

    data_lines = []
    offsets = []
    widths = []
    heights = []
    yOffsets = []
    unicode_vals = []

    offset = 0
    for index, char in enumerate(CHARSET):
        unicode_val = ord(char)
        if char == " ":
            w = max(1, size // 3)
            h = 1
            yOff = ascent // 2
            img = Image.new("1", (w, h), 0)
        else:
            img = Image.new("1", (size*2, full_height), 0)
            draw = ImageDraw.Draw(img)
            draw.text((0, 0), char, font=font, fill=1)
            img = trim_x(img)
            bbox = img.getbbox()
            if bbox:
                top, bottom = bbox[1], bbox[3]
                img = img.crop((0, top, img.width, bottom))
                h = bottom - top
                yOff = top
            else:
                h = 1
                yOff = 0
            w = max(1, img.width)

        widths.append(w)
        heights.append(h)
        yOffsets.append(yOff)
        offsets.append(offset)
        unicode_vals.append(unicode_val)
        offset += w

        cols = []
        for x in range(w):
            col_val = 0
            for y in range(h):
                if img.getpixel((x, y)):
                    col_val |= (1 << y)
            cols.append(f"0x{col_val:08X}")

        display_char = char
        if char == '\n':
            display_char = '\\n'
        elif char == '\t':
            display_char = '\\t'
        elif char == ' ':
            display_char = 'space'
        data_lines.append(f"/* [{index}] '{display_char}' (0x{unicode_val:04X}) */ {', '.join(cols)}")

    line_length = 16
    with open(output_file, "w", encoding="utf-8") as f:
        f.write(f"#pragma once\n\n")
        f.write("#include <pgmspace.h>\n#include <cstdint>\n#include <font.h>\n\n")
        f.write("namespace ST7796S {\n")

        f.write(f"    const uint32_t {font_name}_{size}_data[] PROGMEM = {{\n")
        for line in data_lines:
            f.write(f"        {line},\n")
        f.write("    };\n")

        f.write(f"    const uint16_t {font_name}_{size}_offsets[] PROGMEM = {{\n")
        for i in range(0, len(offsets), line_length):
            chunk = offsets[i:i+line_length]
            f.write("        " + ", ".join(map(str, chunk)) + ",\n")
        f.write("    };\n")

        f.write(f"    const uint8_t {font_name}_{size}_widths[] PROGMEM = {{\n")
        for i in range(0, len(widths), line_length):
            chunk = widths[i:i+line_length]
            f.write("        " + ", ".join(map(str, chunk)) + ",\n")
        f.write("    };\n")

        f.write(f"    const uint8_t {font_name}_{size}_heights[] PROGMEM = {{\n")
        for i in range(0, len(heights), line_length):
            chunk = heights[i:i+line_length]
            f.write("        " + ", ".join(map(str, chunk)) + ",\n")
        f.write("    };\n")

        f.write(f"    const int8_t {font_name}_{size}_yOffsets[] PROGMEM = {{\n")
        for i in range(0, len(yOffsets), line_length):
            chunk = yOffsets[i:i+line_length]
            f.write("        " + ", ".join(map(str, chunk)) + ",\n")
        f.write("    };\n")

        f.write(f"    const uint16_t {font_name}_{size}_unicode[] PROGMEM = {{\n")
        for i in range(0, len(unicode_vals), 8):
            chunk = unicode_vals[i:i+8]
            f.write("        " + ", ".join(f"0x{val:04X}" for val in chunk) + ",\n")
        f.write("        0xFFFF\n")
        f.write("    };\n")

        f.write(
            f"    const Font {font_name}_{size} = {{\n"
            f"        {max(widths)},\n"
            f"        {full_height},\n"
            f"        {font_name}_{size}_data,\n"
            f"        {font_name}_{size}_offsets,\n"
            f"        {font_name}_{size}_widths,\n"
            f"        {font_name}_{size}_heights,\n"
            f"        {font_name}_{size}_yOffsets,\n"
            f"        {font_name}_{size}_unicode\n"
            f"    }};\n"
        )
        f.write("}\n")

    print(f"Generated: {output_file}")

def main():
    if len(sys.argv) < 2:
        print("Drag & drop TTF here")
        input()
        return

    for font_path in sys.argv[1:]:
        for size in SIZES:
            generate_font(font_path, size)

    print("\nDONE")
    input()

if __name__ == "__main__":
    main()