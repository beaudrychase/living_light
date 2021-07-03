//#include <TelegramCertificate.h>
//#include <UniversalTelegramBot.h>
#include <main.h>


TaskHandle_t networkingTask;

WiFiClientSecure client;
Breath breath = Breath();
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
  
  initTime();
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
  

}

void loop() {
  SmoothColor color;
  if (isDay) {
    color = dayColor();
  } else if (isTwilight){
    color = twilightColor();
  } else {
    color = nightColor();
  }
//  color = twilightColor();

  if (randomModeOn) {
    color = randomColor();
  }

  if (lightOn) {
    breath.breathe(color);
  } else {
    breath.turnOff();
  }
}

SmoothColor dayColor() {
  double brightness = 0.2;
  double rand1 = 40000000.0 / 1000000000.0;
  double rand2 = random(28000000, 300000000) / 1000000000.0;
  double rand3 = random(25000000, 300000000) / 1000000000.0;
  double high = max(rand1, max(rand2, rand3));
  double low = min(rand1, min(rand2, rand3));
  double med = max(min(rand1, rand2), min(rand2, rand3));
  return SmoothColor(low, med, high, brightness);

}

SmoothColor twilightColor() {
  double brightness = 0.18;
  double rand1 = 40000000.0 / 1000000000.0;
  double rand2 = random(6000000, 9000000) / 1000000000.0;
  double rand3 = random(2000000, 3000000) / 1000000000.0;
  double high = max(rand1, max(rand2, rand3));
  double low = min(rand1, min(rand2, rand3));
  double med = max(min(rand1, rand2), min(rand2, rand3));
  return SmoothColor(high, med, low, brightness);
}

SmoothColor nightColor() {
  double brightness = 0.17  ;
  double rand1 = random(160000000, 600000000) / 2000000000.0;
  double rand2 = random(5000000, 10000000) / 2000000000.0;
  double rand3 = random(0, 30000) / 2000000000.0;
  double high = max(rand1, max(rand2, rand3));
  double low = min(rand1, min(rand2, rand3));
  double med = max(min(rand1, rand2), min(rand2, rand3));
  return SmoothColor(high, med, low, brightness);
}

SmoothColor randomColor() {
  double randomNumbers[3];
  double brightness = 0.25;
  randomNumbers[0] = random(200000000, 400000000) / 2000000000.0;
  randomNumbers[1] = random(200000, 400000000) / 2000000000.0;
  randomNumbers[2] = random(200000, 20000000) / 2000000000.0;
  for (int i = 0; i < 3; i++) {
    int n = random(0, 3);  
    double temp = randomNumbers[n];
    randomNumbers[n] =  randomNumbers[i];
    randomNumbers[i] = temp;
  }
  return SmoothColor(randomNumbers[0], randomNumbers[1], randomNumbers[2], brightness);

}

int getTimeOfDay(time_t time){
  return hour(time) * 100 + minute(time);
}

void networkingCode( void * pvParameters ) {
//  Serial.print("Task2 running on core ");
//  Serial.println(xPortGetCoreID());

  for (;;) {
    Serial.print("now TimeOfDay: ");
    Serial.println(getTimeOfDay(now()));
    Serial.print("sunrise TimeOfDay: ");
    Serial.println(getTimeOfDay(sunriseTime));
    Serial.print("sunset TimeOfDay: ");
    Serial.println(getTimeOfDay(sunsetTime));
    
    isDay = getTimeOfDay(now()) > getTimeOfDay(sunriseTime) && getTimeOfDay(now()) < getTimeOfDay(sunsetTime);
    // it is not day, it is before twilight end, it is after sunset
    bool nightTwilight = isDay == false && getTimeOfDay(now()) <= getTimeOfDay(twilightEndTime) && getTimeOfDay(now()) >= getTimeOfDay(sunsetTime);
    bool morningTwilight = isDay == false && getTimeOfDay(now()) >= getTimeOfDay(twilightBeginTime) && getTimeOfDay(now()) <= getTimeOfDay(sunriseTime);
    isTwilight = nightTwilight || morningTwilight;

    if (currentDay != day(now()) || sunriseTime == 0) {
      setCurrentTime();
      fetchDaylightInfo();
    }
    if (day(sunsetTime) != day(now())) {
      setCurrentTime();
      fetchDaylightInfo();
    }
    delay(1000 * 3);
    
    handleTelegramMessages(lightOn, randomModeOn, vol_breath_seconds);
    ArduinoOTA.handle();

  }
}
