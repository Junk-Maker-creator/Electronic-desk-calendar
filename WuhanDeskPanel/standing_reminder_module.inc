// Daily sedentary reminder and a small standing stretch animation.
// Keep the animation bounded to local LVGL objects so it does not disturb the RGB display.

static lv_obj_t *standingPopup = nullptr;
static lv_obj_t *standingHead = nullptr;
static lv_obj_t *standingBody = nullptr;
static lv_obj_t *standingEye = nullptr;
static lv_obj_t *standingLeg = nullptr;
static lv_obj_t *standingLeg2 = nullptr;
static lv_obj_t *standingArm = nullptr;
static lv_obj_t *standingArm2 = nullptr;
static lv_timer_t *standingAutoCloseTimer = nullptr;
static lv_anim_t standingAnimation;

static int32_t standingSmoothStep(int32_t progress)
{
    if (progress <= 0) return 0;
    if (progress >= 1000) return 1000;
    return (int32_t)((int64_t)progress * progress * (3000 - 2 * progress) / 1000000LL);
}

static int32_t standingMove(int32_t progress, int32_t start, int32_t end)
{
    if (progress <= start) return 0;
    if (progress >= end) return 1000;
    return standingSmoothStep((progress - start) * 1000 / (end - start));
}

static int32_t standingReach(int32_t progress)
{
    if (progress < 100) return 0;
    if (progress < 520) return standingMove(progress, 100, 520);
    if (progress < 700) return 1000;
    if (progress < 960) return 1000 - standingMove(progress, 700, 960);
    return 0;
}

static void standingAnimationCb(void *, int32_t progress)
{
    const int32_t reach = standingReach(progress);
    const int32_t lift = reach * 6 / 1000;
    const int32_t sway = reach * 2 / 1000;

    // A single, readable gesture: the person is already standing, raises both
    // arms overhead, holds the stretch, then relaxes them back down.
    const int32_t headX = 78 + sway;
    const int32_t headY = 77 - lift;
    if (standingHead) lv_obj_set_pos(standingHead, headX, headY);
    if (standingEye) lv_obj_set_pos(standingEye, headX + 10, headY + 16);
    if (standingBody) {
        lv_obj_set_size(standingBody, 56, 58 + lift);
        lv_obj_set_pos(standingBody, 71 + sway, 126 - lift);
    }
    if (standingLeg) {
        lv_obj_set_pos(standingLeg, 80 + sway, 178 - lift);
        lv_obj_set_style_transform_angle(standingLeg, 0, 0);
    }
    if (standingLeg2) {
        lv_obj_set_pos(standingLeg2, 105 + sway, 178 - lift);
        lv_obj_set_style_transform_angle(standingLeg2, 0, 0);
    }
    if (standingArm) {
        lv_obj_set_pos(standingArm, 119 + sway, 132 - lift);
        lv_obj_set_style_transform_angle(standingArm, -1550 * reach / 1000, 0);
    }
    if (standingArm2) {
        lv_obj_set_pos(standingArm2, 64 + sway, 132 - lift);
        lv_obj_set_style_transform_angle(standingArm2, 1550 * reach / 1000, 0);
    }
}

static void closeStandingPopup(lv_event_t *)
{
    if (standingPopup) lv_obj_add_flag(standingPopup, LV_OBJ_FLAG_HIDDEN);
    standingReminderActive = false;
    lv_anim_del(nullptr, standingAnimationCb);
    if (standingAutoCloseTimer) {
        lv_timer_del(standingAutoCloseTimer);
        standingAutoCloseTimer = nullptr;
    }
}

static void standingAutoCloseCb(lv_timer_t *timer)
{
    if (standingPopup) lv_obj_add_flag(standingPopup, LV_OBJ_FLAG_HIDDEN);
    standingReminderActive = false;
    lv_anim_del(nullptr, standingAnimationCb);
    standingAutoCloseTimer = nullptr;
    lv_timer_del(timer);
}

