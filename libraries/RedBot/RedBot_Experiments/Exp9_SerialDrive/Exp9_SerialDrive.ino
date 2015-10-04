/***********************************************************************
 * Exp9_SerialDrive -- RedBot Experiment 9
 * 
 * The first step to controlling the RedBot remotely is to first drive it 
 * from the Serial Monitor in a tethered setup. 
 *
 * Hardware setup:
 * After uploading this sketch, keep the RedBot tethered to your computer with 
 * the USB cable. Open up the Seral Monitor to send commands to the RedBot to 
 * drive. 
 * 
 * This sketch was written by SparkFun Electronics, with lots of help from 
 * the Arduino community. This code is completely free for any use.
 * 
 * 15 Dec 2014 B. Huang 
 * 
 * This experiment was inspired by Paul Kassebaum at Mathworks, who made
 * one of the very first non-SparkFun demo projects and brought it to the
 * 2013 Open Hardware Summit in Boston. Thanks Paul!
 ***********************************************************************/

#include <RedBot.h>
RedBotMotors motors;
int motorPower;  // variable for setting the drive power
int data;  // variable for holding incoming data from PC to Arduino

void setup(void) {
  Serial.begin(57600);
    pinMode(13, OUTPUT);
    pinMode(12, INPUT_PULLUP);
}

void loop(void) {
  if(Serial.available() > 0) {
    data = Serial.parseInt();
    if (data == 1) {  
      digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
    } else if (data == 0) {
      digitalWrite(13, LOW);   // turn the LED on (HIGH is the voltage level)
    }
  }  

  if (digitalRead(12) == 0) {
    Serial.print("Hello");
    while (digitalRead(12) == 0) {
      // Do nothing until the release
    }
  }

  delay(100);
}
