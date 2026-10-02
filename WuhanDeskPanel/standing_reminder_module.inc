// Daily sedentary reminder with an articulated, locally drawn stretch loop.
// One small invalidated scene avoids transformed objects and RGB/PSRAM churn.

static lv_obj_t *standingPopup = nullptr;
static lv_obj_t *standingScene = nullptr;
static lv_timer_t *standingAutoCloseTimer = nullptr;
static lv_anim_t standingAnimation;
static int32_t standingProgress = 0;

static int32_t standingReach(int32_t progress)
{
    if (progress < 95) return 0;
    if (progress < 320) return reminderPhase(progress, 95, 320);
    if (progress < 755) return 1000;
    return 1000 - reminderPhase(progress, 755, 925);
}

static void standingDrawArm(const ReminderPainter &p, int32_t shoulderX,
                            int32_t shoulderY, int32_t elbowX, int32_t elbowY,
                            int32_t handX, int32_t handY)
{
    const uint32_t skin = 0xF6C6A8;
    reminderLine(p, shoulderX, shoulderY, elbowX, elbowY, 13, 0x173F37);
    reminderLine(p, elbowX, elbowY, handX, handY, 13, 0x173F37);
    reminderLine(p, shoulderX, shoulderY, elbowX, elbowY, 10, skin);
    reminderLine(p, elbowX, elbowY, handX, handY, 10, skin);
    reminderLine(p, shoulderX, shoulderY,
                 reminderMix(shoulderX, elbowX, 410),
                 reminderMix(shoulderY, elbowY, 410), 17, 0x38C99A);
    reminderCircle(p, handX, handY, 6, skin);
    reminderCircle(p, handX - 1, handY - 2, 2, 0xFFE5CB, 170);
}

