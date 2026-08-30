#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include <time.h>

#include <esp_display_panel.hpp>
#include <esp_err.h>
#include <lvgl.h>

#include "esp_lv_adapter_arduino.h"

LV_FONT_DECLARE(ui_font_cn_16);
LV_FONT_DECLARE(ui_font_todo_cjk_16);
LV_FONT_DECLARE(ui_font_say_16);
LV_FONT_DECLARE(ui_font_cn_24);
LV_FONT_DECLARE(ui_font_cn_32);
LV_FONT_DECLARE(ui_font_blessing_32);
LV_FONT_DECLARE(ui_font_units_14);
LV_FONT_DECLARE(ui_font_weather_64);
LV_FONT_DECLARE(ui_font_weather_36);

using namespace esp_panel::drivers;
using namespace esp_panel::board;

// Wuhan, China. Open-Meteo does not require an API key.  Use HTTP here: on
// several local Wi-Fi networks the ESP32-S3 TLS handshake to this public API
// intermittently fails, while the same public forecast endpoint supports HTTP.
// Weather data is public and contains no credentials or user information.
static const char *WEATHER_URL =
    "http://api.open-meteo.com/v1/forecast?latitude=30.5928&longitude=114.3055"
    "&current=temperature_2m,relative_humidity_2m,weather_code,wind_speed_10m"
    "&daily=weather_code,temperature_2m_max,temperature_2m_min,precipitation_probability_max,precipitation_sum"
    "&forecast_days=7"
    "&timezone=Asia%2FShanghai";

Preferences prefs;
Board *panelBoard = nullptr;
String savedSsid;
String savedPassword;

lv_obj_t *timeLabel;
lv_obj_t *dateLabel;
lv_obj_t *todaySpecialLabel;
lv_obj_t *wifiLabel;
lv_obj_t *wifiInfoLabel;
lv_obj_t *weatherTempLabel;
lv_obj_t *weatherDetailLabel;
lv_obj_t *weatherUpdateLabel;
lv_obj_t *homeWeatherIconLabel;
lv_obj_t *statusLabel;
lv_obj_t *ssidArea;
lv_obj_t *passwordArea;
lv_obj_t *keyboard;
lv_obj_t *toastLabel;
lv_obj_t *wifiPanel;
lv_obj_t *statusButtons[9] = {};
lv_obj_t *dashboardScreen;
lv_obj_t *wifiScreen;
lv_obj_t *todoScreen;
lv_obj_t *calendarScreen;
lv_obj_t *muyuScreen;
lv_obj_t *forecastScreen;
lv_obj_t *countdownScreen;
lv_obj_t *guideScreen;
lv_obj_t *countdownLabel;
lv_obj_t *countdownHourRoller;
lv_obj_t *countdownMinuteRoller;
lv_obj_t *countdownSecondRoller;
lv_obj_t *countdownStartButtonLabel;
lv_obj_t *muyuBody;
lv_obj_t *muyuStick;
lv_obj_t *muyuCountLabel;
lv_obj_t *muyuMeritLabel;
lv_obj_t *muyuBlessingCard;
lv_obj_t *muyuBlessingHalo;
lv_obj_t *muyuParticles[8] = {};
lv_obj_t *homeWeatherReminderLabel;
lv_obj_t *forecastDayLabels[7] = {};
lv_obj_t *forecastIconLabels[7] = {};
lv_obj_t *forecastStatusLabel;
lv_obj_t *homeWeatherCityLabel;
lv_obj_t *forecastCityLabel;
lv_obj_t *wifiSetupStatusLabel;
lv_obj_t *wifiScanList;
lv_obj_t *wifiScanButton;
lv_obj_t *phoneControlPanel;
lv_obj_t *phoneQrCode;
lv_obj_t *phoneUrlLabel;
static String scannedSsids[12];
static String rememberedSsids[5];
static String rememberedPasswords[5];
static bool wifiScanRunning = false;
static uint32_t wifiScanStarted = 0;
lv_obj_t *durationPanel;
lv_obj_t *dayAdjustPanel;
lv_obj_t *dayCountLabel;
lv_obj_t *statusUntilLabel;
static uint8_t pendingTimedState = 0;
static time_t stateUntilEpoch = 0;
static uint32_t stateUntilMillis = 0;
static uint16_t pendingDays = 1;
static uint32_t countdownRemainingSeconds = 0;
static uint32_t countdownEndMillis = 0;
static bool countdownRunning = false;
// Phone-configurable daily reminders. Times are minutes since midnight.
static uint16_t hydrationStartMinutes = 8 * 60;
static uint16_t hydrationEndMinutes = 18 * 60;
static uint16_t hydrationIntervalMinutes = 2 * 60;
static uint32_t hydrationLastSlotKey = 0;
static uint16_t lunchEndMinutes = 13 * 60;
static uint32_t muyuCount = 0;
static uint32_t lastMuyuTap = 0;

