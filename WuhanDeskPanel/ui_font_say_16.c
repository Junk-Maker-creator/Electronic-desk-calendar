// Focused 16 px SimHei glyph for U+8BF4 (说).
// The existing UI font intentionally omits this character.
#include "lvgl.h"

static const uint8_t glyph_bitmap[] = {
    0x0, 0x10, 0xc6, 0x20, 0xc4, 0xc0, 0x9, 0x0,
    0x3f, 0x9c, 0x41, 0x8, 0x82, 0x11, 0xfc, 0x20,
    0xa0, 0x42, 0x40, 0xb4, 0x81, 0xc9, 0x22, 0x22,
    0x61, 0xc7, 0x80, 0x0, 0x0,
};

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 256, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 256, .box_w = 15, .box_h = 15, .ofs_x = 1, .ofs_y = -2},
};

static const lv_font_fmt_txt_cmap_t cmaps[] = {
    {.range_start = 0x8BF4, .range_length = 1, .glyph_id_start = 1,
     .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0,
     .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY},
};

#if LVGL_VERSION_MAJOR == 8
static lv_font_fmt_txt_glyph_cache_t cache;
#endif
static const lv_font_fmt_txt_dsc_t font_dsc = {
    .glyph_bitmap = glyph_bitmap, .glyph_dsc = glyph_dsc, .cmaps = cmaps,
    .kern_dsc = NULL, .kern_scale = 0, .cmap_num = 1, .bpp = 1,
    .kern_classes = 0, .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache,
#endif
};

const lv_font_t ui_font_say_16 = {
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,
    .line_height = 18, .base_line = 3, .subpx = LV_FONT_SUBPX_NONE,
    .underline_position = -1, .underline_thickness = 1,
    .dsc = &font_dsc, .fallback = NULL, .user_data = NULL,
};
