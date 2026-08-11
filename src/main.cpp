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

void handleRoot() {
  server.send(200, "text/plain", "ESP32 OK");
}

void handleCool() {
  Serial.println("HTTP /cool");
  
  ac.begin();
  delay(3000);

  Serial.println("Send Cool 26");

  ac.on();
  ac.setMode(kMitsubishiAcCool);
  ac.setTemp(26);
  ac.setFan(kMitsubishiAcFanAuto);
  ac.send();

  server.send(200, "text/plain", "OK");

}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // WiFi接続
  Serial.println();
  Serial.println("WiFi connecting...");

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.disconnect();
  delay(500);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.print("IP address:");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/cool", handleCool);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  
}
