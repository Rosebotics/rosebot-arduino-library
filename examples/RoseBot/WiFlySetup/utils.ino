/*
 * Connects to the WiFly and enter the command mode.
 */
int enterCommandModeWithWiFly() {
  char response[] = "XXX";
  unsigned long millisInitialValue = 0;
  
  Serial.println(F("Select the WiFly baudrate:"));
  Serial.println(F("  1 - 9600 (the default when you plug in a new WiFly module)"));
  Serial.println(F("  2 - 57600 (the standard Firmata baudrate)"));
  Serial.println(F("  3 - 112500 (the baudrate used by the RoseBotFirmata sketch)"));
  Serial.print(F("Use the Send box above to send 1, 2, or 3: "));

  while (!Serial.available()); // Wait for the Serial Monitor response.

  switch (Serial.read()) {
    case '1':
      Serial.println(F("1"));
      WiflySerial.begin(9600);
      break;
    case '2':
      Serial.println(F("2"));
      WiflySerial.begin(57600);
      break;
    case '3':
      Serial.println(F("3"));
      WiflySerial.begin(115200);
      break;
    default:
      Serial.println(F("That is not a valid choice. :)"));
      return 1; // Return an error.
  }

  // Attempt to enter with a nice normal state, flushing buffers, putting things to a normal starting point.
  WiflySerial.write("exit\r"); // Puts the WiFly back into normal mode if you are accidentally already in command mode.
  Serial.println(F("Clearing any pending WiFly messages..."));
  millisInitialValue = millis();
  while (millis() - millisInitialValue < 1000) {
    if (WiflySerial.available()) {
      WiflySerial.read();  // Trash an WiFly messages if any are sent during this time.
    }
    if (Serial.available()) {
      Serial.read();  // Trash an Serial Monitor messages if any are sent during this time.
    }
  }

  // Enter into command mode
  // You should see a slow (0.5 Hz) flashing green LED on the WiFly.
  Serial.println(F("Entering WiFly command mode by sending $$$ to the WiFly..."));
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
    Serial.println(F("\nSomething went wrong. :("));
    Serial.println(F("Turn off your RoseBot, turn it back on, then try again."));
    Serial.println(F("Or try pulling the WiFly out and putting it back in, then close and reopen the Serial Monitor"));
    Serial.println(F("Also check the WiFly Baudrate too!"));
    return 1;  // Return an error.
  }

  // Hide any extra characters sent from WiFly.
  millisInitialValue = millis();
  while (millis() - millisInitialValue < 1000) {
    if (WiflySerial.available()) {
      WiflySerial.read();  // Trash an WiFly messages if any are sent during this time.
    }
    if (Serial.available()) {
      Serial.read();  // Trash an Serial Monitor messages if any are sent during this time.
    }
  }
  Serial.println(F("You are now in command mode with the WiFly!"));
  return 0;  // Return success!
}


/*
 * Displays the main menu and parses the user response.
 * Returns a char for the choice made and populates the parameterString as necessary.
 */
char getMenuSelection(char* parameterString) {
  Serial.println(F("Select an option below:"));
  Serial.println(F("   0: Exit this shortcut menu and just let me manually type commands to send to the WiFly."));
  Serial.println(F("   1: scan (this option looks for WiFi networks that are in range)"));
  Serial.println(F("   2: set wlan ssid RHIT-OPEN"));
  Serial.println(F("   3: set wlan ssid <network name> (example 3 myNetworkName)"));
  Serial.println(F("   4: set wlan pass <password> (example 4 ifUneed1)"));
  Serial.println(F("   b: set uart baudrate <baudrate choice of either 1 for 9600, 2 for 57600, 3 for 115200>"));
  Serial.println(F("   s: show net (see if the WiFly is connected to the WiFi or not)"));
  Serial.println(F("   i: get ip a (get the ip address of the WiFly)"));
  Serial.println(F("   h: set opt deviceid <customhostname> (Rose students please don't change this)"));
  Serial.println(F("   a: set wlan join 1 (auto connect on startup. This is the default but good to set once)"));
  Serial.println(F("   f: factory RESET (just in case you are really having issues)"));
  Serial.println(F("   s: save"));
  Serial.println(F("   r: reboot"));
  Serial.println(F("   x: exit (this exits the command mode)"));
  Serial.print(F("Use the Send box above to send an option and optional parameter: "));

  while (!Serial.available()); // Wait for the Serial Monitor response.

  // TODO: Handle the parameter when it is sent!!!
  // TODO: Convert spaces to $ for the ssid parameter
  // TODO: Convert a 1, 2, 3 into 9600, 57600, 115200

  return Serial.read();
}


/*
 * This meant to get the WiFly response to a command.  Every command except for factory RESET and reboot should give a predicable reply.
 */
bool getWiFlyResponse(String expectedResponse) {
  // Hide any extra characters sent from WiFly.
  unsigned long millisInitialValue = millis();
  while (millis() - millisInitialValue < 2000) {
    if (WiflySerial.available()) {
      Serial.write(WiflySerial.read());  // Display an WiFly messages if any are sent during this time.
    }
    if (Serial.available()) {
      Serial.read();  // Trash an Serial Monitor messages if any are sent during this time.
    }
  }

  // TODO: Parse the response to look for the expectedResponse.
  return true;
  
}

