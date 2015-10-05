/*
The purpose of this sketch is to help you connect your WiFly module to your WiFi network from your Sparkfun Redbot board.
You will use the RoseBotFirmata for all Python/Scratch programs and you will use this sketch ONLY to get your WiFly setup.
This sketch is nice because you don't need any other hardware beyong a WiFly and a RedBot (RoseBot).

In order to use this sketch you need to:
  - Plug a WiFly module into your RedBot (RoseBot) and flip the switch at the top center of the board to "XBEE SW SERIAL" (this is best done with power off)
  - Power up the board with battery or USB (or both) and connect a mini USB cable to your computer (you will be talking to the Arduino, the Arduino will talk to the WiFly)
  - Upload this sketch to your Arduino
  - Open the Serial Monitor (icon in top right that looks like a magnifying glass)
  - Set the Serial monitor to "Carriage Return" and "115200 baud" (keep Autoscroll checked)
  - Follow the promps to setup your WiFly (make sure to always finish with save then reboot, s <Send> then r <Send>)
    - Depending on your setup, you should probably write down the ip address before you end the program
  - After your WiFly is setup, upload the Examples -> RoseBot -> RoseBotFirmata sketch onto your board again
  - Power down, flip the switch back to "XBEE HW SERIAL", and unplug the USB.
  - Then you should be able to Python and Scratch programs to drive the RoseBot again

There are many things you can do with this sketch but here are the common recipes that I predict you need.

#1.  When you buy a new WiFly module and plug it in for the very first time the baudrate is 9600.  You need to fix that.
For this one time you will need to connect to WiFly using the 9600 baudrate and send some commands.  This sketch will
automatically put the WiFly into command mode instead of the normal mode.  Once in command mode here is the receipe.
Raw commands to send to WiFly:
  set uart baudrate 115200
  save
  reboot
Using the menu shortcuts:
  b 3 (the value 3 represents the third choice which is 115200)
  s
  r
You will only need to do this once ever.  After your WiFly reboots it will be at a different baudrate.  Also after a reboot
your WiFly will no longer be in command mode, so after an r command it's best to close the Serial Monitor unless you know
what you are doing in manual mode.


#2.  When you want to get the WiFly onto the network.
Connect at whatever baudrate your WiFly is using from the receipe above (probably 115200 which is choice 3).
Let the sketch put you into command mode.
Raw commands to send to WiFly:
  set wlan join 1
  scan
  set wlan ssid YOUR$WIFI$NAME$NO$SPACES
  set wlan pass ifUneed1
  save
  reboot
Using the menu shortcuts:
  a
  1
  3 YOUR WIFI NAME NO SPACES (this sketch will convert spaces into $ for you)
  4 ifUneed1
  s
  r
The reboot should show you the IP Address of your WiFly on the network.


#3. Getting the IP Address of your WiFly if you forgot it.
Raw commands to send to WiFly:
  get ip a
  exit
Using the menu shortcuts:
  i
  x
Note that when you finish a receipe with a It happens. :)  Write it down.  BTW you can try to avoid IP Addresses by setting the WiFly hostname, for example
  h r05
To name your WiFly, then maybe your WiFi network will use the hostname to give you a NAMED address.
If you try that trick you can figure out if it worked by opening a command window (Terminal) and typing nslookup 127.0.0.1 (put in your WiFly ip address, 127.0.0.1 is just an example)

  Good luck!
  David Fisher 10-2-2015
 */

#include <SoftwareSerial.h>

// These are shortcuts that you can use if you find yourself typing the same thing a lot into the box (if you switch networks a lot)
#define DEFAULT_WIFLY_BAUDRATE_CHOICE '3' // coorisponds to choice 3 which is 115200 (default RoseBot speed).  For the initial question just hit Send instead of 3 <Send> if you want this choice.
// Note the DEFAULT_WIFLY_BAUDRATE_CHOICE can also be used with the b shortcut key to set the WiFly to a 1152000 baud rate.
#define HARDCODED_NETWORK_NAME "RHIT-OPEN"  // This is used for option 2 in the shortcut menu.  Rose students need RHIT-OPEN, but set it differently if needed.
#define DEFAULT_OTHER_NETWORK_NAME "Casa$de$Fisher"  // This is used if you don't pass a parameter to option 3 (set as needed to reduce typing if you like).
#define DEFAULT_NETWORK_PASSWORD "Rose-Hulman" // This is used if you don't pass a parameter to option 4 (set as needed to reduce typing if you like).


SoftwareSerial WiflySerial(15, 14); // RX, TX

void setup() {
  int potentialErrorEnterringCommandMode = 0;
  Serial.begin(115200);  // Baudrate used in the Serial Monitor window (can be anything)

  potentialErrorEnterringCommandMode = enterCommandModeWithWiFly();
  if (potentialErrorEnterringCommandMode) {
    Serial.println(F("\n\nAbort!"));
    while(1);  // I'm choosing to abort this run by stopping any further execution.  Nothing more will happen.
  }
}

