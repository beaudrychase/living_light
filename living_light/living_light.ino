#include "freeRTOS_config.h"
#include <FastLED.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Time.h>
#include "constants.h"
#include "secrets.h"
#include "breath.h"
#include "time_manager.h"

TaskHandle_t networkingTask;

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

  xTaskCreatePinnedToCore(
    networkingCode,   /* Task function. */
    "networking",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &networkingTask,      /* Task handle to keep track of created task */
    0);          /* pin task to core 1 */
  setCurrentTime();
  fetchDaylightInfo();
}
volatile bool red_bool = true;
volatile bool isDay = true;

void handleNewMessage() {
  String text = bot.messages[0].text;
  if (text == "/on") {
    red_bool = true;
  } else if (text == "/off") {
    red_bool = false;
  }
}

void loop() {
  double r = 0.0;
  double g = 0.0;
  double b = 0.0;
  dayColor(r, g, b);
  if (isDay){
    dayColor(r, g, b);
  } else {
    nightColor(r, g, b);
  }
  if (!red_bool) {
    r = 0.0;
    g = 0.0;
    b = 0.0;
  }
  breathe(r, g, b);

}

void dayColor(double &red, double &green, double &blue) {
  double rand1 = random(4000, 30000) / 100000.0;
  double rand2 = random(1500, 20000) / 100000.0;
  double rand3 = random(500, 10000) / 100000.0;
  double high = max(rand1, max(rand2, rand3));
  double low = min(rand1, min(rand2, rand3));
  double med = max(min(rand1, rand2), min(rand2, rand3));
  red = low;
  green = med;
  blue = high;

}

void nightColor(double &red, double &green, double &blue) {
  double rand1 = random(4000, 30000) / 100000.0;
  double rand2 = random(500, 1000) / 100000.0;
  double rand3 = random(0, 500) / 100000.0;
  double high = max(rand1, max(rand2, rand3));
  double low = min(rand1, min(rand2, rand3));
  double med = max(min(rand1, rand2), min(rand2, rand3));
  red = high;
  green = med;
  blue = low;

}



void networkingCode( void * pvParameters ) {
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    if (currentDay != day(now()) || sunriseTime == 0) {
      setCurrentTime();

      fetchDaylightInfo();
    }
    if( now() > sunsetTime){
      isDay = false;
    } else if (now() > sunriseTime){
      isDay = true;
    }
    delay(1000);
    if (bot.getUpdates(bot.last_message_received + 1)) {
      handleNewMessage();
    }
    Serial.print("Now:  ");
    Serial.println(now());
    Serial.print("Sunset: ");
    Serial.println(sunsetTime);
  }
}
