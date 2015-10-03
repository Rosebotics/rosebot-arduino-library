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
char response[] = "XXX";
unsigned long startTimer = 0;

void setup() {
  int errorEnterringCommandMode = 0;
  Serial.begin(115200);  // Baudrate used in the Serial Monitor window (can be anything)
  errorEnterringCommandMode = enterCommandModeWithWiFly();
  if (errorEnterringCommandMode) {
    Serial.println("\n\nAbort");
    while(1);  // I'm choosing to abort this run by stopping any further execution.  Nothing more will happen.
  }
}

void loop() {
  // TODO: Make a menu to help guide a user through setting up their WiFly
  // Select an option below:
  //   0: Exit this shortcut menu and just let me manually type commands to send to the WiFly.
  //   1: scan
  //   2: set wlan ssid RHIT-OPEN
  //   3: set wlan ssid <enter network name in the next step>
  //   4: set wlan pass <enter password in next step>
  //   b: set uart baudrate <select baudrate in next step>
  //   a: set wlan join 1 (set mode to auto connect in the future, this is the default)
  //   s: save
  //   r: reboot
  if (WiflySerial.available()) {
    Serial.write(WiflySerial.read());
  }
  if (Serial.available()) {
    WiflySerial.write(Serial.read());
  }
}


int enterCommandModeWithWiFly() {
  Serial.println("Select the WiFly baudrate:");
  Serial.println("  1 - 9600 (the default when you plug in a new WiFly module)");
  Serial.println("  2 - 57600 (the standard Firmata baudrate)");
  Serial.println("  3 - 112500 (the baudrate used by the RoseBotFirmata sketch)");
  Serial.print("Use the Send box above to send 1, 2, or 3: ");

  while (!Serial.available()); // Wait for the Serial Monitor response.

  switch (Serial.read()) {
    case '1':
      Serial.println("1");
      WiflySerial.begin(9600);
      break;
    case '2':
      Serial.println("2");
      WiflySerial.begin(57600);
      break;
    case '3':
      Serial.println("3");
      WiflySerial.begin(115200);
      break;
    default:
      Serial.println("That is not a valid choice. :)");
      return 1; // Return an error.
  }

  // Attempt to enter with a nice normal state, flushing buffers, putting things to a normal starting point.
  WiflySerial.write("exit\r"); // Puts the WiFly back into normal mode if you are accidentally already in command mode.
  Serial.println("Clearing any pending WiFly messages...");
  startTimer = millis();
  while (millis() - startTimer < 1000) {
    if (WiflySerial.available()) {
      WiflySerial.read();  // Trash an WiFly messages if any are sent during this time.
    }
    if (Serial.available()) {
      Serial.read();  // Trash an Serial Monitor messages if any are sent during this time.
    }
  }

  // Enter into command mode
  // You should see a slow (0.5 Hz) flashing green LED on the WiFly.
  Serial.println("Entering WiFly command mode by sending $$$ to the WiFly...");
  delay(500);
  WiflySerial.write("$$$"); // Hopefully there is a CMD response
  delay(1000);
  if (WiflySerial.available()) {
    response[0] = WiflySerial.read();
    response[1] = WiflySerial.read();
    response[2] = WiflySerial.read();
  }
  response[3] = '\0'; // null terminator for the string to print

  Serial.println(response);
  if (response[0] != 'C' || response[1] != 'M' || response[2] != 'D') {
    Serial.println("\nSomething went wrong. :(");
    Serial.println("Turn off your RoseBot, turn it back on, then try again.");
    Serial.println("Or try pulling the WiFly out and putting it back in, then close and reopen the Serial Monitor");
    Serial.println("Also check the WiFly Baudrate too!");
    return 1;  // Return an error.
  }

  // Hide any extra characters sent from WiFly.
  startTimer = millis();
  while (millis() - startTimer < 1000) {
    if (WiflySerial.available()) {
      WiflySerial.read();  // Trash an WiFly messages if any are sent during this time.
    }
    if (Serial.available()) {
      Serial.read();  // Trash an Serial Monitor messages if any are sent during this time.
    }
  }
  Serial.println("You are now in command mode with the WiFly!");
  return 0;  // Return success!
}
