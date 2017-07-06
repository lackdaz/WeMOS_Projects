/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
//#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include <Adafruit_ssd1306syp.h>                        // Adafruit oled library for display
Adafruit_ssd1306syp display(D3,D4);      


const char* ssid     = "chewchew";
const char* password = "chewkumwing";

const char* ssidPhone     = "mao";
const char* passwordPhone = "abrasion";

//SSD1306  display(0x3c, D3, D4);

const char* host = "data.sparkfun.com";
const char* streamId   = "....................";
const char* privateKey = "....................";

void setup() {
  Serial.begin(115200);
  delay(10);

//  // Initialising the UI will init the display too.
//  display.init();
//  display.flipScreenVertically();
//  display.setFont(ArialMT_Plain_10);
  
    // you set up the OLED first
  display.initialize();                                 // Initialize OLED display
  display.clear();                                      // Clear OLED display
  display.setTextSize(1);                               // Set OLED text size to small
  display.setTextColor(WHITE);                          // Set OLED color to White
  display.setCursor(0,0);                               // Set cursor to 0,0


  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  display.print("Connecting to ");
  display.println(ssid);  
  display.update();
  
//  display.drawString(0, 0, "Connecting to ");
//  display.drawString(0, 10, ssid);
//  display.display();
//  display.drawString(0, 0, ".");

  String temp = ssid;
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED && temp.length() <= 15) {
    delay(500);
    display.clear();
    Serial.print(".");
//    display.drawString(0, 0, "Connecting to ");
//    display.drawString(0, 10, temp);
//    display.drawString(0, 20, String(temp.length()));    
//    display.display();

    display.print(".");
    display.update(); 
    temp += '.';
  }

  display.clear();
  display.print("Connecting to ");
  display.println(temp);  
  display.update();

  if (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssidPhone, passwordPhone);
      temp = ssidPhone;
      while (WiFi.status() != WL_CONNECTED || temp.length() <= 10) {
        delay(500);
        Serial.print(".");
//        display.drawString(0, 0, "Connecting to ");
//        display.drawString(0, 10, temp);
//        display.display();
        display.print(".");
        display.update(); 
        temp += '.';
      }      
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

//  display.drawString(0, 20, "Wifi connected");
//  display.drawString(0, 30, "IP address: ");
//  display.drawString(0, 40, WiFi.localIP().toString().c_str());
//  display.display();

  display.println(WiFi.localIP());
  display.update();

  delay(1000);

}

int value = 0;

void loop() {
  delay(5000);
  ++value;

  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = "/input/";
  url += streamId;
  url += "?private_key=";
  url += privateKey;
  url += "&value=";
  url += value;
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
}

