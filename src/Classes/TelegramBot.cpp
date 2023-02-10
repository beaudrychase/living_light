#include "TelegramBot.h"

UniversalTelegramBot *bot;
WiFiClientSecure wifiClientSecure;

void initTelegramBot()
{
  wifiClientSecure.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  bot = new UniversalTelegramBot(BOTtoken, wifiClientSecure);
  // register commands for telegram bot
  const String commands = F("["
                            "{\"command\":\"on\",  \"description\":\"Turn light on\"},"
                            "{\"command\":\"off\", \"description\":\"Turn light off\"},"
                            "{\"command\":\"ipaddr\", \"description\":\"Get the IP address of the light\"},"
                            "{\"command\":\"random\",\"description\":\"Toggle random color mode\"},"
                            "{\"command\":\"length\",\"description\":\"set the length of breath cycle (seconds)\"}" // no comma on last command
                            "]");
  bot->setMyCommands(commands);
  bot->sendMessage(CHAT_ID, "just turned on", "");
  Serial.println("Just set commands");
}

void handleTelegramMessages(volatile bool &lightOn, volatile bool &randomModeOn, volatile int &breath_length)
{
  wifiClientSecure.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  int numNewMessages = bot->getUpdates(bot->last_message_received + 1);
  for (int i = 0; i < numNewMessages; i++)
  {

    Serial.println(bot->messages[i].text);
    if (bot->messages[i].text == "/on")
    {
      lightOn = true;
    }
    else if (bot->messages[i].text == "/off")
    {
      lightOn = false;
    }
    else if (bot->messages[i].text == "/random")
    {
      randomModeOn = !randomModeOn;
    }
    else if (bot->messages[i].text == "/ipaddr")
    {
      bot->sendMessage(CHAT_ID, WiFi.localIP().toString(), "");
    }
    else if (bot->messages[i].text.length() > 7 &&
             bot->messages[i].text.substring(0, 7) == "/length")
    {
      Serial.println("got length");
      char str_array[bot->messages[i].text.length() + 1];
      bot->messages[i].text.toCharArray(str_array, bot->messages[i].text.length() + 1);
      char *read_number = str_array;
      Serial.println(str_array);
      read_number = strtok(read_number, " ");
      Serial.println(read_number);
      read_number = strtok(NULL, " ");
      // checks that there is something after the space
      if (read_number != 0)
      {
        int number = atoi(read_number);
        if (number > 0 && number <= 17)
        {
          breath_length = number;
        }
      }
    }
  }
}