void loop() {
  // Select an option below:
  //   0: Exit this shortcut menu and just let me manually type commands to send to the WiFly.
  //   1: scan (this option looks for WiFi networks that are in range)
  //   2: set wlan ssid RHIT-OPEN
  //   3: set wlan ssid <network name> (example 3 myNetworkName)
  //   4: set wlan pass <password> (example 4 ifUneed1)
  //   b: set uart baudrate <baudrate choice of either 1 for 9600, 2 for 57600, 3 for 115200>
  //   n: show net (see if the WiFly is connected to the WiFi or not)
  //   i: get ip a (get the ip address of the WiFly)
  //   h: set opt deviceid <customhostname> (Rose students please don't change this)
  //   a: set wlan join 1 (auto connect on startup. This is the default but good to set once)
  //   f: factory RESET (just in case you are really having issues)
  //   s: save
  //   r: reboot
  //   x: exit (this exits the command mode)
  boolean wiflyResponseWasSuccessful = false;
  char stringParameter[64];
  unsigned long millisInitialValue;
  String expectedResponse = "";
  char selection = getMenuSelection(stringParameter);
  switch (selection) {
    case 0:
      runManualMode();  // never to return
      break;
    case '1':
      Serial.println(F("scan")); // Display for user
      Serial.println(F("Scanning..."));
      WiflySerial.print(F("scan\r")); // Send it to WiFly
      // Give the WiFly a few seconds to respond
      // From datasheet: "The default scan time is 200 ms/channel or about 3 seconds."
      millisInitialValue = millis();
      while (millis() - millisInitialValue < 3000) {
        if (WiflySerial.available()) {
            Serial.write(WiflySerial.read());  // Display any WiFly messages
        }
        if (Serial.available()) {
          Serial.read();  // Trash an Serial Monitor messages if any are sent during this time.
        }
      }
      break;
    case '2':
      Serial.print(F("set wlan ssid "));
      Serial.println(HARDCODED_NETWORK_NAME);
      WiflySerial.print(F("set wlan ssid "));
      WiflySerial.print(HARDCODED_NETWORK_NAME);
      WiflySerial.print(F("\r"));
      break;
    case '3':
      Serial.print(F("set wlan ssid "));
      WiflySerial.print(F("set wlan ssid "));
      if (stringParameter[0] == '\0') {
        Serial.println(DEFAULT_OTHER_NETWORK_NAME);
        WiflySerial.print(DEFAULT_OTHER_NETWORK_NAME);
      } else {
        Serial.println(stringParameter);
        WiflySerial.print(stringParameter);
      }
      WiflySerial.print(F("\r"));
      break;
    case '4':
      Serial.print(F("set wlan pass "));
      WiflySerial.print(F("set wlan pass "));
      if (stringParameter[0] == '\0') {
        Serial.println(DEFAULT_NETWORK_PASSWORD);
        WiflySerial.print(DEFAULT_NETWORK_PASSWORD);
      } else {
        Serial.println(stringParameter);
        WiflySerial.print(stringParameter);
      }
      WiflySerial.print(F("\r"));
      break;
    case 'b':
      Serial.print(F("set uart baudrate "));
      WiflySerial.print(F("set uart baudrate "));
      if (stringParameter[0] == '\0') {
        stringParameter[0] == DEFAULT_WIFLY_BAUDRATE_CHOICE;
      }
      switch (stringParameter[0]) {
        case '1':
          Serial.println(F("9600"));
          WiflySerial.print(F("9600\r"));
          break;
        case '2':
          Serial.println(F("57600"));
          WiflySerial.print(F("57600\r"));
          break;
        case '3':
          Serial.println(F("115200"));
          WiflySerial.print(F("115200\r"));
          break;
      }
      break;
    case 'n':
      Serial.println(F("show net"));
      WiflySerial.print(F("show net\r"));
      break;
    case 'i':
      Serial.println(F("get ip a"));
      WiflySerial.print(F("get ip a\r"));
      break;
    case 'h':
      Serial.print(F("set opt deviceid "));
      Serial.println(stringParameter);
      WiflySerial.print(F("set opt deviceid "));
      WiflySerial.print(stringParameter);
      WiflySerial.print(F("\r"));
      break;
    case 'a':
      Serial.println(F("set wlan join 1"));
      WiflySerial.print(F("set wlan join 1\r"));
      break;
    case 'f':
      Serial.println(F("factory RESET"));
      WiflySerial.print(F("factory RESET\r"));
      runManualMode();  // never to return
      break;
    case 's':
      Serial.println(F("save"));
      WiflySerial.print(F("save\r"));
      break;
    case 'r':
      Serial.println(F("reboot"));
      WiflySerial.print(F("reboot\r"));
      runManualMode();  // never to return
      break;
    case 'x':
      Serial.println(F("exit"));
      WiflySerial.print(F("exit\r"));
      runManualMode();  // never to return
      break;
  }
  // Give the WiFly an opportunity to respond
  wiflyResponseWasSuccessful = getWiFlyResponse(expectedResponse);
  if (!wiflyResponseWasSuccessful) {
    // TODO: Implement this feature.
  }
  delay(100); // not required I just like having small delays at the end of the loop function if possible. :)
}

void runManualMode() {
  Serial.println(F("You are now in manual mode.  Things you type will be sent to WiFly.  Things from WiFly display to you."));
  Serial.println(F("If you ever want to go back to the menu mode, just close and then reopen the Serial Monitor."));
  while(1) {
    if (WiflySerial.available()) {
      Serial.write(WiflySerial.read());
    }
    if (Serial.available()) {
      WiflySerial.write(Serial.read());
    }
  }
}
