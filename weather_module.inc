// Open-Meteo JSON primary weather source and China Weather fallback
// Included by WuhanDeskPanel.ino; keep in the same sketch directory.

static String jsonNumber(const String &json, const char *key)
{
    int p = json.indexOf(key);
    if (p < 0) return "";
    p += strlen(key);
    while (p < (int)json.length() && (json[p] == ' ' || json[p] == ':')) p++;
    int e = p;
    while (e < (int)json.length() && (isDigit(json[e]) || json[e] == '-' || json[e] == '.')) e++;
    return json.substring(p, e);
}

static String jsonArrayItem(const String &json, const char *key, int index)
{
    int p = json.indexOf(key);
    if (p < 0 || (p = json.indexOf('[', p)) < 0) return "";
    ++p;
    for (int item = 0; item <= index; ++item) {
        while (p < (int)json.length() && (json[p] == ' ' || json[p] == ',')) ++p;
        bool quoted = p < (int)json.length() && json[p] == '"';
        if (quoted) ++p;
        int e = p;
        while (e < (int)json.length() && (quoted ? json[e] != '"' : json[e] != ',' && json[e] != ']')) ++e;
        if (item == index) return json.substring(p, e);
        p = quoted ? e + 1 : e;
    }
    return "";
}

static String jsonString(const String &json, const char *key)
{
    int p = json.indexOf(key);
    if (p < 0 || (p = json.indexOf(':', p)) < 0) return "";
    p = json.indexOf('"', p);
    if (p < 0) return "";
    int e = json.indexOf('"', ++p);
    return e < 0 ? "" : json.substring(p, e);
}

static String urlEncode(const String &value)
{
    static const char hex[] = "0123456789ABCDEF";
    String out;
    for (size_t i = 0; i < value.length(); ++i) {
        uint8_t c = (uint8_t)value[i];
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_') out += (char)c;
        else { out += '%'; out += hex[c >> 4]; out += hex[c & 15]; }
    }
    return out;
}

static void publishWeatherFailure(const char *message)
{
    portENTER_CRITICAL(&weatherMux);
    strlcpy(weatherFailureMessage, message, sizeof(weatherFailureMessage));
    weatherFailureReady = true;
    lastWeatherFailure = millis();
    portEXIT_CRITICAL(&weatherMux);
    Serial.printf("[WEATHER] %s\n", message);
}

static void discoverWeatherCity()
{
    WiFiClient plainClient;
    HTTPClient locationHttp;
    locationHttp.setTimeout(8000);
    if (!locationHttp.begin(plainClient, "http://ip-api.com/json/?lang=zh-CN&fields=status,city")) return;
    if (locationHttp.GET() != HTTP_CODE_OK) { locationHttp.end(); return; }
    String city = jsonString(locationHttp.getString(), "\"city\"");
    locationHttp.end();
    city.replace("市", "");
    if (city.isEmpty()) return;

    WiFiClientSecure searchClient;
    searchClient.setInsecure();
    HTTPClient searchHttp;
    searchHttp.setTimeout(8000);
    searchHttp.setUserAgent("Mozilla/5.0");
    String url = String("https://toy1.weather.com.cn/search?cityname=") + urlEncode(city);
    if (!searchHttp.begin(searchClient, url)) return;
    searchHttp.addHeader("Referer", "https://www.weather.com.cn/");
    if (searchHttp.GET() != HTTP_CODE_OK) { searchHttp.end(); return; }
    String result = searchHttp.getString();
    searchHttp.end();
    int p = result.indexOf("\"ref\":\"");
    if (p < 0) return;
    p += 7;
    String code = result.substring(p, p + 9);
    if (code.length() == 9) {
        strlcpy(weatherCityCode, code.c_str(), sizeof(weatherCityCode));
        strlcpy(weatherCityName, city.c_str(), sizeof(weatherCityName));
        lastLocationCheck = millis();
    }
}

static String htmlBetween(const String &html, const char *start, const char *end)
{
    int p = html.indexOf(start);
    if (p < 0) return "";
    p += strlen(start);
    int e = html.indexOf(end, p);
    return e < 0 ? "" : html.substring(p, e);
}

static const char *weatherIconFor(const String &text)
{
    if (text.indexOf("雷") >= 0) return "⚡";
    if (text.indexOf("雪") >= 0) return "❄";
    if (text.indexOf("雨") >= 0) return "☂";
    if (text.indexOf("雾") >= 0 || text.indexOf("霾") >= 0 || text.indexOf("沙") >= 0) return "≋";
    if (text.indexOf("阴") >= 0) return "☁";
    if (text.indexOf("云") >= 0) return "☁";
    return "☀";
}

