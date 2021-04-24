#include <LiquidCrystal.h>

#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <AsyncEventSource.h>
#include <WebResponseImpl.h>
#include <WebHandlerImpl.h>
#include <StringArray.h>
#include <AsyncWebSynchronization.h>
#include <AsyncWebSocket.h>
#include <AsyncJson.h>
#include <WebAuthentication.h>

#include "secrets.h"
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "telegramBot.h"
WiFiClientSecure telegramClient;
UniversalTelegramBot bot(BOTtoken, telegramClient);

void initTelegramBot(WiFiClientSecure &wifiClient){
  wifiClient.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  telegramClient = wifiClient;
  bot = UniversalTelegramBot(BOTtoken, wifiClient);
  // register commands for telegram bot
  const String commands = F("["
                            "{\"command\":\"on\",  \"description\":\"Turn light on\"},"
                            "{\"command\":\"off\", \"description\":\"Turn light off\"},"
                            "{\"command\":\"ipaddr\", \"description\":\"Get the IP address of the light\"},"
                            "{\"command\":\"random\",\"description\":\"Toggle random color mode\"}" // no comma on last command
                            "]");
  bot.setMyCommands(commands);
  bot.sendMessage(CHAT_ID,"just turned on", "");
  Serial.println("Just set commands");
}

void handleTelegramMessages(volatile bool &lightOn, volatile bool &randomModeOn){
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  for (int i = 0; i < numNewMessages; i++) {
    String text = bot.messages[0].text;
    Serial.println(bot.messages[0].text);
    if (text == "/on") {
      lightOn = true;
    } else if (text == "/off") {
      lightOn = false;
    } else if (text == "/random") {
      randomModeOn = !randomModeOn;
    } else if (text == "/ipaddr") {
      bot.sendMessage(CHAT_ID, WiFi.localIP().toString(), "");
    }
  }
}
