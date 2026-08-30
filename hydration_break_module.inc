// Daily hydration reminder and manually-started lunch break.
// Kept small: the animation moves only the cup/arm objects, not the screen.

static lv_obj_t *hydrationPopup = nullptr;
static lv_obj_t *hydrationCup = nullptr;
static lv_obj_t *hydrationArm = nullptr;
static lv_obj_t *hydrationDrinkDots = nullptr;
static lv_timer_t *hydrationAutoCloseTimer = nullptr;
static lv_anim_t hydrationAnimation;

static void hydrationAnimationCb(void *, int32_t angle)
{
    if (hydrationArm) lv_obj_set_style_transform_angle(hydrationArm, angle, 0);
    if (hydrationCup) lv_obj_set_y(hydrationCup, 122 - angle / 9);
    if (hydrationDrinkDots) lv_obj_set_style_opa(hydrationDrinkDots,
                                                  angle > 0 ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
}

static void closeHydrationPopup(lv_event_t *)
{
    if (hydrationPopup) lv_obj_add_flag(hydrationPopup, LV_OBJ_FLAG_HIDDEN);
    lv_anim_del(nullptr, hydrationAnimationCb);
    if (hydrationAutoCloseTimer) {
        lv_timer_del(hydrationAutoCloseTimer);
        hydrationAutoCloseTimer = nullptr;
    }
}

static void hydrationAutoCloseCb(lv_timer_t *timer)
{
    if (hydrationPopup) lv_obj_add_flag(hydrationPopup, LV_OBJ_FLAG_HIDDEN);
    lv_anim_del(nullptr, hydrationAnimationCb);
    hydrationAutoCloseTimer = nullptr;
    lv_timer_del(timer);
}

static void showHydrationPopup()
{
    if (!hydrationPopup) return;
    // Keep this animation label ASCII-only; the water drops are drawn below.
    if (hydrationDrinkDots) lv_label_set_text(hydrationDrinkDots, "...");
    lv_obj_clear_flag(hydrationPopup, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(hydrationPopup);
    lv_anim_init(&hydrationAnimation);
    lv_anim_set_var(&hydrationAnimation, hydrationPopup);
    lv_anim_set_values(&hydrationAnimation, 0, 300);
    lv_anim_set_time(&hydrationAnimation, 550);
    lv_anim_set_playback_time(&hydrationAnimation, 550);
    lv_anim_set_repeat_count(&hydrationAnimation, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&hydrationAnimation, hydrationAnimationCb);
    lv_anim_start(&hydrationAnimation);
    if (hydrationAutoCloseTimer) lv_timer_del(hydrationAutoCloseTimer);
    hydrationAutoCloseTimer = lv_timer_create(hydrationAutoCloseCb, 20000, nullptr);
}

static void createHydrationReminderUi()
{
    hydrationPopup = lv_obj_create(lv_layer_top());
    lv_obj_set_size(hydrationPopup, 500, 275);
    lv_obj_center(hydrationPopup);
    lv_obj_set_style_bg_color(hydrationPopup, lv_color_hex(0x0B2342), 0);
    lv_obj_set_style_border_width(hydrationPopup, 2, 0);
    lv_obj_set_style_border_color(hydrationPopup, lv_color_hex(0x38BDF8), 0);
    lv_obj_set_style_radius(hydrationPopup, 24, 0);
    lv_obj_set_style_shadow_width(hydrationPopup, 24, 0);
    lv_obj_set_style_shadow_color(hydrationPopup, lv_color_hex(0x0EA5E9), 0);
    lv_obj_clear_flag(hydrationPopup, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title = lv_label_create(hydrationPopup);
    lv_label_set_text(title, "该喝水啦");
    lv_obj_set_style_text_font(title, &ui_font_todo_cjk_16, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xE0F2FE), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 55, 18);
    lv_obj_t *hint = lv_label_create(hydrationPopup);
    lv_label_set_text(hint, "喝一杯水，继续元气满满");
    lv_obj_set_style_text_font(hint, &ui_font_todo_cjk_16, 0);
    lv_label_set_text(hint, "喝一杯水 继续元气满满!");
    lv_obj_set_style_text_color(hint, lv_color_hex(0x7DD3FC), 0);
    lv_obj_align(hint, LV_ALIGN_TOP_MID, 55, 65);

    // A tiny person holding a cup. The forearm and cup are animated together.
    lv_obj_t *head = lv_obj_create(hydrationPopup);
    lv_obj_set_size(head, 42, 42); lv_obj_set_pos(head, 70, 72);
    lv_obj_set_style_radius(head, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(head, lv_color_hex(0xFCD5B5), 0);
    lv_obj_set_style_border_width(head, 0, 0);
    lv_obj_t *body = lv_obj_create(hydrationPopup);
    lv_obj_set_size(body, 50, 72); lv_obj_set_pos(body, 66, 116);
    lv_obj_set_style_radius(body, 20, 0);
    lv_obj_set_style_bg_color(body, lv_color_hex(0x4F8CFF), 0);
    lv_obj_set_style_border_width(body, 0, 0);
    lv_obj_t *eye = lv_obj_create(hydrationPopup);
    lv_obj_set_size(eye, 6, 6); lv_obj_set_pos(eye, 80, 88);
    lv_obj_set_style_radius(eye, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(eye, lv_color_hex(0x0F172A), 0);
    lv_obj_set_style_border_width(eye, 0, 0);
    lv_obj_t *leg = lv_obj_create(hydrationPopup);
    lv_obj_set_size(leg, 12, 30); lv_obj_set_pos(leg, 74, 178);
    lv_obj_set_style_radius(leg, 6, 0);
    lv_obj_set_style_bg_color(leg, lv_color_hex(0x2563EB), 0);
    lv_obj_set_style_border_width(leg, 0, 0);
    lv_obj_t *leg2 = lv_obj_create(hydrationPopup);
    lv_obj_set_size(leg2, 12, 30); lv_obj_set_pos(leg2, 96, 178);
    lv_obj_set_style_radius(leg2, 6, 0);
    lv_obj_set_style_bg_color(leg2, lv_color_hex(0x2563EB), 0);
    lv_obj_set_style_border_width(leg2, 0, 0);
    hydrationArm = lv_obj_create(hydrationPopup);
    lv_obj_set_size(hydrationArm, 64, 12); lv_obj_set_pos(hydrationArm, 105, 135);
    lv_obj_set_style_radius(hydrationArm, 8, 0);
    lv_obj_set_style_bg_color(hydrationArm, lv_color_hex(0xFCD5B5), 0);
    lv_obj_set_style_border_width(hydrationArm, 0, 0);
    lv_obj_set_style_transform_pivot_x(hydrationArm, 4, 0);
    lv_obj_set_style_transform_pivot_y(hydrationArm, 6, 0);
    hydrationCup = lv_obj_create(hydrationPopup);
    lv_obj_set_size(hydrationCup, 31, 42); lv_obj_set_pos(hydrationCup, 160, 122);
    lv_obj_set_style_radius(hydrationCup, 5, 0);
    lv_obj_set_style_bg_color(hydrationCup, lv_color_hex(0x67D5FF), 0);
    lv_obj_set_style_border_width(hydrationCup, 3, 0);
    lv_obj_set_style_border_color(hydrationCup, lv_color_hex(0xE0F2FE), 0);
    lv_obj_t *cupHandle = lv_obj_create(hydrationPopup);
    lv_obj_set_size(cupHandle, 16, 22); lv_obj_set_pos(cupHandle, 187, 132);
    lv_obj_set_style_radius(cupHandle, 10, 0);
    lv_obj_set_style_bg_color(cupHandle, lv_color_hex(0x0B2342), 0);
    lv_obj_set_style_border_width(cupHandle, 3, 0);
    lv_obj_set_style_border_color(cupHandle, lv_color_hex(0xE0F2FE), 0);
    lv_obj_t *water = lv_obj_create(hydrationCup);
    lv_obj_set_size(water, 21, 16); lv_obj_set_pos(water, 5, 21);
    lv_obj_set_style_radius(water, 3, 0);
    lv_obj_set_style_bg_color(water, lv_color_hex(0x0EA5E9), 0);
    lv_obj_set_style_border_width(water, 0, 0);
    hydrationDrinkDots = lv_label_create(hydrationPopup);
    lv_label_set_text(hydrationDrinkDots, "···");
    lv_obj_set_pos(hydrationDrinkDots, 125, 93);
    lv_obj_set_style_text_color(hydrationDrinkDots, lv_color_hex(0xBAE6FD), 0);

    lv_obj_t *done = lv_btn_create(hydrationPopup);
    lv_obj_set_size(done, 190, 52);
    lv_obj_align(done, LV_ALIGN_BOTTOM_RIGHT, -28, -24);
    lv_obj_set_style_bg_color(done, lv_color_hex(0x0284C7), 0);
    lv_obj_set_style_radius(done, 14, 0);
    lv_obj_t *doneLabel = lv_label_create(done);
    lv_label_set_text(doneLabel, "我喝好了");
    lv_obj_set_style_text_font(doneLabel, &ui_font_todo_cjk_16, 0);
    lv_obj_center(doneLabel);
    lv_obj_add_event_cb(done, closeHydrationPopup, LV_EVENT_CLICKED, nullptr);
    lv_obj_add_flag(hydrationPopup, LV_OBJ_FLAG_HIDDEN);
}

static void runHydrationReminder(const struct tm &now)
{
    uint16_t minuteOfDay = now.tm_hour * 60 + now.tm_min;
    if (hydrationStartMinutes >= hydrationEndMinutes || hydrationIntervalMinutes == 0 ||
        minuteOfDay < hydrationStartMinutes || minuteOfDay >= hydrationEndMinutes) return;
    uint16_t slot = (minuteOfDay - hydrationStartMinutes) / hydrationIntervalMinutes;
    uint32_t dayKey = (uint32_t)(now.tm_year + 1900) * 10000UL +
                      (uint32_t)(now.tm_mon + 1) * 100UL + (uint32_t)now.tm_mday;
    uint32_t slotKey = dayKey * 100UL + slot;
    if (slotKey == hydrationLastSlotKey) return;
    hydrationLastSlotKey = slotKey;
    prefs.putULong("drinkSlot", slotKey);
    showHydrationPopup();
}

static bool startLunchBreak()
{
    struct tm localNow;
    if (!getLocalTime(&localNow, 5)) {
        showToast("时间未同步，暂不能开始午休");
        return false;
    }
    struct tm endTime = localNow;
    endTime.tm_hour = lunchEndMinutes / 60;
    endTime.tm_min = lunchEndMinutes % 60;
    endTime.tm_sec = 0;
    time_t endEpoch = mktime(&endTime);
    if (endEpoch <= time(nullptr)) {
        showToast("午休结束时间已过，请在手机修改");
        return false;
    }
    stateUntilEpoch = endEpoch;
    stateUntilMillis = 0;
    prefs.putULong64("stateUntil", (uint64_t)endEpoch);
    prefs.putUChar("state", 7);
    updateStatusUi(7);
    char notice[40];
    snprintf(notice, sizeof(notice), "午休将在 %02u:%02u 结束", lunchEndMinutes / 60, lunchEndMinutes % 60);
    if (statusUntilLabel) lv_label_set_text(statusUntilLabel, notice);
    showToast(notice);
    return true;
}