static String normalizeWeatherText(const String &text)
{
    String normalized = text;
    normalized.replace("转", "→");
    return normalized;
}

static bool fetchChinaWeather(WiFiClientSecure &client)
{
    if (lastLocationCheck == 0 || millis() - lastLocationCheck > 6UL * 60UL * 60UL * 1000UL) discoverWeatherCity();
    HTTPClient currentHttp;
    currentHttp.setTimeout(12000);
    currentHttp.setUserAgent("Mozilla/5.0");
    String currentUrl = String("https://d1.weather.com.cn/weather_index/") + weatherCityCode + ".html";
    if (!currentHttp.begin(client, currentUrl)) return false;
    currentHttp.addHeader("Referer", "https://www.weather.com.cn/");
    if (currentHttp.GET() != HTTP_CODE_OK) { currentHttp.end(); return false; }
    String currentBody = currentHttp.getString();
    currentHttp.end();
    int skPos = currentBody.indexOf("var dataSK =");
    if (skPos < 0) return false;
    String sk = currentBody.substring(skPos);
    String temp = jsonString(sk, "\"temp\"");
    String humidity = jsonString(sk, "\"SD\"");
    String wind = jsonString(sk, "\"WS\"");
    String currentText = normalizeWeatherText(jsonString(sk, "\"weather\""));
    if (temp.isEmpty() || currentText.isEmpty()) return false;

    HTTPClient forecastHttp;
    forecastHttp.setTimeout(15000);
    forecastHttp.setUserAgent("Mozilla/5.0");
    String forecastUrl = String("https://www.weather.com.cn/weather/") + weatherCityCode + ".shtml";
    if (!forecastHttp.begin(client, forecastUrl)) return false;
    int forecastCode = forecastHttp.GET();
    if (forecastCode != HTTP_CODE_OK) { forecastHttp.end(); return false; }
    String page = forecastHttp.getString();
    forecastHttp.end();
    int listPos = page.indexOf("<ul class=\"t clearfix\">");
    if (listPos < 0) return false;

    char dailyLines[7][80] = {};
    char dailyIcons[7][8] = {};
    String tomorrowText;
    int cursor = listPos;
    for (int i = 0; i < 7; ++i) {
        int li = page.indexOf("<li", cursor);
        int liEnd = page.indexOf("</li>", li);
        if (li < 0 || liEnd < 0) return false;
        String item = page.substring(li, liEnd);
        String heading = htmlBetween(item, "<h1>", "</h1>");
        String condition = normalizeWeatherText(htmlBetween(item, "class=\"wea\">", "</p>"));
        String high = htmlBetween(item, "<span>", "℃</span>");
        String low = htmlBetween(item, "<i>", "℃</i>");
        if (high.isEmpty()) high = temp;
        snprintf(dailyLines[i], sizeof(dailyLines[i]), "%s\n%s\n%s~%s C",
                 heading.c_str(), condition.c_str(), low.c_str(), high.c_str());
        strlcpy(dailyIcons[i], weatherIconFor(condition), sizeof(dailyIcons[i]));
        if (i == 1) tomorrowText = condition;
        cursor = liEnd + 5;
    }

    char reminder[64] = "明天天气不错，放心出行";
    if (tomorrowText.indexOf("雪") >= 0) strlcpy(reminder, "明天有雪，注意保暖和路滑", sizeof(reminder));
    else if (tomorrowText.indexOf("雨") >= 0) strlcpy(reminder, "明天有雨，记得带伞", sizeof(reminder));
    else if (tomorrowText.indexOf("雾") >= 0 || tomorrowText.indexOf("霾") >= 0)
        strlcpy(reminder, "明天能见度较低，注意出行安全", sizeof(reminder));

    struct tm now;
    char updated[40] = "中国天气网 刚刚更新";
    if (getLocalTime(&now, 50)) strftime(updated, sizeof(updated), "中国天气网 %H:%M 更新", &now);
    portENTER_CRITICAL(&weatherMux);
    snprintf(weatherTemp, sizeof(weatherTemp), "%s C", temp.c_str());
    snprintf(weatherDetail, sizeof(weatherDetail), "%s    湿度 %s\n风力 %s", currentText.c_str(), humidity.c_str(), wind.c_str());
    strlcpy(weatherUpdated, updated, sizeof(weatherUpdated));
    strlcpy(currentWeatherIcon, weatherIconFor(currentText), sizeof(currentWeatherIcon));
    for (int i = 0; i < 7; ++i) {
        strlcpy(forecastLines[i], dailyLines[i], sizeof(forecastLines[i]));
        strlcpy(forecastIcons[i], dailyIcons[i], sizeof(forecastIcons[i]));
    }
    strlcpy(weatherReminder, reminder, sizeof(weatherReminder));
    weatherReady = true;
    portEXIT_CRITICAL(&weatherMux);
    return true;
}

