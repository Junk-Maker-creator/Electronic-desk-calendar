// Local mobile web control. HTTP handlers only enqueue commands; LVGL owns UI changes.

static WebServer webServer(80);
static portMUX_TYPE webCommandMux = portMUX_INITIALIZER_UNLOCKED;
static volatile uint32_t pendingWebCommands = 0;
static uint8_t pendingWebState = 0;
static uint32_t pendingWebCountdown = 0;
static uint8_t pendingWebBrightness = 100;
static uint16_t pendingHydrationStart = 480;
static uint16_t pendingHydrationEnd = 1080;
static uint16_t pendingHydrationInterval = 120;
static uint16_t pendingLunchEnd = 780;
static char pendingWebTodo[96] = {};
static char pendingWebDeadline[24] = {};
static char pendingWebSsid[64] = {};
static char pendingWebPassword[64] = {};
static uint8_t pendingWebTodoIndex = 0;
static bool pendingWebTodoDelete = false;
static bool webMdnsStarted = false;

enum : uint32_t {
    WEB_CMD_STATE = 1U << 0,
    WEB_CMD_TODO = 1U << 1,
    WEB_CMD_COUNTDOWN = 1U << 2,
    WEB_CMD_BRIGHTNESS = 1U << 3,
    WEB_CMD_WIFI = 1U << 4,
    WEB_CMD_TODO_ACTION = 1U << 5,
    WEB_CMD_REMINDERS = 1U << 6,
    WEB_CMD_HYDRATION_TEST = 1U << 7,
};

