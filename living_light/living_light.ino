//#include <TelegramCertificate.h>
//#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h> 
#include <ArduinoOTA.h>
#include <Time.h>
#include "constants.h"
#include "secrets.h"
#include "breath.h"
#include "time_manager.h"
#include "telegramBot.h"

TaskHandle_t networkingTask;

WiFiClientSecure client;
//UniversalTelegramBot bot(BOTtoken, client);


void setup() {

  Serial.begin(115200);
  randomSeed(analogRead(0));

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
    Serial.println(WiFi.status());
  }
  initTelegramBot(client);
  
//  initTime();
  xTaskCreatePinnedToCore(
    networkingCode,   /* Task function. */
    "networking",     /* name of task. */
    20000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &networkingTask,      /* Task handle to keep track of created task */
    0);          /* pin task to core 1 */

  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  })
  .onEnd([]() {
    Serial.println("\nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  })
  .setPassword(OTA_PASS);
  ArduinoOTA.begin();
  initBreathe();

}
volatile bool lightOn = true;
volatile bool randomModeOn = false;
volatile bool isDay = true;



void loop() {
  double r = 0.0;
  double g = 0.0;
  double b = 0.0;
  if (isDay) {
    dayColor(r, g, b);
  } else {
    nightColor(r, g, b);
  }

  if (randomModeOn) {
    randomColor(r, g, b);
  }

  if (lightOn) {
    breathe(r, g, b);
  } else {
    turnOff();
  }
}

void dayColor(double &red, double &green, double &blue) {
  double brightness = 0.2;
  double rand1 = random(40000000, 300000000) / 1000000000.0;
  double rand2 = random(15000000, 200000000) / 1000000000.0;
  double rand3 = random(10000000, 200000000) / 1000000000.0;
  normalizeColor(brightness, rand1, rand2, rand3);
  double high = max(rand1, max(rand2, rand3));
  double low = min(rand1, min(rand2, rand3));
  double med = max(min(rand1, rand2), min(rand2, rand3));
  red = low;
  green = med;
  blue = high;

}

void nightColor(double &red, double &green, double &blue) {
  double brightness = 0.18  ;
  double rand1 = random(160000000, 600000000) / 2000000000.0;
  double rand2 = random(5000000, 10000000) / 2000000000.0;
  double rand3 = random(0, 200000) / 2000000000.0;
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
  double brightness = 0.25;
  randomNumbers[0] = random(200000000, 400000000) / 2000000000.0;
  randomNumbers[1] = random(200000, 400000000) / 2000000000.0;
  randomNumbers[2] = random(200000, 20000000) / 2000000000.0;
  normalizeColor(brightness, randomNumbers[0], randomNumbers[1], randomNumbers[2]);
  for (int i = 0; i < 3; i++) {
    int n = random(0, 3);  
    double temp = randomNumbers[n];
    randomNumbers[n] =  randomNumbers[i];
    randomNumbers[i] = temp;
  }

  red = randomNumbers[0];
  green = randomNumbers[1];
  blue = randomNumbers[2];

}



void networkingCode( void * pvParameters ) {
//  Serial.print("Task2 running on core ");
//  Serial.println(xPortGetCoreID());

  for (;;) {

    isDay = now() > sunriseTime && now() < sunsetTime;

    if (currentDay != day(now()) || sunriseTime == 0) {
      setCurrentTime();
      fetchDaylightInfo();
    }
    if (day(sunsetTime) != day(now())) {
      fetchDaylightInfo();
    }
    delay(1000 * 3);
    Serial.println("before handle telegram");
    handleTelegramMessages(lightOn, randomModeOn);
    ArduinoOTA.handle();

  }
}
