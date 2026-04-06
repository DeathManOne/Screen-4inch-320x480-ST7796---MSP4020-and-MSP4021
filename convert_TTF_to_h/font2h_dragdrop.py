import sys
import os
from PIL import Image, ImageFont, ImageDraw

# === CONFIG ===
FIRST_CHAR = 32
LAST_CHAR = 126
NAMESPACE = "ST7796S"
SIZES = [12, 16, 18]  # tailles générées

def get_chars_per_line(width):
    try:
        term_width = os.get_terminal_size().columns
    except OSError:
        term_width = 80
    return max(1, term_width // (width + 1))

def print_line_preview(chars_imgs, width, height):
    chars_per_line = get_chars_per_line(width)
    for i in range(0, len(chars_imgs), chars_per_line):
        block = chars_imgs[i:i+chars_per_line]
        for y in range(height):
            line = ""
            for img in block:
                # on parcourt y de 0 → hauteur, pas d'inversion
                line += "".join("#" if img.getpixel((x, y)) else "." for x in range(img.width)) + " "
            print(line)
        print("\n")  # séparation entre les blocs

def generate_font(font_path, size):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    parent_dir = os.path.dirname(script_dir)
    output_dir = os.path.join(parent_dir, "fonts")
    os.makedirs(output_dir, exist_ok=True)

    font_name = os.path.splitext(os.path.basename(font_path))[0]
    font_name = "".join(c if c.isalnum() else "_" for c in font_name)
    output_file = os.path.join(output_dir, f"{font_name}_{size}.h")
    font = ImageFont.truetype(font_path, size)

    # === calcul largeur max et hauteur ===
    char_widths = []
    max_height = 0
    for c in range(FIRST_CHAR, LAST_CHAR + 1):
        bbox = font.getbbox(chr(c))
        w, h = bbox[2] - bbox[0], bbox[3] - bbox[1]
        char_widths.append(w)
        max_height = max(max_height, h)

    height = max_height
    print(f"\n=== {font_name} ({size}px) === Max Box: {max(char_widths)}x{height}")

    data_lines = []
    offsets = []
    widths = []
    chars_imgs = []

    offset = 0
    for idx, c in enumerate(range(FIRST_CHAR, LAST_CHAR + 1)):
        char = chr(c)
        width = char_widths[idx]
        widths.append(width)
        offsets.append(offset)
        offset += width

        # === Création image caractère ===
        img = Image.new("1", (width, height), 0)
        draw = ImageDraw.Draw(img)
        bbox = font.getbbox(char)
        
        # === dessiner directement à la baseline ===
        y_offset = height - (bbox[3] - bbox[1])
        descender_chars = "gjpqy"
        if char not in descender_chars:
            y_offset -= 4
        draw.text((-bbox[0], y_offset - bbox[1]), char, font=font, fill=1)

        # PREVIEW
        chars_imgs.append(img)

        # === Conversion colonnes en uint32_t ===
        cols = []
        for x in range(width):
            col_val = 0
            for y in range(height):
                if img.getpixel((x, y)):  # PAS d'inversion y
                    col_val |= (1 << y)   # stocker normalement
            cols.append(f"0x{col_val:08X}")
        data_lines.append(", ".join(cols))

    # === preview console ===
    print_line_preview(chars_imgs, max(char_widths), height)

    # === écriture fichier .h ===
    with open(output_file, "w") as f:
        guard = f"_ST7796S_FONT_{font_name.upper()}_{size}_"
        f.write(f"#ifndef {guard}\n#define {guard}\n\n")
        f.write("#include <Arduino.h>\n#include \"font.h\"\n\n")
        f.write(f"namespace {NAMESPACE} {{\n\n")
        f.write("const uint32_t data[] PROGMEM = {\n")
        for line in data_lines:
            f.write(f"    {line},\n")
        f.write("};\n\n")
        f.write("const uint16_t offsets[] PROGMEM = {\n")
        f.write("    " + ", ".join(str(o) for o in offsets) + "\n};\n\n")
        f.write("const uint8_t widths[] PROGMEM = {\n")
        f.write("    " + ", ".join(str(w) for w in widths) + "\n};\n\n")
        f.write(f"const Font {font_name}_{size} = "
                f"{{{max(widths)}, {height}, {FIRST_CHAR}, {LAST_CHAR}, data, offsets, widths}};\n")
        f.write("\n}\n\n#endif\n")
    print(f"\n✔ Generated: {output_file}")

def main():
    if len(sys.argv) < 2:
        print("👉 Glisse un ou plusieurs fichiers TTF sur ce script")
        input("Press Enter...")
        return

    for font_path in sys.argv[1:]:
        for size in SIZES:
            generate_font(font_path, size)

    print("\n✅ DONE")
    input("Press Enter to exit...")

if __name__ == "__main__":
    main()