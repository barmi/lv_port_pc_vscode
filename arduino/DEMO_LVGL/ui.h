#ifndef UI_H
#define UI_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ARDUINO)
#include <lvgl.h>
#else
#include "lvgl/lvgl.h"
#endif

/* --- 전역 변수: 화면 객체들 --- */
extern lv_obj_t * scr_main;
extern lv_obj_t * scr_settings;
extern lv_obj_t * scr_wifi;
extern lv_obj_t * scr_chart;
extern lv_obj_t * scr_info;

/* --- UI 초기화 함수 --- */
void ui_init(void);

#ifdef __cplusplus
}
#endif

#endif /*UI_H*/
