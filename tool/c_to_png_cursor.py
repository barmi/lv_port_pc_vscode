import os
import re
from PIL import Image

def convert_c_to_png():
    # File paths
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.dirname(script_dir)
    c_file_path = os.path.join(project_root, 'src', 'mouse_cursor_icon.c')
    output_png_path = os.path.join(script_dir, 'mouse_cursor_icon.png')
    
    print(f"Reading from: {c_file_path}")
    
    if not os.path.exists(c_file_path):
        # Fallback if the above doesn't work for some reason (e.g. CWD)
        c_file_path = 'src/mouse_cursor_icon.c'
        if not os.path.exists(c_file_path):
            print(f"Error: C file not found.")
            return

    with open(c_file_path, 'r', encoding='utf-8') as f:
        content = f.read()

    # Extract data from mouse_cursor_icon_map array
    # Looking for: const uint8_t mouse_cursor_icon_map[] = { ... };
    match = re.search(r'mouse_cursor_icon_map\[\]\s*=\s*\{(.*?)\};', content, re.DOTALL)
    if not match:
        print("Error: Could not find mouse_cursor_icon_map in the C file.")
        return

    data_str = match.group(1)
    # Find all hex values like 0x19
    hex_values = re.findall(r'0x[0-9a-fA-F]+', data_str)
    
    if not hex_values:
        print("Error: No hex data found in the array.")
        return

    # Convert hex strings to integers
    data_bytes = [int(h, 16) for h in hex_values]
    
    width = 14
    height = 20
    expected_size = width * height * 4
    
    if len(data_bytes) < expected_size:
        print(f"Warning: Data size ({len(data_bytes)}) is less than expected ({expected_size}). Padding with 0.")
        data_bytes += [0] * (expected_size - len(data_bytes))
    elif len(data_bytes) > expected_size:
        print(f"Warning: Data size ({len(data_bytes)}) is more than expected ({expected_size}). Truncating extra {len(data_bytes) - expected_size} bytes.")
        data_bytes = data_bytes[:expected_size]

    # Create image from RGBA data
    # LV_IMG_CF_RGBA8888: R, G, B, A order
    img = Image.new("RGBA", (width, height))
    
    pixels = []
    for i in range(0, expected_size, 4):
        r = data_bytes[i]
        g = data_bytes[i+1]
        b = data_bytes[i+2]
        a = data_bytes[i+3]
        pixels.append((r, g, b, a))
    
    img.putdata(pixels)
    img.save(output_png_path)
    print(f"Successfully converted C array to {output_png_path} ({width}x{height} RGBA)")

if __name__ == "__main__":
    convert_c_to_png()
