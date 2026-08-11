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

void sendCool() {
  ac.on();
  ac.setMode(kMitsubishiAcCool);
  ac.setTemp(26);
  ac.setFan(kMitsubishiAcFanAuto);
  ac.send();
}

void sendHeat() {
  ac.on();
  ac.setMode(kMitsubishiAcHeat);
  ac.setTemp(27);
  ac.setFan(kMitsubishiAcFanAuto);
  ac.send();
}

// ハンドラ
void handleOff() {
  Serial.println("HTTP /api/off");

  sendOff();

  server.send(200, "text/plain", "OK");
}

void handleCool() {
  Serial.println("HTTP /api/cool");

  sendCool();

  server.send(200, "text/plain", "OK");
}

void handleHeat() {
  Serial.println("HTTP /api/heat");

  sendHeat();

  server.send(200, "text/plain", "OK");
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
  server.on("/api/cool", handleCool);
  server.on("/api/heat", handleHeat);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  
}
