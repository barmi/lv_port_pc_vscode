import os
from PIL import Image

def png_to_lvgl_c(png_path):
    img = Image.open(png_path).convert("RGBA")
    width, height = img.size
    
    # 32-bit BGRA format
    data_32 = []
    for y in range(height):
        for x in range(width):
            r, g, b, a = img.getpixel((x, y))
            data_32.append(b)
            data_32.append(g)
            data_32.append(r)
            data_32.append(a)
            
    # 16-bit RGB565 + Alpha8 format (3 bytes per pixel)
    # With LV_COLOR_16_SWAP = 1
    data_16 = []
    for y in range(height):
        for x in range(width):
            r, g, b, a = img.getpixel((x, y))
            r5 = (r >> 3) & 0x1F
            g6 = (g >> 2) & 0x3F
            b5 = (b >> 3) & 0x1F
            
            val = (r5 << 11) | (g6 << 5) | b5
            low = val & 0xFF
            high = (val >> 8) & 0xFF
            
            # If swapped, high byte comes first
            data_16.append(high)
            data_16.append(low)
            data_16.append(a)

    return data_32, data_16

def format_c_array(data, name):
    s = f"static const uint8_t {name}[] = {{\n"
    for i in range(0, len(data), 12):
        line = ", ".join([f"0x{b:02x}" for b in data[i:i+12]])
        s += "    " + line + ",\n"
    s += "};\n"
    return s

def main():
    png_path = "tool/mouse_cursor_icon.png"
    if not os.path.exists(png_path):
        print(f"Error: {png_path} not found")
        return
    
    d32, d16 = png_to_lvgl_c(png_path)
    
    content = '#include "lvgl/lvgl.h"\n\n'
    
    content += "#if LV_COLOR_DEPTH == 32\n"
    content += format_c_array(d32, "mouse_cursor_icon_map_32")
    content += "#elif LV_COLOR_DEPTH == 16\n"
    content += format_c_array(d16, "mouse_cursor_icon_map_16")
    content += "#endif\n\n"
    
    content += "lv_img_dsc_t mouse_cursor_icon = {\n"
    content += "  .header.always_zero = 0,\n"
    content += "  .header.reserved = 0,\n"
    content += "  .header.w = 14,\n"
    content += "  .header.h = 20,\n"
    content += "  .header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,\n"
    content += "#if LV_COLOR_DEPTH == 32\n"
    content += "  .data_size = 14 * 20 * 4,\n"
    content += "  .data = mouse_cursor_icon_map_32,\n"
    content += "#elif LV_COLOR_DEPTH == 16\n"
    content += "  .data_size = 14 * 20 * 3,\n"
    content += "  .data = mouse_cursor_icon_map_16,\n"
    content += "#endif\n"
    content += "};\n"
    
    target_path = "src/mouse_cursor_icon.c"
    with open(target_path, "w") as f:
        f.write(content)
    print(f"Updated {target_path}")

if __name__ == "__main__":
    main()