static const char WEB_PAGE[] PROGMEM = R"HTML(<!doctype html><html lang="zh-CN"><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>桌面电子日历</title><style>
body{margin:0;background:#071426;color:#eef6ff;font-family:system-ui;padding:16px}.wrap{max-width:720px;margin:auto}
h1{font-size:24px}.card{background:#10243d;border:1px solid #28527c;border-radius:16px;padding:16px;margin:12px 0}
button,input,select{font-size:16px;padding:11px;border-radius:9px;border:1px solid #47749d;margin:5px;background:#f4f8fc}
button{background:#1778d1;color:white;border:0}.grid{display:grid;grid-template-columns:repeat(3,1fr);gap:7px}.grid button{margin:0}.grid button.active{background:#f59e0b;box-shadow:0 0 0 3px #fff inset;font-weight:700}
label{display:block;color:#a9c9e8;margin-top:8px}small{color:#91abc4}#msg{color:#67e8a5;min-height:22px}.todo-item{background:#0b1c31;border:1px solid #315a80;border-radius:10px;padding:10px;margin:9px 0}.todo-actions{display:flex;gap:8px;margin-top:8px}.todo-actions button{flex:1;margin:0}.todo-actions .delete{background:#c6283d}
</style></head><body><div class="wrap"><h1>桌面电子日历</h1><div id="msg"></div>
<div class="card"><h3>工作状态</h3><div class="grid" id="states"></div></div>
<div class="card"><h3>添加待办</h3><input id="todo" maxlength="40" placeholder="待办内容"><label>截止日期</label><input id="deadline" type="date"><button onclick="todoAdd()">添加</button></div>
<div class="card"><h3>待办事项管理</h3><div id="todos"><small>正在加载…</small></div></div>
<div class="card"><h3>倒计时</h3><input id="minutes" type="number" min="1" max="1440" value="25"> 分钟 <button onclick="countdown()">开始</button></div>
<div class="card"><h3>屏幕</h3><label>背光开关</label><button onclick="brightness(100)">点亮</button><button onclick="brightness(0)">熄灭</button><br><small>本机背光由 IO 扩展器控制，硬件仅支持亮/灭。</small></div>
<div class="card"><h3>修改 Wi-Fi</h3><input id="ssid" maxlength="32" placeholder="SSID"><input id="pass" maxlength="63" type="password" placeholder="密码"><button onclick="wifiSet()">保存并连接</button></div>
<div class="card"><h3>设备状态</h3><pre id="status">加载中…</pre><button onclick="refresh()">刷新</button></div>
</div><script>
const names=['上班','忙碌','开会','休息','下班','出差','休假','午休','厕所'];
const statusEl=document.getElementById('status'),todosEl=document.getElementById('todos'),msgEl=document.getElementById('msg');
const reminderCard=document.createElement('div');
reminderCard.className='card';
reminderCard.innerHTML=`<h3>喝水提醒</h3><label>提醒时段</label><input id="drinkStart" type="time" value="08:00"> 至 <input id="drinkEnd" type="time" value="18:00"><label>间隔（小时）</label><input id="drinkInterval" type="number" min="0.25" max="12" step="0.25" value="2"><button onclick="saveReminders()">保存喝水设置</button><button onclick="drinkTest()">喝水弹窗启动</button><hr><h3>午休</h3><label>午休结束时间</label><input id="lunchEnd" type="time" value="13:00"><button onclick="saveReminders()">保存午休时间</button><br><small>保存时间不会自动进入午休；请在状态栏手动点击“午休”，屏幕会显示结束时间。</small>`;
document.querySelector('.wrap').insertBefore(reminderCard,document.getElementById('status').parentElement);
function toMinutes(value){let p=value.split(':');return Number(p[0])*60+Number(p[1])}
function asTime(value){return String(Math.floor(value/60)).padStart(2,'0')+':'+String(value%60).padStart(2,'0')}
async function saveReminders(){let hours=Number(drinkInterval.value);if(!Number.isFinite(hours)){msg.textContent='请输入有效的喝水间隔';return}await post('/api/reminders',{drinkStart:toMinutes(drinkStart.value),drinkEnd:toMinutes(drinkEnd.value),drinkInterval:Math.round(hours*60),lunchEnd:toMinutes(lunchEnd.value)})}
async function drinkTest(){await post('/api/hydration-test',{})}
async function loadReminderSettings(){try{let s=await(await fetch('/api/status?ts='+Date.now(),{cache:'no-store'})).json();if(document.activeElement!==drinkStart)drinkStart.value=asTime(s.drinkStart);if(document.activeElement!==drinkEnd)drinkEnd.value=asTime(s.drinkEnd);if(document.activeElement!==drinkInterval)drinkInterval.value=(s.drinkInterval/60).toFixed(2).replace(/\.00$/,'');if(document.activeElement!==lunchEnd)lunchEnd.value=asTime(s.lunchEnd)}catch(e){}}
loadReminderSettings();
states.innerHTML=names.map((n,i)=>`<button onclick="setState(${i})">${n}</button>`).join('');
function markState(i){document.querySelectorAll('#states button').forEach((b,n)=>b.classList.toggle('active',n===Number(i)))}
async function post(url,data){try{let b=new URLSearchParams(data);let r=await fetch(url,{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded;charset=UTF-8'},body:b,cache:'no-store'});let t=await r.text();msg.textContent=t;if(!r.ok)throw Error(t);setTimeout(refresh,120);return true}catch(e){msg.textContent='操作失败，请检查设备连接';return false}}
async function setState(i){markState(i);msg.textContent='正在同步到设备…';if(!await post('/api/state',{state:i}))return;for(let n=0;n<12;n++){await new Promise(r=>setTimeout(r,100));let s=await(await fetch('/api/status?ts='+Date.now(),{cache:'no-store'})).json();markState(s.stateIndex);if(Number(s.stateIndex)===i){msg.textContent='工作状态已同步：'+names[i];return}}msg.textContent='设备响应超时，请重试'}
function todoAdd(){post('/api/todo',{text:todo.value,deadline:deadline.value});todo.value=''}
function countdown(){post('/api/countdown',{seconds:Math.max(60,Number(minutes.value)*60)})}
function brightness(v){post('/api/brightness',{value:v})}
function wifiSet(){if(confirm('连接新网络后手机可能需要重新扫码，继续吗？'))post('/api/wifi',{ssid:ssid.value,password:pass.value})}
async function todoAction(i,del){if(del&&!confirm('确定删除这条待办吗？'))return;await post('/api/todo-action',{index:i,del:del?1:0})}
async function refresh(){try{let s=await(await fetch('/api/status?ts='+Date.now(),{cache:'no-store'})).json();statusEl.textContent=`状态：${s.state}\nIP：${s.ip}\nWi-Fi：${s.ssid} (${s.rssi} dBm)\n倒计时：${s.countdown}s\n空闲内存：${s.heap}\nPSRAM：${s.psram}\n运行：${s.uptime}s`}catch(e){statusEl.textContent='设备状态加载失败'}try{let t=await(await fetch('/api/todos?ts='+Date.now(),{cache:'no-store'})).json();todosEl.innerHTML=t.map((x,i)=>`<div class="todo-item">${x.done?'✓ ':''}<b>${x.text}</b><br>${x.deadline?'<small>截止 '+x.deadline+'</small>':'<small>无截止日期</small>'}<div class="todo-actions"><button onclick="todoAction(${i},false)">${x.done?'恢复':'完成'}</button><button class="delete" onclick="todoAction(${i},true)">删除待办</button></div></div>`).join('')||'<small>暂无待办事项</small>'}catch(e){todosEl.innerHTML='<small>待办加载失败，请点击刷新重试</small>'}}
async function syncState(){try{let s=await(await fetch('/api/status?ts='+Date.now(),{cache:'no-store'})).json();markState(s.stateIndex)}catch(e){}}
refresh();syncState();setInterval(refresh,5000);setInterval(syncState,1000);
</script></body></html>)HTML";

static void webReply(bool ok, const char *message)
{
    webServer.send(ok ? 200 : 400, "text/plain; charset=utf-8", message);
}

static String jsonSafe(const String &value)
{
    String out;
    out.reserve(value.length() + 8);
    for (size_t i = 0; i < value.length(); ++i) {
        char c = value[i];
        if (c == '\\' || c == '"') out += '\\';
        if ((uint8_t)c >= 0x20) out += c;
    }
    return out;
}

static void startWebControl()
{
    webServer.on("/", HTTP_GET, []() { webServer.sendHeader("Cache-Control", "no-store"); webServer.send_P(200, "text/html; charset=utf-8", WEB_PAGE); });
    webServer.on("/api/status", HTTP_GET, []() {
        webServer.sendHeader("Cache-Control", "no-store");
        String body = String("{\"state\":\"") + jsonSafe(workStates[currentWorkState]) +
                      "\",\"stateIndex\":" + currentWorkState +
                      "\",\"ip\":\"" + WiFi.localIP().toString() +
                      "\",\"ssid\":\"" + jsonSafe(WiFi.SSID()) +
                      "\",\"rssi\":" + WiFi.RSSI() +
                      ",\"countdown\":" + countdownRemainingSeconds +
                      ",\"drinkStart\":" + hydrationStartMinutes +
                      ",\"drinkEnd\":" + hydrationEndMinutes +
                      ",\"drinkInterval\":" + hydrationIntervalMinutes +
                      ",\"lunchEnd\":" + lunchEndMinutes +
                      ",\"heap\":" + ESP.getFreeHeap() +
                      ",\"psram\":" + ESP.getFreePsram() +
                      ",\"uptime\":" + millis() / 1000UL + "}";
        webServer.send(200, "application/json; charset=utf-8", body);
    });
    webServer.on("/api/todos", HTTP_GET, []() {
        webServer.sendHeader("Cache-Control", "no-store");
        String body = "["; bool first = true;
        for (int i = 0; i < 8; ++i) if (!todoItems[i].text.isEmpty()) {
            if (!first) body += ','; first = false;
            body += String("{\"text\":\"") + jsonSafe(todoItems[i].text) + "\",\"deadline\":\"" +
                    jsonSafe(todoItems[i].deadline) + "\",\"done\":" + (todoItems[i].done ? "true" : "false") + "}";
        }
        body += ']'; webServer.send(200, "application/json; charset=utf-8", body);
    });
    webServer.on("/api/state", HTTP_POST, []() {
        int value = webServer.arg("state").toInt();
        if (value < 0 || value >= 9) { webReply(false, "状态参数无效"); return; }
        portENTER_CRITICAL(&webCommandMux); pendingWebState = value; pendingWebCommands |= WEB_CMD_STATE; portEXIT_CRITICAL(&webCommandMux);
        webReply(true, "状态已更新");
    });
    webServer.on("/api/todo", HTTP_POST, []() {
        String text = webServer.arg("text"); text.trim();
        String deadline = webServer.arg("deadline");
        if (text.isEmpty() || text.length() > 40) { webReply(false, "待办内容应为 1-40 个字符"); return; }
        if (!deadline.isEmpty() && (deadline.length() != 10 || deadline[4] != '-' || deadline[7] != '-')) { webReply(false, "截止日期格式无效"); return; }
        portENTER_CRITICAL(&webCommandMux);
        strlcpy(pendingWebTodo, text.c_str(), sizeof(pendingWebTodo));
        strlcpy(pendingWebDeadline, deadline.c_str(), sizeof(pendingWebDeadline));
        pendingWebCommands |= WEB_CMD_TODO;
        portEXIT_CRITICAL(&webCommandMux);
        webReply(true, "待办已添加");
    });
    webServer.on("/api/todo-action", HTTP_POST, []() {
        if (!webServer.hasArg("index")) { webReply(false, "缺少待办序号"); return; }
        int index = webServer.arg("index").toInt();
        if (index < 0 || index >= 8 || todoItems[index].text.isEmpty()) { webReply(false, "待办不存在"); return; }
        portENTER_CRITICAL(&webCommandMux);
        pendingWebTodoIndex = index; pendingWebTodoDelete = webServer.arg("del") == "1";
        pendingWebCommands |= WEB_CMD_TODO_ACTION;
        portEXIT_CRITICAL(&webCommandMux);
        webReply(true, pendingWebTodoDelete ? "待办已删除" : "待办状态已更新");
    });
    webServer.on("/api/countdown", HTTP_POST, []() {
        uint32_t value = strtoul(webServer.arg("seconds").c_str(), nullptr, 10);
        if (value < 1 || value > 86400UL) { webReply(false, "倒计时范围为 1 秒至 24 小时"); return; }
        portENTER_CRITICAL(&webCommandMux); pendingWebCountdown = value; pendingWebCommands |= WEB_CMD_COUNTDOWN; portEXIT_CRITICAL(&webCommandMux);
        webReply(true, "倒计时已开始");
    });
    webServer.on("/api/reminders", HTTP_POST, []() {
        int drinkStart = webServer.arg("drinkStart").toInt();
        int drinkEnd = webServer.arg("drinkEnd").toInt();
        int drinkInterval = webServer.arg("drinkInterval").toInt();
        int lunchEnd = webServer.arg("lunchEnd").toInt();
        if (drinkStart < 0 || drinkStart > 1439 || drinkEnd < 1 || drinkEnd > 1440 ||
            drinkStart >= drinkEnd || drinkInterval < 15 || drinkInterval > 720 ||
            lunchEnd < 0 || lunchEnd > 1439) {
            webReply(false, "提醒参数无效"); return;
        }
        portENTER_CRITICAL(&webCommandMux);
        pendingHydrationStart = (uint16_t)drinkStart; pendingHydrationEnd = (uint16_t)drinkEnd;
        pendingHydrationInterval = (uint16_t)drinkInterval; pendingLunchEnd = (uint16_t)lunchEnd;
        pendingWebCommands |= WEB_CMD_REMINDERS;
        portEXIT_CRITICAL(&webCommandMux);
        webReply(true, "提醒设置已保存");
    });
    webServer.on("/api/hydration-test", HTTP_POST, []() {
        portENTER_CRITICAL(&webCommandMux);
        pendingWebCommands |= WEB_CMD_HYDRATION_TEST;
        portEXIT_CRITICAL(&webCommandMux);
        webReply(true, "喝水弹窗已启动");
    });
    webServer.on("/api/brightness", HTTP_POST, []() {
        int value = webServer.arg("value").toInt();
        if (value != 0 && value != 100) { webReply(false, "本机背光仅支持 0 或 100"); return; }
        portENTER_CRITICAL(&webCommandMux); pendingWebBrightness = value; pendingWebCommands |= WEB_CMD_BRIGHTNESS; portEXIT_CRITICAL(&webCommandMux);
        webReply(true, value ? "屏幕已点亮" : "屏幕已熄灭，触摸屏仍可操作");
    });
    webServer.on("/api/wifi", HTTP_POST, []() {
        String ssid = webServer.arg("ssid");
        String password = webServer.arg("password");
        if (ssid.isEmpty() || ssid.length() > 32 || password.length() > 63) { webReply(false, "Wi-Fi 参数无效"); return; }
        portENTER_CRITICAL(&webCommandMux);
        strlcpy(pendingWebSsid, ssid.c_str(), sizeof(pendingWebSsid));
        strlcpy(pendingWebPassword, password.c_str(), sizeof(pendingWebPassword));
        pendingWebCommands |= WEB_CMD_WIFI;
        portEXIT_CRITICAL(&webCommandMux);
        webReply(true, "Wi-Fi 已保存，设备正在重新连接");
    });
    webServer.onNotFound([]() { webServer.send(404, "text/plain", "Not found"); });
    webServer.begin();
}

static void handleWebControl()
{
    webServer.handleClient();
    if (!webMdnsStarted && WiFi.status() == WL_CONNECTED) webMdnsStarted = MDNS.begin("wuhan-panel");
}

static void updatePhoneQr()
{
    if (!phoneQrCode || !phoneUrlLabel) return;
    if (WiFi.status() != WL_CONNECTED) {
        lv_label_set_text(phoneUrlLabel, "请先连接 Wi-Fi");
        return;
    }
    String url = String("http://") + WiFi.localIP().toString();
    lv_qrcode_update(phoneQrCode, url.c_str(), url.length());
    lv_label_set_text(phoneUrlLabel, (url + "\n或 wuhan-panel.local").c_str());
}

static void showPhoneControlEvent(lv_event_t *)
{
    updatePhoneQr();
    lv_obj_clear_flag(phoneControlPanel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(phoneControlPanel);
}

static void closePhoneControlEvent(lv_event_t *)
{
    lv_obj_add_flag(phoneControlPanel, LV_OBJ_FLAG_HIDDEN);
}

static void processWebCommands()
{
    uint32_t commands;
    uint8_t state, brightness, todoIndex;
    bool todoDelete;
    uint32_t seconds;
    uint16_t drinkStart, drinkEnd, drinkInterval, lunchEnd;
    char todo[96], deadline[24], ssid[64], password[64];
    portENTER_CRITICAL(&webCommandMux);
    commands = pendingWebCommands; pendingWebCommands = 0;
    state = pendingWebState; brightness = pendingWebBrightness; seconds = pendingWebCountdown;
    drinkStart = pendingHydrationStart; drinkEnd = pendingHydrationEnd;
    drinkInterval = pendingHydrationInterval; lunchEnd = pendingLunchEnd;
    todoIndex = pendingWebTodoIndex; todoDelete = pendingWebTodoDelete;
    strlcpy(todo, pendingWebTodo, sizeof(todo)); strlcpy(deadline, pendingWebDeadline, sizeof(deadline));
    strlcpy(ssid, pendingWebSsid, sizeof(ssid)); strlcpy(password, pendingWebPassword, sizeof(password));
    portEXIT_CRITICAL(&webCommandMux);
    if (!commands) return;
    if (commands & WEB_CMD_HYDRATION_TEST) showHydrationPopup();

    if ((commands & WEB_CMD_STATE) && state != 7) {
        stateUntilEpoch = 0;
        stateUntilMillis = 0;
        prefs.remove("stateUntil");
        updateStatusUi(state);
        prefs.putUChar("state", state);
        if (statusUntilLabel) lv_label_set_text(statusUntilLabel, "");
        showToast("工作状态已由手机更新");
    }
    if ((commands & WEB_CMD_STATE) && state == 7) startLunchBreak();
    if (commands & WEB_CMD_REMINDERS) {
        hydrationStartMinutes = drinkStart; hydrationEndMinutes = drinkEnd;
        hydrationIntervalMinutes = drinkInterval; lunchEndMinutes = lunchEnd;
        prefs.putUShort("drinkStart", hydrationStartMinutes);
        prefs.putUShort("drinkEnd", hydrationEndMinutes);
        prefs.putUShort("drinkEvery", hydrationIntervalMinutes);
        prefs.putUShort("lunchEnd", lunchEndMinutes);
        showToast("提醒设置已由手机保存");
    }
    if (commands & WEB_CMD_TODO) {
        for (int i = 0; i < 8; ++i) if (todoItems[i].text.isEmpty()) {
            todoItems[i].text = todo; todoItems[i].deadline = deadline; todoItems[i].done = false;
            sortTodosByDeadline(); saveTodos(); rebuildTodoList(); homeTodoRotationIndex = 0; refreshHomeTodoPreview(); break;
        }
    }
    if ((commands & WEB_CMD_TODO_ACTION) && todoIndex < 8 && !todoItems[todoIndex].text.isEmpty()) {
        if (todoDelete) {
            for (int i = todoIndex; i < 7; ++i) todoItems[i] = todoItems[i + 1];
            todoItems[7] = {};
        } else todoItems[todoIndex].done = !todoItems[todoIndex].done;
        sortTodosByDeadline(); saveTodos(); rebuildTodoList(); homeTodoRotationIndex = 0; refreshHomeTodoPreview();
    }
    if (commands & WEB_CMD_COUNTDOWN) {
        countdownRemainingSeconds = seconds; countdownEndMillis = millis() + seconds * 1000UL; countdownRunning = true;
        if (countdownStartButtonLabel) lv_label_set_text(countdownStartButtonLabel, "暂停");
    }
    if ((commands & WEB_CMD_BRIGHTNESS) && panelBoard && panelBoard->getBacklight()) {
        panelBoard->getBacklight()->setBrightness(brightness);
    }
    if (commands & WEB_CMD_WIFI) {
        savedSsid = ssid; savedPassword = password; rememberConnectedNetwork(savedSsid, savedPassword);
        wifiConnectStarted = millis(); WiFi.disconnect(); WiFi.begin(savedSsid.c_str(), savedPassword.c_str());
    }
}
