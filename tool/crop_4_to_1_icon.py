import os
from PIL import Image

def get_tight_bbox(img, threshold=3):
    # 알파 채널만 추출 (RGBA 모드 가정)
    if img.mode != 'RGBA':
        img = img.convert('RGBA')
    alpha = img.getchannel('A')
    
    # threshold보다 큰 픽셀만 255(불투명)로, 나머지는 0(투명)으로 변환
    # point()는 8비트 채널에 대해 룩업 테이블(LUT)을 적용함
    mask = alpha.point(lambda p: 255 if p > threshold else 0)
    return mask.getbbox()

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

def generate_ui_img_c(script_dir, filenames):
    # LVGL Image Descriptor names mapping
    mapping = {
        "1_setting.png": "img_settings_48",
        "2_wifi.png": "img_wifi_48",
        "3_graph.png": "img_monitor_48",
        "4_info.png": "img_info_48"
    }
    
    # Target path: arduino/DEMO_LVGL/ui_img.c
    # script_dir is project_root/tool
    project_root = os.path.dirname(script_dir)
    target_path = os.path.join(project_root, "arduino", "DEMO_LVGL", "ui_img.c")
    
    content = '#include "lvgl.h"\n'
    content += '#include "ui_img.h"\n\n'
    
    # 1. Generate Data Arrays
    for filename in filenames:
        png_path = os.path.join(script_dir, filename)
        if not os.path.exists(png_path):
            continue
            
        array_base_name = mapping.get(filename)
        d32, d16 = png_to_lvgl_c(png_path)
        
        content += f"#if LV_COLOR_DEPTH == 32\n"
        content += format_c_array(d32, array_base_name + "_data_32")
        content += f"#elif LV_COLOR_DEPTH == 16\n"
        content += format_c_array(d16, array_base_name + "_data_16")
        content += f"#endif\n\n"
        
    # 2. Generate Image Descriptors
    for filename in filenames:
        array_base_name = mapping.get(filename)
        content += f"const lv_img_dsc_t {array_base_name} = {{\n"
        content += f"  .header.always_zero = 0,\n"
        content += f"  .header.reserved = 0,\n"
        content += f"  .header.w = 48,\n"
        content += f"  .header.h = 48,\n"
        content += f"  .header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,\n"
        content += f"#if LV_COLOR_DEPTH == 32\n"
        content += f"  .data_size = 48 * 48 * 4,\n"
        content += f"  .data = {array_base_name}_data_32,\n"
        content += f"#elif LV_COLOR_DEPTH == 16\n"
        content += f"  .data_size = 48 * 48 * 3,\n"
        content += f"  .data = {array_base_name}_data_16,\n"
        content += f"#endif\n"
        content += f"}};\n\n"
        
    with open(target_path, "w") as f:
        f.write(content)
    print(f"Generated: {target_path}")

def crop_icons():
    # 경로 설정: 스크립트가 위치한 디렉토리의 파일을 대상으로 함
    script_dir = os.path.dirname(os.path.abspath(__file__))
    
    # 4_icons.png와 icons.png 둘 다 확인
    input_filename = '4_icons.png'
    input_path = os.path.join(script_dir, input_filename)
    
    if not os.path.exists(input_path):
        input_filename = 'icons.png'
        input_path = os.path.join(script_dir, input_filename)
        
    if not os.path.exists(input_path):
        # 현재 디렉토리도 확인 (CWD가 tool일 경우 대응)
        for name in ['4_icons.png', 'icons.png']:
            if os.path.exists(name):
                input_path = name
                script_dir = '.'
                break
        else:
            print(f"Error: No source image (4_icons.png or icons.png) found.")
            return

    print(f"Using source: {input_path}")
    # 이미지 열기
    img = Image.open(input_path).convert("RGBA")
    width, height = img.size
    
    # 2열 2행 (2x2) 그리드로 분할
    mid_x, mid_y = width // 2, height // 2
    
    # 각 영역 정의: (left, top, right, bottom)
    regions = [
        (0, 0, mid_x, mid_y),          # 1_setting
        (mid_x, 0, width, mid_y),       # 2_wifi
        (0, mid_y, mid_x, height),      # 3_graph
        (mid_x, mid_y, width, height)    # 4_info
    ]
    
    filenames = [
        "1_setting.png",
        "2_wifi.png",
        "3_graph.png",
        "4_info.png"
    ]
    
    for i, region in enumerate(regions):
        # 1. 원본에서 해당 사분면 영역만 자름
        sub_img = img.crop(region)
        
        # 2. 사분면 내에서 불투명 영역(Bounding Box) 찾기 (Alpha > threshold인 픽셀 영역)
        bbox = get_tight_bbox(sub_img, threshold=3)
        if bbox:
            left, top, right, bottom = bbox
            w = right - left
            h = bottom - top
            
            # 아이콘 내용물만 크롭 (원본에서 threshold 이상의 유효 영역)
            icon_content = sub_img.crop(bbox)
            
            # 비율 유지하며 48x48 영역 내에 맞도록 리사이즈
            # 가로/세로 중 긴 쪽이 48이 되도록 스케일링 비율 결정
            scale = 48.0 / max(w, h)
            new_w = int(w * scale)
            new_h = int(h * scale)
            
            # 적어도 한 변은 48이 되도록 보장
            if w > h:
                new_w = 48
            else:
                new_h = 48
                
            resized_icon = icon_content.resize((new_w, new_h), Image.Resampling.LANCZOS)
            
            # 3. 48*48 투명 배경 캔버스 생성 및 중앙 배치
            final_img = Image.new("RGBA", (48, 48), (0, 0, 0, 0))
            
            # 중앙 위치 계산
            offset_x = (48 - new_w) // 2
            offset_y = (48 - new_h) // 2
            
            # 아이콘을 중앙에 붙여넣기 (알파 채널 유지)
            # RGBA 이미지를 붙일 때 마스크로 자신을 전달하여 알파를 보존함
            final_img.paste(resized_icon, (offset_x, offset_y), resized_icon)
            
            # 4. 지정된 이름의 png 파일로 저장
            output_path = os.path.join(script_dir, filenames[i])
            final_img.save(output_path)
            print(f"Generated: {output_path} (original content: {w}x{h}, resized to: {new_w}x{new_h})")
        else:
            print(f"Skipping region {i} ({filenames[i]}): No visible content (threshold=3) found.")
            
    # ui_img.c 자동 생성
    generate_ui_img_c(script_dir, filenames)

if __name__ == "__main__":
    crop_icons()
