#include <Arduino.h>
#include <Servo.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LiquidCrystal_I2C.h>

#define SSID_WIFI "Grand G7 Pasar Baru"
#define SSID_PASSWORD "pasarBaru"
#define FIRST_SERVO_PIN 13
#define SECOND_SERVO_PIN 27
#define SCL_PIN 33
#define SDA_PIN 32
#define COLUMS    16
#define ROWS      2

Servo servos = Servo();
AsyncWebServer server(80);
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

int neutral = 2150;
int turn_off = 2900;
int turn_on = 1500;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>ESP32 Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      background-color: #f0f0f0;
      margin: 0;
      padding: 0;
    }
    h1 {
      color: #333;
    }
    .container {
      margin: 50px auto;
      padding: 20px;
      max-width: 600px;
      background-color: #fff;
      border-radius: 10px;
      box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
    }
    button {
      display: inline-block;
      margin: 10px;
      padding: 10px 20px;
      font-size: 16px;
      color: #fff;
      background-color: #007bff;
      border: none;
      border-radius: 5px;
      cursor: pointer;
      transition: background-color 0.3s;
    }
    button:hover {
      background-color: #0056b3;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>ESP32 Web Server</h1>
    <p>Control First Servo</p>
    <button onclick="fetch('/first-servo?action=neutral')">Neutral</button>
    <button onclick="fetch('/first-servo?action=turn_on')">Turn On</button>
    <button onclick="fetch('/first-servo?action=turn_off')">Turn Off</button>

    <p>Control Second Servo</p>
    <button onclick="fetch('/second-servo?second=neutral')">Neutral</button>
    <button onclick="fetch('/second-servo?second=turn_on')">Turn On</button>
    <button onclick="fetch('/second-servo?second=turn_off')">Turn Off</button>
  </div>
  <script>
    function fetch(url) {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", url, true);
      console.log(xhr);
      console.log(url);
      xhr.send();
    }
  </script>
</body>
</html>
)rawliteral";

void setup() {
    Serial.begin(115200);
    delay(1000);

    while (lcd.begin(COLUMS, ROWS, LCD_5x8DOTS) != 1) 
    {
      Serial.println(F("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal."));
      delay(5000);   
    }

  
    lcd.setCursor(0, 0);
    lcd.print("hola user!");
    
    lcd.setCursor(0, 1);
    lcd.print(F("waiting for ip..."));

    Serial.println("Connecting to Wi-Fi");
    WiFi.begin(SSID_WIFI, SSID_PASSWORD);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED) {
        attempts++;
        Serial.print(".");
        delay(300);
        if (attempts > 30) {
            lcd.setCursor(0, 0);
            lcd.print("Abort Wi-Fi");
            
            lcd.setCursor(0, 1);
            lcd.print(F("Restarting..."));
            Serial.println("\n\nFailed to connect to Wi-Fi. Restarting...");
            ESP.restart();
        }
    }

    Serial.println("\n\nConnected to Wi-Fi");
    Serial.print("IP address: http://");
    Serial.println(WiFi.localIP());
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IP Address");
    
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP().toString());

    // Serve index.html
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("Serving /");
        request->send_P(200, "text/html", index_html);
    });

    // Handle first servo actions
    server.on("/first-servo", HTTP_GET, [](AsyncWebServerRequest *request){
        if (request->hasParam("action")) {
            String action = request->getParam("action")->value();
            Serial.print("First servo action: ");
            Serial.println(action);
            if (action == "neutral") {
                int mappingValue = map(neutral, 700, 3395, 0, 180);
                servos.write(FIRST_SERVO_PIN, mappingValue);
            } else if (action == "turn_on") {
              Serial.println("first servo");
              Serial.println(turn_on - 1000);
              int mappingValue = map(turn_on, 700, 3395, 0, 180);
              servos.write(FIRST_SERVO_PIN, mappingValue);
            } else if (action == "turn_off") {
                int mappingValue = map(turn_off, 700, 3395, 0, 180);
                servos.write(FIRST_SERVO_PIN, mappingValue);
            }
            request->send(200, "text/plain", "OK");
        } else {
            request->send(400, "text/plain", "Bad Request");
        }
    });

    // Handle second servo actions
    server.on("/second-servo", HTTP_GET, [](AsyncWebServerRequest *request){
        if (request->hasParam("second")) {
            String action = request->getParam("second")->value();
            Serial.print("Second servo action: ");
            Serial.println(action);
            if (action == "neutral") {
                int mappingValue = map(neutral, 700, 3395, 0, 180);
                servos.write(SECOND_SERVO_PIN, mappingValue);
            } else if (action == "turn_on") {
              Serial.println("second servo");
              Serial.println(turn_off);
                int mappingValue = map(turn_off, 700, 3395, 0, 180);
                servos.write(SECOND_SERVO_PIN, mappingValue);
            } else if (action == "turn_off") {
                int mappingValue = map(turn_on, 700, 3395, 0, 180);
                servos.write(SECOND_SERVO_PIN, mappingValue);
            }
            request->send(200, "text/plain", "OK");
        } else {
            request->send(400, "text/plain", "Bad Request");
        }
    });

    server.begin();
}

void loop() {
    // No code needed here, everything is handled by the server
}
