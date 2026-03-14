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
            
            # 정사각형 영역의 4개의 꽉찬 영역을 찾으라고 했으므로
            # 가장 긴 쪽을 기준으로 정사각형 한 변의 길이를 정함
            size = max(w, h)
            
            # 중심점을 기준으로 정사각형 영역 확장
            # size만큼의 영역을 가지도록 좌/우/상/하로 확장
            cx = (left + right) / 2
            cy = (top + bottom) / 2
            
            # 정확한 정수 좌표로 변환 (중심 기준 정사각형)
            # sub_img.crop()은 영역을 벗어나도 자동으로 투명하게 처리함
            square_bbox = (
                int(cx - size / 2),
                int(cy - size / 2),
                int(cx + size / 2),
                int(cy + size / 2)
            )
            
            # 3. 정사각형으로 다시 자름
            final_crop = sub_img.crop(square_bbox)
            
            # 4. 최종적인 이미지 사이즈 48*48으로 축소
            final_img = final_crop.resize((48, 48), Image.Resampling.LANCZOS)
            
            # 5. 지정된 이름의 png 파일로 저장
            output_path = os.path.join(script_dir, filenames[i])
            final_img.save(output_path)
            print(f"Generated: {output_path} (original content: {w}x{h}, square crop: {size}x{size})")
        else:
            print(f"Skipping region {i} ({filenames[i]}): No visible content found.")

if __name__ == "__main__":
    crop_icons()
