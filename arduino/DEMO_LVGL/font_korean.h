#ifndef FONT_KOREAN_H
#define FONT_KOREAN_H

#if defined(ARDUINO)
    #include <lvgl.h>
#elif defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif

extern const lv_font_t lv_font_korean_844;

#endif // FONT_KOREAN_H
