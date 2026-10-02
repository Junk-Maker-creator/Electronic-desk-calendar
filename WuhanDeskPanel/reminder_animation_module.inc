// Shared, bounded vector drawing for the two daily reminder illustrations.
// Uses the existing LVGL draw context: no image buffers or transformed layers.

struct ReminderPainter {
    lv_draw_ctx_t *ctx;
    lv_coord_t x;
    lv_coord_t y;
};

static int32_t reminderEase(int32_t t)
{
    if (t <= 0) return 0;
    if (t >= 1000) return 1000;
    return (int32_t)((int64_t)t * t * (3000 - 2 * t) / 1000000LL);
}

static int32_t reminderPhase(int32_t progress, int32_t start, int32_t end)
{
    if (progress <= start) return 0;
    if (progress >= end) return 1000;
    return reminderEase((progress - start) * 1000 / (end - start));
}

static int32_t reminderMix(int32_t a, int32_t b, int32_t t)
{
    const int32_t delta = (b - a) * t;
    return a + (delta + (delta >= 0 ? 500 : -500)) / 1000;
}

static ReminderPainter reminderPainter(lv_event_t *event)
{
    lv_area_t area;
    lv_obj_get_coords(lv_event_get_target(event), &area);
    return {lv_event_get_draw_ctx(event), area.x1, area.y1};
}

static void reminderRect(const ReminderPainter &p, int x, int y, int w, int h,
                         int radius, uint32_t color, lv_opa_t opa = LV_OPA_COVER)
{
    if (w <= 0 || h <= 0 || opa == LV_OPA_TRANSP) return;
    lv_draw_rect_dsc_t dsc;
    lv_draw_rect_dsc_init(&dsc);
    dsc.bg_color = lv_color_hex(color);
    dsc.bg_opa = opa;
    dsc.radius = radius;
    lv_area_t area = {(lv_coord_t)(p.x + x), (lv_coord_t)(p.y + y),
                      (lv_coord_t)(p.x + x + w - 1), (lv_coord_t)(p.y + y + h - 1)};
    lv_draw_rect(p.ctx, &dsc, &area);
}

static void reminderCircle(const ReminderPainter &p, int x, int y, int radius,
                           uint32_t color, lv_opa_t opa = LV_OPA_COVER)
{
    reminderRect(p, x - radius, y - radius, radius * 2 + 1, radius * 2 + 1,
                 LV_RADIUS_CIRCLE, color, opa);
}

static void reminderLine(const ReminderPainter &p, int x1, int y1, int x2, int y2,
                         int width, uint32_t color, lv_opa_t opa = LV_OPA_COVER)
{
    if (opa == LV_OPA_TRANSP) return;
    lv_draw_line_dsc_t dsc;
    lv_draw_line_dsc_init(&dsc);
    dsc.color = lv_color_hex(color);
    dsc.width = width;
    dsc.opa = opa;
    dsc.round_start = 1;
    dsc.round_end = 1;
    lv_point_t a = {(lv_coord_t)(p.x + x1), (lv_coord_t)(p.y + y1)};
    lv_point_t b = {(lv_coord_t)(p.x + x2), (lv_coord_t)(p.y + y2)};
    lv_draw_line(p.ctx, &dsc, &a, &b);
}

static void reminderArc(const ReminderPainter &p, int x, int y, int radius,
                        int start, int end, int width, uint32_t color,
                        lv_opa_t opa = LV_OPA_COVER)
{
    if (opa == LV_OPA_TRANSP) return;
    lv_draw_arc_dsc_t dsc;
    lv_draw_arc_dsc_init(&dsc);
    dsc.color = lv_color_hex(color);
    dsc.width = width;
    dsc.opa = opa;
    dsc.rounded = 1;
    lv_point_t center = {(lv_coord_t)(p.x + x), (lv_coord_t)(p.y + y)};
    lv_draw_arc(p.ctx, &dsc, &center, radius, start, end);
}

static lv_obj_t *reminderCreateScene(lv_obj_t *parent, int x, int y, int w, int h,
                                     lv_event_cb_t drawCb)
{
    lv_obj_t *scene = lv_obj_create(parent);
    lv_obj_remove_style_all(scene);
    lv_obj_set_pos(scene, x, y);
    lv_obj_set_size(scene, w, h);
    lv_obj_clear_flag(scene, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(scene, drawCb, LV_EVENT_DRAW_MAIN, nullptr);
    return scene;
}

static void reminderStylePopup(lv_obj_t *popup, uint32_t background, uint32_t accent)
{
    lv_obj_set_size(popup, 540, 304);
    lv_obj_center(popup);
    lv_obj_set_style_pad_all(popup, 0, 0);
    lv_obj_set_style_bg_color(popup, lv_color_hex(background), 0);
    lv_obj_set_style_bg_opa(popup, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(popup, 1, 0);
    lv_obj_set_style_border_color(popup, lv_color_hex(accent), 0);
    lv_obj_set_style_border_opa(popup, LV_OPA_40, 0);
    lv_obj_set_style_radius(popup, 26, 0);
    lv_obj_set_style_shadow_width(popup, 24, 0);
    lv_obj_set_style_shadow_color(popup, lv_color_hex(0x020B14), 0);
    lv_obj_set_style_shadow_opa(popup, LV_OPA_40, 0);
    lv_obj_clear_flag(popup, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *accentBar = lv_obj_create(popup);
    lv_obj_remove_style_all(accentBar);
    lv_obj_set_pos(accentBar, 244, 27);
    lv_obj_set_size(accentBar, 32, 4);
    lv_obj_set_style_radius(accentBar, 2, 0);
    lv_obj_set_style_bg_color(accentBar, lv_color_hex(accent), 0);
    lv_obj_set_style_bg_opa(accentBar, LV_OPA_COVER, 0);
    lv_obj_clear_flag(accentBar, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
}

static lv_obj_t *reminderLabel(lv_obj_t *parent, const char *text, const lv_font_t *font,
                               uint32_t color, int x, int y, int width)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(label, width);
    lv_obj_set_pos(label, x, y);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    lv_obj_set_style_text_line_space(label, 7, 0);
    return label;
}

static lv_obj_t *reminderButton(lv_obj_t *parent, const char *text, const lv_font_t *font,
                                uint32_t color, lv_event_cb_t onClick)
{
    lv_obj_t *button = lv_btn_create(parent);
    lv_obj_set_pos(button, 244, 224);
    lv_obj_set_size(button, 264, 52);
    lv_obj_set_style_radius(button, 16, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(color), 0);
    lv_obj_set_style_shadow_width(button, 0, 0);
    lv_obj_set_style_text_color(button, lv_color_hex(0xFFFFFF), 0);
    lv_obj_t *label = lv_label_create(button);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_center(label);
    lv_obj_add_event_cb(button, onClick, LV_EVENT_CLICKED, nullptr);
    return button;
}
