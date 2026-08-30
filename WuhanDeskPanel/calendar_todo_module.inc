// Todo persistence, calendar logic and related events
// Included by WuhanDeskPanel.ino; keep in the same sketch directory.

static void saveTodos()
{
    for (int i = 0; i < 8; ++i) {
        String textKey = "todo" + String(i);
        String timeKey = "time" + String(i);
        String doneKey = "done" + String(i);
        prefs.putString(textKey.c_str(), todoItems[i].text);
        prefs.putString(timeKey.c_str(), todoItems[i].deadline);
        prefs.putBool(doneKey.c_str(), todoItems[i].done);
    }
}

static void loadTodos()
{
    for (int i = 0; i < 8; ++i) {
        String textKey = "todo" + String(i);
        String timeKey = "time" + String(i);
        String doneKey = "done" + String(i);
        todoItems[i].text = prefs.getString(textKey.c_str(), "");
        todoItems[i].deadline = prefs.getString(timeKey.c_str(), "");
        todoItems[i].done = prefs.getBool(doneKey.c_str(), false);
    }
}

static void rebuildTodoList();

static bool todoComesAfter(const TodoItem &a, const TodoItem &b)
{
    if (a.text.isEmpty()) return !b.text.isEmpty();
    if (b.text.isEmpty()) return false;
    if (a.done != b.done) return a.done;
    if (a.deadline.isEmpty() != b.deadline.isEmpty()) return a.deadline.isEmpty();
    return !a.deadline.isEmpty() && a.deadline.compareTo(b.deadline) > 0;
}

static void sortTodosByDeadline()
{
    for (int i = 0; i < 7; ++i) for (int j = i + 1; j < 8; ++j) {
        if (todoComesAfter(todoItems[i], todoItems[j])) {
            TodoItem temp = todoItems[i]; todoItems[i] = todoItems[j]; todoItems[j] = temp;
        }
    }
}

static void refreshHomeTodoPreview()
{
    if (!homeTodoPreviewLabel) return;
    int active[8], count = 0;
    for (int i = 0; i < 8; ++i) if (!todoItems[i].text.isEmpty() && !todoItems[i].done) active[count++] = i;
    if (count == 0) {
        homeTodoRotationIndex = 0;
        lv_label_set_text(homeTodoPreviewLabel, "最近待办: 暂无");
    } else {
        if (homeTodoRotationIndex >= count) homeTodoRotationIndex = 0;
        const TodoItem &item = todoItems[active[homeTodoRotationIndex]];
        String preview = "最近待办: " + item.text;
        if (!item.deadline.isEmpty()) preview += "  " + item.deadline;
        lv_label_set_text(homeTodoPreviewLabel, preview.c_str());
    }
    if (homeTodoPreviewLabel)
        lv_obj_set_style_text_color(homeTodoPreviewLabel,
                                    lv_color_hex(homeTodoRotationPaused ? 0x6F8EAA : 0x9CCCF4), 0);
}

static void homeTodoCardEvent(lv_event_t *)
{
    homeTodoRotationPaused = !homeTodoRotationPaused;
    lastHomeTodoRotation = millis();
    refreshHomeTodoPreview();
}

static void updateHomeTodoRotation()
{
    if (homeTodoRotationPaused || millis() - lastHomeTodoRotation < 5000UL) return;
    lastHomeTodoRotation = millis();
    ++homeTodoRotationIndex;
    refreshHomeTodoPreview();
}

static void todoToggleEvent(lv_event_t *e)
{
    intptr_t index = reinterpret_cast<intptr_t>(lv_event_get_user_data(e));
    if (index < 0 || index >= 8) return;
    todoItems[index].done = lv_obj_has_state(lv_event_get_target(e), LV_STATE_CHECKED);
    saveTodos();
}

static void todoDeleteEvent(lv_event_t *e)
{
    intptr_t index = reinterpret_cast<intptr_t>(lv_event_get_user_data(e));
    if (index < 0 || index >= 8) return;
    for (int i = (int)index; i < 7; ++i) todoItems[i] = todoItems[i + 1];
    todoItems[7] = {};
    saveTodos();
    rebuildTodoList();
}

