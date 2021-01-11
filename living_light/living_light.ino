#include "freeRTOS_config.h"
#include <FastLED.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <UniversalTelegramBot.h>
#include "constants.h"
#include "secrets.h"
#include "breath.h"

TaskHandle_t breatheTask;
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
//  xTaskCreatePinnedToCore(
//    breatheCode,   /* Task function. */
//    "breathe",     /* name of task. */
//    20000,       /* Stack size of task */
//    NULL,        /* parameter of the task */
//    10,           /* priority of the task */
//    &breatheTask,      /* Task handle to keep track of created task */
//    1);          /* pin task to core 0 */

  xTaskCreatePinnedToCore(
    networkingCode,   /* Task function. */
    "networking",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &networkingTask,      /* Task handle to keep track of created task */
    0);          /* pin task to core 1 */
}
volatile bool red_bool = true;

void handleNewMessage() {
  String text = bot.messages[0].text;
  if (text == "/on") {
    red_bool = true;
  } else if (text == "/off") {
    red_bool = false;
  }
}

void loop() {
  double rand1 = random(4000, 30000) / 100000.0;
    double rand2 = random(1500, 20000) / 100000.0;
    double rand3 = random(500, 10000) / 100000.0;
    double high = max(rand1, max(rand2, rand3));
    double low = min(rand1, min(rand2, rand3));
    double med = max(min(rand1, rand2), min(rand2, rand3));
    double r = high;


    double g = med;
    double b = low;
    if (!red_bool) {
      r = 0.0;
      g = 0.0;
      b = 0.0;
    }
    breathe(r, g, b);

}


void breatheCode( void * pvParameters ) {
  for (;;) {
    double rand1 = random(4000, 30000) / 100000.0;
    double rand2 = random(1500, 20000) / 100000.0;
    double rand3 = random(500, 10000) / 100000.0;
    double high = max(rand1, max(rand2, rand3));
    double low = min(rand1, min(rand2, rand3));
    double med = max(min(rand1, rand2), min(rand2, rand3));
    double r = high;


    double g = med;
    double b = low;
    if (!red_bool) {
      r = 0.0;
      g = 0.0;
      b = 0.0;
    }
    breathe(r, g, b);
  }
}

void networkingCode( void * pvParameters ) {
  const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
//    vTaskdelay(xDelay);
    delay(1000);
    if (bot.getUpdates(bot.last_message_received + 1)) {
      handleNewMessage();
    }
  }
}
