// Periodic UI updates and background state handoff
// Included by WuhanDeskPanel.ino; keep in the same sketch directory.

static void uiTimer(lv_timer_t *)
{
    processWebCommands();
    updateHomeTodoRotation();
    struct tm now;
    if (getLocalTime(&now, 5)) {
        char timeText[16];
        char dateText[48];
        strftime(timeText, sizeof(timeText), "%H:%M:%S", &now);
        static const char *weekdays[] = {"日", "一", "二", "三", "四", "五", "六"};
        snprintf(dateText, sizeof(dateText), "%d年%d月%d日  星期%s",
                 now.tm_year + 1900, now.tm_mon + 1, now.tm_mday, weekdays[now.tm_wday]);
        static int lastSecond = -1;
        static int lastDateKey = -1;
        int dateKey = (now.tm_year + 1900) * 10000 + (now.tm_mon + 1) * 100 + now.tm_mday;
        if (now.tm_sec != lastSecond) {
            lastSecond = now.tm_sec;
            lv_label_set_text(timeLabel, timeText);
            lv_label_set_text(dateLabel, dateText);
        }
        if (dateKey != lastDateKey) {
            lastDateKey = dateKey;
            const char *todayFestival = festivalName(now.tm_year + 1900, now.tm_mon + 1, now.tm_mday);
            bool todayOfficialRest = isOfficialRestDay(now.tm_year + 1900, now.tm_mon + 1, now.tm_mday);
            bool todayAdjustedWork = isAdjustedWorkday(now.tm_year + 1900, now.tm_mon + 1, now.tm_mday);
            String specialText;
            uint32_t specialColor = 0x7FC2FF;
            if (todayFestival[0]) {
                specialText = String("今日：") + todayFestival;
                if (todayOfficialRest) specialText += " · 法定休息";
                specialColor = todayOfficialRest ? 0xFF9AA5 : 0xFFD35A;
            } else if (todayOfficialRest) {
                specialText = "今日：法定休息日";
                specialColor = 0xFF9AA5;
            } else if (todayAdjustedWork) {
                specialText = "今日：调休工作日";
                specialColor = 0x62B5FF;
            } else if (now.tm_wday == 0 || now.tm_wday == 6) {
                specialText = "今日：周末";
                specialColor = 0xE58A9B;
            } else {
                specialText = "今日：普通工作日";
            }
            if (todaySpecialLabel != nullptr) {
                lv_label_set_text(todaySpecialLabel, specialText.c_str());
                lv_obj_set_style_text_color(todaySpecialLabel, lv_color_hex(specialColor), 0);
            }
            if (datePickerCalendar != nullptr)
                lv_calendar_set_today_date(datePickerCalendar, now.tm_year + 1900, now.tm_mon + 1, now.tm_mday);
            if (mainCalendar != nullptr)
                lv_calendar_set_today_date(mainCalendar, now.tm_year + 1900, now.tm_mon + 1, now.tm_mday);
        }
        if (calendarTodayLabel != nullptr && !calendarSelectionActive) {
            static int lastCalendarDateKey = -1;
            if (lastCalendarDateKey != dateKey) {
                lastCalendarDateKey = dateKey;
                bool restDay = isRestDate(now.tm_year + 1900, now.tm_mon + 1, now.tm_mday, now.tm_wday);
                char todayText[64];
                snprintf(todayText, sizeof(todayText), "%d月%d日\n星期%s\n\n%s",
                         now.tm_mon + 1, now.tm_mday, weekdays[now.tm_wday], restDay ? "休息日" : "工作日");
                lv_label_set_text(calendarTodayLabel, todayText);
                lv_obj_set_style_text_color(calendarTodayLabel,
                                            lv_color_hex(restDay ? 0xFF7B87 : 0x72D6FF), 0);
            }
        }
    }

    if (getLocalTime(&now, 0)) runHydrationReminder(now);

    if (mainCalendar != nullptr) {
        static uint32_t highlightedYear = 0, highlightedMonth = 0;
        const lv_calendar_date_t *shown = lv_calendar_get_showed_date(mainCalendar);
        if (shown != nullptr && (shown->year != highlightedYear || shown->month != highlightedMonth)) {
            highlightedYear = shown->year;
            highlightedMonth = shown->month;
            updateWeekendHighlights(shown->year, shown->month);
            updateCalendarAgenda(shown->year, shown->month);
        }
    }

    if (wifiScanRunning) {
        int scanResult = WiFi.scanComplete();
        if (scanResult >= 0) populateWifiScanList(scanResult);
        else if (scanResult == WIFI_SCAN_FAILED) {
            wifiScanRunning = false;
            wifiScanStarted = 0;
            WiFi.setAutoReconnect(true);
            lv_obj_clear_state(wifiScanButton, LV_STATE_DISABLED);
            lv_label_set_text(wifiSetupStatusLabel, "扫描失败，请重试");
        } else if (millis() - wifiScanStarted > 20000UL) {
            WiFi.scanDelete();
            wifiScanRunning = false;
            wifiScanStarted = 0;
            WiFi.setAutoReconnect(true);
            lv_obj_clear_state(wifiScanButton, LV_STATE_DISABLED);
            lv_label_set_text(wifiSetupStatusLabel, "扫描超时，请靠近路由器后重试");
        }
    }

    static wl_status_t previous = WL_NO_SHIELD;
    wl_status_t current = WiFi.status();
    if (current != previous) {
        previous = current;
        if (current == WL_CONNECTED) {
            wifiConnectStarted = 0;
            rememberConnectedNetwork(savedSsid, savedPassword);
            lv_label_set_text(wifiLabel, LV_SYMBOL_WIFI);
            lv_obj_set_style_text_color(wifiLabel, lv_color_hex(0x4ADE80), 0);
            String info = "SSID：" + WiFi.SSID() + "\nIP：" + WiFi.localIP().toString();
            if (wifiInfoLabel != nullptr) lv_label_set_text(wifiInfoLabel, info.c_str());
            lv_label_set_text(wifiSetupStatusLabel, "连接成功，时间和天气正在更新");
            lastLocationCheck = 0;
            startWeatherUpdate();
            updatePhoneQr();
        } else if (!savedSsid.isEmpty()) {
            if (wifiConnectStarted && millis() - wifiConnectStarted > 20000) {
                lv_label_set_text(wifiLabel, LV_SYMBOL_WIFI);
                lv_obj_set_style_text_color(wifiLabel, lv_color_hex(0xEF4444), 0);
                lv_label_set_text(wifiSetupStatusLabel, "连接失败，请检查名称和密码");
            } else {
                lv_label_set_text(wifiLabel, LV_SYMBOL_WIFI);
                lv_obj_set_style_text_color(wifiLabel, lv_color_hex(0xFBBF24), 0);
            }
        } else {
            lv_label_set_text(wifiLabel, LV_SYMBOL_WIFI);
            lv_obj_set_style_text_color(wifiLabel, lv_color_hex(0x64748B), 0);
        }
    }

    if (weatherReady) {
        char temp[24], detail[96], updated[40];
        char daily[7][80];
        char reminder[64];
        char icon[8], dailyIcons[7][8];
        portENTER_CRITICAL(&weatherMux);
        strlcpy(temp, weatherTemp, sizeof(temp));
        strlcpy(detail, weatherDetail, sizeof(detail));
        strlcpy(updated, weatherUpdated, sizeof(updated));
        for (int i = 0; i < 7; ++i) strlcpy(daily[i], forecastLines[i], sizeof(daily[i]));
        strlcpy(reminder, weatherReminder, sizeof(reminder));
        strlcpy(icon, currentWeatherIcon, sizeof(icon));
        for (int i = 0; i < 7; ++i) strlcpy(dailyIcons[i], forecastIcons[i], sizeof(dailyIcons[i]));
        weatherReady = false;
        portEXIT_CRITICAL(&weatherMux);
        lv_label_set_text(weatherTempLabel, temp);
        lv_label_set_text(weatherDetailLabel, detail);
        lv_label_set_text(weatherUpdateLabel, updated);
        if (homeWeatherIconLabel != nullptr) lv_label_set_text(homeWeatherIconLabel, icon);
        if (homeWeatherReminderLabel != nullptr) lv_label_set_text(homeWeatherReminderLabel, reminder);
        for (int i = 0; i < 7; ++i)
            if (forecastDayLabels[i] != nullptr) {
                lv_label_set_text(forecastDayLabels[i], daily[i]);
                if (forecastIconLabels[i] != nullptr) lv_label_set_text(forecastIconLabels[i], dailyIcons[i]);
            }
        if (forecastStatusLabel != nullptr) lv_label_set_text(forecastStatusLabel, updated);
        if (homeWeatherCityLabel != nullptr) lv_label_set_text(homeWeatherCityLabel, weatherCityName);
        if (forecastCityLabel != nullptr) {
            String title = String(weatherCityName) + "未来七天天气";
            lv_label_set_text(forecastCityLabel, title.c_str());
        }
    }

    if (weatherFailureReady) {
        char failure[80];
        bool hasData;
        portENTER_CRITICAL(&weatherMux);
        strlcpy(failure, weatherFailureMessage, sizeof(failure));
        hasData = weatherHasData;
        weatherFailureReady = false;
        portEXIT_CRITICAL(&weatherMux);
        if (!hasData && weatherDetailLabel != nullptr) lv_label_set_text(weatherDetailLabel, failure);
        if (weatherUpdateLabel != nullptr) lv_label_set_text(weatherUpdateLabel, failure);
        if (homeWeatherReminderLabel != nullptr) lv_label_set_text(homeWeatherReminderLabel, failure);
        if (forecastStatusLabel != nullptr) lv_label_set_text(forecastStatusLabel, failure);
    }

    if (currentWorkState == 2 || currentWorkState == 3 || currentWorkState == 5 || currentWorkState == 6 || currentWorkState == 7) {
        int64_t remaining = 0;
        time_t nowEpoch = time(nullptr);
        if (stateUntilEpoch > 0 && nowEpoch > 100000) remaining = (int64_t)stateUntilEpoch - nowEpoch;
        else if (stateUntilMillis > 0) remaining = ((int64_t)stateUntilMillis - (int64_t)millis()) / 1000;
        if (remaining <= 0 && (stateUntilEpoch > 0 || stateUntilMillis > 0)) {
            currentWorkState = 0;
            stateUntilEpoch = 0;
            stateUntilMillis = 0;
            prefs.putUChar("state", 0);
            prefs.remove("stateUntil");
            updateStatusUi(0);
            lv_label_set_text(statusUntilLabel, "");
            showToast("定时状态已结束，恢复上班");
        } else if (remaining > 0) {
            char remainingText[48];
            int days = (int)(remaining / 86400);
            int hours = (int)((remaining % 86400) / 3600);
            int minutes = (int)((remaining % 3600) / 60);
            if (currentWorkState == 7) snprintf(remainingText, sizeof(remainingText), "午休将在 %02u:%02u 结束", lunchEndMinutes / 60, lunchEndMinutes % 60);
            else if (days > 0) snprintf(remainingText, sizeof(remainingText), "剩余 %d天%d小时", days, hours);
            else snprintf(remainingText, sizeof(remainingText), "剩余 %d小时%d分", hours, minutes);
            lv_label_set_text(statusUntilLabel, remainingText);
        }
    }

    if (countdownRunning) {
        int32_t ms = (int32_t)(countdownEndMillis - millis());
        if (ms <= 0) {
            countdownRunning = false;
            countdownRemainingSeconds = 0;
            lv_label_set_text(countdownLabel, "00:00:00");
            lv_label_set_text(countdownStartButtonLabel, "开始");
            showToast("倒计时结束");
        } else {
            countdownRemainingSeconds = (uint32_t)(ms + 999) / 1000;
            uint32_t hours = countdownRemainingSeconds / 3600;
            uint32_t minutes = (countdownRemainingSeconds % 3600) / 60;
            uint32_t seconds = countdownRemainingSeconds % 60;
            char countdownText[20];
            snprintf(countdownText, sizeof(countdownText), "%02lu:%02lu:%02lu",
                     (unsigned long)hours, (unsigned long)minutes, (unsigned long)seconds);
            lv_label_set_text(countdownLabel, countdownText);
        }
    }
}
