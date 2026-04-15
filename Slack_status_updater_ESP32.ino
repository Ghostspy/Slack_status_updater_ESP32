/*******************************************************************
    Sets a custom status on your slack account. It will toggle between
    two every 30 seconds

    You will need a bearer token, see readme for more details

    Parts:
    ESP32 D1 Mini style Dev board* - http://s.click.aliexpress.com/e/C6ds4my

 *  * = Affiliate

    If you find what I do useful and would like to support me,
    please consider becoming a sponsor on Github
    https://github.com/sponsors/witnessmenow/


    Written by Brian Lough
    YouTube: https://www.youtube.com/brianlough
    Tindie: https://www.tindie.com/stores/brianlough/
    Twitter: https://twitter.com/witnessmenow
 *******************************************************************/

// ----------------------------
// Standard Libraries
// ----------------------------

#include <WiFi.h>
#include <WiFiClientSecure.h>

// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------

#include <ArduinoSlack.h>
// Library for connecting to the Slack API

// Install from Github
// https://github.com/witnessmenow/arduino-slack-api

#include <ArduinoJson.h>
// Library used for parsing Json from the API responses

// Search for "Arduino Json" in the Arduino Library manager
// https://github.com/bblanchon/ArduinoJson

//------- Replace the following! ------

char ssid[] = "gh-iot";         // your network SSID (name)
char password[] = "littleorangemen"; // your network password

#define SLACK_ACCESS_TOKEN "xoxp-441976140354-463372417300-4044683452359-caf019c6188a0fbb3afd4a420edd0a86"

//------- ---------------------- ------

// including a "slack_server_cert" variable
// header is included as part of the ArduinoSlack libary
#include <ArduinoSlackCert.h>

WiFiClientSecure client;
ArduinoSlack slack(client, SLACK_ACCESS_TOKEN);

unsigned long delayBetweenRequests = 15000; // Time between requests
unsigned long requestDueTime;               // time when request due

// these constants won't change:
//const int  switchPin[] = {13, 12, 14, 4, 5, 16};    // the pins that the switch is attached to
const int  switchPin[] = {13, 25, 33, 18, 17, 16};    // the pins that the switch is attached to
int pinCount = 6;           // the number of pins (i.e. the length of the array)

// Variables will change:
int switchState[] = {0,0,0,0,0,0};         // current state of the switch
int lastSwitchState[] = {0,0,0,0,0,0};     // previous state of the switch

bool changeStatus = false;
int whichStatus = 0;

void setup() {
  // initialize the switch pins as inputs:
  for (int i = 0; i < pinCount; i++) {
    pinMode(switchPin[i], INPUT_PULLUP);
  }

    Serial.begin(115200);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    client.setCACert(slack_server_cert);

    // If you want to enable some extra debugging
    // uncomment the "#define SLACK_ENABLE_DEBUG" in ArduinoSlack.h
}

void displayProfile(SlackProfile profile)
{
    if (!profile.error)
    {
        Serial.println("--------- Profile ---------");

        Serial.print("Display Name: ");
        Serial.println(profile.displayName);

        Serial.print("Status Text: ");
        Serial.println(profile.statusText);

        Serial.print("Status Emoji: ");
        Serial.println(profile.statusEmoji);

        Serial.print("Status Expiration: ");
        Serial.println(profile.statusExpiration);

        Serial.println("------------------------");
    }
    else
    {
        Serial.println("error getting profile");
    }
}

void loop() {

for (int i = 0; i < pinCount; i++) {
    // read the pushbutton input pin:
  switchState[i] = digitalRead(switchPin[i]);
}
for (int j = 0; j < pinCount; j++) {  
  // compare the switchState to its previous state
  if (switchState[j] != lastSwitchState[j]) { // if the state has changed
    if (switchState[j] == HIGH) {
      // if the current state is HIGH then the switch is not connected
      //Serial.print(switchPin[j]);
      //Serial.println(" is not connected");
    } else {
      // if the current state is LOW then the switch is connected to the common ground pin
      Serial.print(switchPin[j]);
      Serial.println(" is connected");
      whichStatus = j;
      changeStatus = true;
      requestDueTime = millis() + 2000; // add time to move the rotary switch to another position before updating status
    }
    // Delay a little bit to avoid bouncing
    delay(20);
  }
  // save the current state as the last state, for next time through the loop
  lastSwitchState[j] = switchState[j];

}
if (millis() > requestDueTime && changeStatus)
        {
          SlackProfile profile;
switch (whichStatus) {
            case 0:
            slack.setPresence(SLACK_PRESENCE_AWAY);
             profile = slack.setCustomStatus("Walking the dog", ":dog2:");
             break;
            case 1:
              slack.setPresence(SLACK_PRESENCE_AWAY);
              profile = slack.setCustomStatus("Lunch", ":hamburger:");
              // There is an optional third parameter which takes a Unix timestamp for
              // when this custom status expires:
              // slack.setCustomStatus("I am the second status", ":v:", 1532627506);
              break;
            case 2:
              slack.setPresence(SLACK_PRESENCE_AUTO);
              profile = slack.setCustomStatus("In a meeting", ":calendar:");
              break;
            case 3:
              slack.setPresence(SLACK_PRESENCE_AUTO);
              profile = slack.setCustomStatus("Available to chat", ":speech_balloon:");
              break;
            case 4:
              slack.setPresence(SLACK_PRESENCE_AWAY);
              profile = slack.setCustomStatus("Lurking", ":crystal_ball:");
              break;
            case 5:
              slack.setPresence(SLACK_PRESENCE_AWAY);
              profile = slack.setCustomStatus("Offline", ":no_entry:");
              break;
          }
            displayProfile(profile);
            requestDueTime = millis() + delayBetweenRequests;
            changeStatus = false;
        }
}