static void rebuildTodoList()
{
    sortTodosByDeadline();
    if (todoList == nullptr) { refreshHomeTodoPreview(); return; }
    lv_obj_clean(todoList);
    int count = 0;
    for (intptr_t i = 0; i < 8; ++i) {
        if (todoItems[i].text.isEmpty()) continue;
        ++count;
        lv_obj_t *row = lv_obj_create(todoList);
        lv_obj_set_size(row, lv_pct(100), 68);
        lv_obj_set_style_bg_color(row, lv_color_hex(todoItems[i].done ? 0x0B1B2D : 0x102B4C), 0);
        lv_obj_set_style_border_width(row, 1, 0);
        lv_obj_set_style_border_color(row, lv_color_hex(0x21466E), 0);
        lv_obj_set_style_radius(row, 12, 0);
        lv_obj_set_style_border_side(row, LV_BORDER_SIDE_LEFT, 0);
        lv_obj_set_style_border_width(row, 4, 0);
        lv_obj_set_style_border_color(row, lv_color_hex(todoItems[i].done ? 0x38B87C : 0x2C91FF), 0);
        lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_t *check = lv_checkbox_create(row);
        lv_checkbox_set_text(check, todoItems[i].text.c_str());
        lv_obj_set_width(check, 365);
        lv_obj_set_style_text_font(check, &ui_font_todo_cjk_16, 0);
        lv_obj_align(check, LV_ALIGN_LEFT_MID, 6, -11);
        if (todoItems[i].done) lv_obj_add_state(check, LV_STATE_CHECKED);
        lv_obj_add_event_cb(check, todoToggleEvent, LV_EVENT_VALUE_CHANGED, reinterpret_cast<void *>(i));
        lv_obj_t *deadline = lv_label_create(row);
        String deadlineText = todoItems[i].deadline.isEmpty() ? "  无截止时间  " : ("  " + todoItems[i].deadline + " 前  ");
        lv_label_set_text(deadline, deadlineText.c_str());
        lv_obj_set_style_text_font(deadline, &ui_font_cn_16, 0);
        lv_obj_set_style_text_color(deadline, lv_color_hex(0x7FC2FF), 0);
        lv_obj_set_style_bg_color(deadline, lv_color_hex(0x153C63), 0);
        lv_obj_set_style_bg_opa(deadline, LV_OPA_COVER, 0);
        lv_obj_set_style_radius(deadline, 8, 0);
        lv_obj_set_style_pad_ver(deadline, 3, 0);
        lv_obj_align(deadline, LV_ALIGN_BOTTOM_LEFT, 36, -1);
        lv_obj_t *remove = lv_btn_create(row);
        lv_obj_set_size(remove, 45, 42);
        lv_obj_align(remove, LV_ALIGN_RIGHT_MID, -4, 0);
        lv_obj_set_style_bg_color(remove, lv_color_hex(0xB42332), 0);
        lv_obj_add_event_cb(remove, todoDeleteEvent, LV_EVENT_CLICKED, reinterpret_cast<void *>(i));
        lv_obj_t *removeLabel = lv_label_create(remove);
        lv_label_set_text(removeLabel, LV_SYMBOL_CLOSE);
        lv_obj_center(removeLabel);
    }
    String summary = count == 0 ? "今天还没有任务" : (String("清单中有 ") + count + " 项任务 · 自动保存");
    if (todoStatusLabel) lv_label_set_text(todoStatusLabel, summary.c_str());
    refreshHomeTodoPreview();
    if (mainCalendar != nullptr) {
        const lv_calendar_date_t *shown = lv_calendar_get_showed_date(mainCalendar);
        if (shown) {
            updateWeekendHighlights(shown->year, shown->month);
            updateCalendarAgenda(shown->year, shown->month);
        }
    }
}

static void todoTextAreaEvent(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_FOCUSED) return;
    lv_obj_t *target = lv_event_get_target(e);
    lv_keyboard_set_textarea(todoKeyboard, target);
    lv_keyboard_set_mode(todoKeyboard, target == todoTimeArea ? LV_KEYBOARD_MODE_NUMBER
                                                               : LV_KEYBOARD_MODE_TEXT_LOWER);
    if (target == todoTextArea) {
        lv_label_set_text(todoStatusLabel, "中文输入：键入单字完整拼音，再点击候选字（如 wu → 武）");
    }
    lv_obj_clear_flag(todoKeyboard, LV_OBJ_FLAG_HIDDEN);
    if (todoTypingLabel != nullptr) lv_obj_clear_flag(todoTypingLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(todoKeyboard);
}

static void todoTypingEvent(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED || todoTypingLabel == nullptr) return;
    String text = "正在输入：";
    text += lv_textarea_get_text(lv_event_get_target(e));
    lv_label_set_text(todoTypingLabel, text.c_str());
}

