/*
 This sketch is used to set the WiFly Board in preparation for the RedBot to communicate wirelessly to your computer. 

Once this sketch is uploaded to your board then follow the setup procedure detailed at:
https://learn.sparkfun.com/tutorials/wifly-shield-hookup-guide#connecting-to-a-network

 
 Receives from the hardware serial, sends to software serial.
 Receives from software serial, sends to hardware serial.

 The circuit:
 * RX is digital pin 15 (connect to TX of other device)
 * TX is digital pin 14 (connect to RX of other device)


 created back in the mists of time
 modified 25 May 2012
 by Tom Igoe
 based on Mikal Hart's example

 This example code is in the public domain.

 */

#include <SoftwareSerial.h>

SoftwareSerial WiflySerial(15, 14); // RX, TX pins of the RedBot board

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600); //Initially your Wifly board will be set at a baudrate of 9600

  // set the data rate for the SoftwareSerial port
  WiflySerial.begin(9600);//Initially your Wifly board will be set at a baudrate of 9600
}

void loop() // run over and over
{
  if (WiflySerial.available())
    Serial.write(WiflySerial.read());
  if (Serial.available())
    WiflySerial.write(Serial.read());
}

