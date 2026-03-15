# Python Tool Guide

이 문서는 `tool` 디렉토리에 포함된 Python 스크립트들의 역할과 사용법에 대해 설명합니다. 이 툴들은 폰트 변환, 아이콘 처리, 마우스 커서 데이터 관리 등을 자동화하기 위해 작성되었습니다.

## 1. 개요

이 프로젝트에서는 LVGL 라이브러리를 사용하며, 이미지나 폰트 데이터를 C 언어 배열 형태로 변환하여 내장해야 합니다. 제공되는 Python 스크립트들은 이러한 변환 작업을 수행하며, 특히 조합형 한글 폰트와 프로젝트 전용 아이콘들을 관리하는 데 최적화되어 있습니다.

## 2. 툴 상세 설명

### 2.1 폰트 관련 툴

#### `font_to_c.py`
*   **역할**: 8x4x4 조합형 한글 글꼴(`H04.FNT`)과 영문 글꼴(`E2.FNT`)을 읽어 LVGL에서 사용할 수 있는 C 코드(`font_korean.c`)로 변환합니다.
*   **입력 파일**: 
    *   `tool/font/H04.FNT` (한글 무른모 글꼴)
    *   `tool/font/E2.FNT` (영문 무른모 글꼴)
*   **출력 파일**: 
    *   `arduino/DEMO_LVGL/font_korean.c` (LVGL용 폰트 정의 및 렌더링 로직 포함)
*   **주요 기능**: 폰트 데이터 내장, 유니코드 인덱스를 조합형 인덱스로 매핑하는 테이블 생성, LVGL의 `get_glyph_dsc` 및 `get_glyph_bitmap` 콜백 구현을 포함합니다.

---

### 2.2 아이콘 및 이미지 관련 툴

#### `analyze_icons.py`
*   **역할**: 여러 아이콘이 합쳐진 `icons.png` 파일을 분석하여 각 사분면에 위치한 아이콘의 실제 크기(Bounding Box)를 계산하여 출력합니다.
*   **입력 파일**: `tool/icons.png`
*   **출력**: 콘솔 메시지 (각 영역의 좌표 및 크기 정보)

#### `crop_4_to_1_icon.py`
*   **역할**: `icons.png` 파일로부터 4개의 아이콘(설정, 와이파이, 모니터, 정보)을 각각 추출하고 리사이징하여 개별 PNG 파일로 저장하며, 동시에 LVGL용 C 코드(`ui_img.c`)를 생성합니다.
*   **입력 파일**: `tool/icons.png`
*   **출력 파일**:
    *   `tool/1_setting.png`, `tool/2_wifi.png`, `tool/3_graph.png`, `tool/4_info.png` (48x48 크기로 리사이징 및 중앙 배치됨)
    *   `arduino/DEMO_LVGL/ui_img.c` (LVGL 이미지 디스크립터 정의)
*   **주요 기능**: 투명 영역을 제외한 실제 아이콘 내용물만 추출, 48x48 영역 내 비율 유지 리사이징, `ui_img.c` 자동 업데이트.

#### `png_to_c.py`
*   **역할**: `tool/` 디렉토리의 개별 아이콘 PNG 파일들을 읽어 LVGL용 C 코드로 변환하여 화면에 출력합니다.
*   **입력 파일**: `tool/1_setting.png` ~ `4_info.png`
*   **출력**: 표준 출력 (콘솔에 C 코드가 출력됨)
*   **참고**: `crop_4_to_1_icon.py`가 파일 생성을 직접 처리하므로, 이 툴은 코드 생성 확인이나 개별 변환 시에 주로 사용됩니다.

---

### 2.3 마우스 커서 관련 툴

#### `png_to_c_cursor.py`
*   **역할**: `tool/mouse_cursor_icon.png` 파일을 읽어 `src/mouse_cursor_icon.c` 파일을 업데이트합니다.
*   **입력 파일**: `tool/mouse_cursor_icon.png`
*   **출력 파일**: `src/mouse_cursor_icon.c` (14x20 RGBA 이미지 데이터)

#### `c_to_png_cursor.py`
*   **역할**: `src/mouse_cursor_icon.c` 파일 내의 헥사 데이터를 다시 PNG 파일로 변환하여 저장합니다. C 코드 내의 데이터를 시각적으로 확인하거나 수정하기 위해 사용됩니다.
*   **입력 파일**: `src/mouse_cursor_icon.c`
*   **출력 파일**: `tool/mouse_cursor_icon.png`

## 3. 실행 환경

이 툴들은 Python 3 환경에서 동작하며, 이미지 처리를 위해 `Pillow` 라이브러리가 필요합니다.

```bash
# Pillow 라이브러리 설치
pip install Pillow
```

## 4. 사용 예시

아이콘 이미지를 수정하고 프로젝트에 적용하는 일반적인 절차는 다음과 같습니다.

1.  `tool/icons.png` 파일을 그래픽 툴로 수정합니다.
2.  `tool/crop_4_to_1_icon.py`를 실행하여 개별 이미지 추출 및 `ui_img.c`를 업데이트합니다.
    ```bash
    python3 tool/crop_4_to_1_icon.py
    ```
3.  한글 폰트를 재생성해야 할 경우 `tool/font_to_c.py`를 실행합니다.
    ```bash
    python3 tool/font_to_c.py
    ```
