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
#define IR_PIN D5 // IR signal pin

IRrecv irrecv(IR_PIN);
decode_results results;

unsigned long CurrentValue = 0;
unsigned long StoredCode = 0;

//int SPDTState = 0;         // variable for reading the pushbutton status
//int progState = 0;         // variable for reading the pushbutton status

const int rangeNum = 2;
byte pressNum = 0;

int progState = 1;         //set default buttonstate to off
int RecordState = 0;         //is the reciever in record mode
int outputState = 1;         //is the output on or off
int overrideState = 0;      //override state set to off

bool overrideButtonState = false;
bool prevOverrideState = true;

byte limit[rangeNum] = {S1, S2};
byte motor[rangeNum] = {M1, M2};
bool active[rangeNum] = {M1, M2};

unsigned long IRcode[3];

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
    if (elapsed > 5000) {
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

if (active[x] == false) {
  Serial.print("Error: Range");
  Serial.print(x+1);
  Serial.println(" is offline!");
  return;
}

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
      delay(50);
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

bool checkOverrideButton() {
//  Serial.println(digitalRead(S4));
  if (digitalRead(S4)){
    prevOverrideState = false;
    return true;
  }
  else {
    return false;
  }
}

bool checkProgButton() {
//  Serial.println(analogRead(S0));
  if (analogRead(S0) < 500){
    return true;
  }
  else return false;
}

void programmingMode() {
  digitalWrite(LED_BUILTIN, LOW);
  bool ledState = digitalRead(LED_BUILTIN);
  byte progNum = 0;
  while (ledState == LOW) {
    Serial.println("Release the button");
    while (checkOverrideButton()){
      delay(10);
    }
    Serial.println("Please click range #");
    while (!checkOverrideButton()){
      delay(10);
    }
    if (checkOverrideButton()){
      unsigned long start = millis();
      unsigned long elapsed = 0;
      while (elapsed < 1000) {
        elapsed = millis() - start;
        delay(10);
        if (!checkOverrideButton() && (!prevOverrideState)) {
          progNum++;
          prevOverrideState = true;
        }
      }
      Serial.print("Programming Range # ");
      Serial.println(progNum);
      if (!progNum) {
        Serial.println("Error! Please Try Again");
        break;
      }
      byte index = progNum - 1;
      if (!active[index]) {
        Serial.println("Error! Inactive range cannot be programmed");

        break;
      }
      bool listenSuccess = false;
      listenSuccess = listenIR(index);
      digitalWrite(LED_BUILTIN, HIGH);
      pressNum = 0;
      elapsed = 0;
      ledState = digitalRead(LED_BUILTIN);
    }
  }
  return;
}

bool listenIR(byte rangeProgNum) {
  unsigned long start = millis();
  unsigned long elapsed = 0;
  while (!irrecv.decode(&results) && elapsed < 10000) {
    irrecv.resume();
    delay(300);
    if (irrecv.decode(&results)) {
      Serial.print("Signal detected!");
      Serial.println(results.value);
      CurrentValue = (results.value);

      // if the record mode is activated store the current value as the programed value
      Serial.print("Saving to");
      Serial.println(rangeProgNum);
      IRcode[rangeProgNum] = CurrentValue; // temporary
      Serial.println(IRcode[rangeProgNum]);  //displays stored code for reference
      delay(1000);
      irrecv.resume();
      return true;
    }
    elapsed = millis() - start;
//    Serial.println(elapsed);
  }
  Serial.println("No IR input detected");
  return false;
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
  irrecv.enableIRIn();

// due to the reverse logic of the Wemos LED, this turns it off
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);

  for (byte i = 0; i < rangeNum; i++) {
    if (range_check(i)){
      active[i] = true;
    }
    else active[i] = false;
  }

  // to shut all motors
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);
  //digitalWrite(M3, LOW);

  Serial.println("Setup completed");
  byte temp = 0;
  for (byte i = 0; i < rangeNum; i++) {
    if (active[i]){
      Serial.print("Range #");
      Serial.print(i);
      Serial.println(": ONLINE");
      temp++;
    }
    else {
      Serial.print("Range #");
      Serial.print(i);
      Serial.println(": OFFLINE");
    }
  }
  blinkX(temp);
}



void loop() {

// read the state of programming button
if (checkProgButton()){
  progState = true;
}
else progState = false;

// read the state of override button
if (checkOverrideButton()){
  unsigned long start = millis();
  unsigned long elapsed = 0;
  while (elapsed < 1000) {
    elapsed = millis() - start;
    delay(10);
//    Serial.print("Time Elapsed: ");
//    Serial.println(elapsed);
    if (!checkOverrideButton() && (!prevOverrideState)) {
      // if button was previously pressed, and just released
      pressNum++;
      prevOverrideState = true;
    }

  }
//  Serial.print("Press #");
//  Serial.println(pressNum);
  if (pressNum == 0) {
    Serial.println("You are now in programming mode");
    programmingMode();
    return;
  }
  else {
    range_reset(pressNum-1);
  }
  pressNum = 0;
}
else {
  ;
}

// if a signal is detected, store the value
if (irrecv.decode(&results)) {
  Serial.print("Signal detected!");
  Serial.println(results.value);
  CurrentValue = (results.value);

   // if the recieved value equals the programed value, then toggle the selected range
  for (byte i = 0; i < rangeNum ; i++) {
    if (active[i] == false) {
      continue;
    }
    Serial.print("Range #");
    Serial.print(i);
    Serial.print(":");
    if (IRcode[i] == CurrentValue) {
      Serial.print(IRcode[i]);
      Serial.println("(MATCHED)");
      range_reset(i);
      break;
    }
    Serial.println(IRcode[i]);
  }
  irrecv.resume();
  delay(300);
}
delay(50);
}




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