static void todoKeyboardEvent(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_READY || lv_event_get_code(e) == LV_EVENT_CANCEL) {
        lv_obj_add_flag(todoKeyboard, LV_OBJ_FLAG_HIDDEN);
        if (todoTypingLabel != nullptr) lv_obj_add_flag(todoTypingLabel, LV_OBJ_FLAG_HIDDEN);
    }
}

static void datePickerCalendarEvent(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) return;
    lv_calendar_date_t selected;
    if (lv_calendar_get_pressed_date(lv_event_get_target(e), &selected) != LV_RES_OK) return;
    snprintf(pendingDeadline, sizeof(pendingDeadline), "%04u-%02u-%02u",
             selected.year, selected.month, selected.day);
    String value = String("已选择：") + pendingDeadline;
    lv_label_set_text(datePickerValueLabel, value.c_str());
}

static void openDatePickerEvent(lv_event_t *)
{
    strlcpy(pendingDeadline, lv_textarea_get_text(todoTimeArea), sizeof(pendingDeadline));
    lv_label_set_text(datePickerValueLabel,
                      pendingDeadline[0] ? (String("当前日期：") + pendingDeadline).c_str() : "请选择截止日期");
    lv_scr_load(datePickerPanel);
}

static void confirmDatePickerEvent(lv_event_t *)
{
    if (pendingDeadline[0]) lv_textarea_set_text(todoTimeArea, pendingDeadline);
    lv_scr_load(todoScreen);
}

static void closeDatePickerEvent(lv_event_t *)
{
    lv_scr_load(todoScreen);
}

static bool dateInRange(uint32_t month, uint32_t day, uint32_t fromMonth, uint32_t fromDay,
                        uint32_t toMonth, uint32_t toDay)
{
    uint32_t value = month * 100 + day;
    return value >= fromMonth * 100 + fromDay && value <= toMonth * 100 + toDay;
}

static bool isAdjustedWorkday(uint32_t year, uint32_t month, uint32_t day)
{
    if (year != 2026) return false;
    uint32_t value = month * 100 + day;
    return value == 104 || value == 214 || value == 228 || value == 509 || value == 920 || value == 1010;
}

static bool isOfficialRestDay(uint32_t year, uint32_t month, uint32_t day)
{
    if (year != 2026) return false;
    return dateInRange(month, day, 1, 1, 1, 3) || dateInRange(month, day, 2, 15, 2, 23) ||
           dateInRange(month, day, 4, 4, 4, 6) || dateInRange(month, day, 5, 1, 5, 5) ||
           dateInRange(month, day, 6, 19, 6, 21) || dateInRange(month, day, 9, 25, 9, 27) ||
           dateInRange(month, day, 10, 1, 10, 7);
}

static const char *festivalName(uint32_t year, uint32_t month, uint32_t day)
{
    uint32_t value = month * 100 + day;
    if (value == 101) return "元旦";
    if (value == 214) return "情人节";
    if (value == 308) return "妇女节";
    if (value == 312) return "植树节";
    if (value == 501) return "劳动节";
    if (value == 504) return "青年节";
    if (value == 601) return "儿童节";
    if (value == 701) return "建党节";
    if (value == 801) return "建军节";
    if (value == 910) return "教师节";
    if (value == 1001) return "国庆节";
    if (year == 2026 && value == 217) return "春节";
    if (year == 2026 && value == 303) return "元宵节";
    if (year == 2026 && value == 405) return "清明节";
    if (year == 2026 && value == 619) return "端午节";
    if (year == 2026 && value == 925) return "中秋节";
    return "";
}

static bool isRestDate(uint32_t year, uint32_t month, uint32_t day, int weekday)
{
    if (isAdjustedWorkday(year, month, day)) return false;
    if (isOfficialRestDay(year, month, day)) return true;
    return weekday == 0 || weekday == 6;
}

