// Static usage guide. Kept in a focused module so the dashboard UI stays small.

static void showGuideScreenEvent(lv_event_t *)
{
    if (guideScreen) lv_scr_load(guideScreen);
}

static void createGuideScreen()
{
    guideScreen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(guideScreen, lv_color_hex(0x020C1B), 0);
    lv_obj_clear_flag(guideScreen, LV_OBJ_FLAG_SCROLLABLE);
    makeMainNav(guideScreen, 6);

    lv_obj_t *titleUse = lv_label_create(guideScreen);
    lv_label_set_text(titleUse, "使用");
    lv_obj_set_style_text_font(titleUse, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(titleUse, lv_color_white(), 0);
    lv_obj_align(titleUse, LV_ALIGN_TOP_LEFT, 35, 84);
    lv_obj_t *titleSay = lv_label_create(guideScreen);
    lv_label_set_text(titleSay, "说");
    lv_obj_set_style_text_font(titleSay, &ui_font_say_16, 0);
    lv_obj_set_style_text_color(titleSay, lv_color_white(), 0);
    lv_obj_align_to(titleSay, titleUse, LV_ALIGN_OUT_RIGHT_MID, 1, 0);
    lv_obj_t *titleMing = lv_label_create(guideScreen);
    lv_label_set_text(titleMing, "明");
    lv_obj_set_style_text_font(titleMing, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(titleMing, lv_color_white(), 0);
    lv_obj_align_to(titleMing, titleSay, LV_ALIGN_OUT_RIGHT_MID, 1, 0);

    lv_obj_t *card = lv_obj_create(guideScreen);
    lv_obj_set_size(card, 730, 320);
    lv_obj_align(card, LV_ALIGN_TOP_MID, 0, 125);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x0B2342), 0);
    lv_obj_set_style_border_width(card, 1, 0);
    lv_obj_set_style_border_color(card, lv_color_hex(0x287CC2), 0);
    lv_obj_set_style_radius(card, 18, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *text = lv_label_create(card);
    lv_label_set_text(text,
        "1. 首页: 查看当前时间, 日期, 天气, 和工作状态.\n"
        "2. 工作状态: 点击状态卡片, 选择上班, 开会或忙碌状态.\n"
        "3. 状态时间: 选择持续时间, 到期后自动恢复为上班状态.\n"
        "4. 待办: 新增, 完成或删除当天需要处理的事项.\n"
        "5. 日历: 查看日期和设置一些特殊的日子.\n"
        "6. 倒计时: 设置时, 分, 秒, 开始后可暂停或重置.\n"
        "7. 木鱼: 点击木鱼记录次数, 用于短时专注和休息.\n"
        "8. 手机设置: 手机和设备连接同一 Wi-Fi, 扫码进入设置.\n"
        "9. 喝水提醒: 设置开始, 结束和间隔时间, 到时弹窗提醒喝水. 需要在手机上设置.\n"
        "10. 久坐站立: 默认每 3 小时提醒一次, 可在手机设置时间和间隔.\n"
        "    与喝水同一分钟到点时优先提醒站立, 不会同时弹窗.\n"
        "11. 午休: 先手动切换到午休状态, 屏幕显示午休结束时间.\n"
        "12. 设备 IP: 也可在手机浏览器输入 IP 打开设置页面.");
    lv_obj_set_size(text, 675, 282);
    lv_label_set_long_mode(text, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(text, &ui_font_todo_cjk_16, 0);
    lv_obj_set_style_text_color(text, lv_color_hex(0xD7E9FF), 0);
    lv_obj_align(text, LV_ALIGN_TOP_LEFT, 18, 16);

    lv_obj_t *author = lv_label_create(guideScreen);
    lv_label_set_text(author, "作者 JunkMaker    Email: 1077964665@qq.com");
    lv_obj_set_style_text_font(author, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(author, lv_color_hex(0x7DD3FC), 0);
    lv_obj_align(author, LV_ALIGN_BOTTOM_MID, 0, -16);
}
