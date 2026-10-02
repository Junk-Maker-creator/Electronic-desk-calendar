// Daily hydration reminder and manually-started lunch break.
// One small vector scene; hands and glass share geometry throughout the cycle.

static lv_obj_t *hydrationPopup = nullptr;
static lv_obj_t *hydrationScene = nullptr;
static lv_timer_t *hydrationAutoCloseTimer = nullptr;
static lv_anim_t hydrationAnimation;
static int32_t hydrationProgress = 0;

struct HydrationPose {
    int32_t lift;
    int32_t tilt;
    int32_t swallow;
    int32_t blink;
};

static HydrationPose hydrationPoseAt(int32_t progress)
{
    HydrationPose pose;
    // Settle, raise, sip twice, straighten the glass, then lower it.
    pose.lift = reminderPhase(progress, 110, 350) - reminderPhase(progress, 730, 940);
    pose.tilt = reminderPhase(progress, 370, 460) - reminderPhase(progress, 640, 720);
    pose.swallow = reminderPhase(progress, 475, 500) - reminderPhase(progress, 500, 535)
                 + reminderPhase(progress, 560, 585) - reminderPhase(progress, 585, 620);
    pose.blink = reminderPhase(progress, 35, 50) - reminderPhase(progress, 60, 80);
    if (pose.tilt > pose.blink) pose.blink = pose.tilt;
    return pose;
}

// Cup-local geometry pivots around the lip, so the rim stays at the mouth.
static lv_point_t hydrationCupPoint(int x, int y, int angle, int localX, int localY)
{
    const int32_t sine = lv_trigo_sin(angle);
    const int32_t cosine = lv_trigo_sin(angle + 90);
    return {(lv_coord_t)(x + (localX * cosine - localY * sine) / 32768),
            (lv_coord_t)(y + (localX * sine + localY * cosine) / 32768)};
}

static void hydrationCupPolygon(const ReminderPainter &p, const lv_point_t *points,
                                 uint32_t color)
{
    lv_point_t absolute[4];
    for (uint8_t i = 0; i < 4; ++i) {
        absolute[i].x = p.x + points[i].x;
        absolute[i].y = p.y + points[i].y;
    }
    lv_draw_rect_dsc_t dsc;
    lv_draw_rect_dsc_init(&dsc);
    dsc.bg_color = lv_color_hex(color);
    lv_draw_polygon(p.ctx, &dsc, absolute, 4);
}

static void hydrationDrawGlass(const ReminderPainter &p, int x, int y, int angle,
                                int32_t progress)
{
    lv_point_t glass[4] = {
        hydrationCupPoint(x, y, angle, 0, 0), hydrationCupPoint(x, y, angle, 26, 0),
        hydrationCupPoint(x, y, angle, 23, 32), hydrationCupPoint(x, y, angle, 3, 32)
    };
    hydrationCupPolygon(p, glass, 0xC4E9EF);

    // The surface stays horizontal in screen space while the glass tips.
    // Restore the level during the lowering gesture for a matching loop seam.
    const int32_t consumed = reminderPhase(progress, 470, 620)
                           - reminderPhase(progress, 760, 1000);
    const int surfaceY = y + reminderMix(14, 21, consumed) + angle / 3;
    lv_point_t water[4] = {
        hydrationCupPoint(x, y, angle, 4, 0), hydrationCupPoint(x, y, angle, 22, 0),
        hydrationCupPoint(x, y, angle, 20, 28), hydrationCupPoint(x, y, angle, 6, 28)
    };
    for (uint8_t i = 0; i < 2; ++i) {
        const lv_point_t bottom = water[3 - i];
        const int dy = bottom.y - water[i].y;
        const int level = surfaceY < bottom.y - 1 ? surfaceY : bottom.y - 1;
        if (dy > 0) water[i].x += (bottom.x - water[i].x) * (level - water[i].y) / dy;
        water[i].y = level;
    }
    hydrationCupPolygon(p, water, 0x29AFCB);
    reminderLine(p, water[0].x, water[0].y, water[1].x, water[1].y, 2, 0x8AE4EE);
    for (uint8_t i = 0; i < 4; ++i) {
        const lv_point_t &a = glass[i];
        const lv_point_t &b = glass[(i + 1) % 4];
        reminderLine(p, a.x, a.y, b.x, b.y, 2, 0xEBFBFF);
    }
    const lv_point_t glintA = hydrationCupPoint(x, y, angle, 5, 5);
    const lv_point_t glintB = hydrationCupPoint(x, y, angle, 6, 12);
    reminderLine(p, glintA.x, glintA.y, glintB.x, glintB.y, 2, 0xFFFFFF);
}

