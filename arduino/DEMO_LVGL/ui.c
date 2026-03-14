#include "ui.h"
#include "ui_img.h"
#include <stdio.h>

/* --- 전역 변수: 화면 객체들 --- */
lv_obj_t * scr_main = NULL;
lv_obj_t * scr_settings = NULL;
lv_obj_t * scr_wifi = NULL;
lv_obj_t * scr_chart = NULL;
lv_obj_t * scr_info = NULL;

/* --- 스타일 변수 --- */
static lv_style_t style_btn_main;
static lv_style_t style_header;
static lv_style_t style_title;

/* --- 내부 함수 선언 --- */
static void ui_init_styles();
static void create_header(lv_obj_t * parent, const char * title_text);
static void build_main_screen();
static void build_settings_screen();
static void build_wifi_screen();
static void build_chart_screen();
static void build_info_screen();

/* [수정됨] 화면 전환 이벤트 콜백 */
static void nav_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * target = (lv_obj_t *)lv_event_get_user_data(e);

    if(code == LV_EVENT_CLICKED && target != NULL) {
        // 중요: 마지막 인자를 false로 설정하여 이전 화면을 삭제하지 않음
        lv_scr_load_anim(target, LV_SCR_LOAD_ANIM_MOVE_LEFT, 250, 0, false);
    }
}

/* [수정됨] 홈 버튼 이벤트 콜백 */
static void home_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        if(scr_main != NULL) {
             // 중요: 마지막 인자를 false로 설정하여 현재 화면을 삭제하지 않음
            lv_scr_load_anim(scr_main, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 250, 0, false);
        }
    }
}

/* ==========================================
 * UI 빌더 함수들
 * ========================================== */