static const char *weatherName(int code)
{
    if (code == 0) return "晴";
    if (code <= 3) return "多云";
    if (code == 45 || code == 48) return "雾";
    if (code >= 51 && code <= 57) return "小雨";
    if (code >= 61 && code <= 67) return "降雨";
    if (code >= 71 && code <= 77) return "降雪";
    if (code >= 80 && code <= 82) return "阵雨";
    if (code >= 85 && code <= 86) return "阵雪";
    if (code >= 95) return "雷雨";
    return "未知";
}

static const char *forecastWeatherName(int code, int rainProbability, float precipitationMm)
{
    bool snowCode = (code >= 71 && code <= 77) || (code >= 85 && code <= 86);
    bool rainCode = (code >= 51 && code <= 67) || (code >= 80 && code <= 82) || code >= 95;
    if (snowCode) {
        if (rainProbability >= 50 || precipitationMm >= 1.0f) return "降雪";
        return "雨雪可能";
    }
    if (rainCode) {
        if (rainProbability >= 50 || precipitationMm >= 1.0f) return code >= 95 ? "雷雨" : "降雨";
        if (rainProbability >= 30 || precipitationMm >= 0.5f) return "可能有雨";
        return "多云";
    }
    return weatherName(code);
}

static void weatherTask(void *)
{
    if (WiFi.status() == WL_CONNECTED) {
        // Open-Meteo's public HTTP endpoint must use a plain client.  Passing
        // WiFiClientSecure here still attempts a TLS handshake even when the
        // URL begins with http://.
        WiFiClient weatherClient;
        WiFiClientSecure chinaWeatherClient;
        chinaWeatherClient.setInsecure();
        bool jsonUpdated = false;
        int openMeteoCode = 0;
        bool parseFailed = false;
        HTTPClient http;
        http.setTimeout(12000);
        if (http.begin(weatherClient, WEATHER_URL)) {
            openMeteoCode = http.GET();
            if (openMeteoCode == HTTP_CODE_OK) {
                String body = http.getString();
                int currentPos = body.indexOf("\"current\":");
                String current = currentPos >= 0 ? body.substring(currentPos) : body;
                String temp = jsonNumber(current, "\"temperature_2m\"");
                String humidity = jsonNumber(current, "\"relative_humidity_2m\"");
                String weatherCode = jsonNumber(current, "\"weather_code\"");
                String wind = jsonNumber(current, "\"wind_speed_10m\"");
                int dailyPos = body.indexOf("\"daily\":");
                String daily = dailyPos >= 0 ? body.substring(dailyPos) : "";

                // Reject incomplete JSON instead of publishing zeroes/blank cards.
                bool parseOk = !temp.isEmpty() && !humidity.isEmpty() &&
                               !weatherCode.isEmpty() && !wind.isEmpty() && dailyPos >= 0;

                char dailyLines[7][80] = {};
                char dailyIconValues[7][8] = {};
                int tomorrowCode = -1;
                int tomorrowRain = 0;
                float tomorrowPrecipitation = 0.0f;
                for (int i = 0; i < 7; ++i) {
                    String date = jsonArrayItem(daily, "\"time\"", i);
                    int dayCode = jsonArrayItem(daily, "\"weather_code\"", i).toInt();
                    String high = jsonArrayItem(daily, "\"temperature_2m_max\"", i);
                    String low = jsonArrayItem(daily, "\"temperature_2m_min\"", i);
                    int rain = jsonArrayItem(daily, "\"precipitation_probability_max\"", i).toInt();
                    float precipitation = jsonArrayItem(daily, "\"precipitation_sum\"", i).toFloat();
                    if (date.length() < 10 || high.isEmpty() || low.isEmpty()) parseOk = false;
                    const char *dayName = i == 0 ? "今天" : (i == 1 ? "明天" : nullptr);
                    char dateText[12] = "--/--";
                    if (date.length() >= 10) snprintf(dateText, sizeof(dateText), "%s/%s", date.substring(5, 7).c_str(), date.substring(8, 10).c_str());
                    char heading[20];
                    if (dayName) strlcpy(heading, dayName, sizeof(heading));
                    else snprintf(heading, sizeof(heading), "%s", dateText);
                    snprintf(dailyLines[i], sizeof(dailyLines[i]), "%s\n%s\n%s~%s C\n降水 %d%%\n%.1f mm",
                             heading, forecastWeatherName(dayCode, rain, precipitation),
                             low.c_str(), high.c_str(), rain, precipitation);
                    String iconSource = dailyLines[i];
                    strlcpy(dailyIconValues[i], weatherIconFor(iconSource), sizeof(dailyIconValues[i]));
                    if (i == 1) {
                        tomorrowCode = dayCode;
                        tomorrowRain = rain;
                        tomorrowPrecipitation = precipitation;
                    }
                }

                char reminder[64] = "明天天气平稳，放心出行";
                bool tomorrowSnow = (tomorrowCode >= 71 && tomorrowCode <= 77) || (tomorrowCode >= 85 && tomorrowCode <= 86);
                if (tomorrowSnow && (tomorrowRain >= 40 || tomorrowPrecipitation >= 0.5f))
                    strlcpy(reminder, "明天有雪，注意保暖和路滑", sizeof(reminder));
                else if (tomorrowRain >= 50 || tomorrowPrecipitation >= 1.0f)
                    strlcpy(reminder, "明天可能下雨，记得带伞", sizeof(reminder));

                struct tm now;
                char updated[40] = "Open-Meteo 刚刚更新";
                if (getLocalTime(&now, 50)) strftime(updated, sizeof(updated), "Open-Meteo %H:%M 更新", &now);

                if (parseOk) {
                    portENTER_CRITICAL(&weatherMux);
                    strlcpy(weatherCityCode, "101200101", sizeof(weatherCityCode));
                    strlcpy(weatherCityName, "武汉", sizeof(weatherCityName));
                    snprintf(weatherTemp, sizeof(weatherTemp), "%s C", temp.c_str());
                    snprintf(weatherDetail, sizeof(weatherDetail), "%s    湿度 %s%%\n风速 %s km/h",
                             weatherName(weatherCode.toInt()), humidity.c_str(), wind.c_str());
                    strlcpy(weatherUpdated, updated, sizeof(weatherUpdated));
                    strlcpy(currentWeatherIcon, weatherIconFor(weatherName(weatherCode.toInt())), sizeof(currentWeatherIcon));
                    for (int i = 0; i < 7; ++i) {
                        strlcpy(forecastLines[i], dailyLines[i], sizeof(forecastLines[i]));
                        strlcpy(forecastIcons[i], dailyIconValues[i], sizeof(forecastIcons[i]));
                    }
                    strlcpy(weatherReminder, reminder, sizeof(weatherReminder));
                    weatherReady = true;
                    weatherHasData = true;
                    lastWeatherSuccess = millis();
                    portEXIT_CRITICAL(&weatherMux);
                    jsonUpdated = true;
                } else parseFailed = true;
            }
            http.end();
        } else openMeteoCode = -1;
        // China Weather is deliberately secondary because its HTML/JS layout
        // can change; it keeps the panel useful when the JSON API is offline.
        if (!jsonUpdated) {
            bool chinaUpdated = fetchChinaWeather(chinaWeatherClient);
            if (chinaUpdated) {
                portENTER_CRITICAL(&weatherMux);
                weatherHasData = true;
                lastWeatherSuccess = millis();
                portEXIT_CRITICAL(&weatherMux);
            } else if (parseFailed) {
                publishWeatherFailure("天气数据格式异常，60 秒后重试");
            } else {
                char message[80];
                snprintf(message, sizeof(message), "天气接口连接失败 (%d)，60 秒后重试", openMeteoCode);
                publishWeatherFailure(message);
            }
        }
    } else {
        publishWeatherFailure("Wi-Fi 未连接，等待网络后自动更新");
    }
    weatherTaskRunning = false;
    vTaskDelete(nullptr);
}

static void startWeatherUpdate()
{
    if (weatherTaskRunning) return;
    if (WiFi.status() != WL_CONNECTED) {
        publishWeatherFailure("Wi-Fi 未连接，等待网络后自动更新");
        return;
    }
    weatherTaskRunning = true;
    lastWeatherAttempt = millis();
    if (xTaskCreatePinnedToCore(weatherTask, "weather", 8192, nullptr, 1, nullptr, 0) != pdPASS) {
        weatherTaskRunning = false;
        Serial.println("[WEATHER] Unable to create update task");
    }
}