static void hydrationDrawScene(lv_event_t *event)
{
    const ReminderPainter p = reminderPainter(event);
    const HydrationPose pose = hydrationPoseAt(hydrationProgress);
    const int rise = reminderMix(0, 2, pose.lift);
    const int headX = 88 - rise;
    const int headY = 90 - rise;
    const int cupX = reminderMix(135, 108 - rise, pose.lift);
    const int cupY = reminderMix(144, 100 - rise, pose.lift);
    const int cupAngle = reminderMix(0, -32, pose.tilt);
    const lv_point_t hand = hydrationCupPoint(cupX, cupY, cupAngle, 25, 21);
    const int elbowX = reminderMix(132, 144, pose.lift);
    const int elbowY = reminderMix(156, 139, pose.lift);
    const uint32_t skin = 0xF5CBAA;

    reminderRect(p, 0, 0, 204, 252, 22, 0x102D43);
    reminderCircle(p, 102, 119, 77, 0x173B50);
    reminderArc(p, 102, 119, 85, 214, 285, 2, 0x27566A);
    reminderCircle(p, 171, 64, 3, 0x5BA0B0);
    reminderCircle(p, 29, 163, 2, 0x5BA0B0);
    reminderRect(p, 52, 220, 85, 9, LV_RADIUS_CIRCLE, 0x0A2131);

    // Shoes and legs stay grounded. Only the chest and head breathe slightly.
    reminderLine(p, 78, 176, 75, 215, 16, 0x365875);
    reminderLine(p, 101, 176, 104, 215, 16, 0x426986);
    reminderRect(p, 63, 211, 25, 13, 6, 0xE0EDF0);
    reminderRect(p, 96, 211, 27, 13, 6, 0xF0F6F2);
    reminderLine(p, 66, 221, 85, 221, 2, 0x91B3C1);
    reminderLine(p, 99, 221, 120, 221, 2, 0x91B3C1);
    reminderLine(p, 68, 129 - rise, 56, 152, 12, 0x3E9EB9);
    reminderLine(p, 56, 152, 62, 174, 10, skin);
    reminderCircle(p, 62, 175, 5, skin);

    reminderRect(p, 64, 117 - rise, 52, 65 + rise, 18, 0x60C2D6);
    reminderRect(p, 70, 161, 40, 20, 7, 0x4FB1CA);
    reminderLine(p, 78, 137 - rise, 78, 153 - rise, 3, 0x8AD8E2);
    reminderRect(p, 82 - rise, 106 - rise, 14, 18, 6, skin);
    reminderArc(p, 89 - rise, 113 - rise, 12, 12, 168, 3, 0xB3EDF0);
    reminderCircle(p, headX, headY, 23, skin);
    reminderCircle(p, headX - 22, headY + 3, 5, 0xEAAF90);
    reminderRect(p, headX - 23, headY - 23, 43, 16, 8, 0x243E52);
    reminderCircle(p, headX - 15, headY - 11, 9, 0x243E52);
    reminderCircle(p, headX - 1, headY - 20, 11, 0x243E52);
    reminderCircle(p, headX + 21, headY + 3, 4, skin);
    if (pose.blink > 650) {
        reminderArc(p, headX + 10, headY - 3, 4, 25, 155, 2, 0x243E52);
    } else {
        reminderRect(p, headX + 9, headY - 4, 3, reminderMix(5, 2, pose.blink), 2, 0x243E52);
    }
    reminderRect(p, headX + 11, headY + 5, 6, 3, 2, 0xE8A28A);
    reminderArc(p, headX + 16, headY + 7, 5, 20, 130, 2, 0x9A6554);
    if (pose.swallow > 0) {
        reminderCircle(p, headX + 7, headY + 27, 2, 0xE3AB8B,
                       (lv_opa_t)(pose.swallow * 180 / 1000));
    }

    // The wrist is derived from the glass transform, never animated separately.
    reminderLine(p, 111, 130 - rise, elbowX, elbowY, 13, 0x60C2D6);
    reminderLine(p, elbowX, elbowY, hand.x, hand.y, 10, skin);
    hydrationDrawGlass(p, cupX, cupY, cupAngle, hydrationProgress);
    reminderCircle(p, hand.x, hand.y, 5, skin);
    reminderLine(p, hand.x - 3, hand.y - 2, hand.x + 1, hand.y - 2, 2, 0xFFE1C4);

    // Small rising bubbles accompany the sip; they fade at both endpoints.
    for (uint8_t i = 0; i < 2; ++i) {
        const int t = (hydrationProgress - 440 - i * 90) * 1000 / 160;
        if (t > 0 && t < 1000) {
            const int fade = t < 500 ? t * 2 : (1000 - t) * 2;
            reminderCircle(p, 164 + i * 9, 87 - t * 22 / 1000,
                           2 + i, 0x94E7EB, (lv_opa_t)(fade * 190 / 1000));
        }
    }
    const int preparing = 1000 - reminderPhase(hydrationProgress, 310, 380)
                              + reminderPhase(hydrationProgress, 915, 980);
    const int relaxing = reminderPhase(hydrationProgress, 705, 775)
                             - reminderPhase(hydrationProgress, 915, 980);
    const int phaseOpacity[3] = {preparing, 1000 - preparing - relaxing, relaxing};
    for (int i = 0; i < 3; ++i) {
        reminderRect(p, 77 + i * 19, 242, 12, 3, 2, 0x2B5064);
        reminderRect(p, 77 + i * 19, 242, 12, 3, 2, 0x82DAE7,
                     (lv_opa_t)(phaseOpacity[i] * 255 / 1000));
    }
}

