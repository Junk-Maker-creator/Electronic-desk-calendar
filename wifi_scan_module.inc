// Wi-Fi scanning and remembered-network selection
// Included by WuhanDeskPanel.ino; keep in the same sketch directory.

static void selectNetworkEvent(lv_event_t *e)
{
    intptr_t index = reinterpret_cast<intptr_t>(lv_event_get_user_data(e));
    if (index < 0 || index >= 12 || scannedSsids[index].isEmpty()) return;
    lv_textarea_set_text(ssidArea, scannedSsids[index].c_str());
    bool passwordFound = false;
    for (int i = 0; i < 5; ++i) {
        if (rememberedSsids[i] == scannedSsids[index]) {
            lv_textarea_set_text(passwordArea, rememberedPasswords[i].c_str());
            passwordFound = true;
            break;
        }
    }
    if (!passwordFound) lv_textarea_set_text(passwordArea, "");
    lv_label_set_text(wifiSetupStatusLabel, passwordFound ? "已载入保存的密码，可直接连接" : "已选择网络，请输入密码");
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
}

static void startWifiScanEvent(lv_event_t *)
{
    if (wifiScanRunning) return;
    // Do not let the background reconnect state machine contend with the
    // radio while an asynchronous scan is in progress.
    WiFi.setAutoReconnect(false);
    WiFi.disconnect(false, false);
    delay(100);
    WiFi.scanDelete();
    WiFi.mode(WIFI_STA);
    int16_t result = WiFi.scanNetworks(true, true);
    if (result == WIFI_SCAN_RUNNING) {
        wifiScanRunning = true;
        wifiScanStarted = millis();
        lv_label_set_text(wifiSetupStatusLabel, "正在扫描附近的 2.4GHz Wi-Fi...");
        lv_obj_add_state(wifiScanButton, LV_STATE_DISABLED);
    } else if (result >= 0) {
        populateWifiScanList(result);
    } else {
        WiFi.setAutoReconnect(true);
        lv_label_set_text(wifiSetupStatusLabel, "扫描未能启动，请稍后重试");
    }
}

static void populateWifiScanList(int count)
{
    lv_obj_clean(wifiScanList);
    for (int i = 0; i < 12; ++i) scannedSsids[i] = "";
    int shown = 0;
    for (int i = 0; i < 5 && shown < 12; ++i) {
        if (rememberedSsids[i].isEmpty()) continue;
        scannedSsids[shown] = rememberedSsids[i];
        String line = rememberedSsids[i] + "   已保存";
        lv_obj_t *button = lv_btn_create(wifiScanList);
        lv_obj_set_width(button, lv_pct(100));
        lv_obj_set_height(button, 42);
        lv_obj_set_style_bg_color(button, lv_color_hex(0x155E4B), 0);
        lv_obj_set_style_radius(button, 8, 0);
        lv_obj_add_event_cb(button, selectNetworkEvent, LV_EVENT_CLICKED, reinterpret_cast<void *>(shown));
        lv_obj_t *label = lv_label_create(button);
        lv_label_set_text(label, line.c_str());
        lv_obj_set_style_text_font(label, &ui_font_cn_16, 0);
        lv_obj_set_style_text_color(label, lv_color_white(), 0);
        lv_obj_center(label);
        ++shown;
    }
    for (int i = 0; i < count && shown < 12; ++i) {
        String ssid = WiFi.SSID(i);
        if (ssid.isEmpty()) continue;
        bool duplicate = false;
        for (int j = 0; j < shown; ++j) if (scannedSsids[j] == ssid) { duplicate = true; break; }
        if (duplicate) continue;
        scannedSsids[shown] = ssid;
        String line = ssid + "   " + String(WiFi.RSSI(i)) + " dBm";
        if (WiFi.encryptionType(i) != WIFI_AUTH_OPEN) line += "  *";
        lv_obj_t *button = lv_btn_create(wifiScanList);
        lv_obj_set_width(button, lv_pct(100));
        lv_obj_set_height(button, 42);
        lv_obj_set_style_bg_color(button, lv_color_hex(0x1E3A5F), 0);
        lv_obj_set_style_radius(button, 8, 0);
        lv_obj_add_event_cb(button, selectNetworkEvent, LV_EVENT_CLICKED, reinterpret_cast<void *>(shown));
        lv_obj_t *label = lv_label_create(button);
        lv_label_set_text(label, line.c_str());
        lv_obj_set_style_text_font(label, &ui_font_cn_16, 0);
        lv_obj_set_style_text_color(label, lv_color_white(), 0);
        lv_obj_center(label);
        ++shown;
    }
    lv_label_set_text(wifiSetupStatusLabel, shown > 0 ? "点击网络名称即可选择" : "未发现网络，请重新扫描");
    WiFi.scanDelete();
    WiFi.setAutoReconnect(true);
    wifiScanRunning = false;
    wifiScanStarted = 0;
    lv_obj_clear_state(wifiScanButton, LV_STATE_DISABLED);
}
