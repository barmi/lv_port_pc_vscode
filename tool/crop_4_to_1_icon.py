import os
from PIL import Image

def crop_icons():
    # 경로 설정: 스크립트가 위치한 디렉토리의 파일을 대상으로 함
    script_dir = os.path.dirname(os.path.abspath(__file__))
    input_path = os.path.join(script_dir, 'icons.png')
    
    if not os.path.exists(input_path):
        # 현재 디렉토리도 확인 (CWD가 tool일 경우 대응)
        if os.path.exists('icons.png'):
            input_path = 'icons.png'
            script_dir = '.'
        else:
            print(f"Error: {input_path} not found.")
            return

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
        
        # 2. 사분면 내에서 불투명 영역(Bounding Box) 찾기 (Alpha > 0인 픽셀 영역)
        bbox = sub_img.getbbox()
        if bbox:
            left, top, right, bottom = bbox
            w = right - left
            h = bottom - top
            
            # 아이콘 내용물만 크롭
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
            print(f"Generated: {output_path} (original content: {w}x{h}, resized to: {new_w}x{h} -> {new_w}x{new_h})")
        else:
            print(f"Skipping region {i} ({filenames[i]}): No visible content found.")

if __name__ == "__main__":
    crop_icons()
