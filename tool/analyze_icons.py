import os
from PIL import Image

def analyze():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    input_path = os.path.join(script_dir, 'icons.png')
    img = Image.open(input_path).convert("RGBA")
    width, height = img.size
    mid_x, mid_y = width // 2, height // 2

    regions = [
        (0, 0, mid_x, mid_y),          # 1_setting
        (mid_x, 0, width, mid_y),       # 2_wifi
        (0, mid_y, mid_x, height),      # 3_graph
        (mid_x, mid_y, width, height)    # 4_info
    ]

    names = ["1_setting", "2_wifi", "3_graph", "4_info"]

    for i, region in enumerate(regions):
        sub_img = img.crop(region)
        bbox = sub_img.getbbox()
        if bbox:
            left, top, right, bottom = bbox
            w = right - left
            h = bottom - top
            print(f"{names[i]}: bbox={bbox}, w={w}, h={h}")
        else:
            print(f"{names[i]}: No content found")

if __name__ == "__main__":
    analyze()