static void standingDrawScene(lv_event_t *event)
{
    const ReminderPainter p = reminderPainter(event);
    const int32_t progress = standingProgress;
    const int32_t reach = standingReach(progress);
    // A small knee bend precedes the reach. Both feet remain on the floor.
    int32_t anticipation = reminderPhase(progress, 0, 55);
    if (progress >= 55) anticipation = 1000 - reminderPhase(progress, 55, 150);
    const int32_t dip = anticipation * 4 / 1000;
    const int32_t lift = reach * 8 / 1000;
    int32_t lean = 0;
    if (progress >= 385 && progress < 495)
        lean = -reminderMix(0, 11, reminderPhase(progress, 385, 495));
    else if (progress >= 495 && progress < 635)
        lean = reminderMix(-11, 11, reminderPhase(progress, 495, 635));
    else if (progress >= 635 && progress < 750)
        lean = reminderMix(11, 0, reminderPhase(progress, 635, 750));

    const int32_t headX = 102 + lean;
    const int32_t headY = 99 - lift + dip;
    const int32_t shoulderY = 131 - lift + dip;
    const int32_t opening = reach < 450 ? reminderEase(reach * 1000 / 450) : 1000;
    const int32_t overhead = reach <= 450 ? 0 : reminderEase((reach - 450) * 1000 / 550);
    const int32_t elbowX = reach <= 450 ? reminderMix(56, 49, opening)
                                                       : reminderMix(49, 69, overhead);
    const int32_t elbowY = reach <= 450 ? reminderMix(158, 117, opening)
                                                       : reminderMix(117, 88, overhead);
    const int32_t handX = reach <= 450 ? reminderMix(65, 42, opening)
                                                      : reminderMix(42, 97, overhead);
    const int32_t handY = reach <= 450 ? reminderMix(188, 83, opening)
                                                      : reminderMix(83, 58, overhead);
    const int32_t armDip = dip * (1000 - reach) / 1000;

    reminderRect(p, 0, 0, 204, 252, 22, 0x123A32);
    reminderCircle(p, 102, 127, 86, 0x173F36);
    reminderCircle(p, 102, 127, 72, 0x19473B);
    reminderCircle(p, 35, 61, 3, 0x56D7AD, 150);
    reminderCircle(p, 167, 88, 2, 0xA7F3D0, 165);
    reminderLine(p, 157, 50, 157, 60, 2, 0x6DE4BB, 180);
    reminderLine(p, 152, 55, 162, 55, 2, 0x6DE4BB, 180);
    reminderRect(p, 51, 222, 103, 9, 5, 0x0B2924, 180);
    reminderRect(p, 69, 224, 67, 3, 2, 0x061E1B, 155);

    // Trouser legs bend at the knees while the ankles stay fixed.
    const int32_t kneeSpread = anticipation * 3 / 1000;
    reminderLine(p, 92, 174 + dip, 88 - kneeSpread, 198, 17, 0x216A59);
    reminderLine(p, 88 - kneeSpread, 198, 87, 215, 16, 0x216A59);
    reminderLine(p, 112, 174 + dip, 116 + kneeSpread, 198, 17, 0x1C5A4D);
    reminderLine(p, 116 + kneeSpread, 198, 117, 215, 16, 0x1C5A4D);
    reminderLine(p, 85, 184 + dip, 82 - kneeSpread, 200, 3, 0x3B8E76, 160);
    reminderRect(p, 72, 216, 28, 11, 6, 0xD5EEE2);
    reminderRect(p, 105, 216, 28, 11, 6, 0xE5F5EA);
    reminderRect(p, 73, 223, 27, 4, 2, 0x8EB6A6);
    reminderRect(p, 105, 223, 27, 4, 2, 0x9BC2B2);

    // Separate upper arms and forearms allow the elbows to lead the gesture.
    standingDrawArm(p, 80 + lean, shoulderY,
                    elbowX + lean, elbowY + armDip,
                    handX + lean, handY + armDip);
    standingDrawArm(p, 124 + lean, shoulderY,
                    204 - elbowX + lean, elbowY + armDip,
                    204 - handX + lean, handY + armDip);

    reminderLine(p, 102 + lean / 3, 159 + dip,
                 102 + lean, 142 - lift + dip, 47, 0x123B32);
    reminderLine(p, 102 + lean / 3, 159 + dip,
                 102 + lean, 142 - lift + dip, 44, 0x38C99A);
    reminderLine(p, 87 + lean / 3, 155 + dip,
                 87 + lean, 139 - lift + dip, 4, 0x9AF0CD, 125);
    reminderLine(p, 92 + lean / 3, 177 + dip,
                 112 + lean / 3, 177 + dip, 3, 0x229D77, 175);
    reminderCircle(p, 112 + lean, 137 - lift + dip, 3, 0xCBFCE4, 180);
    reminderLine(p, headX, headY + 18, headX, headY + 29, 13, 0xEDB496);
    reminderArc(p, headX, headY + 25, 9, 10, 170, 3, 0x209C76);

    // Hair, cheeks and a relaxed expression make the figure readable at 1x.
    reminderCircle(p, headX, headY - 2, 23, 0x163B34);
    reminderCircle(p, headX - 22, headY + 3, 5, 0xF0B797);
    reminderCircle(p, headX + 22, headY + 3, 5, 0xF0B797);
    reminderCircle(p, headX, headY + 3, 21, 0xF6C6A8);
    reminderArc(p, headX, headY - 1, 22, 183, 355, 8, 0x163B34);
    reminderLine(p, headX - 17, headY - 11, headX - 5, headY - 16, 8, 0x163B34);
    reminderLine(p, headX - 5, headY - 16, headX + 8, headY - 11, 8, 0x163B34);
    reminderCircle(p, headX - 13, headY + 9, 4, 0xE98F83, 85);
    reminderCircle(p, headX + 13, headY + 9, 4, 0xE98F83, 85);
    if (reach > 900) {
        reminderArc(p, headX - 7, headY + 5, 3, 190, 350, 2, 0x28483D);
        reminderArc(p, headX + 7, headY + 5, 3, 190, 350, 2, 0x28483D);
    } else {
        reminderCircle(p, headX - 7, headY + 3, 2, 0x28483D);
        reminderCircle(p, headX + 7, headY + 3, 2, 0x28483D);
    }
    reminderArc(p, headX, headY + 8, 6, 20, 160, 2, 0xA05A49);

    const lv_opa_t breath = (lv_opa_t)(reach * 120 / 1000);
    reminderArc(p, 102 + lean, 65, 25, 208, 238, 2, 0xA7F3D0, breath);
    reminderArc(p, 102 + lean, 65, 25, 302, 332, 2, 0xA7F3D0, breath);
    // Phase marks crossfade, including the return to the first loop frame.
    const int32_t preparing = 1000 - reminderPhase(progress, 260, 330)
                                  + reminderPhase(progress, 915, 980);
    const int32_t relaxing = reminderPhase(progress, 735, 805)
                                - reminderPhase(progress, 915, 980);
    const int32_t phaseOpacity[3] = {preparing, 1000 - preparing - relaxing, relaxing};
    for (int32_t i = 0; i < 3; ++i) {
        reminderRect(p, 77 + i * 19, 242, 12, 3, 2, 0x285446);
        reminderRect(p, 77 + i * 19, 242, 12, 3, 2, 0x6EE7B7,
                     (lv_opa_t)(phaseOpacity[i] * 255 / 1000));
    }
}