lv_obj_t *todoList;
lv_obj_t *todoTextArea;
lv_obj_t *todoTimeArea;
lv_obj_t *todoKeyboard;
lv_obj_t *todoIme;
lv_obj_t *todoStatusLabel;
lv_obj_t *durationTitleLabel;
lv_obj_t *todoCalendar;
lv_obj_t *datePickerPanel;
lv_obj_t *datePickerCalendar;
lv_obj_t *datePickerValueLabel;
lv_obj_t *mainCalendar;
lv_obj_t *calendarTodayLabel;
lv_obj_t *calendarMarkerPanel;
lv_obj_t *calendarMarkerDateLabel;
lv_obj_t *calendarMarkerRoller;
lv_obj_t *calendarMarkerNoteArea;
lv_obj_t *calendarMarkerKeyboard;
lv_obj_t *calendarMarkerIme;
lv_obj_t *calendarDetailLabel;
lv_obj_t *calendarMonthAgendaLabel;
lv_obj_t *calendarMarkButton;
lv_obj_t *statusPickerPanel;
lv_obj_t *homeTodoPreviewLabel;
lv_obj_t *homeTodoCard;
lv_obj_t *homeTodoPauseLabel;
lv_obj_t *todoTypingLabel;
static lv_calendar_date_t weekendDates[24];
static char pendingDeadline[20] = "";
static char pendingMarkerDate[20] = "";
static bool calendarSelectionActive = false;
struct SpecialDay { String date; String note; };
static SpecialDay specialDays[12];
struct TodoItem {
    String text;
    String deadline;
    bool done;
};
static TodoItem todoItems[8];
static uint8_t currentWorkState = 0;
static bool homeTodoRotationPaused = false;
static uint8_t homeTodoRotationIndex = 0;
static uint32_t lastHomeTodoRotation = 0;
static uint32_t wifiConnectStarted = 0;

static void updateWeekendHighlights(uint32_t year, uint32_t month);
static void updateCalendarAgenda(uint32_t year, uint32_t month);
static void populateWifiScanList(int count);
static void showPhoneControlEvent(lv_event_t *e);
static void closePhoneControlEvent(lv_event_t *e);
static void updatePhoneQr();
static void showGuideScreenEvent(lv_event_t *e);
static void createGuideScreen();
static void createHydrationReminderUi();
static void runHydrationReminder(const struct tm &now);
static bool startLunchBreak();

static portMUX_TYPE weatherMux = portMUX_INITIALIZER_UNLOCKED;
static char weatherTemp[24] = "--.- C";
static char weatherDetail[96] = "Waiting for network...";
static char weatherUpdated[40] = "Not updated";
static char forecastLines[7][80] = {};
static char weatherReminder[64] = "未来天气加载中";
static char currentWeatherIcon[8] = "☀";
static char forecastIcons[7][8] = {};
static volatile bool weatherReady = false;
static volatile bool weatherTaskRunning = false;
static volatile bool weatherFailureReady = false;
static bool weatherHasData = false;
static uint32_t lastWeatherSuccess = 0;
static uint32_t lastWeatherFailure = 0;
static char weatherFailureMessage[80] = "天气数据暂不可用，请检查 Wi-Fi";
static uint32_t lastWeatherAttempt = 0;
static uint32_t lastLocationCheck = 0;
static char weatherCityCode[16] = "101200101";
static char weatherCityName[32] = "武汉";