static void calendarCellDrawEvent(lv_event_t *e)
{
    lv_obj_draw_part_dsc_t *dsc = (lv_obj_draw_part_dsc_t *)lv_event_get_param(e);
    lv_obj_t *matrix = lv_event_get_target(e);
    if (!dsc || dsc->part != LV_PART_ITEMS || dsc->id < 7 || !dsc->rect_dsc || !dsc->label_dsc) return;
    if (lv_btnmatrix_has_btn_ctrl(matrix, dsc->id, LV_BTNMATRIX_CTRL_DISABLED)) return;
    const char *text = lv_btnmatrix_get_btn_text(matrix, dsc->id);
    if (!text) return;
    int day = atoi(text);
    const lv_calendar_date_t *shown = lv_calendar_get_showed_date(mainCalendar);
    if (!shown || day < 1 || day > 31) return;
    struct tm dateTm = {};
    dateTm.tm_year = (int)shown->year - 1900;
    dateTm.tm_mon = (int)shown->month - 1;
    dateTm.tm_mday = day;
    mktime(&dateTm);
    char dateText[12];
    snprintf(dateText, sizeof(dateText), "%04u-%02u-%02d", shown->year, shown->month, day);
    bool personal = false;
    for (int i = 0; i < 12; ++i) if (specialDays[i].date == dateText) personal = true;
    bool festival = festivalName(shown->year, shown->month, day)[0];
    bool officialRest = isOfficialRestDay(shown->year, shown->month, day);
    bool adjustedWork = isAdjustedWorkday(shown->year, shown->month, day);
    bool weekend = !adjustedWork && (dateTm.tm_wday == 0 || dateTm.tm_wday == 6);
    uint32_t color = personal ? 0x7138A8 :
                     (officialRest ? 0xC43D52 :
                     (festival ? 0xB77912 :
                     (weekend ? 0x713044 : 0x164A78)));
    dsc->rect_dsc->bg_opa = LV_OPA_COVER;
    dsc->rect_dsc->bg_color = lv_color_hex(color);
    dsc->rect_dsc->radius = 8;
    dsc->rect_dsc->border_opa = LV_OPA_COVER;
    dsc->rect_dsc->border_width = personal || festival || officialRest ? 2 : 1;
    dsc->rect_dsc->border_color = lv_color_hex(personal ? 0xD8B4FE :
                                               (festival ? 0xFFD35A :
                                               (officialRest ? 0xFF9AA5 : 0x3477A8)));
    dsc->label_dsc->color = lv_color_white();
}

static void updateCalendarAgenda(uint32_t year, uint32_t month)
{
    if (calendarMonthAgendaLabel == nullptr) return;
    String agenda = "节日：";
    int festivalCount = 0;
    for (int day = 1; day <= 31; ++day) {
        const char *festival = festivalName(year, month, day);
        if (festival[0]) {
            if (festivalCount++) agenda += "、";
            agenda += String(day) + "日" + festival;
        }
    }
    if (!festivalCount) agenda += "无";
    lv_label_set_text(calendarMonthAgendaLabel, agenda.c_str());
}

static void updateWeekendHighlights(uint32_t year, uint32_t month)
{
    if (mainCalendar == nullptr) return;
    struct tm first = {};
    first.tm_year = (int)year - 1900;
    first.tm_mon = (int)month - 1;
    first.tm_mday = 1;
    mktime(&first);
    static const uint8_t monthDays[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int days = monthDays[month - 1];
    if (month == 2 && ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))) days = 29;
    uint8_t count = 0;
    for (int day = 1; day <= days && count < 24; ++day) {
        int weekday = (first.tm_wday + day - 1) % 7;
        bool marked = isRestDate(year, month, day, weekday) || festivalName(year, month, day)[0];
        if (marked) weekendDates[count++] = {year, month, (uint32_t)day};
    }
    for (int i = 0; i < 12 && count < 24; ++i) {
        if (specialDays[i].date.length() != 10) continue;
        uint32_t markedYear = specialDays[i].date.substring(0, 4).toInt();
        uint32_t markedMonth = specialDays[i].date.substring(5, 7).toInt();
        uint32_t markedDay = specialDays[i].date.substring(8, 10).toInt();
        if (markedYear == year && markedMonth == month)
            weekendDates[count++] = {markedYear, markedMonth, markedDay};
    }
    lv_calendar_set_highlighted_dates(mainCalendar, weekendDates, count);
}

static void loadSpecialDays()
{
    for (int i = 0; i < 12; ++i) {
        specialDays[i].date = prefs.getString((String("markD") + i).c_str(), "");
        specialDays[i].note = prefs.getString((String("markN") + i).c_str(), "");
    }
}

static void saveSpecialDays()
{
    for (int i = 0; i < 12; ++i) {
        prefs.putString((String("markD") + i).c_str(), specialDays[i].date);
        prefs.putString((String("markN") + i).c_str(), specialDays[i].note);
    }
}

