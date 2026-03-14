#include "hal.h"
#include <SDL2/SDL.h>
#include "lvgl.h"
#include <stdio.h>
#include <stdlib.h>

static SDL_Window * window;
static SDL_Renderer * renderer;
static SDL_Texture * texture;
static uint32_t * tft_fb;

static void flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    for(int y = area->y1; y <= area->y2; y++) {
        for(int x = area->x1; x <= area->x2; x++) {
            tft_fb[y * disp_drv->hor_res + x] = lv_color_to32(*color_p);
            color_p++;
        }
    }

    SDL_UpdateTexture(texture, NULL, tft_fb, disp_drv->hor_res * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    lv_disp_flush_ready(disp_drv);
}

static void mouse_read_cb(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    int x, y;
    uint32_t buttons = SDL_GetMouseState(&x, &y);
    data->point.x = x;
    data->point.y = y;
    data->state = (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}

lv_disp_t * sdl_hal_init(int32_t w, int32_t h)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }
    window = SDL_CreateWindow("LVGL Simulator (v8)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
    if(window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, w, h);
    if(texture == NULL) {
        printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }
    tft_fb = malloc(w * h * sizeof(uint32_t));
    if(tft_fb == NULL) {
        printf("Memory allocation failed for tft_fb!\n");
        return NULL;
    }

    static lv_disp_draw_buf_t draw_buf;
    lv_color_t * buf = malloc(w * h * sizeof(lv_color_t));
    if(buf == NULL) {
        printf("Memory allocation failed for draw_buf!\n");
        return NULL;
    }
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, w * h);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &draw_buf;
    disp_drv.flush_cb = flush_cb;
    disp_drv.hor_res = w;
    disp_drv.ver_res = h;
    lv_disp_t * disp = lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = mouse_read_cb;
    lv_indev_t * mouse_indev = lv_indev_drv_register(&indev_drv);

    extern const lv_img_dsc_t mouse_cursor_icon;
    lv_obj_t * cursor_obj = lv_img_create(lv_scr_act());
    lv_img_set_src(cursor_obj, &mouse_cursor_icon);
    lv_indev_set_cursor(mouse_indev, cursor_obj);

    return disp;
}
