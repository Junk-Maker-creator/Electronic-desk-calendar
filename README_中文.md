# 桌面电子日历

适用硬件：Waveshare `ESP32-S3-Touch-LCD-7`（非 7B），800×480 触控版。

## 功能

- 屏幕软键盘输入并保存 Wi-Fi SSID、密码
- 北京时间、日期和星期（NTP 自动校时）
- 武汉实时天气：温度、湿度、风速、天气状态
- 上班、忙碌、开会、休息、下班五种工作状态
- Wi-Fi 自动重连、忘记网络、手动刷新天气
- 喝水提醒、久坐站立提醒和午休辅助
- Wi-Fi 与工作状态掉电保存

为了避免额外安装大型中文字库，屏幕文字使用英文；本说明为中文。Wi-Fi 名称和密码可输入英文、数字及符号。

## Arduino IDE 环境

1. 安装 Arduino IDE 2.x。
2. 开发板管理器安装 `esp32 by Espressif Systems` 3.0.6 或更新的 3.0.x 版本。
3. 安装本项目所需库：
   - `ESP32_Display_Panel`（优先使用微雪示例包内的 1.0.0）
   - `ESP32_IO_Expander`（优先使用微雪示例包内的 1.0.1）
   - `lvgl` 8.4.0（不要使用 LVGL 9）
4. 微雪官方示例包已经包含匹配的库和 `lv_conf.h`。如果库管理器安装后编译异常，使用官方仓库 `examples/Arduino/libraries` 中的版本覆盖安装。

### 使用仓库自带依赖包（推荐）

如果你下载的是本仓库 ZIP：

1. 关闭 Arduino IDE。
2. 解压 ZIP，在仓库根目录进入 `_Complete\Arduino_libraries`。
3. 把 `Arduino_libraries` 文件夹里的**所有内容**复制到 Arduino 的 sketchbook `libraries` 文件夹。
4. 最终应能看到 `ESP32_Display_Panel\library.properties`。
5. 同一目录还应有 `ESP32_IO_Expander`、`esp-lib-utils`、`lvgl` 四个库文件夹，以及一个 `lv_conf.h` 文件。
6. 重新打开 Arduino IDE，再打开仓库内的 `WuhanDeskPanel\WuhanDeskPanel.ino` 编译。

如果出现“目标文件已存在”，选择替换。不要使用 LVGL 9，否则接口不兼容。

## 编译与烧录

1. 保持 `WuhanDeskPanel` 文件夹内的 `.ino`、`.inc`、`.c`、`.cpp` 和 `.h` 文件完整，不要只复制主文件。
2. 打开 `WuhanDeskPanel\WuhanDeskPanel.ino`。
3. 开发板选择 `Waveshare ESP32-S3-Touch-LCD-7`，选择对应 COM 口。
   - `PSRAM`：`OPI PSRAM`
   - `Flash Mode`：`QIO 80MHz`
   - `Flash Size`：`8MB`
   - `Partition Scheme`：`huge_app`（3MB 应用分区）
   - 使用 `USB` 接口上传时将 `USB CDC On Boot` 设为 `Enabled`；使用 `USB TO UART` 接口时设为 `Disabled`
4. 使用有数据传输能力的 Type-C 线连接板上标有 `USB TO UART`/`UART` 的接口。
5. 点击“上传”。完成后按一下板上的 `RESET`。
6. 如果无法进入下载模式，按住 `BOOT`，短按 `RESET`，松开 `BOOT` 后再次上传。

### Arduino CLI 快速编译

在仓库根目录运行 `powershell -ExecutionPolicy Bypass -File .\build.ps1`。脚本会自动使用 `_Complete\Arduino_libraries`，并使用与已验证固件相同的开发板、PSRAM、Flash 和 `huge_app` 参数。

## 首次使用

首次启动会自动进入 `WI-FI` 页面。点击输入框调出屏幕键盘，输入 Wi-Fi 名称和密码，点击 `SAVE & CONNECT`。连接后时钟会自动校准，武汉天气会自动刷新。

注意：ESP32-S3 只支持 2.4GHz Wi-Fi。如果路由器把 2.4GHz 和 5GHz 合并为同一名称，通常也能连接；连接失败时建议单独开启 2.4GHz 网络进行测试。

天气数据来自 Open-Meteo，程序不需要 API Key。网络请求使用 HTTPS；示例为方便嵌入式设备部署未校验证书链，不应用于传输敏感数据。
