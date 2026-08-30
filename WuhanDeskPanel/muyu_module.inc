// Electronic wooden-fish interaction and blessing animation
// Included by WuhanDeskPanel.ino; keep in the same sketch directory.

static void muyuEvent(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED) {
        lv_obj_set_style_transform_zoom(muyuBody, 235, 0);
        lv_obj_set_style_translate_y(muyuBody, 8, 0);
        if (muyuStick != nullptr) {
            lv_obj_set_style_translate_x(muyuStick, -24, 0);
            lv_obj_set_style_translate_y(muyuStick, 32, 0);
            lv_obj_set_style_transform_angle(muyuStick, -120, 0);
        }
    } else if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        lv_obj_set_style_transform_zoom(muyuBody, 256, 0);
        lv_obj_set_style_translate_y(muyuBody, 0, 0);
        if (muyuStick != nullptr) {
            lv_obj_set_style_translate_x(muyuStick, 0, 0);
            lv_obj_set_style_translate_y(muyuStick, 0, 0);
            lv_obj_set_style_transform_angle(muyuStick, 0, 0);
        }
        if (code == LV_EVENT_RELEASED) {
            uint32_t now = millis();
            if (now - lastMuyuTap < 120) return;
            lastMuyuTap = now;
            ++muyuCount;
            char text[32];
            snprintf(text, sizeof(text), "功德 +1  ·  %lu", (unsigned long)muyuCount);
            lv_label_set_text(muyuCountLabel, text);
            if ((muyuCount % 20) == 0 && muyuBlessingCard != nullptr) {
                lv_anim_del(muyuBlessingCard, nullptr);
                if (muyuBlessingHalo != nullptr) {
                    lv_anim_del(muyuBlessingHalo, nullptr);
                    lv_anim_t haloAnim;
                    lv_anim_init(&haloAnim);
                    lv_anim_set_var(&haloAnim, muyuBlessingHalo);
                    lv_anim_set_values(&haloAnim, 3, 9);
                    lv_anim_set_time(&haloAnim, 520);
                    lv_anim_set_playback_time(&haloAnim, 520);
                    lv_anim_set_repeat_count(&haloAnim, 1);
                    lv_anim_set_exec_cb(&haloAnim, [](void *obj, int32_t value) {
                        lv_obj_set_style_border_width(static_cast<lv_obj_t *>(obj), value, 0);
                    });
                    lv_anim_start(&haloAnim);
                }
                static const int16_t particleStartY[8] = {139, 126, 150, 118, 145, 132, 153, 121};
                for (int i = 0; i < 8; ++i) {
                    if (muyuParticles[i] == nullptr) continue;
                    lv_anim_del(muyuParticles[i], nullptr);
                    lv_obj_set_y(muyuParticles[i], particleStartY[i]);
                    lv_anim_t particleAnim;
                    lv_anim_init(&particleAnim);
                    lv_anim_set_var(&particleAnim, muyuParticles[i]);
                    lv_anim_set_values(&particleAnim, particleStartY[i], particleStartY[i] - 72 - (i % 3) * 12);
                    lv_anim_set_delay(&particleAnim, i * 55);
                    lv_anim_set_time(&particleAnim, 620);
                    lv_anim_set_playback_time(&particleAnim, 420);
                    lv_anim_set_playback_delay(&particleAnim, 380);
                    lv_anim_set_exec_cb(&particleAnim, [](void *obj, int32_t value) {
                        lv_obj_set_y(static_cast<lv_obj_t *>(obj), value);
                    });
                    lv_anim_start(&particleAnim);
                }
                lv_obj_clear_flag(muyuBlessingCard, LV_OBJ_FLAG_HIDDEN);
                lv_obj_move_foreground(muyuBlessingCard);
                lv_obj_set_y(muyuBlessingCard, 132);
                lv_anim_t anim;
                lv_anim_init(&anim);
                lv_anim_set_var(&anim, muyuBlessingCard);
                lv_anim_set_values(&anim, 132, 116);
                lv_anim_set_time(&anim, 420);
                lv_anim_set_playback_time(&anim, 420);
                lv_anim_set_playback_delay(&anim, 1250);
                lv_anim_set_exec_cb(&anim, [](void *obj, int32_t value) {
                    lv_obj_set_y(static_cast<lv_obj_t *>(obj), value);
                });
                lv_anim_set_ready_cb(&anim, [](lv_anim_t *a) {
                    lv_obj_add_flag(static_cast<lv_obj_t *>(a->var), LV_OBJ_FLAG_HIDDEN);
                });
                lv_anim_start(&anim);
            }
        }
    }
}
