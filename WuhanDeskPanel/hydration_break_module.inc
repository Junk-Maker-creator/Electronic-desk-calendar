// Daily hydration reminder and manually-started lunch break.
// Kept small: animate only the person and cup objects, not the screen.

static lv_obj_t *hydrationPopup = nullptr;
static lv_obj_t *hydrationHead = nullptr;
static lv_obj_t *hydrationBody = nullptr;
static lv_obj_t *hydrationEye = nullptr;
static lv_obj_t *hydrationLeg = nullptr;
static lv_obj_t *hydrationLeg2 = nullptr;
static lv_obj_t *hydrationCup = nullptr;
static lv_obj_t *hydrationArm = nullptr;
static lv_obj_t *hydrationDrinkDots = nullptr;
static lv_timer_t *hydrationAutoCloseTimer = nullptr;
static lv_anim_t hydrationAnimation;

static int32_t hydrationSmoothStep(int32_t progress)
{
    if (progress <= 0) return 0;
    if (progress >= 1000) return 1000;
    return (int32_t)((int64_t)progress * progress * (3000 - 2 * progress) / 1000000LL);
}

static int32_t hydrationMove(int32_t progress, int32_t start, int32_t end)
{
    if (progress <= start) return 0;
    if (progress >= end) return 1000;
    return hydrationSmoothStep((progress - start) * 1000 / (end - start));
}

static int32_t hydrationLift(int32_t progress)
{
    if (progress < 120) return 0;
    if (progress < 440) return hydrationMove(progress, 120, 440);
    if (progress < 560) return 1000;
    if (progress < 900) return 1000 - hydrationMove(progress, 560, 900);
    return 0;
}

static int32_t hydrationSip(int32_t progress)
{
    if (progress < 440) return 0;
    if (progress < 500) return hydrationMove(progress, 440, 500);
    if (progress < 590) return 1000;
    if (progress < 650) return 1000 - hydrationMove(progress, 590, 650);
    return 0;
}

static void hydrationAnimationCb(void *, int32_t progress)
{
    const int32_t lift = hydrationLift(progress);
    const int32_t sip = hydrationSip(progress);
    const int32_t bob = lift * 2 / 1000;
    if (hydrationHead) lv_obj_set_y(hydrationHead, 72 - bob);
    if (hydrationBody) lv_obj_set_y(hydrationBody, 116 - bob);
    if (hydrationEye) lv_obj_set_y(hydrationEye, 88 - bob);
    if (hydrationLeg) lv_obj_set_y(hydrationLeg, 178 - bob);
    if (hydrationLeg2) lv_obj_set_y(hydrationLeg2, 178 - bob);
    if (hydrationArm) {
        lv_obj_set_y(hydrationArm, 135 - bob);
        lv_obj_set_style_transform_angle(hydrationArm, -(lift * 230) / 1000, 0);
    }
    if (hydrationCup) {
        lv_obj_set_x(hydrationCup, 160 - (lift * 22) / 1000);
        lv_obj_set_y(hydrationCup, 122 - (lift * 28) / 1000 - bob);
        lv_obj_set_style_transform_angle(hydrationCup, -(sip * 120) / 1000, 0);
    }
    if (hydrationDrinkDots) lv_obj_set_style_opa(hydrationDrinkDots,
                                                  sip > 120 ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
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
    if (standingReminderActive) return;
    // Keep this animation label ASCII-only so it renders on every font set.
    if (hydrationDrinkDots) lv_label_set_text(hydrationDrinkDots, "...");
    lv_obj_clear_flag(hydrationPopup, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(hydrationPopup);
    lv_anim_init(&hydrationAnimation);
    lv_anim_set_var(&hydrationAnimation, hydrationPopup);
    lv_anim_set_values(&hydrationAnimation, 0, 1000);
    lv_anim_set_time(&hydrationAnimation, 2600);
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
    hydrationHead = lv_obj_create(hydrationPopup);
    lv_obj_set_size(hydrationHead, 42, 42); lv_obj_set_pos(hydrationHead, 70, 72);
    lv_obj_set_style_radius(hydrationHead, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(hydrationHead, lv_color_hex(0xFCD5B5), 0);
    lv_obj_set_style_border_width(hydrationHead, 0, 0);
    hydrationBody = lv_obj_create(hydrationPopup);
    lv_obj_set_size(hydrationBody, 50, 72); lv_obj_set_pos(hydrationBody, 66, 116);
    lv_obj_set_style_radius(hydrationBody, 20, 0);
    lv_obj_set_style_bg_color(hydrationBody, lv_color_hex(0x4F8CFF), 0);
    lv_obj_set_style_border_width(hydrationBody, 0, 0);
    hydrationEye = lv_obj_create(hydrationPopup);
    lv_obj_set_size(hydrationEye, 6, 6); lv_obj_set_pos(hydrationEye, 80, 88);
    lv_obj_set_style_radius(hydrationEye, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(hydrationEye, lv_color_hex(0x0F172A), 0);
    lv_obj_set_style_border_width(hydrationEye, 0, 0);
    hydrationLeg = lv_obj_create(hydrationPopup);
    lv_obj_set_size(hydrationLeg, 12, 30); lv_obj_set_pos(hydrationLeg, 74, 178);
    lv_obj_set_style_radius(hydrationLeg, 6, 0);
    lv_obj_set_style_bg_color(hydrationLeg, lv_color_hex(0x2563EB), 0);
    lv_obj_set_style_border_width(hydrationLeg, 0, 0);
    hydrationLeg2 = lv_obj_create(hydrationPopup);
    lv_obj_set_size(hydrationLeg2, 12, 30); lv_obj_set_pos(hydrationLeg2, 96, 178);
    lv_obj_set_style_radius(hydrationLeg2, 6, 0);
    lv_obj_set_style_bg_color(hydrationLeg2, lv_color_hex(0x2563EB), 0);
    lv_obj_set_style_border_width(hydrationLeg2, 0, 0);
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
    lv_obj_set_style_transform_pivot_x(hydrationCup, 5, 0);
    lv_obj_set_style_transform_pivot_y(hydrationCup, 36, 0);
    lv_obj_t *cupHandle = lv_obj_create(hydrationCup);
    lv_obj_set_size(cupHandle, 16, 22); lv_obj_set_pos(cupHandle, 27, 10);
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
    lv_label_set_text(hydrationDrinkDots, "...");
    lv_obj_set_style_text_font(hydrationDrinkDots, &lv_font_montserrat_20, 0);
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

static void runHydrationReminder(const struct tm &now, bool standingPriority)
{
    uint16_t minuteOfDay = now.tm_hour * 60 + now.tm_min;
    if (hydrationStartMinutes >= hydrationEndMinutes || hydrationIntervalMinutes == 0 ||
        minuteOfDay < hydrationStartMinutes || minuteOfDay >= hydrationEndMinutes) return;
    uint16_t slot = (minuteOfDay - hydrationStartMinutes) / hydrationIntervalMinutes;
    uint32_t dayKey = (uint32_t)(now.tm_year + 1900) * 10000UL +
                      (uint32_t)(now.tm_mon + 1) * 100UL + (uint32_t)now.tm_mday;
    uint32_t slotKey = dayKey * 100UL + slot;
    if (slotKey == hydrationLastSlotKey) return;
    if (standingPriority || standingReminderActive) {
        // Consume this slot so the lower-priority hydration reminder does not
        // appear a moment later after the standing popup closes.
        hydrationLastSlotKey = slotKey;
        prefs.putULong("drinkSlot", slotKey);
        return;
    }
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
