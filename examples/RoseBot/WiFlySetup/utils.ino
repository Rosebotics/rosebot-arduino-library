/*
 * Connects to the WiFly and puts it into command mode.
 */
int enterCommandModeWithWiFly() {
  char selection;
  char response[] = "XXX";
  unsigned long millisInitialValue = 0;

  Serial.println(F("Select the WiFly baudrate:"));
  Serial.println(F("  1 - 9600 (the default when you plug in a new WiFly module)"));
  Serial.println(F("  2 - 57600 (the standard Firmata baudrate)"));
  Serial.println(F("  3 - 112500 (the baudrate used by the RoseBotFirmata sketch)"));
  Serial.print(F("Use the Send box above to send 1, 2, or 3: "));

  while (!Serial.available()); // Wait for the Serial Monitor response.
  selection = Serial.read();
  if (selection == '\r') {
    selection = DEFAULT_WIFLY_BAUDRATE_CHOICE;
  }
  switch (selection) {
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
    Serial.println(F("Check the WiFly Baudrate that you used.  Make sure the WiFly switch is using XBEE SW Serial.  Then try again by either:"));
    Serial.println(F("  Closing and reopening the Serial Monitor (resets just this program)"));
    Serial.println(F("  Turning off your RoseBot and turning it back on (resets this program and the WiFly)"));

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
  Serial.println(F("You are now in command mode with the WiFly! The green light should be flashing quickly."));
  return 0;  // Return success!
}


/*
 * Displays the main menu and parses the user response.
 * Returns a char for the choice made and populates the parameterString as necessary.
 */
char getMenuSelection(char* parameterString) {
  char selection;
  char stringParameterIndex = 0;
  char stringParameterByte;
  Serial.println(F("\n----------------------------------------------------------------------"));
  Serial.println(F("Select an option below:"));
  Serial.println(F("   0: Exit this shortcut menu and just let me manually type commands to send to the WiFly."));
  Serial.println(F("   1: scan (this option looks for WiFi networks that are in range)"));
  Serial.print(F("   2: set wlan ssid "));
  Serial.println(HARDCODED_NETWORK_NAME);
  Serial.println(F("   3: set wlan ssid <network name> (example 3 myNetworkName)"));
  Serial.println(F("   4: set wlan pass <password> (example 4 ifUneed1)"));
  Serial.println(F("   b: set uart baudrate <baudrate choice of either 1 for 9600, 2 for 57600, 3 for 115200>"));
  Serial.println(F("   n: show net (see if the WiFly is connected to the WiFi or not)"));
  Serial.println(F("   i: get ip a (get the ip address of the WiFly)"));
  Serial.println(F("   h: set opt deviceid <customhostname> (Rose students please don't change this)"));
  Serial.println(F("   a: set wlan join 1 (auto connect on startup. This is the default but good to set once)"));
  Serial.println(F("   f: factory RESET (just in case you are really having issues)"));
  Serial.println(F("   s: save"));
  Serial.println(F("   r: reboot"));
  Serial.println(F("   x: exit (this exits the command mode)"));
  Serial.print(F("Use the Send box above to send an option and optional parameter: "));

  while (!Serial.available()); // Wait for the Serial Monitor response.

  selection = Serial.read();
  delay(10); // 10 ms to allow next message to arrive

  // Record the string parameter (if one was sent).
  while(Serial.available()) {
    stringParameterByte = (char)Serial.read();
    if (stringParameterByte == '\r') {
      break;
    } else if (stringParameterByte == ' ' && stringParameterIndex == 0) {
      continue; // There will be a parameter coming.
    } else {
      if (stringParameterByte == ' ') {
        stringParameterByte = '$'; // Replace spaces with $ instead
        // Commands are case sensitive, and you cannot use spaces in parameters.
        // Use a $ to indicate a space, e.g., MY NETWORK should be written as MY$NETWORK
      }
      parameterString[stringParameterIndex] = stringParameterByte;
      stringParameterIndex++;
    }
    delay(10); // 100 ms to allow next message to arrive
  }
  parameterString[stringParameterIndex] = '\0'; // null terminate the string
  return selection;
}


/*
 * Get the WiFly response to a command.
 */
bool checkForWiFlyConfirmation(char meunSelection) {
  char* expectedResponse; // Pointer to one of the arrays below
  char replyScan[] = "SCAN";
  char replyAok[] = "AOK";
  char replyStoringInConfig[] = "Storing in config";
  char replyExit[] = "EXIT";
  char expectedResponseIndex = 0;
  char expectedResponseLength = 0;
  boolean receivedExpectedResponse = false;
  String wiflyResponseString = "";
  wiflyResponseString.reserve(200); // reserve 200 bytes for the wiflyResponseString
  char responseChar;
  unsigned long millisInitialValue;

  // Prepare the expected response.  This can appear ANYWHERE in the response.
  switch (meunSelection) {
    case '1':
      expectedResponse = replyScan;
      expectedResponseLength = sizeof(replyScan) - 1;
      break;
    case '2':
    case '3':
    case '4':
    case 'b':
    case 'h':
    case 'a':
    case 'f':
      expectedResponse = replyAok;
      expectedResponseLength = sizeof(replyAok) - 1;
      break;
    case 'n':
    case 'i':
    case 'r':
      // Not applicable
      break;
    case 's':
      expectedResponse = replyStoringInConfig;
      expectedResponseLength = sizeof(replyStoringInConfig) - 1;
      break;
    case 'x':
      expectedResponse = replyExit;
      expectedResponseLength = sizeof(replyExit) - 1;
      break;
  }


  // Record WiFly response
  millisInitialValue = millis();
  while (millis() - millisInitialValue < 2000) {
    if (WiflySerial.available()) {
      responseChar = (char)WiflySerial.read();
      if (responseChar >= ' ' && responseChar <= 'z') {
        // Only store printable ascii values.
        wiflyResponseString += responseChar;
        if (responseChar == expectedResponse[expectedResponseIndex]) {
          expectedResponseIndex++;
          if (expectedResponseIndex >= expectedResponseLength) {
            receivedExpectedResponse = true;
          }
        } else {
          expectedResponseIndex = 0;
        }
      }
    }
    if (Serial.available()) {
      Serial.read();  // Trash an Serial Monitor messages if any are sent during this time.
    }
  }

  // TODO: Add a note about remembering to save this change.

  if (receivedExpectedResponse) {
    Serial.print(F("WiFly confirmed your command.  Received: "));
    Serial.println(expectedResponse);
  } else {
    Serial.print(F("WiFly did NOT confirmed your command.  Received: "));
    Serial.println(wiflyResponseString);
  }
  return receivedExpectedResponse;
}


/*
 * Displays any printable WiFly response characters for a period of time
 */
void displayPrintableWiFlyCharacters(unsigned long durationMs) {
  char responseChar;
  boolean currentlyHidingVersionChars = false;
  unsigned long millisInitialValue = millis();
  while (millis() - millisInitialValue < durationMs) {
    if (WiflySerial.available()) {
      char responseChar = (char)WiflySerial.read();
      if ((responseChar >= ' ' && responseChar <= 'z') || responseChar == '\n' || responseChar == '\r') {
        if (!currentlyHidingVersionChars && responseChar == '<') {
          currentlyHidingVersionChars = true;
        } else if (currentlyHidingVersionChars && responseChar == '>') {
          currentlyHidingVersionChars = true;
          millisInitialValue = millis() - durationMs + 100; // Short circuit the end to allow only 1 more char.
        } else if (!currentlyHidingVersionChars) {
         Serial.print(responseChar);  // Display WiFly messages
        }
      }
      if (Serial.available()) {
        Serial.read();  // Trash an Serial Monitor messages if any are sent during this time.
      }
    }
  }
}
