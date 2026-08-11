# ESP32 AC Controller

Seeed Studio XIAO ESP32C6 で三菱エアコンへ赤外線信号を送るHTTPサーバーです。

## 機能

- Wi-Fiに接続してHTTPサーバーを起動
- 三菱エアコンの停止、除湿、冷房、暖房を赤外線で送信
- 冷房・暖房は `temp` クエリで温度指定可能
- Wi-Fi接続は最大5回リトライ

## 環境

- Board: Seeed Studio XIAO ESP32C6
- Framework: Arduino
- Build tool: PlatformIO
- IR library: `crankyoldgit/IRremoteESP8266`

設定は [platformio.ini](platformio.ini) を参照。

## 配線

現状コードでは赤外線送信ピンは `D3` です。

```cpp
#define IR_PIN D3
```

IR LEDや送信モジュールの配線は、使用するモジュールの仕様に合わせてください。

## Wi-Fi設定

実際のSSIDとパスワードは `include/secrets.h` に書きます。

```cpp
#pragma once

#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
```

雛形は [include/secrets.example.h](include/secrets.example.h) にあります。

`include/secrets.h` は秘密情報なのでGitHubには上げないでください。

## ビルド

```powershell
pio run
```

## 書き込み

```powershell
pio run -t upload
```

## シリアルモニタ

```powershell
pio device monitor -b 115200
```

起動後、接続に成功するとIPアドレスが表示されます。

```text
WiFi connected
IP address:<ESP32_IP>
HTTP server started
```

## API

以下の `<ESP32_IP>` は、シリアルモニタに表示されたIPアドレスに置き換えてください。

### 動作確認

```text
GET /
```

レスポンス:

```text
ESP32 OK
```

例:

```powershell
curl "http://<ESP32_IP>/"
```

### 停止

```text
GET /api/off
```

レスポンス:

```text
Send Off
```

例:

```powershell
curl "http://<ESP32_IP>/api/off"
```

### 除湿

```text
GET /api/dry
```

レスポンス:

```text
Send Dry
```

例:

```powershell
curl "http://<ESP32_IP>/api/dry"
```

### 冷房

```text
GET /api/cool
GET /api/cool?temp=25
```

`temp` を省略すると `26` 度になります。

レスポンス例:

```text
OK Cool25
```

例:

```powershell
curl "http://<ESP32_IP>/api/cool?temp=25"
```

### 暖房

```text
GET /api/heat
GET /api/heat?temp=27
```

`temp` を省略すると `27` 度になります。

レスポンス例:

```text
OK heat27
```

例:

```powershell
curl "http://<ESP32_IP>/api/heat?temp=27"
```

## 温度指定

冷房・暖房の `temp` は `16` から `31` の範囲です。

範囲外の場合は以下を返します。

```text
Invalid temp
```

HTTPステータスは `400` です。

## Wi-Fi接続の挙動

起動時にWi-Fiへ接続します。接続処理では以下を行います。

- STAモードにする
- Wi-Fiスリープを無効化
- 接続前にWi-Fiを一度OFFにする
- 1回あたり最大20秒待つ
- 最大5回リトライする

5回失敗するとHTTPサーバーは起動しません。