static void standingAnimationCb(void *, int32_t progress)
{
    if (standingProgress == progress) return;
    standingProgress = progress;
    if (standingScene) lv_obj_invalidate(standingScene);
}

static void closeStandingPopup(lv_event_t *)
{
    if (standingPopup) {
        lv_obj_add_flag(standingPopup, LV_OBJ_FLAG_HIDDEN);
        lv_anim_del(standingPopup, standingAnimationCb);
    }
    standingReminderActive = false;
    if (standingAutoCloseTimer) {
        lv_timer_del(standingAutoCloseTimer);
        standingAutoCloseTimer = nullptr;
    }
}

static void standingAutoCloseCb(lv_timer_t *timer)
{
    standingAutoCloseTimer = nullptr;
    lv_timer_del(timer);
    closeStandingPopup(nullptr);
}

static void showStandingPopup()
{
    if (!standingPopup) return;
    // Reset the previous run, including its timer, before restarting the loop.
    closeStandingPopup(nullptr);
    closeHydrationPopup(nullptr);
    standingReminderActive = true;
    standingProgress = 0;
    lv_obj_invalidate(standingScene);
    lv_obj_clear_flag(standingPopup, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(standingPopup);
    lv_anim_init(&standingAnimation);
    lv_anim_set_var(&standingAnimation, standingPopup);
    lv_anim_set_values(&standingAnimation, 0, 1000);
    lv_anim_set_time(&standingAnimation, 6000);
    lv_anim_set_path_cb(&standingAnimation, lv_anim_path_linear);
    lv_anim_set_repeat_count(&standingAnimation, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&standingAnimation, standingAnimationCb);
    lv_anim_start(&standingAnimation);
    standingAutoCloseTimer = lv_timer_create(standingAutoCloseCb, 20000, nullptr);
}

static void createStandingReminderUi()
{
    standingPopup = lv_obj_create(lv_layer_top());
    reminderStylePopup(standingPopup, 0x102F2B, 0x6EE7B7);
    standingScene = reminderCreateScene(standingPopup, 18, 24, 204, 252, standingDrawScene);
    reminderLabel(standingPopup, "站起来伸个懒腰", &ui_font_standing_16,
                  0xD1FAE5, 244, 52, 264);
    reminderLabel(standingPopup, "伸个懒腰\n放松一下身体!", &ui_font_standing_16,
                  0x89BBA7, 244, 90, 248);
    reminderLabel(standingPopup, "STRETCH.  BREATHE.  RESET.", &lv_font_montserrat_14,
                  0x6EA590, 244, 174, 264);
    reminderButton(standingPopup, "我起来啦", &ui_font_standing_16,
                   0x059669, closeStandingPopup);
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