static const char *workStates[] = {"上班", "忙碌", "开会", "休息", "下班", "出差", "休假", "午休", "厕所"};
static const uint32_t stateColors[] = {0x1677FF, 0xF59E0B, 0x7C3AED, 0x06A6B8, 0x64748B, 0xE45B8B, 0x16A085, 0xD97706, 0x0D9488};

#include "wifi_storage_module.inc"

#include "weather_module.inc"

#include "wifi_events_module.inc"

static void updateStatusUi(uint8_t index)
{
    currentWorkState = index;
    lv_label_set_text(statusLabel, workStates[index]);
    lv_obj_set_style_bg_color(statusLabel, lv_color_hex(stateColors[index]), 0);
    for (uint8_t i = 0; i < 9; ++i) {
        if (statusButtons[i] == nullptr) continue;
        lv_obj_set_style_border_width(statusButtons[i], i == index ? 3 : 0, 0);
        lv_obj_set_style_border_color(statusButtons[i], lv_color_white(), 0);
        lv_obj_set_style_opa(statusButtons[i], i == index ? LV_OPA_COVER : LV_OPA_70, 0);
    }
}

static void statusEvent(lv_event_t *e)
{
    intptr_t index = reinterpret_cast<intptr_t>(lv_event_get_user_data(e));
    if (index < 0 || index >= 9) return;
    if (index == 2 || index == 3) {
        pendingTimedState = (uint8_t)index;
        if (statusPickerPanel != nullptr) lv_obj_add_flag(statusPickerPanel, LV_OBJ_FLAG_HIDDEN);
        if (durationTitleLabel != nullptr) {
            String title = String(workStates[index]) + " - 选择持续时间";
            lv_label_set_text(durationTitleLabel, title.c_str());
        }
        lv_obj_clear_flag(durationPanel, LV_OBJ_FLAG_HIDDEN);
        return;
    }
    if (index == 5 || index == 6) {
        pendingTimedState = (uint8_t)index;
        pendingDays = 1;
        lv_label_set_text(dayCountLabel, "1 天");
        lv_obj_clear_flag(dayAdjustPanel, LV_OBJ_FLAG_HIDDEN);
        return;
    }
    if (index == 7) {
        if (statusPickerPanel != nullptr) lv_obj_add_flag(statusPickerPanel, LV_OBJ_FLAG_HIDDEN);
        startLunchBreak();
        return;
    }
    stateUntilEpoch = 0;
    stateUntilMillis = 0;
    prefs.remove("stateUntil");
    prefs.putUChar("state", (uint8_t)index);
    updateStatusUi((uint8_t)index);
    if (statusPickerPanel != nullptr) lv_obj_add_flag(statusPickerPanel, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(statusUntilLabel, "");
    showToast("工作状态已更新");
}

static void dayChangeEvent(lv_event_t *e)
{
    int delta = (int)reinterpret_cast<intptr_t>(lv_event_get_user_data(e));
    int next = (int)pendingDays + delta;
    if (next < 1) next = 1;
    if (next > 365) next = 365;
    pendingDays = (uint16_t)next;
    char text[20];
    snprintf(text, sizeof(text), "%u 天", pendingDays);
    lv_label_set_text(dayCountLabel, text);
}

static void confirmDaysEvent(lv_event_t *)
{
    uint32_t seconds = (uint32_t)pendingDays * 86400UL;
    time_t now = time(nullptr);
    if (now > 100000) {
        stateUntilEpoch = now + seconds;
        prefs.putULong64("stateUntil", (uint64_t)stateUntilEpoch);
    } else {
        stateUntilEpoch = 0;
        stateUntilMillis = millis() + seconds * 1000UL;
    }
    prefs.putUChar("state", pendingTimedState);
    updateStatusUi(pendingTimedState);
    lv_obj_add_flag(dayAdjustPanel, LV_OBJ_FLAG_HIDDEN);
    showToast("按天状态已设置");
}

static void closeDaysEvent(lv_event_t *) { lv_obj_add_flag(dayAdjustPanel, LV_OBJ_FLAG_HIDDEN); }

static void durationEvent(lv_event_t *e)
{
    uint32_t seconds = (uint32_t)reinterpret_cast<uintptr_t>(lv_event_get_user_data(e));
    time_t now = time(nullptr);
    if (now > 100000) {
        stateUntilEpoch = now + seconds;
        prefs.putULong64("stateUntil", (uint64_t)stateUntilEpoch);
    } else {
        stateUntilEpoch = 0;
        stateUntilMillis = millis() + seconds * 1000UL;
    }
    prefs.putUChar("state", pendingTimedState);
    updateStatusUi(pendingTimedState);
    lv_obj_add_flag(durationPanel, LV_OBJ_FLAG_HIDDEN);
    showToast("定时状态已设置");
}

static void closeDurationEvent(lv_event_t *)
{
    lv_obj_add_flag(durationPanel, LV_OBJ_FLAG_HIDDEN);
}

static void showWifiScreenEvent(lv_event_t *)
{
    lv_scr_load(wifiScreen);
}

static void showDashboardEvent(lv_event_t *)
{
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    lv_scr_load(dashboardScreen);
}

static void showCalendarScreenEvent(lv_event_t *)
{
    lv_scr_load(calendarScreen);
}

static void showTodoScreenEvent(lv_event_t *)
{
    lv_scr_load(todoScreen);
}

static void showMuyuScreenEvent(lv_event_t *) { lv_scr_load(muyuScreen); }
static void showForecastScreenEvent(lv_event_t *) { lv_scr_load(forecastScreen); }
static void showCountdownScreenEvent(lv_event_t *) { lv_scr_load(countdownScreen); }

#include "muyu_module.inc"

#include "hydration_break_module.inc"

static void countdownSetEvent(lv_event_t *)
{
    countdownRunning = false;
    countdownRemainingSeconds = lv_roller_get_selected(countdownHourRoller) * 3600UL +
                                lv_roller_get_selected(countdownMinuteRoller) * 60UL +
                                lv_roller_get_selected(countdownSecondRoller);
    if (countdownRemainingSeconds == 0) countdownRemainingSeconds = 60;
    countdownEndMillis = millis() + countdownRemainingSeconds * 1000UL;
    countdownRunning = true;
    lv_label_set_text(countdownStartButtonLabel, "暂停");
}

static void countdownToggleEvent(lv_event_t *)
{
    if (!countdownRunning) {
        if (countdownRemainingSeconds == 0) { countdownSetEvent(nullptr); return; }
        countdownEndMillis = millis() + countdownRemainingSeconds * 1000UL;
        countdownRunning = true;
        lv_label_set_text(countdownStartButtonLabel, "暂停");
    } else {
        int32_t ms = (int32_t)(countdownEndMillis - millis());
        countdownRemainingSeconds = ms > 0 ? (uint32_t)(ms + 999) / 1000 : 0;
        countdownRunning = false;
        lv_label_set_text(countdownStartButtonLabel, "继续");
    }
}

static void countdownResetEvent(lv_event_t *)
{
    countdownRunning = false;
    countdownRemainingSeconds = 0;
    lv_label_set_text(countdownLabel, "00:00:00");
    lv_label_set_text(countdownStartButtonLabel, "开始");
}

#include "wifi_scan_module.inc"

#include "calendar_todo_module.inc"

#include "ui_module.inc"

#include "guide_module.inc"

#include "web_control_module.inc"

#include "runtime_module.inc"

void setup()
{
    Serial.begin(115200);
    delay(300);
    Serial.println("[BOOT] Wuhan Desk Panel starting");
    prefs.begin("desk-panel", false);
    savedSsid = prefs.getString("ssid", "");
    savedPassword = prefs.getString("pass", "");
    hydrationStartMinutes = prefs.getUShort("drinkStart", hydrationStartMinutes);
    hydrationEndMinutes = prefs.getUShort("drinkEnd", hydrationEndMinutes);
    hydrationIntervalMinutes = prefs.getUShort("drinkEvery", hydrationIntervalMinutes);
    lunchEndMinutes = prefs.getUShort("lunchEnd", lunchEndMinutes);
    hydrationLastSlotKey = prefs.getULong("drinkSlot", 0);
    loadRememberedNetworks();
    stateUntilEpoch = (time_t)prefs.getULong64("stateUntil", 0);

    Board *board = new Board();
    panelBoard = board;
    Serial.println("[BOOT] Initializing board");
    if (board == nullptr || !board->init()) {
        Serial.println("Board init failed");
        while (true) delay(1000);
    }

    const esp_lv_adapter_rotation_t rotation = ESP_LV_ADAPTER_ROTATE_0;
    const esp_lv_adapter_tear_avoid_mode_t tearMode = ESP_LV_ADAPTER_TEAR_AVOID_MODE_DEFAULT_RGB;
    LCD *lcd = board->getLCD();
    if (lcd == nullptr) {
        Serial.println("LCD not available");
        while (true) delay(1000);
    }
    auto *bus = lcd->getBus();
    if (bus->getBasicAttributes().type == ESP_PANEL_BUS_TYPE_RGB) {
        lcd->configFrameBufferNumber(esp_lv_adapter_get_required_frame_buffer_count(tearMode, rotation));
        static_cast<BusRGB *>(bus)->configRGB_BounceBufferSize(lcd->getFrameWidth() * 10);
    }
    assert(board->begin());
    Serial.println("[BOOT] LCD and touch ready");

    esp_lv_adapter_config_t adapterConfig = ESP_LV_ADAPTER_DEFAULT_CONFIG();
    adapterConfig.task_stack_size = 24 * 1024;
    adapterConfig.task_priority = 2;
    adapterConfig.task_core_id = ARDUINO_RUNNING_CORE;
    ESP_ERROR_CHECK(esp_lv_adapter_init(&adapterConfig));

    esp_lv_adapter_display_config_t displayConfig = ESP_LV_ADAPTER_DISPLAY_RGB_DEFAULT_CONFIG(
        lcd, lcd->getFrameWidth(), lcd->getFrameHeight(), rotation);
    displayConfig.profile.use_psram = true;
    lv_display_t *display = esp_lv_adapter_register_display(&displayConfig);
    assert(display != nullptr);
    if (board->getTouch() != nullptr) {
        esp_lv_adapter_touch_config_t touchConfig = ESP_LV_ADAPTER_TOUCH_DEFAULT_CONFIG(display, board->getTouch());
        assert(esp_lv_adapter_register_touch(&touchConfig) != nullptr);
    }
    ESP_ERROR_CHECK(esp_lv_adapter_start());
    Serial.println("[BOOT] LVGL adapter ready");

    ESP_ERROR_CHECK(esp_lv_adapter_lock(-1));
    createRecoveryUi();
    lv_timer_create(uiTimer, 100, nullptr);
    esp_lv_adapter_unlock();
    Serial.println("[BOOT] Dashboard created");

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.setAutoReconnect(true);
    if (!savedSsid.isEmpty()) {
        wifiConnectStarted = millis();
        WiFi.begin(savedSsid.c_str(), savedPassword.c_str());
    }
    configTzTime("CST-8", "ntp.aliyun.com", "pool.ntp.org", "time.cloudflare.com");
    loadTodos();
    rebuildTodoList();
    startWebControl();
    Serial.println("[BOOT] Setup complete");
}

void loop()
{
    handleWebControl();
    static uint32_t lastHeartbeat = 0;
    if (millis() - lastHeartbeat >= 1000) {
        lastHeartbeat = millis();
        Serial.printf("[RUN] ms=%lu wifi=%d heap=%u psram=%u\n",
                      (unsigned long)millis(), (int)WiFi.status(),
                      (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getFreePsram());
    }
    uint32_t weatherRefreshDelay = (lastWeatherFailure > lastWeatherSuccess) ? 60UL * 1000UL : 15UL * 60UL * 1000UL;
    if (WiFi.status() == WL_CONNECTED && !weatherTaskRunning &&
        (lastWeatherAttempt == 0 || millis() - lastWeatherAttempt > weatherRefreshDelay)) {
        startWeatherUpdate();
    }
    static uint32_t lastReconnectAttempt = 0;
    if (!wifiScanRunning && !savedSsid.isEmpty() && WiFi.status() != WL_CONNECTED &&
        millis() - lastReconnectAttempt > 30000) {
        lastReconnectAttempt = millis();
        WiFi.reconnect();
    }
    delay(10);
}
