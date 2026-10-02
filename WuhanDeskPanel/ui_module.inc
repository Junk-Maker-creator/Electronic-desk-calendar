// LVGL screen construction and shared UI helpers
// Included by WuhanDeskPanel.ino; keep in the same sketch directory.

// Graphite console surfaces with restrained cyan indicators. Geometry stays owned
// by each screen; these helpers change presentation only.
static constexpr uint32_t UI_BG = 0x15181C;
static constexpr uint32_t UI_SURFACE = 0x21262C;
static constexpr uint32_t UI_EDGE = 0x394149;
static constexpr uint32_t UI_TEXT = 0xE5E9ED;
static constexpr uint32_t UI_MUTED = 0xA4ADB6;
static constexpr uint32_t UI_ACCENT = 0x87BDCE;

// A static inset indicator, deliberately non-interactive and inexpensive on RGB.
static lv_obj_t *makeConsoleIndicator(lv_obj_t *parent, int width, uint32_t color)
{
    lv_obj_t *indicator = lv_obj_create(parent);
    lv_obj_remove_style_all(indicator);
    lv_obj_set_size(indicator, width, 2);
    lv_obj_set_style_bg_opa(indicator, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(indicator, lv_color_hex(color), 0);
    lv_obj_set_style_radius(indicator, 1, 0);
    lv_obj_clear_flag(indicator, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    return indicator;
}

static void styleDeskCard(lv_obj_t *card, bool featured = false)
{
    lv_obj_set_style_bg_color(card, lv_color_hex(featured ? 0x242E37 : UI_SURFACE), 0);
    lv_obj_set_style_bg_grad_dir(card, LV_GRAD_DIR_NONE, 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(card, 1, 0);
    lv_obj_set_style_border_color(card, lv_color_hex(featured ? 0x506574 : UI_EDGE), 0);
    lv_obj_set_style_radius(card, 12, 0);
    lv_obj_set_style_shadow_width(card, 0, 0);
}

static void updateStatusUi(uint8_t index)
{
    currentWorkState = index;
    lv_label_set_text(statusLabel, workStates[index]);
    lv_obj_set_style_bg_color(statusLabel, lv_color_hex(0x293D49), 0);
    lv_obj_set_style_text_color(statusLabel, lv_color_hex(UI_ACCENT), 0);
    static const uint32_t accents[] = {
        0x87BDCE, 0xE4C18D, 0xC8B9DF, 0xA7CED0, 0xB2BAB6,
        0xDCB1BC, 0xA8CCAD, 0xD9BB98, 0xA6C9BE
    };
    for (uint8_t i = 0; i < 9; ++i) {
        lv_obj_t *button = statusButtons[i];
        if (button == nullptr) continue;
        const bool selected = i == index;
        lv_obj_set_style_bg_color(button, lv_color_hex(selected ? 0x293D49 : UI_SURFACE), 0);
        lv_obj_set_style_border_width(button, 1, 0);
        lv_obj_set_style_border_color(button, lv_color_hex(selected ? 0x668A9C : UI_EDGE), 0);
        lv_obj_set_style_opa(button, LV_OPA_COVER, 0);
        lv_obj_set_style_text_color(lv_obj_get_child(button, 0), lv_color_hex(selected ? UI_TEXT : UI_MUTED), 0);
        if (lv_obj_get_child_cnt(button) > 1)
            lv_obj_set_style_text_color(lv_obj_get_child(button, 1), lv_color_hex(selected ? accents[i] : UI_MUTED), 0);
        if (lv_obj_get_child_cnt(button) > 2)
            lv_obj_set_style_bg_color(lv_obj_get_child(button, 2), lv_color_hex(selected ? UI_ACCENT : 0x414B55), 0);
    }
}

static lv_obj_t *makeTitle(lv_obj_t *parent, const char *text)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0x0F172A), 0);
    return label;
}

static lv_obj_t *makeButton(lv_obj_t *parent, const char *text, lv_event_cb_t callback)
{
    lv_obj_t *button = lv_btn_create(parent);
    lv_obj_set_height(button, 54);
    lv_obj_set_style_radius(button, 10, 0);
    lv_obj_set_style_shadow_width(button, 0, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x354F60), 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x456476), LV_STATE_PRESSED);
    lv_obj_set_style_text_color(button, lv_color_hex(UI_TEXT), 0);
    lv_obj_set_style_border_width(button, 1, 0);
    lv_obj_set_style_border_color(button, lv_color_hex(0x536C7B), 0);
    lv_obj_set_style_transform_width(button, 0, LV_STATE_PRESSED);
    lv_obj_set_style_transform_height(button, 0, LV_STATE_PRESSED);
    lv_obj_add_event_cb(button, callback, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *label = lv_label_create(button);
    lv_label_set_text(label, text);
    lv_obj_center(label);
    return button;
}

