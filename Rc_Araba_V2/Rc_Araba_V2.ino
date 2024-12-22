/******* WiFi Robot Uzaktan Kumanda Modu ********/
#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h> 
#include <ArduinoOTA.h>

// Sürüş Motorları için bağlantılar
int PWM_A = D1;
int PWM_B = D2;
int DIR_A = D3;
int DIR_B = D4;

const int buzPin = D5;      // Dijital pin D5'i buzzer pini olarak ayarla (aktif buzzer kullan)
const int ledPin = D8;      // Dijital pin D8'i LED pini olarak ayarla (çok parlak LED kullan)
const int wifiLedPin = D0;  // Dijital pin D0'ı bağlantı göstergesi olarak ayarla, NodeMCU WiFi'ye bağlandığında LED yanar (STA modu)

String command;          // Uygulama komut durumunu saklamak için string
int SPEED = 1023;        // 330 - 1023.
int speed_Coeff = 3;

ESP8266WebServer server(80);      // HTTP isteği dinleyen bir web sunucu nesnesi oluştur

unsigned long previousMillis = 0;

String sta_ssid = "Porche";      // Bağlanmak istediğiniz Wifi ağlarını ayarlayın
String sta_password = "123456789";  // Wifi ağı için şifreyi ayarlayın


void setup(){
  Serial.begin(115200);    // Seri kütüphanesini 115200 bps hızında ayarla
  Serial.println();
  Serial.println("WiFi Robot Uzaktan Kumanda Modu");
  Serial.println("--------------------------------------");
 
  pinMode(buzPin, OUTPUT);      // Buzzer pinini çıkış olarak ayarla
  pinMode(ledPin, OUTPUT);      // LED pinini çıkış olarak ayarla
  pinMode(wifiLedPin, OUTPUT);  // Wifi LED pinini çıkış olarak ayarla
  digitalWrite(buzPin, LOW);
  digitalWrite(ledPin, LOW);
  digitalWrite(wifiLedPin, HIGH);
    
  // Tüm motor kontrol pinlerini çıkış olarak ayarla
  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  
  // Motorları kapat - Başlangıç durumu
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);

  // NodeMCU Wifi ana bilgisayar adını çipin MAC adresine göre ayarla
  String chip_id = String(ESP.getChipId(), HEX);
  int i = chip_id.length()-4;
  chip_id = chip_id.substring(i);
  chip_id = "wificar-" + chip_id;
  String hostname(chip_id);
  
  Serial.println();
  Serial.println("Ana Bilgisayar Adı: "+hostname);

  // İlk olarak, NodeMCU'yu Wifi ağına bağlanmak için STA modunda ayarla
  WiFi.mode(WIFI_STA);
  WiFi.begin(sta_ssid.c_str(), sta_password.c_str());
  Serial.println("");
  Serial.print("Bağlanıyor: ");
  Serial.println(sta_ssid);
  Serial.print("Şifre: ");
  Serial.println(sta_password);

  // Wifi ağına bağlanmayı 10 saniye boyunca dene
  unsigned long currentMillis = millis();
  previousMillis = currentMillis;
  while (WiFi.status() != WL_CONNECTED && currentMillis - previousMillis <= 10000) {
    delay(500);
    Serial.print(".");
    currentMillis = millis();
  }

  // Eğer Wifi ağına bağlanılamazsa, NodeMCU'yu AP modunda ayarla
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi-STA-Modu");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    digitalWrite(wifiLedPin, LOW);    // Wifi LED'i bağlıysa STA modunda açılır
    delay(3000);
  } else {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(hostname.c_str());
    IPAddress myIP = WiFi.softAPIP();
    Serial.println("");
    Serial.println("WiFi, " + sta_ssid + "'ye bağlanamadı");
    Serial.println("");
    Serial.println("WiFi-AP-Modu");
    Serial.print("AP IP adresi: ");
    Serial.println(myIP);
    digitalWrite(wifiLedPin, HIGH);   // Wifi LED'i AP modunda kapanır
    delay(3000);
  }
 

  server.on ( "/", HTTP_handleRoot );       // Bir istemci "/" URI'sini istediğinde 'handleRoot' fonksiyonunu çağır
  server.onNotFound ( HTTP_handleRoot );    // Bir istemci bilinmeyen bir URI (yani "/" dışında) istediğinde 'handleNotFound' fonksiyonunu çağır
  server.begin();                           // Sunucuyu başlat
  
  ArduinoOTA.begin();                       // Wifi üzerinden OTA (Over-The-Air) güncelleme/alma için etkinleştir
}


void loop() {
    ArduinoOTA.handle();          // İstemcilerden gelen OTA güncelleme isteği için dinle
    server.handleClient();        // İstemcilerden gelen HTTP isteklerini dinle
    
    command = server.arg("State");          // HTTP isteğini kontrol et, "State" argümanı varsa
}
