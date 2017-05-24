/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 by Daniel Eichhorn
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

// Include the correct display library
// For a connection via I2C using Wire include
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
// or #include "SH1106.h" alis for `#include "SH1106Wire.h"`
// For a connection via I2C using brzo_i2c (must be installed) include
// #include <brzo_i2c.h> // Only needed for Arduino 1.6.5 and earlier
// #include "SSD1306Brzo.h"
// #include "SH1106Brzo.h"
// For a connection via SPI include
// #include <SPI.h> // Only needed for Arduino 1.6.5 and earlier
// #include "SSD1306Spi.h"
// #include "SH1106SPi.h"

// Include custom images
#include "images.h"

#include <SPI.h>
#include <FS.h>

// Initialize the OLED display using SPI
// D5 -> CLK
// D7 -> MOSI (DOUT)
// D0 -> RES
// D2 -> DC
// D8 -> CS
// SSD1306Spi        display(D0, D2, D8);
// or
// SH1106Spi         display(D0, D2);

// Initialize the OLED display using brzo_i2c
// D3 -> SDA
// D5 -> SCL
// SSD1306Brzo display(0x3c, D3, D5);
// or
// SH1106Brzo  display(0x3c, D3, D5);

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, D3, D4);
// SH1106 display(0x3c, D3, D5);


#define DEMO_DURATION 3000
typedef void (*Demo)(void);

int demoMode = 0;
int counter = 1;


#define relay1   D1  // signal output for relay 1
#define relay2   D2  // signal output for relay 2
 

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println();
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay1, LOW);
  digitalWrite(relay1, LOW);
    
  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

//  // LOAD FILESYSTEM
//  if (!SPIFFS.begin()) {
//    Serial.println("Failed to mount file system");
//  }
//
//  File f;
//  // OPEN FILE (ENTER FILE WITH PATH WIRH SLASH IN FRONT)
//  SPIFFS.format();
//  f = SPIFFS.open("/ginlee.MONO", "r");
//
//  if (f) {
//    int s = f.size();
//    display.drawString(64, 44, "we're in");
//
//    String data = f.readString();
//    //Serial.println(data);
//
//    const char* data1 = data.c_str();
//    f.close();
//
//
//    display.drawXbm(0, 0, 125, 60, data1);
//    display.display();  
//    delay(10000);
//  }
}

unsigned long lap = millis();
uint32_t delayMS = 5000;
int millislen;

void loop() {
  // clear the display
  display.clear();

  // The coordinates define the center of the text
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  millislen = String(millis()).length();
  display.drawString(64, 22, String(millis()).substring(0,millislen-3) + '.' + String(millis()).substring(millislen-3,millislen) + "ms");


  if (millis() % 15000 <= 5000) {
  display.setFont(ArialMT_Plain_10);    
  display.drawString(64, 44, "blowers are on");
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  display.display();
  }
  else {
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  }
  // write the buffer to the display
  display.display();

  
//  if (millis() - lap > delayMS) {
//    demoMode = (demoMode + 1)  % demoLength;
//    timeSinceLastModeSwitch = millis();
//  }
  counter++;
  delay(10);
}

