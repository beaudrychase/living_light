#include <FastLED.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <UniversalTelegramBot.h>
#include "constants.h"
#include "secrets.h"
#include "breath.h"


WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

void setup() {
  
  Serial.begin(115200);
  randomSeed(analogRead(0));
  initBreathe();
//  configTime(0, 0, "pool.ntp.org");
  WiFiClientSecure client;
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
}
bool red_bool = true;

void handleNewMessage(){
  String text = bot.messages[0].text;
  if (text == "/on"){
    red_bool = true;
  } else if (text == "/off"){
    red_bool = false;
  }
}

void loop() {
  Serial.println(xPortGetCoreID());
  double rand1 = random(4000, 30000) / 100000.0;
  double rand2 = random(1500, 20000) / 100000.0;
  double rand3 = random(500, 10000) / 100000.0;
  double high = max(rand1, max(rand2, rand3));
  double low = min(rand1, min(rand2, rand3));
  double med = max(min(rand1, rand2), min(rand2, rand3));
  double r = high;
  
  
  double g = med;
  double b = low;
  if (!red_bool){
    r = 0.0;
    g = 0.0;
    b = 0.0;
  }
  breathe(r, g, b);
  if (bot.getUpdates(bot.last_message_received + 1)){
    handleNewMessage();
  }
//  bot.sendMessage(CHAT_ID, "This is the living light", "");

}