static void hydrationAnimationCb(void *, int32_t progress)
{
    if (hydrationProgress == progress) return;
    hydrationProgress = progress;
    if (hydrationScene) lv_obj_invalidate(hydrationScene);
}

static void closeHydrationPopup(lv_event_t *)
{
    lv_anim_del(hydrationPopup, hydrationAnimationCb);
    if (hydrationPopup) lv_obj_add_flag(hydrationPopup, LV_OBJ_FLAG_HIDDEN);
    if (hydrationAutoCloseTimer) {
        lv_timer_del(hydrationAutoCloseTimer);
        hydrationAutoCloseTimer = nullptr;
    }
}

static void hydrationAutoCloseCb(lv_timer_t *)
{
    closeHydrationPopup(nullptr);
}

static void showHydrationPopup()
{
    if (!hydrationPopup || standingReminderActive) return;
    closeHydrationPopup(nullptr);
    hydrationProgress = 0;
    lv_obj_clear_flag(hydrationPopup, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(hydrationPopup);
    lv_anim_init(&hydrationAnimation);
    lv_anim_set_var(&hydrationAnimation, hydrationPopup);
    lv_anim_set_values(&hydrationAnimation, 0, 1000);
    lv_anim_set_time(&hydrationAnimation, 5600);
    lv_anim_set_path_cb(&hydrationAnimation, lv_anim_path_linear);
    lv_anim_set_repeat_count(&hydrationAnimation, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&hydrationAnimation, hydrationAnimationCb);
    lv_anim_start(&hydrationAnimation);
    hydrationAutoCloseTimer = lv_timer_create(hydrationAutoCloseCb, 20000, nullptr);
}

static void createHydrationReminderUi()
{
    hydrationPopup = lv_obj_create(lv_layer_top());
    reminderStylePopup(hydrationPopup, 0x0D2235, 0x67CDDE);
    hydrationScene = reminderCreateScene(hydrationPopup, 18, 24, 204, 252, hydrationDrawScene);
    reminderLabel(hydrationPopup, "该喝水啦", &ui_font_todo_cjk_16,
                  0xE6F7FA, 244, 52, 264);
    reminderLabel(hydrationPopup, "喝一杯水\n继续元气满满!", &ui_font_todo_cjk_16,
                  0xA0C6D6, 244, 90, 264);
    reminderLabel(hydrationPopup, "SIP.  BREATHE.  RESET.", &lv_font_montserrat_14,
                  0x6A9BAF, 244, 174, 264);
    reminderButton(hydrationPopup, "我喝好了", &ui_font_todo_cjk_16,
                   0x197C9A, closeHydrationPopup);
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
        showToast("午休结束时间无效，请在手机修改");
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