static void mainCalendarEvent(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) return;
    lv_calendar_date_t selected;
    if (lv_calendar_get_pressed_date(mainCalendar, &selected) != LV_RES_OK) return;
    snprintf(pendingMarkerDate, sizeof(pendingMarkerDate), "%04u-%02u-%02u",
             selected.year, selected.month, selected.day);
    calendarSelectionActive = true;
    struct tm selectedTm = {};
    selectedTm.tm_year = (int)selected.year - 1900;
    selectedTm.tm_mon = (int)selected.month - 1;
    selectedTm.tm_mday = (int)selected.day;
    mktime(&selectedTm);
    static const char *weekNames[] = {"日", "一", "二", "三", "四", "五", "六"};
    bool selectedRest = isRestDate(selected.year, selected.month, selected.day, selectedTm.tm_wday);
    bool selectedPersonal = false;
    for (int i = 0; i < 12; ++i) if (specialDays[i].date == pendingMarkerDate) selectedPersonal = true;
    const char *selectedFestival = festivalName(selected.year, selected.month, selected.day);
    const char *dayType = selectedPersonal ? "重要日" : (selectedFestival[0] ? "节日" : (selectedRest ? "休息日" : "工作日"));
    char selectedTitle[64];
    snprintf(selectedTitle, sizeof(selectedTitle), "%u月%u日\n星期%s\n\n%s",
             selected.month, selected.day, weekNames[selectedTm.tm_wday], dayType);
    lv_label_set_text(calendarTodayLabel, selectedTitle);
    lv_obj_set_style_text_color(calendarTodayLabel,
                                lv_color_hex(selectedPersonal ? 0xD8B4FE : (selectedFestival[0] ? 0xFFE08A :
                                             (selectedRest ? 0xFF7B87 : 0x72D6FF))), 0);
    String detail = String(selected.month) + "月" + selected.day + "日";
    const char *festival = festivalName(selected.year, selected.month, selected.day);
    if (festival[0]) detail += String("  ·  ") + festival;
    for (int i = 0; i < 12; ++i)
        if (specialDays[i].date == pendingMarkerDate) detail += String("\n备注：") + specialDays[i].note;
    if (!festival[0]) detail += "\n当天暂无节日";
    lv_label_set_text(calendarDetailLabel, detail.c_str());
}

static void openMarkerEvent(lv_event_t *)
{
    if (!pendingMarkerDate[0]) return;
    String title = String("标记日期：") + pendingMarkerDate;
    lv_label_set_text(calendarMarkerDateLabel, title.c_str());
    lv_roller_set_selected(calendarMarkerRoller, 0, LV_ANIM_OFF);
    lv_textarea_set_placeholder_text(calendarMarkerNoteArea, "输入日期备注");
    lv_textarea_set_text(calendarMarkerNoteArea, "");
    for (int i = 0; i < 12; ++i)
        if (specialDays[i].date == pendingMarkerDate)
            lv_textarea_set_text(calendarMarkerNoteArea, specialDays[i].note.c_str());
    lv_obj_clear_flag(calendarMarkerPanel, LV_OBJ_FLAG_HIDDEN);
}

static void changeCalendarMonth(int delta)
{
    const lv_calendar_date_t *shown = lv_calendar_get_showed_date(mainCalendar);
    if (!shown) return;
    int year = shown->year;
    int month = (int)shown->month + delta;
    if (month < 1) { month = 12; --year; }
    if (month > 12) { month = 1; ++year; }
    lv_calendar_set_showed_date(mainCalendar, year, month);
    updateWeekendHighlights(year, month);
    updateCalendarAgenda(year, month);
}

static void previousMonthEvent(lv_event_t *) { changeCalendarMonth(-1); }
static void nextMonthEvent(lv_event_t *) { changeCalendarMonth(1); }
static void todayMonthEvent(lv_event_t *)
{
    struct tm now;
    if (getLocalTime(&now, 5)) {
        calendarSelectionActive = false;
        lv_calendar_set_showed_date(mainCalendar, now.tm_year + 1900, now.tm_mon + 1);
        updateWeekendHighlights(now.tm_year + 1900, now.tm_mon + 1);
        updateCalendarAgenda(now.tm_year + 1900, now.tm_mon + 1);
    }
}