static void showStandingPopup()
{
    if (!standingPopup) return;
    standingReminderActive = true;
    // Standing takes priority if a hydration popup is already on screen.
    closeHydrationPopup(nullptr);
    lv_obj_clear_flag(standingPopup, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(standingPopup);
    lv_anim_init(&standingAnimation);
    lv_anim_set_var(&standingAnimation, standingPopup);
    lv_anim_set_values(&standingAnimation, 0, 1000);
    lv_anim_set_time(&standingAnimation, 2800);
    lv_anim_set_playback_time(&standingAnimation, 2800);
    lv_anim_set_repeat_count(&standingAnimation, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&standingAnimation, standingAnimationCb);
    lv_anim_start(&standingAnimation);
    if (standingAutoCloseTimer) lv_timer_del(standingAutoCloseTimer);
    standingAutoCloseTimer = lv_timer_create(standingAutoCloseCb, 20000, nullptr);
}

static void createStandingReminderUi()
{
    standingPopup = lv_obj_create(lv_layer_top());
    lv_obj_set_size(standingPopup, 500, 275);
    lv_obj_center(standingPopup);
    lv_obj_set_style_bg_color(standingPopup, lv_color_hex(0x102F2B), 0);
    lv_obj_set_style_border_width(standingPopup, 2, 0);
    lv_obj_set_style_border_color(standingPopup, lv_color_hex(0x34D399), 0);
    lv_obj_set_style_radius(standingPopup, 24, 0);
    lv_obj_set_style_shadow_width(standingPopup, 24, 0);
    lv_obj_set_style_shadow_color(standingPopup, lv_color_hex(0x10B981), 0);
    lv_obj_clear_flag(standingPopup, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title = lv_label_create(standingPopup);
    lv_label_set_text(title, "站起来伸个懒腰");
    lv_obj_set_style_text_font(title, &ui_font_standing_16, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xD1FAE5), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 55, 18);

    lv_obj_t *hint = lv_label_create(standingPopup);
    lv_label_set_text(hint, "伸个懒腰，放松一下身体!");
    lv_obj_set_style_text_font(hint, &ui_font_standing_16, 0);
    lv_obj_set_style_text_color(hint, lv_color_hex(0x6EE7B7), 0);
    lv_obj_align(hint, LV_ALIGN_TOP_MID, 55, 65);

    standingHead = lv_obj_create(standingPopup);
    lv_obj_set_size(standingHead, 42, 42); lv_obj_set_pos(standingHead, 78, 77);
    lv_obj_set_style_radius(standingHead, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(standingHead, lv_color_hex(0xFCD5B5), 0);
    lv_obj_set_style_border_width(standingHead, 0, 0);
    standingBody = lv_obj_create(standingPopup);
    lv_obj_set_size(standingBody, 56, 58); lv_obj_set_pos(standingBody, 71, 126);
    lv_obj_set_style_radius(standingBody, 20, 0);
    lv_obj_set_style_bg_color(standingBody, lv_color_hex(0x10B981), 0);
    lv_obj_set_style_border_width(standingBody, 0, 0);
    standingEye = lv_obj_create(standingPopup);
    lv_obj_set_size(standingEye, 6, 6); lv_obj_set_pos(standingEye, 88, 93);
    lv_obj_set_style_radius(standingEye, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(standingEye, lv_color_hex(0x064E3B), 0);
    lv_obj_set_style_border_width(standingEye, 0, 0);
    standingLeg = lv_obj_create(standingPopup);
    lv_obj_set_size(standingLeg, 12, 42); lv_obj_set_pos(standingLeg, 80, 178);
    lv_obj_set_style_radius(standingLeg, 6, 0);
    lv_obj_set_style_bg_color(standingLeg, lv_color_hex(0x047857), 0);
    lv_obj_set_style_border_width(standingLeg, 0, 0);
    lv_obj_set_style_transform_pivot_x(standingLeg, 6, 0);
    lv_obj_set_style_transform_pivot_y(standingLeg, 4, 0);
    lv_obj_set_style_transform_angle(standingLeg, 0, 0);
    standingLeg2 = lv_obj_create(standingPopup);
    lv_obj_set_size(standingLeg2, 12, 42); lv_obj_set_pos(standingLeg2, 105, 178);
    lv_obj_set_style_radius(standingLeg2, 6, 0);
    lv_obj_set_style_bg_color(standingLeg2, lv_color_hex(0x047857), 0);
    lv_obj_set_style_border_width(standingLeg2, 0, 0);
    lv_obj_set_style_transform_pivot_x(standingLeg2, 6, 0);
    lv_obj_set_style_transform_pivot_y(standingLeg2, 4, 0);
    lv_obj_set_style_transform_angle(standingLeg2, 0, 0);
    standingArm = lv_obj_create(standingPopup);
    lv_obj_set_size(standingArm, 10, 52); lv_obj_set_pos(standingArm, 119, 132);
    lv_obj_set_style_radius(standingArm, 5, 0);
    lv_obj_set_style_bg_color(standingArm, lv_color_hex(0xFCD5B5), 0);
    lv_obj_set_style_border_width(standingArm, 0, 0);
    lv_obj_set_style_transform_pivot_x(standingArm, 5, 0);
    lv_obj_set_style_transform_pivot_y(standingArm, 4, 0);
    lv_obj_set_style_transform_angle(standingArm, 0, 0);
    standingArm2 = lv_obj_create(standingPopup);
    lv_obj_set_size(standingArm2, 10, 52); lv_obj_set_pos(standingArm2, 64, 132);
    lv_obj_set_style_radius(standingArm2, 5, 0);
    lv_obj_set_style_bg_color(standingArm2, lv_color_hex(0xFCD5B5), 0);
    lv_obj_set_style_border_width(standingArm2, 0, 0);
    lv_obj_set_style_transform_pivot_x(standingArm2, 5, 0);
    lv_obj_set_style_transform_pivot_y(standingArm2, 4, 0);
    lv_obj_set_style_transform_angle(standingArm2, 0, 0);

    lv_obj_t *done = lv_btn_create(standingPopup);
    lv_obj_set_size(done, 190, 52);
    lv_obj_align(done, LV_ALIGN_BOTTOM_RIGHT, -28, -24);
    lv_obj_set_style_bg_color(done, lv_color_hex(0x059669), 0);
    lv_obj_set_style_radius(done, 14, 0);
    lv_obj_t *doneLabel = lv_label_create(done);
    lv_label_set_text(doneLabel, "我起来啦");
    lv_obj_set_style_text_font(doneLabel, &ui_font_standing_16, 0);
    lv_obj_center(doneLabel);
    lv_obj_add_event_cb(done, closeStandingPopup, LV_EVENT_CLICKED, nullptr);
    lv_obj_add_flag(standingPopup, LV_OBJ_FLAG_HIDDEN);
}

static bool runStandingReminder(const struct tm &now)
{
    uint16_t minuteOfDay = now.tm_hour * 60 + now.tm_min;
    if (standingStartMinutes >= standingEndMinutes || standingIntervalMinutes == 0 ||
        minuteOfDay < standingStartMinutes || minuteOfDay >= standingEndMinutes) return false;
    uint16_t slot = (minuteOfDay - standingStartMinutes) / standingIntervalMinutes;
    uint32_t dayKey = (uint32_t)(now.tm_year + 1900) * 10000UL +
                      (uint32_t)(now.tm_mon + 1) * 100UL + (uint32_t)now.tm_mday;
    uint32_t slotKey = dayKey * 100UL + slot;
    if (slotKey == standingLastSlotKey) return false;
    standingLastSlotKey = slotKey;
    prefs.putULong("standSlot", slotKey);
    showStandingPopup();
    return true;
}