static lv_obj_t *makeNavButton(lv_obj_t *parent, int x, int width, const char *icon,
                               const char *text, bool active, lv_event_cb_t callback)
{
    lv_obj_t *button = lv_btn_create(parent);
    lv_obj_set_size(button, width, 50);
    lv_obj_set_pos(button, x, 10);
    lv_obj_set_style_radius(button, 8, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(active ? 0x2C3741 : UI_BG), 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x354F60), LV_STATE_PRESSED);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_shadow_width(button, 0, 0);
    lv_obj_set_style_pad_all(button, 0, 0);
    if (callback != nullptr) lv_obj_add_event_cb(button, callback, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *iconLabel = lv_label_create(button);
    lv_label_set_text(iconLabel, icon);
    lv_obj_set_size(iconLabel, 28, 28);
    lv_obj_set_style_text_font(iconLabel, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(iconLabel, lv_color_hex(active ? UI_ACCENT : UI_MUTED), 0);
    lv_obj_set_pos(iconLabel, 9, 14);
    lv_obj_t *textLabel = lv_label_create(button);
    lv_label_set_text(textLabel, text);
    lv_obj_set_style_text_font(textLabel, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(textLabel, lv_color_hex(active ? UI_TEXT : UI_MUTED), 0);
    lv_obj_set_size(textLabel, width - 43, 24);
    lv_obj_set_pos(textLabel, 39, 14);
    lv_obj_set_style_text_align(textLabel, LV_TEXT_ALIGN_LEFT, 0);
    if (active) {
        lv_obj_t *indicator = makeConsoleIndicator(button, width - 28, UI_ACCENT);
        lv_obj_align(indicator, LV_ALIGN_BOTTOM_MID, 0, -3);
    }
    return button;
}

static void makeMainNav(lv_obj_t *parent, int active)
{
    lv_obj_t *divider = makeConsoleIndicator(parent, 768, 0x2E353D);
    lv_obj_set_height(divider, 1);
    lv_obj_set_pos(divider, 16, 65);
    makeNavButton(parent, 8, 86, LV_SYMBOL_HOME, "主页", active == 0, active == 0 ? nullptr : showDashboardEvent);
    makeNavButton(parent, 100, 94, LV_SYMBOL_EDIT, "日历", active == 1, active == 1 ? nullptr : showCalendarScreenEvent);
    makeNavButton(parent, 200, 90, LV_SYMBOL_WIFI, "Wi-Fi", active == 2, active == 2 ? nullptr : showWifiScreenEvent);
    makeNavButton(parent, 296, 118, LV_SYMBOL_IMAGE, "天气预报", active == 3, active == 3 ? nullptr : showForecastScreenEvent);
    makeNavButton(parent, 420, 108, LV_SYMBOL_PLAY, "倒计时", active == 5, active == 5 ? nullptr : showCountdownScreenEvent);
    makeNavButton(parent, 534, 118, LV_SYMBOL_AUDIO, "电子木鱼", active == 4, active == 4 ? nullptr : showMuyuScreenEvent);
    makeNavButton(parent, 660, 132, LV_SYMBOL_SETTINGS, "使用指南", active == 6, active == 6 ? nullptr : showGuideScreenEvent);
}

static void createUi()
{
    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x08111F), 0);
    lv_obj_set_style_bg_grad_color(screen, lv_color_hex(0x172554), 0);
    lv_obj_set_style_bg_grad_dir(screen, LV_GRAD_DIR_HOR, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *brand = lv_label_create(screen);
    lv_label_set_text(brand, "WUHAN DESK");
    lv_obj_set_style_text_font(brand, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(brand, lv_color_hex(0x93C5FD), 0);
    lv_obj_align(brand, LV_ALIGN_TOP_LEFT, 24, 18);

    wifiLabel = lv_label_create(screen);
    lv_label_set_text(wifiLabel, "Wi-Fi: not configured");
    lv_obj_set_style_text_color(wifiLabel, lv_color_hex(0xCBD5E1), 0);
    lv_obj_align(wifiLabel, LV_ALIGN_TOP_RIGHT, -128, 20);
    lv_obj_t *wifiButton = makeButton(screen, "WI-FI", openWifiEvent);
    lv_obj_set_size(wifiButton, 96, 42);
    lv_obj_align(wifiButton, LV_ALIGN_TOP_RIGHT, -20, 9);

    lv_obj_t *clockCard = lv_obj_create(screen);
    lv_obj_set_size(clockCard, 350, 205);
    lv_obj_align(clockCard, LV_ALIGN_TOP_LEFT, 20, 66);
    lv_obj_set_style_bg_color(clockCard, lv_color_hex(0x111C32), 0);
    lv_obj_set_style_bg_opa(clockCard, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(clockCard, 1, 0);
    lv_obj_set_style_border_color(clockCard, lv_color_hex(0x334155), 0);
    lv_obj_set_style_radius(clockCard, 22, 0);
    lv_obj_clear_flag(clockCard, LV_OBJ_FLAG_SCROLLABLE);

    timeLabel = lv_label_create(clockCard);
    lv_label_set_text(timeLabel, "--:--");
    lv_obj_set_style_text_font(timeLabel, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(timeLabel, lv_color_white(), 0);
    lv_obj_align(timeLabel, LV_ALIGN_CENTER, 0, -28);
    dateLabel = lv_label_create(clockCard);
    lv_label_set_text(dateLabel, "Waiting for network time");
    lv_obj_set_style_text_font(dateLabel, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(dateLabel, lv_color_hex(0x94A3B8), 0);
    lv_obj_align(dateLabel, LV_ALIGN_CENTER, 0, 42);

    lv_obj_t *weatherCard = lv_obj_create(screen);
    lv_obj_set_size(weatherCard, 390, 205);
    lv_obj_align(weatherCard, LV_ALIGN_TOP_RIGHT, -20, 66);
    lv_obj_set_style_bg_color(weatherCard, lv_color_hex(0x102A43), 0);
    lv_obj_set_style_bg_opa(weatherCard, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(weatherCard, 1, 0);
    lv_obj_set_style_border_color(weatherCard, lv_color_hex(0x2563EB), 0);
    lv_obj_set_style_radius(weatherCard, 22, 0);
    lv_obj_clear_flag(weatherCard, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *weatherTitle = lv_label_create(weatherCard);
    lv_label_set_text(weatherTitle, "WUHAN WEATHER");
    lv_obj_set_style_text_color(weatherTitle, lv_color_hex(0x93C5FD), 0);
    lv_obj_align(weatherTitle, LV_ALIGN_TOP_LEFT, 4, 0);
    weatherTempLabel = lv_label_create(weatherCard);
    lv_label_set_text(weatherTempLabel, "--.- C");
    lv_obj_set_style_text_font(weatherTempLabel, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(weatherTempLabel, lv_color_white(), 0);
    lv_obj_align(weatherTempLabel, LV_ALIGN_LEFT_MID, 4, -18);
    weatherDetailLabel = lv_label_create(weatherCard);
    lv_label_set_text(weatherDetailLabel, "Waiting for network...");
    lv_obj_set_width(weatherDetailLabel, 345);
    lv_label_set_long_mode(weatherDetailLabel, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_color(weatherDetailLabel, lv_color_hex(0xCBD5E1), 0);
    lv_obj_align(weatherDetailLabel, LV_ALIGN_BOTTOM_LEFT, 4, -30);
    weatherUpdateLabel = lv_label_create(weatherCard);
    lv_label_set_text(weatherUpdateLabel, "Not updated");
    lv_obj_set_style_text_color(weatherUpdateLabel, lv_color_hex(0x64748B), 0);
    lv_obj_align(weatherUpdateLabel, LV_ALIGN_BOTTOM_LEFT, 4, 0);
    lv_obj_t *refresh = makeButton(weatherCard, "UPDATE", refreshWeatherEvent);
    lv_obj_set_size(refresh, 92, 38);
    lv_obj_align(refresh, LV_ALIGN_TOP_RIGHT, 0, -8);

    lv_obj_t *statusCard = lv_obj_create(screen);
    lv_obj_set_size(statusCard, 760, 168);
    lv_obj_align(statusCard, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_style_bg_color(statusCard, lv_color_hex(0x111827), 0);
    lv_obj_set_style_border_width(statusCard, 1, 0);
    lv_obj_set_style_border_color(statusCard, lv_color_hex(0x334155), 0);
    lv_obj_set_style_radius(statusCard, 22, 0);
    lv_obj_clear_flag(statusCard, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *workTitle = lv_label_create(statusCard);
    lv_label_set_text(workTitle, "CURRENT WORK STATUS");
    lv_obj_set_style_text_color(workTitle, lv_color_hex(0x94A3B8), 0);
    lv_obj_align(workTitle, LV_ALIGN_TOP_LEFT, 4, 0);
    currentWorkState = prefs.getUChar("state", 0);
    if (currentWorkState > 4) currentWorkState = 0;
    statusLabel = lv_label_create(statusCard);
    lv_label_set_text(statusLabel, workStates[currentWorkState]);
    lv_obj_set_style_text_font(statusLabel, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(statusLabel, lv_color_white(), 0);
    lv_obj_set_style_bg_color(statusLabel, lv_color_hex(stateColors[currentWorkState]), 0);
    lv_obj_set_style_bg_opa(statusLabel, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(statusLabel, 12, 0);
    lv_obj_set_style_pad_all(statusLabel, 10, 0);
    lv_obj_align(statusLabel, LV_ALIGN_TOP_RIGHT, -4, -7);
    lv_obj_t *row = lv_obj_create(statusCard);
    lv_obj_set_size(row, 720, 76);
    lv_obj_align(row, LV_ALIGN_BOTTOM_MID, 0, -2);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_pad_all(row, 2, 0);
    for (intptr_t i = 0; i < 5; i++) {
        lv_obj_t *button = makeButton(row, workStates[i], statusEvent);
        lv_obj_set_width(button, 132);
        lv_obj_set_height(button, 54);
        lv_obj_set_style_bg_color(button, lv_color_hex(stateColors[i]), 0);
        lv_obj_remove_event_cb(button, statusEvent);
        lv_obj_add_event_cb(button, statusEvent, LV_EVENT_CLICKED, reinterpret_cast<void *>(i));
        statusButtons[i] = button;
    }
    updateStatusUi(currentWorkState);

    wifiPanel = lv_obj_create(screen);
    lv_obj_set_size(wifiPanel, 760, 390);
    lv_obj_center(wifiPanel);
    lv_obj_set_style_bg_color(wifiPanel, lv_color_hex(0xF8FAFC), 0);
    lv_obj_set_style_bg_opa(wifiPanel, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(wifiPanel, 22, 0);
    lv_obj_clear_flag(wifiPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *wifiTitle = makeTitle(wifiPanel, "Wi-Fi setup");
    lv_obj_align(wifiTitle, LV_ALIGN_TOP_LEFT, 12, 5);
    ssidArea = lv_textarea_create(wifiPanel);
    lv_textarea_set_one_line(ssidArea, true);
    lv_textarea_set_placeholder_text(ssidArea, "Wi-Fi name (SSID)");
    lv_textarea_set_text(ssidArea, savedSsid.c_str());
    lv_obj_set_size(ssidArea, 500, 55);
    lv_obj_align(ssidArea, LV_ALIGN_TOP_LEFT, 12, 62);
    lv_obj_add_event_cb(ssidArea, textAreaEvent, LV_EVENT_FOCUSED, nullptr);
    passwordArea = lv_textarea_create(wifiPanel);
    lv_textarea_set_one_line(passwordArea, true);
    lv_textarea_set_password_mode(passwordArea, true);
    lv_textarea_set_placeholder_text(passwordArea, "Wi-Fi password");
    lv_textarea_set_text(passwordArea, savedPassword.c_str());
    lv_obj_set_size(passwordArea, 500, 55);
    lv_obj_align(passwordArea, LV_ALIGN_TOP_LEFT, 12, 132);
    lv_obj_add_event_cb(passwordArea, textAreaEvent, LV_EVENT_FOCUSED, nullptr);
    lv_obj_t *connect = makeButton(wifiPanel, "SAVE & CONNECT", connectEvent);
    lv_obj_set_size(connect, 210, 55);
    lv_obj_align(connect, LV_ALIGN_TOP_RIGHT, -12, 62);
    lv_obj_t *forget = makeButton(wifiPanel, "FORGET WI-FI", forgetEvent);
    lv_obj_set_size(forget, 210, 55);
    lv_obj_align(forget, LV_ALIGN_TOP_RIGHT, -12, 132);
    lv_obj_set_style_bg_color(forget, lv_color_hex(0xDC2626), 0);
    lv_obj_t *close = makeButton(wifiPanel, "CLOSE", closeWifiEvent);
    lv_obj_set_size(close, 110, 44);
    lv_obj_align(close, LV_ALIGN_TOP_RIGHT, -12, 6);

    keyboard = lv_keyboard_create(screen);
    lv_obj_set_size(keyboard, 800, 260);
    lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_event_cb(keyboard, keyboardEvent, LV_EVENT_ALL, nullptr);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);

    toastLabel = lv_label_create(screen);
    lv_obj_set_style_bg_color(toastLabel, lv_color_hex(0x0F172A), 0);
    lv_obj_set_style_bg_opa(toastLabel, LV_OPA_90, 0);
    lv_obj_set_style_text_color(toastLabel, lv_color_white(), 0);
    lv_obj_set_style_pad_all(toastLabel, 14, 0);
    lv_obj_set_style_radius(toastLabel, 12, 0);
    lv_obj_align(toastLabel, LV_ALIGN_BOTTOM_MID, 0, -18);
    lv_obj_add_flag(toastLabel, LV_OBJ_FLAG_HIDDEN);

    if (!savedSsid.isEmpty()) lv_obj_add_flag(wifiPanel, LV_OBJ_FLAG_HIDDEN);
}

// Conservative recovery UI: keep the object tree shallow and use only the
// LVGL primitives that were verified on this RGB panel.
static void createRecoveryUi()
{
    lv_obj_t *screen = lv_scr_act();
    dashboardScreen = screen;
    lv_obj_clean(screen);
    lv_obj_set_style_bg_color(screen, lv_color_hex(UI_BG), 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

    makeMainNav(screen, 0);

    wifiLabel = lv_label_create(screen);
    lv_label_set_text(wifiLabel, LV_SYMBOL_WIFI);
    lv_obj_set_width(wifiLabel, 38);
    lv_obj_set_style_text_align(wifiLabel, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_style_text_font(wifiLabel, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(wifiLabel, lv_color_hex(0x64748B), 0);

    lv_obj_t *clockCard = lv_obj_create(screen);
    lv_obj_set_size(clockCard, 330, 215);
    lv_obj_align(clockCard, LV_ALIGN_TOP_LEFT, 16, 72);
    lv_obj_set_style_bg_color(clockCard, lv_color_hex(UI_SURFACE), 0);
    lv_obj_set_style_border_width(clockCard, 1, 0);
    lv_obj_set_style_border_color(clockCard, lv_color_hex(UI_EDGE), 0);
    lv_obj_set_style_radius(clockCard, 18, 0);
    lv_obj_clear_flag(clockCard, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(clockCard, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(clockCard, showCalendarScreenEvent, LV_EVENT_CLICKED, nullptr);
    styleDeskCard(clockCard);
    lv_obj_t *clockRule = makeConsoleIndicator(clockCard, 288, 0x394149);
    lv_obj_set_pos(clockRule, 4, 28);
    lv_obj_set_parent(wifiLabel, clockCard);
    lv_obj_align(wifiLabel, LV_ALIGN_TOP_RIGHT, -4, 0);
    lv_obj_t *clockTitle = lv_label_create(clockCard);
    lv_label_set_text(clockTitle, "时间");
    lv_obj_set_style_text_font(clockTitle, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(clockTitle, lv_color_hex(UI_TEXT), 0);
    lv_obj_align(clockTitle, LV_ALIGN_TOP_LEFT, 4, 0);
    timeLabel = lv_label_create(clockCard);
    lv_label_set_text(timeLabel, "--:--:--");
    lv_obj_set_style_text_font(timeLabel, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(timeLabel, lv_color_hex(UI_TEXT), 0);
    lv_obj_align(timeLabel, LV_ALIGN_CENTER, 0, -25);
    lv_obj_add_flag(timeLabel, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(timeLabel, showCalendarScreenEvent, LV_EVENT_CLICKED, nullptr);
    dateLabel = lv_label_create(clockCard);
    lv_label_set_text(dateLabel, "等待网络校时");
    lv_obj_set_style_text_font(dateLabel, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(dateLabel, lv_color_hex(UI_MUTED), 0);
    lv_obj_align(dateLabel, LV_ALIGN_CENTER, 0, 35);
    todaySpecialLabel = lv_label_create(clockCard);
    lv_label_set_text(todaySpecialLabel, "今日：普通工作日");
    lv_obj_set_width(todaySpecialLabel, 292);
    lv_obj_set_style_text_align(todaySpecialLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(todaySpecialLabel, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(todaySpecialLabel, lv_color_hex(UI_ACCENT), 0);
    lv_obj_align(todaySpecialLabel, LV_ALIGN_BOTTOM_MID, 0, -8);

    lv_obj_t *weatherCard = lv_obj_create(screen);
    lv_obj_set_size(weatherCard, 430, 215);
    lv_obj_align(weatherCard, LV_ALIGN_TOP_RIGHT, -16, 72);
    lv_obj_set_style_bg_color(weatherCard, lv_color_hex(0x242E37), 0);
    lv_obj_set_style_border_width(weatherCard, 1, 0);
    lv_obj_set_style_border_color(weatherCard, lv_color_hex(UI_EDGE), 0);
    lv_obj_set_style_radius(weatherCard, 18, 0);
    lv_obj_clear_flag(weatherCard, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(weatherCard, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(weatherCard, showForecastScreenEvent, LV_EVENT_CLICKED, nullptr);
    styleDeskCard(weatherCard, true);
    lv_obj_t *weatherRule = makeConsoleIndicator(weatherCard, 390, 0x465663);
    lv_obj_set_height(weatherRule, 1);
    lv_obj_set_pos(weatherRule, 4, 30);
    lv_obj_t *weatherTitle = lv_label_create(weatherCard);
    lv_label_set_text(weatherTitle, "武汉天气");
    lv_obj_set_style_text_font(weatherTitle, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(weatherTitle, lv_color_hex(UI_TEXT), 0);
    lv_obj_align(weatherTitle, LV_ALIGN_TOP_LEFT, 4, 0);
    homeWeatherIconLabel = lv_label_create(weatherCard);
    lv_label_set_text(homeWeatherIconLabel, "☀");
    lv_obj_set_style_text_font(homeWeatherIconLabel, &ui_font_weather_64, 0);
    lv_obj_set_style_text_color(homeWeatherIconLabel, lv_color_hex(0xD9B77B), 0);
    lv_obj_align(homeWeatherIconLabel, LV_ALIGN_LEFT_MID, 30, -2);
    homeWeatherCityLabel = lv_label_create(weatherCard);
    lv_label_set_text(homeWeatherCityLabel, weatherCityName);
    lv_obj_set_style_text_font(homeWeatherCityLabel, &ui_font_cn_24, 0);
    lv_obj_set_style_text_color(homeWeatherCityLabel, lv_color_hex(UI_TEXT), 0);
    lv_obj_align(homeWeatherCityLabel, LV_ALIGN_TOP_RIGHT, -4, 0);
    weatherTempLabel = lv_label_create(weatherCard);
    lv_label_set_text(weatherTempLabel, "--.- C");
    lv_obj_set_style_text_font(weatherTempLabel, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(weatherTempLabel, lv_color_hex(UI_TEXT), 0);
    lv_obj_align(weatherTempLabel, LV_ALIGN_TOP_LEFT, 148, 32);
    weatherDetailLabel = lv_label_create(weatherCard);
    lv_label_set_text(weatherDetailLabel, "等待 Wi-Fi 连接");
    lv_obj_set_style_text_font(weatherDetailLabel, &ui_font_cn_16, 0);
    lv_obj_set_width(weatherDetailLabel, 255);
    lv_label_set_long_mode(weatherDetailLabel, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_color(weatherDetailLabel, lv_color_hex(UI_TEXT), 0);
    lv_obj_set_size(weatherDetailLabel, 245, 40);
    lv_obj_align(weatherDetailLabel, LV_ALIGN_TOP_LEFT, 150, 88);
    weatherUpdateLabel = lv_label_create(weatherCard);
    lv_label_set_text(weatherUpdateLabel, "天气尚未更新");
    lv_obj_set_style_text_font(weatherUpdateLabel, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(weatherUpdateLabel, lv_color_hex(UI_MUTED), 0);
    lv_obj_set_size(weatherUpdateLabel, 245, 24);
    lv_label_set_long_mode(weatherUpdateLabel, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_align(weatherUpdateLabel, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_align(weatherUpdateLabel, LV_ALIGN_TOP_LEFT, 150, 130);
    homeWeatherReminderLabel = lv_label_create(weatherCard);
    lv_label_set_text(homeWeatherReminderLabel, "未来天气加载中");
    lv_obj_set_size(homeWeatherReminderLabel, 390, 26);
    lv_label_set_long_mode(homeWeatherReminderLabel, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_align(homeWeatherReminderLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(homeWeatherReminderLabel, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(homeWeatherReminderLabel, lv_color_hex(0xDDC69D), 0);
    lv_obj_set_style_bg_color(homeWeatherReminderLabel, lv_color_hex(0x39342B), 0);
    lv_obj_set_style_bg_opa(homeWeatherReminderLabel, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(homeWeatherReminderLabel, 8, 0);
    lv_obj_set_style_pad_ver(homeWeatherReminderLabel, 3, 0);
    lv_obj_align(homeWeatherReminderLabel, LV_ALIGN_BOTTOM_MID, 0, -1);

    currentWorkState = prefs.getUChar("state", 0);
    if (currentWorkState > 8) currentWorkState = 0;
    statusLabel = lv_label_create(screen);
    lv_label_set_text(statusLabel, workStates[currentWorkState]);
    lv_obj_set_style_text_color(statusLabel, lv_color_hex(UI_TEXT), 0);
    lv_obj_set_style_text_font(statusLabel, &ui_font_cn_16, 0);
    lv_obj_set_style_bg_color(statusLabel, lv_color_hex(stateColors[currentWorkState]), 0);
    lv_obj_set_style_bg_opa(statusLabel, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(statusLabel, 10, 0);
    lv_obj_set_style_radius(statusLabel, 10, 0);
    lv_obj_set_size(statusLabel, 86, 42);
    lv_obj_set_style_text_align(statusLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(statusLabel, LV_ALIGN_BOTTOM_RIGHT, -22, -151);
    lv_obj_t *statusTitle = lv_label_create(screen);
    lv_label_set_text(statusTitle, "工作状态");
    lv_obj_set_style_text_font(statusTitle, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(statusTitle, lv_color_hex(UI_TEXT), 0);
    lv_obj_align(statusTitle, LV_ALIGN_BOTTOM_LEFT, 24, -151);

    lv_obj_t *row = lv_obj_create(screen);
    lv_obj_set_size(row, 768, 105);
    lv_obj_align(row, LV_ALIGN_BOTTOM_MID, 0, -12);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_pad_all(row, 4, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    static const char *stateIcons[] = {LV_SYMBOL_HOME, LV_SYMBOL_CHARGE, LV_SYMBOL_LIST, LV_SYMBOL_PAUSE,
                                       LV_SYMBOL_POWER, LV_SYMBOL_GPS, LV_SYMBOL_BELL, LV_SYMBOL_STOP,
                                       LV_SYMBOL_REFRESH};
    lv_obj_set_scroll_dir(row, LV_DIR_HOR);
    lv_obj_set_scrollbar_mode(row, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(row, 6, 0);
    for (intptr_t i = 0; i < 9; ++i) {
        lv_obj_t *button = makeButton(row, workStates[i], statusEvent);
        lv_obj_set_size(button, 79, 88);
        lv_obj_set_style_pad_all(button, 4, 0);
        lv_obj_set_style_bg_color(button, lv_color_hex(stateColors[i]), 0);
        lv_obj_remove_event_cb(button, statusEvent);
        lv_obj_add_event_cb(button, statusEvent, LV_EVENT_CLICKED, reinterpret_cast<void *>(i));
        statusButtons[i] = button;
        lv_obj_t *buttonText = lv_obj_get_child(button, 0);
        lv_obj_set_style_text_font(buttonText, &ui_font_cn_16, 0);
        lv_obj_align(buttonText, LV_ALIGN_BOTTOM_MID, 0, -6);
        lv_obj_t *stateIcon = lv_label_create(button);
        lv_label_set_text(stateIcon, stateIcons[i]);
        lv_obj_set_style_text_font(stateIcon, &lv_font_montserrat_28, 0);
        lv_obj_set_style_text_color(stateIcon, lv_color_hex(UI_TEXT), 0);
        lv_obj_align(stateIcon, LV_ALIGN_TOP_MID, 0, 7);
        lv_obj_t *stateIndicator = makeConsoleIndicator(button, 25, 0x414B55);
        lv_obj_align(stateIndicator, LV_ALIGN_TOP_MID, 0, -1);
    }

    lv_obj_t *homeStatusCard = lv_obj_create(screen);
    lv_obj_set_size(homeStatusCard, 245, 175);
    lv_obj_align(homeStatusCard, LV_ALIGN_TOP_RIGHT, -16, 72);
    lv_obj_set_style_bg_color(homeStatusCard, lv_color_hex(UI_SURFACE), 0);
    lv_obj_set_style_border_color(homeStatusCard, lv_color_hex(UI_EDGE), 0);
    lv_obj_set_style_radius(homeStatusCard, 18, 0);
    lv_obj_clear_flag(homeStatusCard, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_parent(statusTitle, homeStatusCard);
    lv_obj_align(statusTitle, LV_ALIGN_TOP_LEFT, 4, 2);
    lv_obj_set_parent(statusLabel, homeStatusCard);
    lv_obj_set_size(statusLabel, 190, 56);
    lv_obj_align(statusLabel, LV_ALIGN_CENTER, 0, 5);
    lv_obj_add_flag(statusLabel, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(statusLabel, openStatusPickerEvent, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *statusHint = lv_label_create(homeStatusCard);
    lv_label_set_text(statusHint, "点击切换状态");
    lv_obj_set_style_text_font(statusHint, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(statusHint, lv_color_hex(UI_ACCENT), 0);
    lv_obj_align(statusHint, LV_ALIGN_BOTTOM_MID, 0, -2);

    homeTodoCard = lv_obj_create(screen);
    lv_obj_set_size(homeTodoCard, 245, 189);
    lv_obj_align(homeTodoCard, LV_ALIGN_BOTTOM_RIGHT, -16, -12);
    lv_obj_set_style_bg_color(homeTodoCard, lv_color_hex(UI_SURFACE), 0);
    lv_obj_set_style_border_color(homeTodoCard, lv_color_hex(UI_EDGE), 0);
    lv_obj_set_style_radius(homeTodoCard, 18, 0);
    lv_obj_clear_flag(homeTodoCard, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(homeTodoCard, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(homeTodoCard, homeTodoCardEvent, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *previewTitle = lv_label_create(homeTodoCard);
    lv_label_set_text(previewTitle, "最近待办");
    lv_obj_set_style_text_font(previewTitle, &ui_font_cn_24, 0);
    lv_obj_set_style_text_color(previewTitle, lv_color_hex(UI_TEXT), 0);
    lv_obj_align(previewTitle, LV_ALIGN_TOP_LEFT, 4, 2);
    homeTodoPreviewLabel = lv_label_create(homeTodoCard);
    lv_label_set_text(homeTodoPreviewLabel, "暂无待办事项");
    lv_obj_set_width(homeTodoPreviewLabel, 205);
    lv_label_set_long_mode(homeTodoPreviewLabel, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(homeTodoPreviewLabel, &ui_font_todo_cjk_16, 0);
    lv_obj_set_style_text_color(homeTodoPreviewLabel, lv_color_hex(UI_TEXT), 0);
    lv_obj_align(homeTodoPreviewLabel, LV_ALIGN_TOP_LEFT, 4, 48);
    homeTodoPauseLabel = lv_label_create(homeTodoCard);
    lv_label_set_text(homeTodoPauseLabel, "自动轮播 · 点击暂停");
    lv_obj_set_style_text_font(homeTodoPauseLabel, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(homeTodoPauseLabel, lv_color_hex(UI_MUTED), 0);
    lv_obj_align(homeTodoPauseLabel, LV_ALIGN_BOTTOM_MID, 0, -2);

    statusPickerPanel = lv_obj_create(screen);
    lv_obj_set_size(statusPickerPanel, 680, 225);
    lv_obj_center(statusPickerPanel);
    lv_obj_set_style_bg_color(statusPickerPanel, lv_color_hex(UI_SURFACE), 0);
    lv_obj_set_style_border_width(statusPickerPanel, 2, 0);
    lv_obj_set_style_border_color(statusPickerPanel, lv_color_hex(0x668A9C), 0);
    lv_obj_set_style_radius(statusPickerPanel, 20, 0);
    lv_obj_clear_flag(statusPickerPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *pickerHeader = lv_label_create(statusPickerPanel);
    lv_label_set_text(pickerHeader, "滑动选择工作状态");
    lv_obj_set_style_text_font(pickerHeader, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(pickerHeader, lv_color_hex(UI_TEXT), 0);
    lv_obj_align(pickerHeader, LV_ALIGN_TOP_LEFT, 8, 4);
    lv_obj_set_parent(row, statusPickerPanel);
    lv_obj_set_size(row, 640, 112);
    lv_obj_align(row, LV_ALIGN_TOP_MID, 0, 48);
    lv_obj_t *statusClose = makeButton(statusPickerPanel, "关闭", closeStatusPickerEvent);
    lv_obj_set_size(statusClose, 100, 40);
    lv_obj_align(statusClose, LV_ALIGN_BOTTOM_RIGHT, -4, -2);
    lv_obj_set_style_bg_color(statusClose, lv_color_hex(0x394149), 0);
    lv_obj_set_style_text_font(lv_obj_get_child(statusClose, 0), &ui_font_cn_16, 0);
    lv_obj_add_flag(statusPickerPanel, LV_OBJ_FLAG_HIDDEN);

    // Restore the dashboard's full-width work-state section.
    lv_obj_set_parent(statusTitle, screen);
    lv_obj_align(statusTitle, LV_ALIGN_BOTTOM_LEFT, 24, -151);
    lv_obj_set_parent(statusLabel, screen);
    lv_obj_set_size(statusLabel, 100, 42);
    lv_obj_align_to(statusLabel, statusTitle, LV_ALIGN_OUT_RIGHT_MID, 14, 0);
    lv_obj_remove_event_cb(statusLabel, openStatusPickerEvent);
    lv_obj_clear_flag(statusLabel, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_parent(row, screen);
    lv_obj_set_size(row, 768, 105);
    lv_obj_align(row, LV_ALIGN_BOTTOM_MID, 0, -12);
    lv_obj_add_flag(homeStatusCard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_parent(homeTodoPreviewLabel, screen);
    lv_obj_set_size(homeTodoPreviewLabel, 500, 44);
    lv_obj_set_style_bg_color(homeTodoPreviewLabel, lv_color_hex(UI_SURFACE), 0);
    lv_obj_set_style_bg_opa(homeTodoPreviewLabel, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(homeTodoPreviewLabel, 10, 0);
    lv_obj_set_style_pad_all(homeTodoPreviewLabel, 12, 0);
    lv_label_set_long_mode(homeTodoPreviewLabel, LV_LABEL_LONG_DOT);
    lv_obj_set_pos(homeTodoPreviewLabel, 276, 300);
    lv_obj_add_flag(homeTodoPreviewLabel, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(homeTodoPreviewLabel, homeTodoCardEvent, LV_EVENT_CLICKED, nullptr);
    lv_obj_add_flag(homeTodoCard, LV_OBJ_FLAG_HIDDEN);

    durationPanel = lv_obj_create(screen);
    lv_obj_set_size(durationPanel, 610, 285);
    lv_obj_center(durationPanel);
    lv_obj_set_style_bg_color(durationPanel, lv_color_hex(UI_SURFACE), 0);
    lv_obj_set_style_bg_opa(durationPanel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(durationPanel, 2, 0);
    lv_obj_set_style_border_color(durationPanel, lv_color_hex(0x668A9C), 0);
    lv_obj_set_style_radius(durationPanel, 22, 0);
    lv_obj_set_style_shadow_width(durationPanel, 0, 0);
    lv_obj_set_style_shadow_color(durationPanel, lv_color_black(), 0);
    lv_obj_set_style_shadow_opa(durationPanel, LV_OPA_70, 0);
    lv_obj_clear_flag(durationPanel, LV_OBJ_FLAG_SCROLLABLE);
    durationTitleLabel = lv_label_create(durationPanel);
    lv_label_set_text(durationTitleLabel, "选择持续时间");
    lv_obj_set_style_text_font(durationTitleLabel, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(durationTitleLabel, lv_color_hex(UI_TEXT), 0);
    lv_obj_align(durationTitleLabel, LV_ALIGN_TOP_LEFT, 12, 5);
    lv_obj_t *durationHint = lv_label_create(durationPanel);
    lv_label_set_text(durationHint, "到期后自动恢复上班状态");
    lv_obj_set_style_text_font(durationHint, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(durationHint, lv_color_hex(UI_MUTED), 0);
    lv_obj_align(durationHint, LV_ALIGN_TOP_LEFT, 12, 42);
    static const char *durationNames[] = {"30分钟", "1小时", "1.5小时", "2小时"};
    static const uint32_t durationSeconds[] = {1800, 3600, 5400, 7200};
    for (int i = 0; i < 4; ++i) {
        lv_obj_t *durationButton = makeButton(durationPanel, durationNames[i], durationEvent);
        lv_obj_set_size(durationButton, 130, 88);
        lv_obj_set_pos(durationButton, 12 + i * 143, 88);
        lv_obj_set_style_bg_color(durationButton, lv_color_hex(0x354F60), 0);
        lv_obj_set_style_border_width(durationButton, 1, 0);
        lv_obj_set_style_border_color(durationButton, lv_color_hex(0x668A9C), 0);
        lv_obj_set_style_text_font(lv_obj_get_child(durationButton, 0), &ui_font_cn_16, 0);
        lv_obj_remove_event_cb(durationButton, durationEvent);
        lv_obj_add_event_cb(durationButton, durationEvent, LV_EVENT_CLICKED,
                            reinterpret_cast<void *>((uintptr_t)durationSeconds[i]));
    }
    lv_obj_t *durationClose = makeButton(durationPanel, "取消", closeDurationEvent);
    lv_obj_set_size(durationClose, 125, 45);
    lv_obj_align(durationClose, LV_ALIGN_BOTTOM_RIGHT, -10, -8);
    lv_obj_set_style_bg_color(durationClose, lv_color_hex(0x394149), 0);
    lv_obj_set_style_text_font(lv_obj_get_child(durationClose, 0), &ui_font_cn_16, 0);
    lv_obj_add_flag(durationPanel, LV_OBJ_FLAG_HIDDEN);

    dayAdjustPanel = lv_obj_create(screen);
    lv_obj_set_size(dayAdjustPanel, 470, 270);
    lv_obj_center(dayAdjustPanel);
    lv_obj_set_style_bg_color(dayAdjustPanel, lv_color_hex(UI_SURFACE), 0);
    lv_obj_set_style_border_width(dayAdjustPanel, 2, 0);
    lv_obj_set_style_border_color(dayAdjustPanel, lv_color_hex(0x668A9C), 0);
    lv_obj_set_style_radius(dayAdjustPanel, 22, 0);
    lv_obj_clear_flag(dayAdjustPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *dayTitle = lv_label_create(dayAdjustPanel);
    lv_label_set_text(dayTitle, "出差 / 休假天数");
    lv_obj_set_style_text_font(dayTitle, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(dayTitle, lv_color_hex(UI_TEXT), 0);
    lv_obj_align(dayTitle, LV_ALIGN_TOP_MID, 0, 4);
    lv_obj_t *dayMinus = makeButton(dayAdjustPanel, "-", dayChangeEvent);
    lv_obj_set_size(dayMinus, 92, 76);
    lv_obj_align(dayMinus, LV_ALIGN_LEFT_MID, 20, -10);
    lv_obj_remove_event_cb(dayMinus, dayChangeEvent);
    lv_obj_add_event_cb(dayMinus, dayChangeEvent, LV_EVENT_CLICKED, reinterpret_cast<void *>(-1));
    dayCountLabel = lv_label_create(dayAdjustPanel);
    lv_label_set_text(dayCountLabel, "1 天");
    lv_obj_set_style_text_font(dayCountLabel, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(dayCountLabel, lv_color_hex(UI_ACCENT), 0);
    lv_obj_center(dayCountLabel);
    lv_obj_t *dayPlus = makeButton(dayAdjustPanel, "+", dayChangeEvent);
    lv_obj_set_size(dayPlus, 92, 76);
    lv_obj_align(dayPlus, LV_ALIGN_RIGHT_MID, -20, -10);
    lv_obj_remove_event_cb(dayPlus, dayChangeEvent);
    lv_obj_add_event_cb(dayPlus, dayChangeEvent, LV_EVENT_CLICKED, reinterpret_cast<void *>(1));
    lv_obj_t *dayOk = makeButton(dayAdjustPanel, "确定", confirmDaysEvent);
    lv_obj_set_size(dayOk, 125, 44);
    lv_obj_align(dayOk, LV_ALIGN_BOTTOM_LEFT, 55, -8);
    lv_obj_set_style_text_font(lv_obj_get_child(dayOk, 0), &ui_font_cn_16, 0);
    lv_obj_t *dayCancel = makeButton(dayAdjustPanel, "取消", closeDaysEvent);
    lv_obj_set_size(dayCancel, 125, 44);
    lv_obj_align(dayCancel, LV_ALIGN_BOTTOM_RIGHT, -55, -8);
    lv_obj_set_style_text_font(lv_obj_get_child(dayCancel, 0), &ui_font_cn_16, 0);
    lv_obj_add_flag(dayAdjustPanel, LV_OBJ_FLAG_HIDDEN);

    statusUntilLabel = lv_label_create(screen);
    lv_label_set_text(statusUntilLabel, "");
    lv_obj_set_style_text_font(statusUntilLabel, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(statusUntilLabel, lv_color_hex(UI_ACCENT), 0);
    lv_obj_set_size(statusUntilLabel, 752, 24);
    lv_obj_set_style_text_align(statusUntilLabel, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_pos(statusUntilLabel, 24, 345);

    toastLabel = lv_label_create(screen);
    lv_obj_set_style_bg_color(toastLabel, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(toastLabel, LV_OPA_90, 0);
    lv_obj_set_style_text_color(toastLabel, lv_color_hex(UI_TEXT), 0);
    lv_obj_set_style_text_font(toastLabel, &ui_font_cn_16, 0);
    lv_obj_set_style_pad_all(toastLabel, 12, 0);
    lv_obj_set_style_radius(toastLabel, 10, 0);
    lv_obj_align(toastLabel, LV_ALIGN_BOTTOM_MID, 0, -8);
    lv_obj_add_flag(toastLabel, LV_OBJ_FLAG_HIDDEN);
    updateStatusUi(currentWorkState);

    forecastScreen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(forecastScreen, lv_color_hex(UI_BG), 0);
    lv_obj_clear_flag(forecastScreen, LV_OBJ_FLAG_SCROLLABLE);
    makeMainNav(forecastScreen, 3);
    forecastCityLabel = lv_label_create(forecastScreen);
    lv_label_set_text(forecastCityLabel, "武汉未来七天天气");
    lv_obj_set_style_text_font(forecastCityLabel, &ui_font_cn_32, 0);
    lv_obj_set_style_text_color(forecastCityLabel, lv_color_hex(UI_TEXT), 0);
    lv_obj_align(forecastCityLabel, LV_ALIGN_TOP_LEFT, 24, 76);
    lv_obj_t *forecastRefresh = makeButton(forecastScreen, "立即更新", refreshWeatherEvent);
    lv_obj_set_size(forecastRefresh, 125, 46);
    lv_obj_align(forecastRefresh, LV_ALIGN_TOP_RIGHT, -24, 68);
    lv_obj_set_style_text_font(lv_obj_get_child(forecastRefresh, 0), &ui_font_cn_16, 0);
    forecastStatusLabel = lv_label_create(forecastScreen);
    lv_label_set_text(forecastStatusLabel, "连接 Wi-Fi 后自动更新");
    lv_obj_set_style_text_font(forecastStatusLabel, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(forecastStatusLabel, lv_color_hex(UI_MUTED), 0);
    lv_obj_align(forecastStatusLabel, LV_ALIGN_TOP_MID, 0, 116);
    for (int i = 0; i < 7; ++i) {
        lv_obj_t *card = lv_obj_create(forecastScreen);
        lv_obj_set_size(card, 104, 285);
        lv_obj_set_pos(card, 17 + i * 111, 150);
        styleDeskCard(card, i == 0);
        lv_obj_set_style_border_color(card, lv_color_hex(i == 0 ? 0x668A9C : UI_EDGE), 0);
        lv_obj_set_style_border_width(card, 1, 0);
        lv_obj_set_style_radius(card, 14, 0);
        lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
        forecastDayLabels[i] = lv_label_create(card);
        lv_label_set_text(forecastDayLabels[i], "加载中...");
        lv_obj_set_width(forecastDayLabels[i], 88);
        lv_label_set_long_mode(forecastDayLabels[i], LV_LABEL_LONG_WRAP);
        lv_obj_set_style_text_align(forecastDayLabels[i], LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_font(forecastDayLabels[i], &ui_font_cn_16, 0);
        lv_obj_set_style_text_color(forecastDayLabels[i], lv_color_hex(UI_TEXT), 0);
        lv_obj_align(forecastDayLabels[i], LV_ALIGN_CENTER, 0, 35);
        forecastIconLabels[i] = lv_label_create(card);
        lv_label_set_text(forecastIconLabels[i], "☀");
        lv_obj_set_style_text_font(forecastIconLabels[i], &ui_font_weather_36, 0);
        lv_obj_set_style_text_color(forecastIconLabels[i], lv_color_hex(0xD9B77B), 0);
        lv_obj_align(forecastIconLabels[i], LV_ALIGN_TOP_MID, 0, 38);
    }

    // Electronic wooden fish module. The icon is built from LVGL shapes so it
    // does not depend on a missing glyph in the Chinese font.
    muyuScreen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(muyuScreen, lv_color_hex(0x120D0A), 0);
    lv_obj_clear_flag(muyuScreen, LV_OBJ_FLAG_SCROLLABLE);
    makeMainNav(muyuScreen, 4);
    lv_obj_t *muyuTitle = lv_label_create(muyuScreen);
    lv_label_set_text(muyuTitle, "电子木鱼");
    lv_obj_set_style_text_font(muyuTitle, &ui_font_cn_32, 0);
    lv_obj_set_style_text_color(muyuTitle, lv_color_hex(0xFFD28A), 0);
    lv_obj_align(muyuTitle, LV_ALIGN_TOP_MID, 0, 82);
    muyuBody = lv_obj_create(muyuScreen);
    lv_obj_set_size(muyuBody, 310, 190);
    lv_obj_align(muyuBody, LV_ALIGN_CENTER, 0, 16);
    lv_obj_set_style_radius(muyuBody, 95, 0);
    lv_obj_set_style_bg_color(muyuBody, lv_color_hex(0xA94E24), 0);
    lv_obj_set_style_bg_grad_color(muyuBody, lv_color_hex(0xE58A3C), 0);
    lv_obj_set_style_bg_grad_dir(muyuBody, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_border_width(muyuBody, 8, 0);
    lv_obj_set_style_border_color(muyuBody, lv_color_hex(0x6D2D16), 0);
    lv_obj_set_style_shadow_width(muyuBody, 30, 0);
    lv_obj_set_style_shadow_color(muyuBody, lv_color_hex(0xF09A45), 0);
    lv_obj_clear_flag(muyuBody, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(muyuBody, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(muyuBody, muyuEvent, LV_EVENT_ALL, nullptr);
    lv_obj_t *muyuEye = lv_obj_create(muyuBody);
    lv_obj_set_size(muyuEye, 25, 25);
    lv_obj_set_style_radius(muyuEye, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(muyuEye, lv_color_hex(0x2A120B), 0);
    lv_obj_set_style_border_width(muyuEye, 0, 0);
    lv_obj_align(muyuEye, LV_ALIGN_TOP_RIGHT, -62, 35);
    lv_obj_clear_flag(muyuEye, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_t *muyuMouth = lv_obj_create(muyuBody);
    lv_obj_set_size(muyuMouth, 130, 12);
    lv_obj_set_style_radius(muyuMouth, 6, 0);
    lv_obj_set_style_bg_color(muyuMouth, lv_color_hex(0x42180D), 0);
    lv_obj_set_style_border_width(muyuMouth, 0, 0);
    lv_obj_align(muyuMouth, LV_ALIGN_BOTTOM_RIGHT, -22, -47);
    lv_obj_clear_flag(muyuMouth, LV_OBJ_FLAG_CLICKABLE);
    muyuStick = lv_obj_create(muyuScreen);
    lv_obj_set_size(muyuStick, 230, 24);
    lv_obj_align(muyuStick, LV_ALIGN_CENTER, 185, -67);
    lv_obj_set_style_radius(muyuStick, 12, 0);
    lv_obj_set_style_bg_color(muyuStick, lv_color_hex(0xE5B15F), 0);
    lv_obj_set_style_border_width(muyuStick, 0, 0);
    lv_obj_add_flag(muyuStick, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(muyuStick, muyuEvent, LV_EVENT_ALL, nullptr);
    muyuCountLabel = lv_label_create(muyuScreen);
    lv_label_set_text(muyuCountLabel, "点击木鱼 · 功德 +1");
    lv_obj_set_style_text_font(muyuCountLabel, &ui_font_cn_24, 0);
    lv_obj_set_style_text_color(muyuCountLabel, lv_color_hex(0xFFE0A8), 0);
    lv_obj_align(muyuCountLabel, LV_ALIGN_BOTTOM_MID, 0, -25);
    muyuBlessingCard = lv_obj_create(muyuScreen);
    lv_obj_set_size(muyuBlessingCard, 360, 270);
    lv_obj_set_pos(muyuBlessingCard, 220, 132);
    lv_obj_set_style_radius(muyuBlessingCard, 28, 0);
    lv_obj_set_style_bg_color(muyuBlessingCard, lv_color_hex(0x3A1608), 0);
    lv_obj_set_style_bg_opa(muyuBlessingCard, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(muyuBlessingCard, 3, 0);
    lv_obj_set_style_border_color(muyuBlessingCard, lv_color_hex(0xD9B77B), 0);
    lv_obj_set_style_shadow_width(muyuBlessingCard, 18, 0);
    lv_obj_set_style_shadow_color(muyuBlessingCard, lv_color_hex(0xFFB62E), 0);
    lv_obj_clear_flag(muyuBlessingCard, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    muyuBlessingHalo = lv_obj_create(muyuBlessingCard);
    lv_obj_set_size(muyuBlessingHalo, 118, 118);
    lv_obj_align(muyuBlessingHalo, LV_ALIGN_TOP_MID, 0, 12);
    lv_obj_set_style_radius(muyuBlessingHalo, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(muyuBlessingHalo, lv_color_hex(0xD9B77B), 0);
    lv_obj_set_style_bg_opa(muyuBlessingHalo, LV_OPA_30, 0);
    lv_obj_set_style_border_width(muyuBlessingHalo, 5, 0);
    lv_obj_set_style_border_color(muyuBlessingHalo, lv_color_hex(0xFFE8A3), 0);
    lv_obj_clear_flag(muyuBlessingHalo, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    static const int16_t particleX[8] = {52, 91, 126, 214, 252, 286, 70, 270};
    static const int16_t particleY[8] = {139, 126, 150, 118, 145, 132, 153, 121};
    for (int i = 0; i < 8; ++i) {
        muyuParticles[i] = lv_obj_create(muyuBlessingCard);
        int size = 7 + (i % 3) * 2;
        lv_obj_set_size(muyuParticles[i], size, size);
        lv_obj_set_pos(muyuParticles[i], particleX[i], particleY[i]);
        lv_obj_set_style_radius(muyuParticles[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(muyuParticles[i], lv_color_hex(i % 2 ? 0xFFF1A8 : 0xD9B77B), 0);
        lv_obj_set_style_border_width(muyuParticles[i], 0, 0);
        lv_obj_set_style_shadow_width(muyuParticles[i], 5, 0);
        lv_obj_set_style_shadow_color(muyuParticles[i], lv_color_hex(0xFFCC45), 0);
        lv_obj_clear_flag(muyuParticles[i], LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    }

    // A small code-drawn Buddha: ushnisha, long ears, calm face, robe and lotus.
    lv_obj_t *buddhaBody = lv_obj_create(muyuBlessingCard);
    lv_obj_set_size(buddhaBody, 166, 78);
    lv_obj_align(buddhaBody, LV_ALIGN_TOP_MID, 0, 100);
    lv_obj_set_style_radius(buddhaBody, 42, 0);
    lv_obj_set_style_bg_color(buddhaBody, lv_color_hex(0xD97706), 0);
    lv_obj_set_style_border_width(buddhaBody, 3, 0);
    lv_obj_set_style_border_color(buddhaBody, lv_color_hex(0xF6C453), 0);
    lv_obj_clear_flag(buddhaBody, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *robe = lv_obj_create(buddhaBody);
    lv_obj_set_size(robe, 72, 86);
    lv_obj_align(robe, LV_ALIGN_RIGHT_MID, 0, 2);
    lv_obj_set_style_radius(robe, 34, 0);
    lv_obj_set_style_bg_color(robe, lv_color_hex(0x8F3E12), 0);
    lv_obj_set_style_border_width(robe, 0, 0);
    lv_obj_clear_flag(robe, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *leftEar = lv_obj_create(muyuBlessingCard);
    lv_obj_set_size(leftEar, 17, 54);
    lv_obj_align(leftEar, LV_ALIGN_TOP_MID, -38, 55);
    lv_obj_set_style_radius(leftEar, 9, 0);
    lv_obj_set_style_bg_color(leftEar, lv_color_hex(0xE6A83A), 0);
    lv_obj_set_style_border_width(leftEar, 0, 0);
    lv_obj_clear_flag(leftEar, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *rightEar = lv_obj_create(muyuBlessingCard);
    lv_obj_set_size(rightEar, 17, 54);
    lv_obj_align(rightEar, LV_ALIGN_TOP_MID, 38, 55);
    lv_obj_set_style_radius(rightEar, 9, 0);
    lv_obj_set_style_bg_color(rightEar, lv_color_hex(0xE6A83A), 0);
    lv_obj_set_style_border_width(rightEar, 0, 0);
    lv_obj_clear_flag(rightEar, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *buddhaHead = lv_obj_create(muyuBlessingCard);
    lv_obj_set_size(buddhaHead, 74, 84);
    lv_obj_align(buddhaHead, LV_ALIGN_TOP_MID, 0, 34);
    lv_obj_set_style_radius(buddhaHead, 34, 0);
    lv_obj_set_style_bg_color(buddhaHead, lv_color_hex(0xF4B942), 0);
    lv_obj_set_style_border_width(buddhaHead, 2, 0);
    lv_obj_set_style_border_color(buddhaHead, lv_color_hex(0xFFD978), 0);
    lv_obj_clear_flag(buddhaHead, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *ushnisha = lv_obj_create(muyuBlessingCard);
    lv_obj_set_size(ushnisha, 32, 27);
    lv_obj_align(ushnisha, LV_ALIGN_TOP_MID, 0, 22);
    lv_obj_set_style_radius(ushnisha, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(ushnisha, lv_color_hex(0x6F350F), 0);
    lv_obj_set_style_border_width(ushnisha, 2, 0);
    lv_obj_set_style_border_color(ushnisha, lv_color_hex(0xC47A22), 0);
    lv_obj_clear_flag(ushnisha, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    for (int i = -2; i <= 2; ++i) {
        lv_obj_t *curl = lv_obj_create(muyuBlessingCard);
        lv_obj_set_size(curl, 15, 15);
        lv_obj_align(curl, LV_ALIGN_TOP_MID, i * 13, 35 + (i == 0 ? -5 : abs(i) * 2));
        lv_obj_set_style_radius(curl, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(curl, lv_color_hex(0x754015), 0);
        lv_obj_set_style_border_width(curl, 1, 0);
        lv_obj_set_style_border_color(curl, lv_color_hex(0xC47A22), 0);
        lv_obj_clear_flag(curl, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    }

    lv_obj_t *face = lv_label_create(muyuBlessingCard);
    lv_label_set_text(face, "—  —\n  ∪");
    lv_obj_set_style_text_font(face, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_align(face, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(face, lv_color_hex(0x63300F), 0);
    lv_obj_align(face, LV_ALIGN_TOP_MID, 0, 65);

    for (int i = -1; i <= 1; ++i) {
        lv_obj_t *petal = lv_obj_create(muyuBlessingCard);
        lv_obj_set_size(petal, 82, 27);
        lv_obj_align(petal, LV_ALIGN_TOP_MID, i * 52, 163 + abs(i) * 7);
        lv_obj_set_style_radius(petal, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(petal, lv_color_hex(i == 0 ? 0xF5B82E : 0xD98917), 0);
        lv_obj_set_style_border_width(petal, 2, 0);
        lv_obj_set_style_border_color(petal, lv_color_hex(0xDDC69D), 0);
        lv_obj_clear_flag(petal, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    }

    muyuMeritLabel = lv_label_create(muyuBlessingCard);
    lv_label_set_text(muyuMeritLabel, "功德圆满");
    lv_obj_set_width(muyuMeritLabel, 320);
    lv_obj_set_style_text_align(muyuMeritLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(muyuMeritLabel, &ui_font_blessing_32, 0);
    lv_obj_set_style_text_color(muyuMeritLabel, lv_color_hex(0xFFE29A), 0);
    lv_obj_align(muyuMeritLabel, LV_ALIGN_BOTTOM_MID, 0, -8);
    lv_obj_add_flag(muyuBlessingCard, LV_OBJ_FLAG_HIDDEN);

    countdownScreen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(countdownScreen, lv_color_hex(UI_BG), 0);
    lv_obj_clear_flag(countdownScreen, LV_OBJ_FLAG_SCROLLABLE);
    makeMainNav(countdownScreen, 5);
    lv_obj_t *countdownTitle = lv_label_create(countdownScreen);
    lv_label_set_text(countdownTitle, "倒计时");
    lv_obj_set_style_text_font(countdownTitle, &ui_font_countdown_title_32, 0);
    lv_obj_set_style_text_color(countdownTitle, lv_color_hex(UI_TEXT), 0);
    lv_obj_align(countdownTitle, LV_ALIGN_TOP_MID, 0, 68);
    countdownLabel = lv_label_create(countdownScreen);
    lv_label_set_text(countdownLabel, "00:00:00");
    lv_obj_set_style_text_font(countdownLabel, &lv_font_montserrat_36, 0);
    lv_obj_set_style_text_color(countdownLabel, lv_color_hex(UI_ACCENT), 0);
    lv_obj_align(countdownLabel, LV_ALIGN_TOP_MID, 0, 105);
    static const char *hours = "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23";
    static const char *minutes = "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59";
    countdownHourRoller = lv_roller_create(countdownScreen);
    countdownMinuteRoller = lv_roller_create(countdownScreen);
    countdownSecondRoller = lv_roller_create(countdownScreen);
    lv_obj_t *rollers[] = {countdownHourRoller, countdownMinuteRoller, countdownSecondRoller};
    for (int i = 0; i < 3; ++i) {
        lv_roller_set_options(rollers[i], i == 0 ? hours : minutes, LV_ROLLER_MODE_INFINITE);
        lv_roller_set_visible_row_count(rollers[i], 3);
        lv_obj_set_size(rollers[i], 110, 142);
        lv_obj_set_pos(rollers[i], 215 + i * 135, 155);
        lv_obj_set_style_text_font(rollers[i], &lv_font_montserrat_24, 0);
        styleDeskCard(rollers[i]);
        lv_obj_set_style_text_color(rollers[i], lv_color_hex(UI_MUTED), 0);
        lv_obj_set_style_bg_color(rollers[i], lv_color_hex(0x354F60), LV_PART_SELECTED);
        lv_obj_set_style_text_color(rollers[i], lv_color_hex(UI_TEXT), LV_PART_SELECTED);
        lv_obj_set_style_text_font(rollers[i], &lv_font_montserrat_28, LV_PART_SELECTED);
    }
    lv_obj_t *units = lv_label_create(countdownScreen);
    lv_label_set_text(units, "时                 分                 秒");
    lv_obj_set_style_text_font(units, &ui_font_units_14, 0);
    lv_obj_set_style_text_color(units, lv_color_hex(UI_MUTED), 0);
    lv_obj_align(units, LV_ALIGN_TOP_MID, 0, 312);
    lv_obj_add_flag(units, LV_OBJ_FLAG_HIDDEN);
    static const char *countdownUnits[] = {"时", "分", "秒"};
    for (int i = 0; i < 3; ++i) {
        lv_obj_t *unit = lv_label_create(countdownScreen);
        lv_label_set_text(unit, countdownUnits[i]);
        lv_obj_set_style_text_font(unit, &ui_font_cn_16, 0);
        lv_obj_set_style_text_color(unit, lv_color_hex(UI_MUTED), 0);
        lv_obj_set_width(unit, 110);
        lv_obj_set_style_text_align(unit, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_pos(unit, 215 + i * 135, 304);
    }
    lv_obj_t *setButton = makeButton(countdownScreen, "设定并开始", countdownSetEvent);
    lv_obj_set_size(setButton, 160, 55);
    lv_obj_align(setButton, LV_ALIGN_BOTTOM_LEFT, 120, -25);
    lv_obj_set_style_text_font(lv_obj_get_child(setButton, 0), &ui_font_cn_16, 0);
    lv_obj_t *toggleButton = makeButton(countdownScreen, "开始", countdownToggleEvent);
    lv_obj_set_size(toggleButton, 160, 55);
    lv_obj_align(toggleButton, LV_ALIGN_BOTTOM_MID, 0, -25);
    countdownStartButtonLabel = lv_obj_get_child(toggleButton, 0);
    lv_obj_set_style_text_font(countdownStartButtonLabel, &ui_font_cn_16, 0);
    lv_obj_t *resetButton = makeButton(countdownScreen, "重置", countdownResetEvent);
    lv_obj_set_size(resetButton, 160, 55);
    lv_obj_align(resetButton, LV_ALIGN_BOTTOM_RIGHT, -120, -25);
    lv_obj_set_style_text_font(lv_obj_get_child(resetButton, 0), &ui_font_cn_16, 0);

    // The to-do feature is intentionally removed from this stability build.
#if 0
    todoScreen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(todoScreen, lv_color_hex(UI_BG), 0);
    lv_obj_clear_flag(todoScreen, LV_OBJ_FLAG_SCROLLABLE);
    makeNavButton(todoScreen, 12, 92, LV_SYMBOL_HOME, "主页", false, showDashboardEvent);
    makeNavButton(todoScreen, 110, 112, LV_SYMBOL_LIST, "待办", true, nullptr);
    makeNavButton(todoScreen, 228, 105, LV_SYMBOL_EDIT, "日历", false, showCalendarScreenEvent);
    makeNavButton(todoScreen, 339, 112, LV_SYMBOL_WIFI, "Wi-Fi", false, showWifiScreenEvent);
    makeNavButton(todoScreen, 457, 96, LV_SYMBOL_SETTINGS, "待开发", false, nullptr);

    lv_obj_t *todoTitle = lv_label_create(todoScreen);
    lv_label_set_text(todoTitle, "工作清单");
    lv_obj_set_style_text_font(todoTitle, &ui_font_cn_32, 0);
    lv_obj_set_style_text_color(todoTitle, lv_color_hex(UI_TEXT), 0);
    lv_obj_align(todoTitle, LV_ALIGN_TOP_LEFT, 24, 78);
    todoStatusLabel = lv_label_create(todoScreen);
    lv_label_set_text(todoStatusLabel, "待办会自动保存");
    lv_obj_set_style_text_font(todoStatusLabel, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(todoStatusLabel, lv_color_hex(UI_ACCENT), 0);
    lv_obj_align(todoStatusLabel, LV_ALIGN_TOP_RIGHT, -24, 86);

    todoList = lv_obj_create(todoScreen);
    lv_obj_set_size(todoList, 510, 330);
    lv_obj_align(todoList, LV_ALIGN_BOTTOM_LEFT, 18, -12);
    lv_obj_set_style_bg_color(todoList, lv_color_hex(0x08182C), 0);
    lv_obj_set_style_border_color(todoList, lv_color_hex(0x21466E), 0);
    lv_obj_set_style_radius(todoList, 14, 0);
    lv_obj_set_flex_flow(todoList, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(todoList, 8, 0);
    lv_obj_set_style_pad_row(todoList, 7, 0);

    lv_obj_t *entryPanel = lv_obj_create(todoScreen);
    lv_obj_set_size(entryPanel, 240, 330);
    lv_obj_align(entryPanel, LV_ALIGN_BOTTOM_RIGHT, -18, -12);
    lv_obj_set_style_bg_color(entryPanel, lv_color_hex(UI_SURFACE), 0);
    lv_obj_set_style_border_width(entryPanel, 2, 0);
    lv_obj_set_style_border_color(entryPanel, lv_color_hex(0x2379C9), 0);
    lv_obj_set_style_radius(entryPanel, 14, 0);
    lv_obj_clear_flag(entryPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *entryTitle = lv_label_create(entryPanel);
    lv_label_set_text(entryTitle, "新增待办");
    lv_obj_set_style_text_font(entryTitle, &ui_font_cn_24, 0);
    lv_obj_set_style_text_color(entryTitle, lv_color_hex(UI_TEXT), 0);
    lv_obj_align(entryTitle, LV_ALIGN_TOP_LEFT, 6, 4);
    todoTextArea = lv_textarea_create(entryPanel);
    lv_textarea_set_one_line(todoTextArea, true);
    lv_textarea_set_placeholder_text(todoTextArea, "待办内容");
    lv_obj_set_size(todoTextArea, 204, 52);
    lv_obj_align(todoTextArea, LV_ALIGN_TOP_MID, 0, 38);
    lv_obj_set_style_text_font(todoTextArea, &ui_font_cn_16, 0);
    lv_obj_set_style_bg_color(todoTextArea, lv_color_hex(0xEAF3FB), 0);
    lv_obj_set_style_radius(todoTextArea, 11, 0);
    lv_obj_set_style_border_color(todoTextArea, lv_color_hex(0x4A90C9), LV_STATE_FOCUSED);
    lv_obj_add_event_cb(todoTextArea, todoTextAreaEvent, LV_EVENT_FOCUSED, nullptr);
    lv_obj_add_event_cb(todoTextArea, todoTypingEvent, LV_EVENT_VALUE_CHANGED, nullptr);
    todoTimeArea = lv_textarea_create(entryPanel);
    lv_textarea_set_one_line(todoTimeArea, true);
    lv_textarea_set_placeholder_text(todoTimeArea, "点击日历选择截止日期");
    lv_textarea_set_text(todoTimeArea, "");
    lv_textarea_set_cursor_click_pos(todoTimeArea, false);
    lv_obj_set_size(todoTimeArea, 204, 38);
    lv_obj_align(todoTimeArea, LV_ALIGN_TOP_MID, 0, 94);
    lv_obj_set_style_text_font(todoTimeArea, &ui_font_cn_16, 0);
    lv_obj_set_style_bg_color(todoTimeArea, lv_color_hex(0xEAF3FB), 0);
    lv_obj_set_style_radius(todoTimeArea, 11, 0);
    lv_obj_set_style_border_color(todoTimeArea, lv_color_hex(0x4A90C9), LV_STATE_FOCUSED);
    lv_obj_clear_flag(todoTimeArea, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(todoTimeArea, openDatePickerEvent, LV_EVENT_CLICKED, nullptr);
    lv_obj_t *dateHint = lv_label_create(entryPanel);
    lv_label_set_text(dateHint, "点击上方日期框，打开大日历选择");
    lv_obj_set_style_text_font(dateHint, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(dateHint, lv_color_hex(0x86BCEB), 0);
    lv_obj_align(dateHint, LV_ALIGN_TOP_MID, 0, 150);

    time_t calendarNow = time(nullptr);
    struct tm calendarTm = {};
    if (calendarNow > 100000) localtime_r(&calendarNow, &calendarTm);
    else { calendarTm.tm_year = 126; calendarTm.tm_mon = 7; calendarTm.tm_mday = 21; }
    lv_obj_t *addTodoButton = makeButton(entryPanel, "添加到清单", addTodoEvent);
    lv_obj_set_size(addTodoButton, 204, 56);
    lv_obj_align(addTodoButton, LV_ALIGN_TOP_MID, 0, 205);
    lv_obj_set_style_text_font(lv_obj_get_child(addTodoButton, 0), &ui_font_cn_16, 0);

    datePickerPanel = lv_obj_create(nullptr);
    lv_obj_set_size(datePickerPanel, 800, 480);
    lv_obj_set_pos(datePickerPanel, 0, 0);
    lv_obj_set_style_bg_color(datePickerPanel, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(datePickerPanel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(datePickerPanel, 0, 0);
    lv_obj_set_style_radius(datePickerPanel, 0, 0);
    lv_obj_clear_flag(datePickerPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *dateDialog = lv_obj_create(datePickerPanel);
    lv_obj_set_size(dateDialog, 620, 410);
    lv_obj_center(dateDialog);
    lv_obj_set_style_bg_color(dateDialog, lv_color_hex(UI_SURFACE), 0);
    lv_obj_set_style_border_width(dateDialog, 2, 0);
    lv_obj_set_style_border_color(dateDialog, lv_color_hex(0x668A9C), 0);
    lv_obj_set_style_radius(dateDialog, 20, 0);
    lv_obj_clear_flag(dateDialog, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *pickerTitle = lv_label_create(dateDialog);
    lv_label_set_text(pickerTitle, "选择截止日期");
    lv_obj_set_style_text_font(pickerTitle, &ui_font_cn_24, 0);
    lv_obj_set_style_text_color(pickerTitle, lv_color_hex(UI_TEXT), 0);
    lv_obj_align(pickerTitle, LV_ALIGN_TOP_LEFT, 8, 2);
    datePickerValueLabel = lv_label_create(dateDialog);
    lv_label_set_text(datePickerValueLabel, "请选择截止日期");
    lv_obj_set_style_text_font(datePickerValueLabel, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(datePickerValueLabel, lv_color_hex(UI_ACCENT), 0);
    lv_obj_align(datePickerValueLabel, LV_ALIGN_TOP_RIGHT, -8, 8);
    datePickerCalendar = lv_calendar_create(dateDialog);
    lv_obj_set_size(datePickerCalendar, 560, 285);
    lv_obj_align(datePickerCalendar, LV_ALIGN_TOP_MID, 0, 45);
    lv_obj_set_style_text_font(datePickerCalendar, &lv_font_montserrat_20, LV_PART_ITEMS);
    lv_obj_set_style_bg_color(datePickerCalendar, lv_color_hex(0x102B4C), 0);
    lv_obj_set_style_border_color(datePickerCalendar, lv_color_hex(0x2C78B8), 0);
    static const char *dayNames[] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
    lv_calendar_set_day_names(datePickerCalendar, dayNames);
    lv_calendar_set_today_date(datePickerCalendar, calendarTm.tm_year + 1900,
                               calendarTm.tm_mon + 1, calendarTm.tm_mday);
    lv_calendar_set_showed_date(datePickerCalendar, calendarTm.tm_year + 1900, calendarTm.tm_mon + 1);
    lv_obj_t *datePickerHeader = lv_calendar_header_arrow_create(datePickerCalendar);
    lv_obj_set_style_text_color(datePickerHeader, lv_color_hex(UI_TEXT), 0);
    lv_obj_set_style_text_font(datePickerHeader, &lv_font_montserrat_20, 0);
    lv_obj_t *datePickerMatrix = lv_calendar_get_btnmatrix(datePickerCalendar);
    lv_obj_set_style_bg_color(datePickerMatrix, lv_color_hex(0x282F36), LV_PART_ITEMS);
    lv_obj_set_style_bg_opa(datePickerMatrix, LV_OPA_COVER, LV_PART_ITEMS);
    lv_obj_set_style_text_color(datePickerMatrix, lv_color_hex(UI_TEXT), LV_PART_ITEMS);
    lv_obj_set_style_border_width(datePickerMatrix, 0, LV_PART_ITEMS);
    lv_obj_add_event_cb(datePickerCalendar, datePickerCalendarEvent, LV_EVENT_VALUE_CHANGED, nullptr);
    lv_obj_t *pickerCancel = makeButton(dateDialog, "取消", closeDatePickerEvent);
    lv_obj_set_size(pickerCancel, 110, 42);
    lv_obj_align(pickerCancel, LV_ALIGN_BOTTOM_RIGHT, -132, -2);
    lv_obj_set_style_bg_color(pickerCancel, lv_color_hex(0x394149), 0);
    lv_obj_set_style_text_font(lv_obj_get_child(pickerCancel, 0), &ui_font_cn_16, 0);
    lv_obj_t *pickerConfirm = makeButton(dateDialog, "确认日期", confirmDatePickerEvent);
    lv_obj_set_size(pickerConfirm, 120, 42);
    lv_obj_align(pickerConfirm, LV_ALIGN_BOTTOM_RIGHT, 0, -2);
    lv_obj_set_style_text_font(lv_obj_get_child(pickerConfirm, 0), &ui_font_cn_16, 0);

    todoKeyboard = lv_keyboard_create(todoScreen);
    lv_obj_set_size(todoKeyboard, 780, 170);
    lv_obj_align(todoKeyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(todoKeyboard, lv_color_hex(UI_BG), 0);
    lv_obj_set_style_bg_opa(todoKeyboard, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(todoKeyboard, 1, 0);
    lv_obj_set_style_border_color(todoKeyboard, lv_color_hex(0x245582), 0);
    lv_obj_set_style_pad_all(todoKeyboard, 6, 0);
    lv_obj_set_style_text_font(todoKeyboard, &lv_font_montserrat_16, LV_PART_ITEMS);
    lv_obj_set_style_text_color(todoKeyboard, lv_color_hex(UI_TEXT), LV_PART_ITEMS);
    lv_obj_set_style_bg_color(todoKeyboard, lv_color_hex(0x303840), LV_PART_ITEMS);
    lv_obj_set_style_bg_opa(todoKeyboard, LV_OPA_COVER, LV_PART_ITEMS);
    lv_obj_set_style_radius(todoKeyboard, 7, LV_PART_ITEMS);
    lv_obj_set_style_border_width(todoKeyboard, 1, LV_PART_ITEMS);
    lv_obj_set_style_border_color(todoKeyboard, lv_color_hex(0x2B5D89), LV_PART_ITEMS);
    lv_obj_add_event_cb(todoKeyboard, todoKeyboardEvent, LV_EVENT_ALL, nullptr);
    lv_obj_add_flag(todoKeyboard, LV_OBJ_FLAG_HIDDEN);

    todoIme = lv_ime_pinyin_create(todoScreen);
    lv_ime_pinyin_set_keyboard(todoIme, todoKeyboard);
    lv_ime_pinyin_set_mode(todoIme, LV_IME_PINYIN_MODE_K26);
    lv_obj_t *candidatePanel = lv_ime_pinyin_get_cand_panel(todoIme);
    if (candidatePanel != nullptr) {
        lv_obj_set_parent(candidatePanel, todoScreen);
        lv_obj_align_to(candidatePanel, todoKeyboard, LV_ALIGN_OUT_TOP_MID, 0, 0);
        lv_obj_set_size(candidatePanel, 780, 50);
        lv_obj_set_style_pad_all(candidatePanel, 2, 0);
        lv_obj_set_style_pad_gap(candidatePanel, 2, 0);
        lv_obj_set_style_bg_color(candidatePanel, lv_color_hex(0x0D2945), 0);
        lv_obj_set_style_bg_opa(candidatePanel, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(candidatePanel, &ui_font_cn_16, LV_PART_ITEMS);
        lv_obj_set_style_text_color(candidatePanel, lv_color_hex(UI_TEXT), LV_PART_ITEMS);
        lv_obj_set_style_bg_color(candidatePanel, lv_color_hex(0x354F60), LV_PART_ITEMS);
        lv_obj_set_style_bg_opa(candidatePanel, LV_OPA_COVER, LV_PART_ITEMS);
        lv_obj_move_foreground(candidatePanel);
    }
    todoTypingLabel = lv_label_create(todoScreen);
    lv_label_set_text(todoTypingLabel, "正在输入：");
    lv_obj_set_size(todoTypingLabel, 760, 36);
    lv_obj_align(todoTypingLabel, LV_ALIGN_TOP_MID, 0, 72);
    lv_obj_set_style_bg_color(todoTypingLabel, lv_color_hex(0x0D2945), 0);
    lv_obj_set_style_bg_opa(todoTypingLabel, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(todoTypingLabel, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(todoTypingLabel, lv_color_hex(UI_TEXT), 0);
    lv_obj_set_style_pad_hor(todoTypingLabel, 10, 0);
    lv_obj_set_style_pad_ver(todoTypingLabel, 7, 0);
    lv_obj_set_style_radius(todoTypingLabel, 8, 0);
    lv_obj_add_flag(todoTypingLabel, LV_OBJ_FLAG_HIDDEN);

    loadTodos();
    rebuildTodoList();
#endif

    time_t calendarNow = time(nullptr);
    struct tm calendarTm = {};
    if (calendarNow > 100000) localtime_r(&calendarNow, &calendarTm);
    else { calendarTm.tm_year = 126; calendarTm.tm_mon = 7; calendarTm.tm_mday = 22; }
    static const char *dayNames[] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
    calendarScreen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(calendarScreen, lv_color_hex(UI_BG), 0);
    lv_obj_clear_flag(calendarScreen, LV_OBJ_FLAG_SCROLLABLE);
    makeMainNav(calendarScreen, 1);
    lv_obj_t *calendarTitle = lv_label_create(calendarScreen);
    lv_label_set_text(calendarTitle, "实时日历");
    lv_obj_set_style_text_font(calendarTitle, &ui_font_cn_32, 0);
    lv_obj_set_style_text_color(calendarTitle, lv_color_hex(UI_TEXT), 0);
    lv_obj_align(calendarTitle, LV_ALIGN_TOP_LEFT, 24, 75);
    lv_obj_t *prevMonth = makeButton(calendarScreen, "上月", previousMonthEvent);
    lv_obj_set_size(prevMonth, 82, 40);
    lv_obj_set_pos(prevMonth, 286, 72);
    lv_obj_set_style_text_font(lv_obj_get_child(prevMonth, 0), &ui_font_cn_16, 0);
    lv_obj_t *todayMonth = makeButton(calendarScreen, "今天", todayMonthEvent);
    lv_obj_set_size(todayMonth, 82, 40);
    lv_obj_set_pos(todayMonth, 376, 72);
    lv_obj_set_style_text_font(lv_obj_get_child(todayMonth, 0), &ui_font_cn_16, 0);
    lv_obj_t *nextMonth = makeButton(calendarScreen, "下月", nextMonthEvent);
    lv_obj_set_size(nextMonth, 82, 40);
    lv_obj_set_pos(nextMonth, 466, 72);
    lv_obj_set_style_text_font(lv_obj_get_child(nextMonth, 0), &ui_font_cn_16, 0);
    mainCalendar = lv_calendar_create(calendarScreen);
    lv_obj_set_size(mainCalendar, 535, 300);
    lv_obj_align(mainCalendar, LV_ALIGN_BOTTOM_LEFT, 18, -48);
    lv_calendar_set_day_names(mainCalendar, dayNames);
    lv_calendar_set_today_date(mainCalendar, calendarTm.tm_year + 1900,
                               calendarTm.tm_mon + 1, calendarTm.tm_mday);
    lv_calendar_set_showed_date(mainCalendar, calendarTm.tm_year + 1900, calendarTm.tm_mon + 1);
    lv_obj_set_style_text_font(mainCalendar, &lv_font_montserrat_20, LV_PART_ITEMS);
    lv_obj_set_style_bg_color(mainCalendar, lv_color_hex(UI_SURFACE), 0);
    lv_obj_set_style_border_width(mainCalendar, 2, 0);
    lv_obj_set_style_border_color(mainCalendar, lv_color_hex(UI_EDGE), 0);
    lv_obj_set_style_radius(mainCalendar, 18, 0);
    styleDeskCard(mainCalendar);
    lv_obj_t *mainCalendarMatrix = lv_calendar_get_btnmatrix(mainCalendar);
    lv_obj_add_event_cb(mainCalendarMatrix, calendarCellDrawEvent, LV_EVENT_DRAW_PART_BEGIN, nullptr);
    lv_obj_set_style_bg_color(mainCalendarMatrix, lv_color_hex(0x282F36), LV_PART_ITEMS);
    lv_obj_set_style_bg_opa(mainCalendarMatrix, LV_OPA_COVER, LV_PART_ITEMS);
    lv_obj_set_style_text_color(mainCalendarMatrix, lv_color_hex(UI_TEXT), LV_PART_ITEMS);
    lv_obj_set_style_border_width(mainCalendarMatrix, 0, LV_PART_ITEMS);
    lv_obj_set_style_bg_color(mainCalendarMatrix, lv_color_hex(0xDC5B67),
                              LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(mainCalendarMatrix, LV_OPA_COVER,
                            LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(mainCalendarMatrix, lv_color_hex(UI_TEXT),
                                LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_border_width(mainCalendarMatrix, 2,
                                  LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_border_color(mainCalendarMatrix, lv_color_hex(0xFFD0D5),
                                  LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_t *mainCalendarHeader = lv_calendar_header_arrow_create(mainCalendar);
    lv_obj_set_style_bg_color(mainCalendarHeader, lv_color_hex(UI_SURFACE), 0);
    lv_obj_set_style_border_width(mainCalendarHeader, 0, 0);
    lv_obj_set_style_text_color(mainCalendarHeader, lv_color_hex(UI_TEXT), 0);
    lv_obj_set_style_text_font(mainCalendarHeader, &lv_font_montserrat_20, 0);
    lv_obj_add_event_cb(mainCalendar, mainCalendarEvent, LV_EVENT_VALUE_CHANGED, nullptr);
    loadSpecialDays();
    updateWeekendHighlights(calendarTm.tm_year + 1900, calendarTm.tm_mon + 1);
    calendarMonthAgendaLabel = lv_label_create(calendarScreen);
    lv_label_set_text(calendarMonthAgendaLabel, "本月待办：暂无设置截止日期的事项");
    lv_obj_set_width(calendarMonthAgendaLabel, 520);
    lv_label_set_long_mode(calendarMonthAgendaLabel, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_font(calendarMonthAgendaLabel, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(calendarMonthAgendaLabel, lv_color_hex(UI_MUTED), 0);
    lv_obj_align(calendarMonthAgendaLabel, LV_ALIGN_BOTTOM_LEFT, 24, -15);
    updateCalendarAgenda(calendarTm.tm_year + 1900, calendarTm.tm_mon + 1);
    lv_obj_t *calendarInfo = lv_obj_create(calendarScreen);
    lv_obj_set_size(calendarInfo, 215, 340);
    lv_obj_align(calendarInfo, LV_ALIGN_BOTTOM_RIGHT, -18, -12);
    lv_obj_set_style_bg_color(calendarInfo, lv_color_hex(UI_SURFACE), 0);
    lv_obj_set_style_border_color(calendarInfo, lv_color_hex(UI_EDGE), 0);
    lv_obj_set_style_radius(calendarInfo, 18, 0);
    styleDeskCard(calendarInfo);
    lv_obj_clear_flag(calendarInfo, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *todayTitle = lv_label_create(calendarInfo);
    lv_label_set_text(todayTitle, "日期详情");
    lv_obj_set_style_text_font(todayTitle, &ui_font_cn_24, 0);
    lv_obj_set_style_text_color(todayTitle, lv_color_hex(UI_TEXT), 0);
    lv_obj_align(todayTitle, LV_ALIGN_TOP_LEFT, 4, 4);
    calendarTodayLabel = lv_label_create(calendarInfo);
    lv_label_set_text(calendarTodayLabel, "正在同步日期");
    lv_obj_set_width(calendarTodayLabel, 180);
    lv_label_set_long_mode(calendarTodayLabel, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(calendarTodayLabel, &ui_font_cn_24, 0);
    lv_obj_set_style_text_color(calendarTodayLabel, lv_color_hex(UI_ACCENT), 0);
    lv_obj_align(calendarTodayLabel, LV_ALIGN_TOP_LEFT, 4, 55);
    calendarDetailLabel = lv_label_create(calendarInfo);
    lv_label_set_text(calendarDetailLabel, "点击日期查看节日、备注和待办");
    lv_obj_set_width(calendarDetailLabel, 180);
    lv_label_set_long_mode(calendarDetailLabel, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(calendarDetailLabel, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(calendarDetailLabel, lv_color_hex(UI_TEXT), 0);
    lv_obj_align(calendarDetailLabel, LV_ALIGN_TOP_LEFT, 4, 160);
    calendarMarkButton = makeButton(calendarInfo, "标记 / 备注", openMarkerEvent);
    lv_obj_set_size(calendarMarkButton, 170, 44);
    lv_obj_align(calendarMarkButton, LV_ALIGN_BOTTOM_MID, 0, -38);
    lv_obj_set_style_text_font(lv_obj_get_child(calendarMarkButton, 0), &ui_font_cn_16, 0);

    calendarMarkerPanel = lv_obj_create(calendarScreen);
    lv_obj_set_size(calendarMarkerPanel, 460, 300);
    lv_obj_center(calendarMarkerPanel);
    lv_obj_set_style_bg_color(calendarMarkerPanel, lv_color_hex(UI_SURFACE), 0);
    lv_obj_set_style_border_width(calendarMarkerPanel, 2, 0);
    lv_obj_set_style_border_color(calendarMarkerPanel, lv_color_hex(0x668A9C), 0);
    lv_obj_set_style_radius(calendarMarkerPanel, 20, 0);
    lv_obj_clear_flag(calendarMarkerPanel, LV_OBJ_FLAG_SCROLLABLE);
    calendarMarkerDateLabel = lv_label_create(calendarMarkerPanel);
    lv_label_set_text(calendarMarkerDateLabel, "标记特殊日期");
    lv_obj_set_style_text_font(calendarMarkerDateLabel, &ui_font_cn_24, 0);
    lv_obj_set_style_text_color(calendarMarkerDateLabel, lv_color_hex(UI_TEXT), 0);
    lv_obj_align(calendarMarkerDateLabel, LV_ALIGN_TOP_LEFT, 8, 5);
    lv_obj_t *markerHint = lv_label_create(calendarMarkerPanel);
    lv_label_set_text(markerHint, "可直接输入备注，也可从下方选择常用类型");
    lv_obj_set_style_text_font(markerHint, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(markerHint, lv_color_hex(UI_MUTED), 0);
    lv_obj_align(markerHint, LV_ALIGN_TOP_LEFT, 8, 45);
    calendarMarkerNoteArea = lv_textarea_create(calendarMarkerPanel);
    lv_textarea_set_one_line(calendarMarkerNoteArea, true);
    lv_textarea_set_placeholder_text(calendarMarkerNoteArea, "输入日期备注");
    lv_obj_set_size(calendarMarkerNoteArea, 400, 45);
    lv_obj_align(calendarMarkerNoteArea, LV_ALIGN_TOP_MID, 0, 72);
    lv_obj_set_style_text_font(calendarMarkerNoteArea, &ui_font_cn_16, 0);
    lv_obj_add_event_cb(calendarMarkerNoteArea, markerTextAreaEvent, LV_EVENT_FOCUSED, nullptr);
    calendarMarkerRoller = lv_roller_create(calendarMarkerPanel);
    lv_roller_set_options(calendarMarkerRoller, "重要事项\n纪念日\n生日\n会议\n出行\n休假\n其他\n取消该日标记",
                          LV_ROLLER_MODE_NORMAL);
    lv_obj_set_size(calendarMarkerRoller, 400, 70);
    lv_obj_align(calendarMarkerRoller, LV_ALIGN_TOP_MID, 0, 128);
    lv_obj_set_style_text_font(calendarMarkerRoller, &ui_font_cn_16, 0);
    lv_obj_add_event_cb(calendarMarkerRoller, markerPresetEvent, LV_EVENT_VALUE_CHANGED, nullptr);
    lv_obj_t *markerCancel = makeButton(calendarMarkerPanel, "取消", closeMarkerEvent);
    lv_obj_set_size(markerCancel, 110, 44);
    lv_obj_align(markerCancel, LV_ALIGN_BOTTOM_RIGHT, -130, -4);
    lv_obj_set_style_bg_color(markerCancel, lv_color_hex(0x394149), 0);
    lv_obj_set_style_text_font(lv_obj_get_child(markerCancel, 0), &ui_font_cn_16, 0);
    lv_obj_t *markerSave = makeButton(calendarMarkerPanel, "保存标记", saveMarkerEvent);
    lv_obj_set_size(markerSave, 120, 44);
    lv_obj_align(markerSave, LV_ALIGN_BOTTOM_RIGHT, 0, -4);
    lv_obj_set_style_text_font(lv_obj_get_child(markerSave, 0), &ui_font_cn_16, 0);
    lv_obj_add_flag(calendarMarkerPanel, LV_OBJ_FLAG_HIDDEN);

    calendarMarkerKeyboard = lv_keyboard_create(calendarScreen);
    lv_obj_set_size(calendarMarkerKeyboard, 780, 170);
    lv_obj_align(calendarMarkerKeyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(calendarMarkerKeyboard, lv_color_hex(UI_BG), 0);
    lv_obj_set_style_text_font(calendarMarkerKeyboard, &lv_font_montserrat_16, LV_PART_ITEMS);
    lv_obj_set_style_text_color(calendarMarkerKeyboard, lv_color_hex(UI_TEXT), LV_PART_ITEMS);
    lv_obj_set_style_bg_color(calendarMarkerKeyboard, lv_color_hex(0x303840), LV_PART_ITEMS);
    lv_obj_set_style_bg_opa(calendarMarkerKeyboard, LV_OPA_COVER, LV_PART_ITEMS);
    lv_obj_add_event_cb(calendarMarkerKeyboard, markerKeyboardEvent, LV_EVENT_ALL, nullptr);
    lv_obj_add_flag(calendarMarkerKeyboard, LV_OBJ_FLAG_HIDDEN);
    calendarMarkerIme = lv_ime_pinyin_create(calendarScreen);
    lv_ime_pinyin_set_keyboard(calendarMarkerIme, calendarMarkerKeyboard);
    lv_ime_pinyin_set_mode(calendarMarkerIme, LV_IME_PINYIN_MODE_K26);
    lv_obj_t *markerCandidates = lv_ime_pinyin_get_cand_panel(calendarMarkerIme);
    if (markerCandidates) {
        lv_obj_set_parent(markerCandidates, calendarScreen);
        lv_obj_align_to(markerCandidates, calendarMarkerKeyboard, LV_ALIGN_OUT_TOP_MID, 0, 0);
        lv_obj_set_size(markerCandidates, 780, 50);
        lv_obj_set_style_pad_all(markerCandidates, 2, 0);
        lv_obj_set_style_pad_gap(markerCandidates, 2, 0);
        lv_obj_set_style_text_font(markerCandidates, &ui_font_cn_16, LV_PART_ITEMS);
        lv_obj_set_style_text_color(markerCandidates, lv_color_hex(UI_TEXT), LV_PART_ITEMS);
        lv_obj_set_style_bg_color(markerCandidates, lv_color_hex(0x354F60), LV_PART_ITEMS);
        lv_obj_set_style_bg_opa(markerCandidates, LV_OPA_COVER, LV_PART_ITEMS);
    }

    wifiScreen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(wifiScreen, lv_color_hex(UI_BG), 0);
    lv_obj_clear_flag(wifiScreen, LV_OBJ_FLAG_SCROLLABLE);
    makeMainNav(wifiScreen, 2);
    lv_obj_t *wifiTitle = lv_label_create(wifiScreen);
    lv_label_set_text(wifiTitle, "Wi-Fi 设置");
    lv_obj_set_style_text_font(wifiTitle, &ui_font_cn_32, 0);
    lv_obj_set_style_text_color(wifiTitle, lv_color_hex(UI_TEXT), 0);
    lv_obj_align(wifiTitle, LV_ALIGN_TOP_LEFT, 28, 72);

    ssidArea = lv_textarea_create(wifiScreen);
    lv_textarea_set_one_line(ssidArea, true);
    lv_textarea_set_placeholder_text(ssidArea, "Wi-Fi 名称（SSID）");
    lv_textarea_set_text(ssidArea, savedSsid.c_str());
    lv_obj_set_size(ssidArea, 360, 52);
    lv_obj_align(ssidArea, LV_ALIGN_TOP_RIGHT, -28, 112);
    lv_obj_set_style_text_font(ssidArea, &ui_font_cn_16, 0);
    lv_obj_set_style_bg_color(ssidArea, lv_color_hex(UI_SURFACE), 0);
    lv_obj_set_style_text_color(ssidArea, lv_color_hex(UI_TEXT), 0);
    lv_obj_set_style_border_color(ssidArea, lv_color_hex(UI_EDGE), 0);
    lv_obj_set_style_border_color(ssidArea, lv_color_hex(UI_ACCENT), LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ssidArea, 12, 0);
    lv_obj_add_event_cb(ssidArea, textAreaEvent, LV_EVENT_FOCUSED, nullptr);

    passwordArea = lv_textarea_create(wifiScreen);
    lv_textarea_set_one_line(passwordArea, true);
    lv_textarea_set_password_mode(passwordArea, true);
    lv_textarea_set_placeholder_text(passwordArea, "Wi-Fi 密码");
    lv_textarea_set_text(passwordArea, savedPassword.c_str());
    lv_obj_set_size(passwordArea, 360, 52);
    lv_obj_align(passwordArea, LV_ALIGN_TOP_RIGHT, -28, 176);
    lv_obj_set_style_text_font(passwordArea, &ui_font_cn_16, 0);
    lv_obj_set_style_bg_color(passwordArea, lv_color_hex(UI_SURFACE), 0);
    lv_obj_set_style_text_color(passwordArea, lv_color_hex(UI_TEXT), 0);
    lv_obj_set_style_border_color(passwordArea, lv_color_hex(UI_EDGE), 0);
    lv_obj_set_style_border_color(passwordArea, lv_color_hex(UI_ACCENT), LV_STATE_FOCUSED);
    lv_obj_set_style_radius(passwordArea, 12, 0);
    lv_obj_add_event_cb(passwordArea, textAreaEvent, LV_EVENT_FOCUSED, nullptr);

    lv_obj_t *saveButton = makeButton(wifiScreen, "保存并连接", connectEvent);
    lv_obj_set_size(saveButton, 172, 50);
    lv_obj_align(saveButton, LV_ALIGN_TOP_RIGHT, -216, 240);
    lv_obj_set_style_text_font(lv_obj_get_child(saveButton, 0), &ui_font_cn_16, 0);
    lv_obj_set_style_bg_color(saveButton, lv_color_hex(0x354F60), 0);
    lv_obj_t *backButton = makeButton(wifiScreen, "返回主页", showDashboardEvent);
    lv_obj_set_size(backButton, 172, 50);
    lv_obj_align(backButton, LV_ALIGN_TOP_RIGHT, -28, 240);
    lv_obj_set_style_text_font(lv_obj_get_child(backButton, 0), &ui_font_cn_16, 0);
    lv_obj_set_style_bg_color(backButton, lv_color_hex(0x394149), 0);

    wifiSetupStatusLabel = lv_label_create(wifiScreen);
    lv_label_set_text(wifiSetupStatusLabel, "仅支持 2.4GHz Wi-Fi");
    lv_obj_set_style_text_font(wifiSetupStatusLabel, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(wifiSetupStatusLabel, lv_color_hex(UI_MUTED), 0);
    lv_obj_align(wifiSetupStatusLabel, LV_ALIGN_TOP_RIGHT, -28, 302);
    wifiInfoLabel = lv_label_create(wifiScreen);
    lv_label_set_text(wifiInfoLabel, "SSID：未连接\nIP：--");
    lv_obj_set_width(wifiInfoLabel, 360);
    lv_obj_set_style_text_font(wifiInfoLabel, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(wifiInfoLabel, lv_color_hex(0xCBD5E1), 0);
    lv_obj_align(wifiInfoLabel, LV_ALIGN_TOP_RIGHT, -28, 334);

    lv_obj_t *phoneButton = makeButton(wifiScreen, "手机控制", showPhoneControlEvent);
    lv_obj_set_size(phoneButton, 172, 46);
    lv_obj_align(phoneButton, LV_ALIGN_TOP_RIGHT, -28, 390);
    lv_obj_set_style_text_font(lv_obj_get_child(phoneButton, 0), &ui_font_cn_16, 0);
    lv_obj_set_style_bg_color(phoneButton, lv_color_hex(0x0E9F6E), 0);

    wifiScanButton = makeButton(wifiScreen, "扫描网络", startWifiScanEvent);
    lv_obj_set_size(wifiScanButton, 330, 50);
    lv_obj_align(wifiScanButton, LV_ALIGN_TOP_LEFT, 28, 112);
    lv_obj_set_style_text_font(lv_obj_get_child(wifiScanButton, 0), &ui_font_cn_16, 0);
    lv_obj_set_style_bg_color(wifiScanButton, lv_color_hex(0x354F60), 0);

    wifiScanList = lv_obj_create(wifiScreen);
    lv_obj_set_size(wifiScanList, 330, 270);
    lv_obj_align(wifiScanList, LV_ALIGN_TOP_LEFT, 28, 176);
    lv_obj_set_style_bg_color(wifiScanList, lv_color_hex(UI_SURFACE), 0);
    lv_obj_set_style_border_color(wifiScanList, lv_color_hex(UI_EDGE), 0);
    lv_obj_set_style_radius(wifiScanList, 12, 0);
    lv_obj_set_flex_flow(wifiScanList, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(wifiScanList, 8, 0);

    keyboard = lv_keyboard_create(wifiScreen);
    lv_obj_set_size(keyboard, 800, 235);
    lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_event_cb(keyboard, keyboardEvent, LV_EVENT_ALL, nullptr);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    populateWifiScanList(0);

    phoneControlPanel = lv_obj_create(wifiScreen);
    lv_obj_set_size(phoneControlPanel, 470, 360);
    lv_obj_align(phoneControlPanel, LV_ALIGN_CENTER, 0, 28);
    lv_obj_set_style_bg_color(phoneControlPanel, lv_color_hex(UI_SURFACE), 0);
    lv_obj_set_style_bg_opa(phoneControlPanel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(phoneControlPanel, 2, 0);
    lv_obj_set_style_border_color(phoneControlPanel, lv_color_hex(UI_ACCENT), 0);
    lv_obj_set_style_radius(phoneControlPanel, 20, 0);
    lv_obj_clear_flag(phoneControlPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *phoneTitle = lv_label_create(phoneControlPanel);
    lv_label_set_text(phoneTitle, "手机扫码控制");
    // ui_font_cn_24 lacks several glyphs in this title and falls back to 16 px,
    // which makes characters in the same line appear at different sizes.
    lv_obj_set_style_text_font(phoneTitle, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(phoneTitle, lv_color_hex(UI_TEXT), 0);
    lv_obj_align(phoneTitle, LV_ALIGN_TOP_MID, 0, 2);
    phoneQrCode = lv_qrcode_create(phoneControlPanel, 210, lv_color_hex(0x071426), lv_color_hex(UI_TEXT));
    lv_obj_align(phoneQrCode, LV_ALIGN_TOP_MID, 0, 42);
    phoneUrlLabel = lv_label_create(phoneControlPanel);
    lv_label_set_text(phoneUrlLabel, "请先连接 Wi-Fi");
    lv_obj_set_width(phoneUrlLabel, 400);
    lv_obj_set_style_text_align(phoneUrlLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(phoneUrlLabel, &ui_font_cn_16, 0);
    lv_obj_set_style_text_color(phoneUrlLabel, lv_color_hex(UI_ACCENT), 0);
    lv_obj_align(phoneUrlLabel, LV_ALIGN_BOTTOM_MID, 0, -38);
    lv_obj_t *phoneClose = makeButton(phoneControlPanel, "关闭", closePhoneControlEvent);
    lv_obj_set_size(phoneClose, 90, 38);
    lv_obj_align(phoneClose, LV_ALIGN_BOTTOM_RIGHT, -10, -4);
    lv_obj_set_style_text_font(lv_obj_get_child(phoneClose, 0), &ui_font_cn_16, 0);
    lv_obj_add_flag(phoneControlPanel, LV_OBJ_FLAG_HIDDEN);
    createHydrationReminderUi();
    createStandingReminderUi();
    createGuideScreen();
}