static void closeMarkerEvent(lv_event_t *)
{
    if (calendarMarkerKeyboard) lv_obj_add_flag(calendarMarkerKeyboard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(calendarMarkerPanel, LV_OBJ_FLAG_HIDDEN);
}

static void markerTextAreaEvent(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_FOCUSED) return;
    lv_keyboard_set_textarea(calendarMarkerKeyboard, calendarMarkerNoteArea);
    lv_keyboard_set_mode(calendarMarkerKeyboard, LV_KEYBOARD_MODE_TEXT_LOWER);
    lv_obj_clear_flag(calendarMarkerKeyboard, LV_OBJ_FLAG_HIDDEN);
}

static void markerKeyboardEvent(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_READY || lv_event_get_code(e) == LV_EVENT_CANCEL)
        lv_obj_add_flag(calendarMarkerKeyboard, LV_OBJ_FLAG_HIDDEN);
}

static void markerPresetEvent(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) return;
    char preset[32];
    lv_roller_get_selected_str(calendarMarkerRoller, preset, sizeof(preset));
    if (strcmp(preset, "取消该日标记") == 0) {
        lv_textarea_set_text(calendarMarkerNoteArea, "");
        lv_textarea_set_placeholder_text(calendarMarkerNoteArea, "保存后移除该日标记");
    } else {
        lv_textarea_set_placeholder_text(calendarMarkerNoteArea, "输入日期备注");
        lv_textarea_set_text(calendarMarkerNoteArea, preset);
    }
}

static void saveMarkerEvent(lv_event_t *)
{
    char preset[32];
    lv_roller_get_selected_str(calendarMarkerRoller, preset, sizeof(preset));
    if (strcmp(preset, "取消该日标记") == 0) {
        for (int i = 0; i < 12; ++i) {
            if (specialDays[i].date == pendingMarkerDate) {
                specialDays[i].date = "";
                specialDays[i].note = "";
            }
        }
        saveSpecialDays();
        const lv_calendar_date_t *shown = lv_calendar_get_showed_date(mainCalendar);
        if (shown) updateWeekendHighlights(shown->year, shown->month);
        if (calendarDetailLabel != nullptr) lv_label_set_text(calendarDetailLabel, "该日个人标记已取消");
        if (calendarMarkerKeyboard) lv_obj_add_flag(calendarMarkerKeyboard, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(calendarMarkerPanel, LV_OBJ_FLAG_HIDDEN);
        return;
    }
    String note = lv_textarea_get_text(calendarMarkerNoteArea);
    note.trim();
    if (note.isEmpty()) note = "重要事项";
    int slot = -1;
    for (int i = 0; i < 12; ++i) {
        if (specialDays[i].date == pendingMarkerDate) { slot = i; break; }
        if (slot < 0 && specialDays[i].date.isEmpty()) slot = i;
    }
    if (slot >= 0) {
        specialDays[slot].date = pendingMarkerDate;
        specialDays[slot].note = note;
        saveSpecialDays();
        const lv_calendar_date_t *shown = lv_calendar_get_showed_date(mainCalendar);
        if (shown) updateWeekendHighlights(shown->year, shown->month);
    }
    if (calendarMarkerKeyboard) lv_obj_add_flag(calendarMarkerKeyboard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(calendarMarkerPanel, LV_OBJ_FLAG_HIDDEN);
}

static void openStatusPickerEvent(lv_event_t *)
{
    lv_obj_clear_flag(statusPickerPanel, LV_OBJ_FLAG_HIDDEN);
}

static void closeStatusPickerEvent(lv_event_t *)
{
    lv_obj_add_flag(statusPickerPanel, LV_OBJ_FLAG_HIDDEN);
}

static void addTodoEvent(lv_event_t *)
{
    String text = lv_textarea_get_text(todoTextArea);
    String deadline = lv_textarea_get_text(todoTimeArea);
    text.trim();
    deadline.trim();
    if (text.isEmpty()) {
        lv_label_set_text(todoStatusLabel, "请先输入待办内容");
        return;
    }
    int slot = -1;
    for (int i = 0; i < 8; ++i) if (todoItems[i].text.isEmpty()) { slot = i; break; }
    if (slot < 0) {
        lv_label_set_text(todoStatusLabel, "最多保存 8 项，请先删除已完成事项");
        return;
    }
    todoItems[slot] = {text, deadline, false};
    saveTodos();
    lv_textarea_set_text(todoTextArea, "");
    lv_textarea_set_text(todoTimeArea, "");
    lv_obj_add_flag(todoKeyboard, LV_OBJ_FLAG_HIDDEN);
    if (todoTypingLabel != nullptr) lv_obj_add_flag(todoTypingLabel, LV_OBJ_FLAG_HIDDEN);
    rebuildTodoList();
}