static void create_header(lv_obj_t * parent, const char * title_text) {
    lv_obj_t * header = lv_obj_create(parent);
    lv_obj_set_size(header, LV_PCT(100), 50);
    lv_obj_add_style(header, &style_header, 0);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * btn_home = lv_btn_create(header);
    lv_obj_set_size(btn_home, 40, 40);
    lv_obj_align(btn_home, LV_ALIGN_LEFT_MID, -5, 0);
    lv_obj_set_style_bg_color(btn_home, lv_palette_main(LV_PALETTE_GREY), 0);
    
    lv_obj_t * lbl_home = lv_label_create(btn_home);
    lv_label_set_text(lbl_home, LV_SYMBOL_HOME);
    lv_obj_center(lbl_home);
    
    lv_obj_add_event_cb(btn_home, home_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * label = lv_label_create(header);
    lv_label_set_text(label, title_text);
    lv_obj_add_style(label, &style_title, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

static void build_main_screen() {
    scr_main = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_main, lv_color_hex(0xEEEEEE), 0);

    lv_obj_t * welcome = lv_label_create(scr_main);
    lv_label_set_text(welcome, "Smart Dashboard");
    lv_obj_set_style_text_font(welcome, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(welcome, lv_color_hex(0x333333), 0);
    lv_obj_align(welcome, LV_ALIGN_TOP_MID, 0, 20);

    int btn_w = 140;
    int btn_h = 100; // [수정됨] 화면 높이를 넘지 않도록 조정
    int gap_x = 30;
    int gap_y = 20;
    int start_y = 60;

    // Btn 1: Settings
    lv_obj_t * btn1 = lv_btn_create(scr_main);
    lv_obj_add_style(btn1, &style_btn_main, 0);
    lv_obj_set_size(btn1, btn_w, btn_h);
    lv_obj_align(btn1, LV_ALIGN_TOP_MID, -(btn_w/2 + gap_x/2), start_y);
    lv_obj_set_style_bg_color(btn1, lv_palette_main(LV_PALETTE_BLUE), 0);
    
    lv_obj_t * img1 = lv_img_create(btn1);
    lv_img_set_src(img1, &img_settings_48);
    lv_obj_align(img1, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t * lbl1 = lv_label_create(btn1);
    lv_label_set_text(lbl1, "Settings");
    lv_obj_align(lbl1, LV_ALIGN_BOTTOM_MID, 0, -10);
    
    lv_obj_add_event_cb(btn1, nav_event_cb, LV_EVENT_CLICKED, scr_settings);

    // Btn 2: Wi-Fi
    lv_obj_t * btn2 = lv_btn_create(scr_main);
    lv_obj_add_style(btn2, &style_btn_main, 0);
    lv_obj_set_size(btn2, btn_w, btn_h);
    lv_obj_align(btn2, LV_ALIGN_TOP_MID, (btn_w/2 + gap_x/2), start_y);
    lv_obj_set_style_bg_color(btn2, lv_palette_main(LV_PALETTE_TEAL), 0);

    lv_obj_t * img2 = lv_img_create(btn2);
    lv_img_set_src(img2, &img_wifi_48);
    lv_obj_align(img2, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t * lbl2 = lv_label_create(btn2);
    lv_label_set_text(lbl2, "Wi-Fi");
    lv_obj_align(lbl2, LV_ALIGN_BOTTOM_MID, 0, -10);
    
    lv_obj_add_event_cb(btn2, nav_event_cb, LV_EVENT_CLICKED, scr_wifi);

    // Btn 3: Chart
    lv_obj_t * btn3 = lv_btn_create(scr_main);
    lv_obj_add_style(btn3, &style_btn_main, 0);
    lv_obj_set_size(btn3, btn_w, btn_h);
    lv_obj_align(btn3, LV_ALIGN_TOP_MID, -(btn_w/2 + gap_x/2), start_y + btn_h + gap_y);
    lv_obj_set_style_bg_color(btn3, lv_palette_main(LV_PALETTE_ORANGE), 0);

    lv_obj_t * img3 = lv_img_create(btn3);
    lv_img_set_src(img3, &img_monitor_48);
    lv_obj_align(img3, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t * lbl3 = lv_label_create(btn3);
    lv_label_set_text(lbl3, "Monitor");
    lv_obj_align(lbl3, LV_ALIGN_BOTTOM_MID, 0, -10);

    lv_obj_add_event_cb(btn3, nav_event_cb, LV_EVENT_CLICKED, scr_chart);

    // Btn 4: Info
    lv_obj_t * btn4 = lv_btn_create(scr_main);
    lv_obj_add_style(btn4, &style_btn_main, 0);
    lv_obj_set_size(btn4, btn_w, btn_h);
    lv_obj_align(btn4, LV_ALIGN_TOP_MID, (btn_w/2 + gap_x/2), start_y + btn_h + gap_y);
    lv_obj_set_style_bg_color(btn4, lv_palette_main(LV_PALETTE_INDIGO), 0);

    lv_obj_t * img4 = lv_img_create(btn4);
    lv_img_set_src(img4, &img_info_48);
    lv_obj_align(img4, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t * lbl4 = lv_label_create(btn4);
    lv_label_set_text(lbl4, "Info");
    lv_obj_align(lbl4, LV_ALIGN_BOTTOM_MID, 0, -10);

    lv_obj_add_event_cb(btn4, nav_event_cb, LV_EVENT_CLICKED, scr_info);
}

static void build_settings_screen() {
    scr_settings = lv_obj_create(NULL);
    create_header(scr_settings, "System Settings");

    lv_obj_t * cont = lv_obj_create(scr_settings);
    lv_obj_set_size(cont, LV_PCT(90), LV_PCT(70));
    lv_obj_align(cont, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);

    lv_obj_t * sw1_cont = lv_obj_create(cont);
    lv_obj_set_size(sw1_cont, LV_PCT(100), 50);
    lv_obj_set_style_border_width(sw1_cont, 0, 0);
    
    lv_obj_t * lbl1 = lv_label_create(sw1_cont);
    lv_label_set_text(lbl1, "Bluetooth");
    lv_obj_align(lbl1, LV_ALIGN_LEFT_MID, 0, 0);
    
    lv_obj_t * sw1 = lv_switch_create(sw1_cont);
    lv_obj_add_state(sw1, LV_STATE_CHECKED);
    lv_obj_align(sw1, LV_ALIGN_RIGHT_MID, 0, 0);

    lv_obj_t * slider_cont = lv_obj_create(cont);
    lv_obj_set_size(slider_cont, LV_PCT(100), 60);
    lv_obj_set_style_border_width(slider_cont, 0, 0);

    lv_obj_t * lbl2 = lv_label_create(slider_cont);
    lv_label_set_text(lbl2, "Brightness");
    lv_obj_align(lbl2, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t * slider = lv_slider_create(slider_cont);
    lv_obj_set_width(slider, LV_PCT(90));
    lv_obj_align(slider, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_slider_set_value(slider, 70, LV_ANIM_OFF);
}

static void build_wifi_screen() {
    scr_wifi = lv_obj_create(NULL);
    create_header(scr_wifi, "Wi-Fi Networks");

    lv_obj_t * list = lv_list_create(scr_wifi);
    lv_obj_set_size(list, LV_PCT(90), LV_PCT(70));
    lv_obj_align(list, LV_ALIGN_BOTTOM_MID, 0, -20);

    lv_obj_t * btn;
    lv_list_add_text(list, "Available Networks");
    btn = lv_list_add_btn(list, LV_SYMBOL_WIFI, "Home_5G_GIGA");
    lv_obj_add_state(btn, LV_STATE_CHECKED);
    btn = lv_list_add_btn(list, LV_SYMBOL_WIFI, "Office_Guest");
    btn = lv_list_add_btn(list, LV_SYMBOL_CLOSE, "Disconnect");
}

static void build_chart_screen() {
    scr_chart = lv_obj_create(NULL);
    create_header(scr_chart, "Real-time Monitor");

    lv_obj_t * chart = lv_chart_create(scr_chart);
    lv_obj_set_size(chart, LV_PCT(80), LV_PCT(60));
    lv_obj_align(chart, LV_ALIGN_CENTER, 0, 20);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);

    lv_chart_series_t * ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t * ser2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);

    for(int i = 0; i < 10; i++) {
        lv_chart_set_next_value(chart, ser1, lv_rand(10, 90));
        lv_chart_set_next_value(chart, ser2, lv_rand(10, 90));
    }
    lv_chart_refresh(chart);
}

static void build_info_screen() {
    scr_info = lv_obj_create(NULL);
    create_header(scr_info, "Device Information");

    lv_obj_t * panel = lv_obj_create(scr_info);
    lv_obj_set_size(panel, LV_PCT(80), LV_PCT(60));
    lv_obj_align(panel, LV_ALIGN_CENTER, 0, 20);

    lv_obj_t * lbl_info = lv_label_create(panel);
    lv_label_set_text(lbl_info, 
        "Model: ESP32-S3 HMI\n"
        "Firmware: v1.2.0\n"
        "Resolution: 480x320\n"
        "LVGL Ver: 8.3.x"
    );
    lv_obj_center(lbl_info);
}

static void ui_init_styles() {
    lv_style_init(&style_btn_main);
    lv_style_set_radius(&style_btn_main, 15);
    lv_style_set_bg_opa(&style_btn_main, LV_OPA_COVER);
    lv_style_set_shadow_width(&style_btn_main, 20);
    lv_style_set_shadow_color(&style_btn_main, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_shadow_ofs_y(&style_btn_main, 5);
    lv_style_set_translate_y(&style_btn_main, 3);

    lv_style_init(&style_header);
    lv_style_set_bg_color(&style_header, lv_color_hex(0xFFFFFF));
    lv_style_set_border_width(&style_header, 0);
    lv_style_set_shadow_width(&style_header, 10);
    lv_style_set_shadow_ofs_y(&style_header, 2);
    lv_style_set_shadow_opa(&style_header, LV_OPA_30);

    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, &lv_font_montserrat_20);
    lv_style_set_text_color(&style_title, lv_color_black());
}

void ui_init(void) {
    ui_init_styles();

    build_settings_screen();
    build_wifi_screen();
    build_chart_screen();
    build_info_screen();
    build_main_screen();

    lv_scr_load(scr_main);
}
