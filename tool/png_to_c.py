import os
import sys
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
    for i in range(0, len(data), 16):
        line = ", ".join([f"0x{b:02x}" for b in data[i:i+16]])
        s += "    " + line + ",\n"
    s += "};\n"
    return s

icons = [
    ("tool/1_setting.png", "img_settings_48"),
    ("tool/2_wifi.png", "img_wifi_48"),
    ("tool/3_graph.png", "img_monitor_48"),
    ("tool/4_info.png", "img_info_48"),
]

print('#include "lvgl.h"')
print('#include "ui_img.h"')
print()

for png_path, array_base_name in icons:
    d32, d16 = png_to_lvgl_c(png_path)
    
    print(f"#if LV_COLOR_DEPTH == 32")
    print(format_c_array(d32, array_base_name + "_data_32"))
    print(f"#elif LV_COLOR_DEPTH == 16")
    print(format_c_array(d16, array_base_name + "_data_16"))
    print(f"#endif")
    print()

for _, array_base_name in icons:
    print(f"const lv_img_dsc_t {array_base_name} = {{")
    print(f"  .header.always_zero = 0,")
    print(f"  .header.reserved = 0,")
    print(f"  .header.w = 48,")
    print(f"  .header.h = 48,")
    print(f"  .header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,")
    print(f"#if LV_COLOR_DEPTH == 32")
    print(f"  .data_size = 48 * 48 * 4,")
    print(f"  .data = {array_base_name}_data_32,")
    print(f"#elif LV_COLOR_DEPTH == 16")
    print(f"  .data_size = 48 * 48 * 3,")
    print(f"  .data = {array_base_name}_data_16,")
    print(f"#endif")
    print(f"}};")
    print()
