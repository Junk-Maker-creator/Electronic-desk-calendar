// Remembered Wi-Fi credential storage
// Included by WuhanDeskPanel.ino; keep in the same sketch directory.

static void loadRememberedNetworks()
{
    for (int i = 0; i < 5; ++i) {
        rememberedSsids[i] = prefs.getString((String("netS") + i).c_str(), "");
        rememberedPasswords[i] = prefs.getString((String("netP") + i).c_str(), "");
    }
    // Migrate installations that used the original single-network storage.
    if (rememberedSsids[0].isEmpty() && !savedSsid.isEmpty()) {
        rememberedSsids[0] = savedSsid;
        rememberedPasswords[0] = savedPassword;
        prefs.putString("netS0", savedSsid);
        prefs.putString("netP0", savedPassword);
    }
}

static void rememberConnectedNetwork(const String &ssid, const String &password)
{
    if (ssid.isEmpty()) return;
    int existing = -1;
    for (int i = 0; i < 5; ++i) if (rememberedSsids[i] == ssid) { existing = i; break; }
    if (existing < 0) existing = 4;
    for (int i = existing; i > 0; --i) {
        rememberedSsids[i] = rememberedSsids[i - 1];
        rememberedPasswords[i] = rememberedPasswords[i - 1];
    }
    rememberedSsids[0] = ssid;
    rememberedPasswords[0] = password;
    for (int i = 0; i < 5; ++i) {
        prefs.putString((String("netS") + i).c_str(), rememberedSsids[i]);
        prefs.putString((String("netP") + i).c_str(), rememberedPasswords[i]);
    }
    prefs.putString("ssid", ssid);
    prefs.putString("pass", password);
}

static void showToast(const char *text)
{
    lv_label_set_text(toastLabel, text);
    lv_obj_set_style_opa(toastLabel, LV_OPA_COVER, 0);
    lv_obj_clear_flag(toastLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_fade_out(toastLabel, 400, 2200);
}
