#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// WiFi Bağlantı bilgileri
const char* ssid = "Bugatti"; 
const char* password = "123456789";

// Web server
ESP8266WebServer server(80);

// Motor pinleri
const int motor1Pin1 = D1; 
const int motor1Pin2 = D2; 
const int motor2Pin1 = D3; 
const int motor2Pin2 = D4; 

void setup() {
  Serial.begin(115200);
  
  // WiFi'ye bağlan
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  // Web server başlat
  server.on("/komut=ileri", []() {
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    server.send(200, "text/plain", "İleri hareket");
  });

  server.on("/komut=geri", []() {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
    server.send(200, "text/plain", "Geri hareket");
  });

  server.on("/komut=sag", []() {
    // Sağ hareket kodları
    server.send(200, "text/plain", "Sağa hareket");
  });

  server.on("/komut=sol", []() {
    // Sol hareket kodları
    server.send(200, "text/plain", "Sola hareket");
  });

  server.begin();
}

void loop() {
  server.handleClient();
}
