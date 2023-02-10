#ifndef TELEGRAMBOT
#define TELEGRAMBOT
#pragma once

#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <AsyncEventSource.h>
#include <WebResponseImpl.h>
#include <WebHandlerImpl.h>
#include <WiFiClientSecure.h>
#include <StringArray.h>
#include <AsyncWebSynchronization.h>
#include <AsyncWebSocket.h>
#include <AsyncJson.h>
#include <WebAuthentication.h>
#include <UniversalTelegramBot.h>

#include <secrets.h>

extern void initTelegramBot();
extern void handleTelegramMessages(volatile bool &lightOn, volatile bool &randomModeOn, volatile int &breath_length);

#endif