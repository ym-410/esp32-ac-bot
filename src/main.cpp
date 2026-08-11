#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <secrets.h>

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Mitsubishi.h>

#define IR_PIN D3

WebServer server(80);
IRMitsubishiAC ac(IR_PIN);

bool connectWifi() {
  Serial.println();
  Serial.println("WiFi connecting...");

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);

  for (int attempt = 1; attempt <= 5; attempt++) {
    Serial.print("WiFi attempt ");
    Serial.println(attempt);

    // 初期化
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    delay(1000);

    // 再接続
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    delay(500);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    // unsigned long: 負の数を扱わない
    unsigned long start = millis(); // millis(): 電源を入れてから何ミリ秒たったか

    while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
      delay(500);
      Serial.print(".");
    }

    // 接続成功
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("WiFi connected");
      Serial.print("IP address:");
      Serial.println(WiFi.localIP());
      return true;
    }

    // 接続失敗
    Serial.print("WiFi failed. status=");
    Serial.println(WiFi.status());
    delay(2000);
  }

  return false;
}

void handleRoot() {
  server.send(200, "text/plain", "ESP32 OK");
}

// エアコン処理
void sendOff() {
  ac.off();
  ac.send();
}

void sendDry() {
  ac.on();
  ac.setMode(kMitsubishiAcDry);
  ac.setFan(kMitsubishiAcFanAuto);
  ac.send();
}

void sendCool(uint8_t temp) {
  ac.on();
  ac.setMode(kMitsubishiAcCool);
  ac.setTemp(temp);
  ac.setFan(kMitsubishiAcFanAuto);
  ac.send();
}

void sendHeat(uint8_t temp) {
  ac.on();
  ac.setMode(kMitsubishiAcHeat);
  ac.setTemp(temp);
  ac.setFan(kMitsubishiAcFanAuto);
  ac.send();
}

// ハンドラ
void handleOff() {
  Serial.println("HTTP /api/off");

  sendOff();

  server.send(200, "text/plain", "Send Off");
}

void handleDry() {
  Serial.println("HTTP /api/dry");

  sendDry();

  server.send(200, "text/plain", "Send Dry");
}

void handleCool() {
  Serial.println("HTTP /api/cool");

  int temp = server.arg("temp").toInt();
  
  if (!server.hasArg("temp")) {
    temp = 26;
  }

  if (temp < 16 || temp > 31) {
    server.send(400, "text/plain", "Invalid temp");
    return;
  }

  sendCool(temp);

  server.send(200, "text/plain", "OK Cool" + String(temp));
}

void handleHeat() {
  Serial.println("HTTP /api/heat");

  int temp = server.arg("temp").toInt();
  
  if (!server.hasArg("temp")) {
    temp = 27;
  }

  if (temp < 16 || temp > 31) {
    server.send(400, "text/plain", "Invalid temp");
    return;
  }

  sendHeat(temp);

  server.send(200, "text/plain", "OK heat" + String(temp));
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  ac.begin();

  if (!connectWifi()) {
    Serial.println("WiFi connection failed after retries.");
    return;
  }

  server.on("/", handleRoot);
  server.on("/api/off", handleOff);
  server.on("/api/dry", handleDry);
  server.on("/api/cool", handleCool);
  server.on("/api/heat", handleHeat);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  
}
