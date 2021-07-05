#include <main.h>


TaskHandle_t networkingTask;

WiFiClientSecure client;
Breath breath = Breath();
TimeManager timeManager = TimeManager();
OrderedColorGenerator dayGenerator(
    OrderedColorGenerator::Order::BGR,
    0.04, 0.04,
    0.028, 0.03,
    0.025, 0.03);
OrderedColorGenerator twilightGenerator(
    OrderedColorGenerator::Order::RGB,
    0.04, 0.04,
    0.006, 0.009,
    0.002, 0.003);
OrderedColorGenerator nightGenerator(
    OrderedColorGenerator::Order::RGB,
    0.1, 0.1,
    0.001, 0.0005,
    0.000, 0.0000015);
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
  timeManager.setCurrentTime();
  timeManager.fetchDaylightInfo();
  initTelegramBot(client);
  xTaskCreatePinnedToCore(
    networkingCode,   /* Task function. */
    "networking",     /* name of task. */
    20000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &networkingTask,      /* Task handle to keep track of created task */
    0);          /* pin task to core 1 */

//   ArduinoOTA
//   .onStart([]() {
//     String type;
//     if (ArduinoOTA.getCommand() == U_FLASH)
//       type = "sketch";
//     else // U_SPIFFS
//       type = "filesystem";

//     // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
//     Serial.println("Start updating " + type);
//   })
//   .onEnd([]() {
//     Serial.println("\nEnd");
//   })
//   .onProgress([](unsigned int progress, unsigned int total) {
//     Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
//   })
//   .onError([](ota_error_t error) {
//     Serial.printf("Error[%u]: ", error);
//     if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
//     else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
//     else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
//     else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
//     else if (error == OTA_END_ERROR) Serial.println("End Failed");
//   })
//   .setPassword(OTA_PASS);
//   ArduinoOTA.begin();
  

}

void loop() {
    SmoothColor color;
    switch(timeManager.getDayStatus()){
        case TimeManager::DayStatus::Day:
            color = dayGenerator.generateColor(0.2);
            break;
        case TimeManager::DayStatus::Twilight:
            color = twilightGenerator.generateColor(0.18);
            break;
        case TimeManager::DayStatus::Night:
            color = nightGenerator.generateColor(0.15);
            break;
        default:
            color = nightGenerator.generateColor(0.15);
            break;
        
    }
    // color = dayGenerator.generateColor(0.2);
    if (randomModeOn) {
        color = randomColor();
    }
    // Serial.println("breath");
    if (lightOn) {
        breath.breathe(color);
    } else {
        breath.turnOff();
    }
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

void networkingCode( void * pvParameters ) {
//  Serial.print("Task2 running on core ");
//  Serial.println(xPortGetCoreID());

  for (;;) {
    timeManager.updateForNewDay();
    delay(1000 * 3);
    
    handleTelegramMessages(lightOn, randomModeOn, vol_breath_seconds);
    // ArduinoOTA.handle();

  }
}
