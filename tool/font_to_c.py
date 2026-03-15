import os

def convert_font(h_fnt_path, e_fnt_path, output_path):
    with open(h_fnt_path, 'rb') as f:
        h_data = f.read()
    with open(e_fnt_path, 'rb') as f:
        e_data = f.read()

    with open(output_path, 'w') as f:
        f.write('#include "font_korean.h"\n')
        f.write('#include <stdint.h>\n')
        f.write('#include <stdbool.h>\n')
        f.write('#include <stddef.h>\n\n')
        
        f.write('static const uint8_t hangul_font_844[] = {\n')
        for i, b in enumerate(h_data):
            f.write(f"0x{b:02x}, ")
            if (i + 1) % 16 == 0:
                f.write("\n")
        f.write("};\n\n")

        f.write('static const uint8_t english_font_816[] = {\n')
        for i, b in enumerate(e_data):
            f.write(f"0x{b:02x}, ")
            if (i + 1) % 16 == 0:
                f.write("\n")
        f.write("};\n\n")

        f.write("""
static uint8_t glyph_buffer[32];

// Unicode indices to Font indices for Chosung
// Unicode order: ㄱ, ㄲ, ㄴ, ㄷ, ㄸ, ㄹ, ㅁ, ㅂ, ㅃ, ㅅ, ㅆ, ㅇ, ㅈ, ㅉ, ㅊ, ㅋ, ㅌ, ㅍ, ㅎ
// Font order:    filler, ㄱ, ㄲ, ㄴ, ㄷ, ㄸ, ㄹ, ㅁ, ㅂ, ㅃ, ㅅ, ㅆ, ㅇ, ㅈ, ㅉ, ㅊ, ㅋ, ㅌ, ㅍ, ㅎ
// Standard Johab font order (1-19)
static const uint8_t chosung_map[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

static bool get_glyph_dsc_cb(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t letter, uint32_t letter_next) {
    if (letter < 128) {
        if (letter < 32) return false;
        dsc_out->adv_w = 8;
        dsc_out->box_w = 8;
        dsc_out->box_h = 16;
        dsc_out->ofs_x = 0;
        dsc_out->ofs_y = 0;
        dsc_out->bpp = 1;
        dsc_out->resolved_font = font;
        return true;
    } else if (letter >= 0xAC00 && letter <= 0xD7AF) {
        dsc_out->adv_w = 16;
        dsc_out->box_w = 16;
        dsc_out->box_h = 16;
        dsc_out->ofs_x = 0;
        dsc_out->ofs_y = 0;
        dsc_out->bpp = 1;
        dsc_out->resolved_font = font;
        return true;
    }
    return false;
}

static const uint8_t * get_glyph_bitmap_cb(const lv_font_t * font, uint32_t letter) {
    if (letter < 128) {
        return &english_font_816[letter * 16];
    } else if (letter >= 0xAC00 && letter <= 0xD7AF) {
        uint32_t idx = letter - 0xAC00;
        uint32_t c_idx = idx / (21 * 28);
        uint32_t m_idx = (idx % (21 * 28)) / 28;
        uint32_t f_idx = idx % 28;

        int m_type;
        if (m_idx <= 7 || m_idx == 20) m_type = 0; // ㅏ ㅐ ㅑ ㅒ ㅓ ㅔ ㅕ ㅖ ㅣ (Vertical)
        else if (m_idx == 13 || m_idx == 17 || m_idx == 18) m_type = 1; // ㅜ ㅠ ㅡ (Below)
        else if (m_idx == 8 || m_idx == 12) m_type = 2; // ㅗ ㅛ (Top)
        else m_type = 3; // ㅘ ㅙ ㅚ ㅝ ㅞ ㅟ ㅢ (Combined)

        int c_set = (f_idx == 0) ? m_type : (m_type + 4);
        int m_set = (f_idx == 0) ? ((c_idx == 0 || c_idx == 15) ? 0 : 1) : ((c_idx == 0 || c_idx == 15) ? 2 : 3);
        
        int f_set;
        if (m_idx <= 7 || m_idx == 20) f_set = 0; // ㅏ ㅐ ㅑ ㅒ ㅓ ㅔ ㅕ ㅖ ㅣ
        else if (m_idx == 13 || m_idx == 17 || m_idx == 18) f_set = 1; // ㅜ ㅠ ㅡ
        else if (m_idx == 8 || m_idx == 12) f_set = 2; // ㅗ ㅛ
        else f_set = 3; // ㅘ ㅙ ㅚ ㅝ ㅞ ㅟ ㅢ

        for (int i = 0; i < 32; i++) glyph_buffer[i] = 0;

        // Chosung
        const uint8_t * c_glyph = &hangul_font_844[(c_set * 20 + chosung_map[c_idx]) * 32];
        for (int i = 0; i < 32; i++) glyph_buffer[i] |= c_glyph[i];

        // Jungsung
        const uint8_t * m_glyph = &hangul_font_844[(160 + m_set * 22 + (m_idx + 1)) * 32];
        for (int i = 0; i < 32; i++) glyph_buffer[i] |= m_glyph[i];

        // Jongsung
        if (f_idx > 0) {
            const uint8_t * f_glyph = &hangul_font_844[(160 + 88 + f_set * 28 + f_idx) * 32];
            for (int i = 0; i < 32; i++) glyph_buffer[i] |= f_glyph[i];
        }

        return glyph_buffer;
    }
    return NULL;
}

const lv_font_t lv_font_korean_844 = {
    .get_glyph_dsc = get_glyph_dsc_cb,
    .get_glyph_bitmap = get_glyph_bitmap_cb,
    .subpx = LV_FONT_SUBPX_NONE,
    .underline_position = -2,
    .underline_thickness = 1,
    .line_height = 16,
    .base_line = 0,
};
""")

if __name__ == "__main__":
    convert_font("tool/font/H04.FNT", "tool/font/E2.FNT", "arduino/DEMO_LVGL/font_korean.c")
