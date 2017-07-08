/*
 WeMOS Shooting Range by Seth Loh (https://github.com/lackdaz)
 Project details: https://github.com/lackdaz/RBG-Range
*/

/* Include Libraries here */
#include <IRremoteESP8266.h>

/* Pin Definitions */

/* Range 1 */
#define S1 D7
#define M1 D3

/* Range 2 */
#define S2 D6
#define M2 D2

/* Range 3 */
//#define S3 D5
//#define M3 D1

/* Programming Switch*/
#define S0 A0 // Programming Button Pin

/* Override Switch*/
#define S4 D8

/* Infra-red Sensor */
#define IR_PIN D0 // IR signal pin

IRrecv irrecv(IR_PIN);
decode_results results;

unsigned long CurrentValue = 0;
unsigned long StoredCode = 0;

//int SPDTState = 0;         // variable for reading the pushbutton status
//int progState = 0;         // variable for reading the pushbutton status

byte rangeNum = 0;

int progState = 1;         //set default buttonstate to off
int RecordState = 0;         //is the reciever in record mode
int outputState = 1;         //is the output on or off
int overrideState = 0;      //override state set to off

byte limit[2] = {S1, S2};
byte motor[2] = {M1, M2};
byte IRcode[2];

bool range_check(byte x){
  int SPDTState = digitalRead(limit[x]);
  Serial.print("Checking Range");
  Serial.println(x+1);
  unsigned long start = millis();

  while (SPDTState == LOW){
    SPDTState = digitalRead(limit[x]);
    digitalWrite(motor[x], HIGH); //flag this!
    delay(50);
    int elapsed = millis() - start;
    Serial.println(elapsed);
    if (elapsed > 10000) {
      return false;
    }
  }
  while (SPDTState == HIGH) {
    SPDTState = digitalRead(limit[x]);
    delay(50);
  }
  digitalWrite(motor[x], LOW);
  delay(1000);
  return true;
}

void range_reset(byte x){

int SPDTState = digitalRead(limit[x]);

      Serial.print("Range ");
      Serial.println(x+1);
      Serial.println(" Motor is moving");

  while (SPDTState == LOW){
      SPDTState = digitalRead(limit[x]);
      digitalWrite(motor[x], HIGH); //flag this!
      delay(50);
    }

      Serial.println("Clearing Obstacle");

    //when the arm hits the flap
    while (SPDTState == HIGH) {
      //keep reading until the flap closes
      SPDTState = digitalRead(limit[x]);
      delay(50);
    }
      digitalWrite(motor[x], LOW);
      //outputState = 0;
      //Serial.print("outputState=");
      //Serial.println(outputState);
      delay(1000);
      Serial.print("Range ");
      Serial.print(x+1);
      Serial.println(" Completed!");
}

void blinkX(byte &x) {
  for (byte i = 0; i < x; i++) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
  }
}

bool checkProgButton() {
  Serial.println(analogRead(S0));
  if (analogRead(S0) < 500){
    progState = true;
  }
  else progState = false;
}

/* SETUP */

void setup() {
  Serial.begin(9600);
  pinMode(S1, INPUT);         // Initialize the SPDT pin as an input
  pinMode(S2, INPUT);         // Initialize the SPDT pin as an input
  pinMode(M1, OUTPUT);  // Initialize the Motor 1 as an output
  pinMode(M2, OUTPUT);  // Initialize the Motor 2 as an output

  pinMode(S0, INPUT_PULLUP);       // Initialize the Programming Button pin as an input
  pinMode(S4, INPUT);         // Initialize the SPDT pin as an input

  pinMode(IR_PIN, INPUT);       // Initialize the Button pin as an input
  pinMode(LED_BUILTIN, OUTPUT);  // Initialize the Motor Pin 1 as an output

// Start the receiver
//  irrecv.enableIRIn();

// due to the reverse logic of the Wemos LED, this turns it off
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);

  for (byte i = 0; i < 2; i++) {
  //  range_reset(i);
    if (range_check(i)){
      rangeNum++;
    }
  }

  Serial.print("No. of ranges: ");
  Serial.println(rangeNum);

  blinkX(rangeNum);

  // to shut all motors
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);
  //digitalWrite(M3, LOW);

  Serial.println("Setup completed");
}



void loop() {

// read the state of programming button
checkProgButton();

// read the state of override button
checkOverrideButton();

//overrideState = digitalRead(S4);
//Serial.println("Override State: ");
//Serial.println(overrideState);
//
//// if a signal is detected, store the value
//if (irrecv.decode(&results)) {
//  Serial.print("Signal detected!");
//  Serial.println(results.value);
//  //delay(100);
//  CurrentValue = (results.value);
//
//   // if the recieved value equals the programed value, then toggle the output state
//
//  for (byte i = 0; i < 2; i++) {
//    if (IRcode[i] == CurrentValue) {
//      range_reset(i);
//      break;
//    }
//  }
//   // if the record mode is activated store the current value as the programed value
//  if (RecordState == 1) {
//    IRcode[0] = CurrentValue; // temporary
//    RecordState = 0;
//    digitalWrite(LED_BUILTIN, HIGH);  // turn off the LED once the frequency is recorded
//    Serial.println(StoredCode);  //displays stored code for reference
//   }
//}
//
//if (progState == LOW){
//  Serial.println("Button Pressed!");
//
//  //wait for the button to be released
//  while (progState == LOW) {
//  if (analogRead(S0) > 500){
//    progState = true;
//    delay(50);
//
//  }
//    else progState = false;
//    delay(50);
//  }
//
//  //turn on the LED to indicate that record mode is on
//    digitalWrite(LED_BUILTIN, LOW);
//    RecordState = 1;
//}
//
//if (overrideState == HIGH){
//  Serial.println();
//  while (overrideState == HIGH) {
//    overrideState = digitalRead(S4);
//    delay(50);
//  }
//  for (byte i = 0; i < 2; i++) {
//      range_reset(i);
//        Serial.println(i);
//  }
//  delay(50);
//    Serial.println("3");
//  overrideState = LOW;
//}

// Receive the next value
//irrecv.resume();
delay(100);
}
