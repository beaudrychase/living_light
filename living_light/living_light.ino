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
  if (isDay) {
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
  double brightness = 0.23;
  double rand1 = random(4000, 30000) / 100000.0;
  double rand2 = random(1500, 20000) / 100000.0;
  double rand3 = random(500, 10000) / 100000.0;
  normalizeColor(brightness, rand1, rand2, rand3);
  double high = max(rand1, max(rand2, rand3));
  double low = min(rand1, min(rand2, rand3));
  double med = max(min(rand1, rand2), min(rand2, rand3));
  red = low;
  green = med;
  blue = high;

}

void nightColor(double &red, double &green, double &blue) {
  double brightness = 0.15;
  double rand1 = random(8000, 30000) / 100000.0;
  double rand2 = random(500, 1000) / 100000.0;
  double rand3 = random(0, 10) / 100000.0;
  normalizeColor(brightness, rand1, rand2, rand3);
  double high = max(rand1, max(rand2, rand3));
  double low = min(rand1, min(rand2, rand3));
  double med = max(min(rand1, rand2), min(rand2, rand3));
  red = high;
  green = med;
  blue = low;

}

void randomColor(double &red, double &green, double &blue) {
  double randomNumbers[3];
  double brightness = 0.23;
  randomNumbers[0] = random(10000, 20000) / 100000.0;
  randomNumbers[1] = random(10, 20000) / 100000.0;
  randomNumbers[2] = random(10, 1000) / 100000.0;
  normalizeColor(brightness, randomNumbers[0], randomNumbers[1], randomNumbers[2]);
  for (int i = 0; i < 3; i++) {
    int n = random(0, 3);  // Integer from 0 to questionCount-1
    double temp = randomNumbers[n];
    randomNumbers[n] =  randomNumbers[i];
    randomNumbers[i] = temp;
  }

  red = randomNumbers[0];
  green = randomNumbers[1];
  blue = randomNumbers[2];

}



void networkingCode( void * pvParameters ) {
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    
      isDay = now() > sunriseTime && now() < sunsetTime;
    
    if (currentDay != day(now()) || sunriseTime == 0) {
      setCurrentTime();

      fetchDaylightInfo();
    }
    
    delay(1000);
    if (bot.getUpdates(bot.last_message_received + 1)) {
      handleNewMessage();
    }
//    Serial.print("Now:  ");
//    Serial.println(now());
//    Serial.print("Sunset: ");
//    Serial.println(sunsetTime);
  }
}
