#define LILYGO_WATCH_2019_WITH_TOUCH
#include <LilyGoWatch.h>
TTGOClass *ttgo;

#include <SimpleDHT.h>

#include <WiFi.h>
#include <HTTPClient.h>


const char* ssid = "Richard";
const char* password = "13zuoshuaige";

//Your Domain name with URL path or IP address with path
const char* serverName = "http://3.94.162.233:8080/setValue";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 300;

String response;

// PIR传感器连接到ESP32的GPIO25
int pinPIR = 25;

// 活动检测变量
unsigned long lastActivityTime = 0;
String httpGETRequest(const char* serverName) {
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
void setup() {
  // 初始化手表
  Serial.begin(115200);
    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    ttgo->openBL();
    ttgo->tft->init();

  // 初始化PIR传感器
  pinMode(pinPIR, INPUT);

  // 初始化屏幕
  ttgo->tft->fillScreen(TFT_BLACK);
  ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK);
  ttgo->tft->setCursor(0, 0);
  ttgo->tft->setTextSize(4);

    WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");

}

void loop() {
  // 检测活动
  int h = 0;
  if (detectActivity()) {
    lastActivityTime = millis();
    displayMessage("alive");
    h=1;
  }

  // 检查自上次活动以来是否已过10秒钟
  if (millis() - lastActivityTime > 10000) {
    displayMessage("Your family has been inactive for a long time");
    h = 2;
  }
  int t = 0;  
  String url = String(serverName) + "?t=" + t + "&h=" + h;
  Serial.println(url);       
  response = httpGETRequest(url.c_str());
}

bool detectActivity() {
  int sensorValue = digitalRead(pinPIR);
  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);
  if (sensorValue == HIGH) {
    return true;
  }
  return false;
}

void displayMessage(String message) {
  ttgo->tft->fillScreen(TFT_BLACK);
  ttgo->tft->setCursor(0, 0);
  ttgo->tft->println(message);
}
