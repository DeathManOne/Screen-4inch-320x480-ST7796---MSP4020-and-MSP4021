import sys, os
from PIL import Image, ImageFont, ImageDraw

FIRST_CHAR = 32
LAST_CHAR = 126
NAMESPACE = "ST7796S"
SIZES = [12, 16, 18]

def get_chars_per_line(width):
    try:
        term_width = os.get_terminal_size().columns
    except OSError:
        term_width = 80
    return max(1, term_width // (width + 1))

def print_line_preview(chars_imgs, height):
    chars_per_line = get_chars_per_line(max(img.width for img in chars_imgs))
    for i in range(0, len(chars_imgs), chars_per_line):
        block = chars_imgs[i:i + chars_per_line]
        for y in range(height):
            line = ""
            for img in block:
                line += "".join("#" if img.getpixel((x, y)) else "." for x in range(img.width)) + " "
            print(line)
        print()

def trim_image(img):
    bbox = img.getbbox()
    if bbox:
        left, _, right, _ = bbox
        return img.crop((left, 0, right, img.height))
    return img

def generate_font(font_path, size):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    parent_dir = os.path.dirname(script_dir)
    output_dir = os.path.join(parent_dir, "src", "fonts")
    os.makedirs(output_dir, exist_ok=True)

    font_name = "".join(c if c.isalnum() else "_" for c in os.path.splitext(os.path.basename(font_path))[0])
    output_file = os.path.join(output_dir, f"{font_name}_{size}.h")

    font = ImageFont.truetype(font_path, size)
    ascent, descent = font.getmetrics()
    height = ascent + descent  # inclut descendantes

    print(f"\n=== {font_name} ({size}px) === Height: {height}")

    data_lines, offsets, widths, chars_imgs = [], [], [], []
    offset = 0

    for c in range(FIRST_CHAR, LAST_CHAR + 1):
        char = chr(c)
        img = Image.new("1", (size*2, height), 0)  # image haute pour les descendantes
        draw = ImageDraw.Draw(img)
        draw.text((0,0), char, font=font, fill=1)   # dessiner à (0,0)
        img = trim_image(img)
        width = img.width

        widths.append(width)
        offsets.append(offset)
        offset += width
        chars_imgs.append(img)

        # conversion colonne → uint32
        cols = []
        for x in range(width):
            col_val = 0
            for y_pix in range(height):
                if y_pix < img.height and img.getpixel((x, y_pix)):
                    col_val |= (1 << y_pix)
            cols.append(f"0x{col_val:08X}")
        data_lines.append(", ".join(cols))

    print_line_preview(chars_imgs, height)

    with open(output_file, "w") as f:
        guard = f"_ST7796S_FONT_{font_name.upper()}_{size}_"
        f.write(f"#ifndef {guard}\n#define {guard}\n\n")
        f.write("#include <Arduino.h>\n#include \"font.h\"\n\n")
        f.write(f"namespace {NAMESPACE} {{\n\n")
        f.write("const uint32_t data[] PROGMEM = {\n")
        for line in data_lines: f.write(f"    {line},\n")
        f.write("};\n\n")
        f.write("const uint16_t offsets[] PROGMEM = {\n")
        f.write("    " + ", ".join(str(o) for o in offsets) + "\n};\n\n")
        f.write("const uint8_t widths[] PROGMEM = {\n")
        f.write("    " + ", ".join(str(w) for w in widths) + "\n};\n\n")
        f.write(f"const Font {font_name}_{size} = {{ {max(widths)}, {height}, {FIRST_CHAR}, {LAST_CHAR}, data, offsets, widths }};\n")
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
