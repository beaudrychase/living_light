
#include <TimeLib.h>
#include <WiFiClientSecure.h> 
#include <ArduinoOTA.h>

#include <Classes/Breath.h>
#include <Classes/TimeManager.h>
#include <constants.h>
#include <secrets.h>
#include "time_manager.h"
#include "telegramBot.h"

volatile bool lightOn = true;
volatile bool randomModeOn = false;
volatile int vol_breath_seconds = 0;
int breath_seconds = 0;

SmoothColor dayColor();
SmoothColor twilightColor();
SmoothColor nightColor();
SmoothColor randomColor();

int getTimeOfDay(time_t time);
void networkingCode( void * pvParameters );