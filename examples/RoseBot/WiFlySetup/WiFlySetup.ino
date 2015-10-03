/*
  Software serial multple serial test

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

SoftwareSerial WiflySerial(15, 14); // RX, TX

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // set the data rate for the SoftwareSerial port
  WiflySerial.begin(115200);
  WiflySerial.println("Hello, world?");
}

void loop() // run over and over
{
  if (WiflySerial.available())
    Serial.write(WiflySerial.read());
  if (Serial.available())
    WiflySerial.write(Serial.read());
}

