// Wi-Fi input, connection and refresh events
// Included by WuhanDeskPanel.ino; keep in the same sketch directory.

static void keyboardEvent(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_READY || lv_event_get_code(e) == LV_EVENT_CANCEL) {
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    }
}

static void textAreaEvent(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_FOCUSED) return;
    lv_obj_t *area = lv_event_get_target(e);
    lv_keyboard_set_textarea(keyboard, area);
    lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_TEXT_LOWER);
    lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(keyboard);
}

static void connectEvent(lv_event_t *)
{
    const char *ssid = lv_textarea_get_text(ssidArea);
    const char *pass = lv_textarea_get_text(passwordArea);
    if (strlen(ssid) == 0) {
        lv_label_set_text(wifiSetupStatusLabel, "请输入 Wi-Fi 名称");
        return;
    }
    savedSsid = ssid;
    savedPassword = pass;
    WiFi.disconnect(false, false);
    delay(100);
    WiFi.begin(savedSsid.c_str(), savedPassword.c_str());
    wifiConnectStarted = millis();
    lv_label_set_text(wifiLabel, "Wi-Fi: connecting...");
    lv_label_set_text(wifiSetupStatusLabel, "正在连接，请稍候...");
}

static void forgetEvent(lv_event_t *)
{
    prefs.remove("ssid");
    prefs.remove("pass");
    savedSsid = "";
    savedPassword = "";
    WiFi.disconnect(true);
    lv_textarea_set_text(ssidArea, "");
    lv_textarea_set_text(passwordArea, "");
    showToast("Saved Wi-Fi removed");
}

static void refreshWeatherEvent(lv_event_t *)
{
    startWeatherUpdate();
    showToast("Refreshing Wuhan weather...");
}

static void openWifiEvent(lv_event_t *)
{
    lv_obj_clear_flag(wifiPanel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(wifiPanel);
}

static void closeWifiEvent(lv_event_t *)
{
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(wifiPanel, LV_OBJ_FLAG_HIDDEN);
}
