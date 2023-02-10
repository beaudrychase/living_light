#ifndef MAIN
#define MAIN
#pragma once

#include <TimeLib.h>

#include <Classes/Breath.h>
#include <Classes/TimeManager.h>
#include <Classes/TelegramBot.h>
#include <Classes/OrderedColorGenerator.h>
#include <constants.h>
#include <secrets.h>

volatile bool lightOn = true;
volatile bool randomModeOn = false;
volatile int vol_breath_seconds = 0;
int breath_seconds = 0;

TaskHandle_t networkingTask;

Breath breath = Breath();
TimeManager timeManager = TimeManager();
OrderedColorGenerator midDayGenerator(
    OrderedColorGenerator::Order::BGR,
    0.04, 0.04,
    0.028, 0.03,
    0.025, 0.03);
OrderedColorGenerator dayGenerator(
    OrderedColorGenerator::Order::BGR,
    0.04, 0.04,
    0.035, 0.04,
    0.035, 0.04);
OrderedColorGenerator horizonGenerator(
    OrderedColorGenerator::Order::RBG,
    0.04, 0.04,
    0.003, 0.0065,
    0.006, 0.01);
OrderedColorGenerator twilightGenerator(
    OrderedColorGenerator::Order::RGB,
    0.04, 0.04,
    0.004, 0.010,
    0.0001, 0.001);
OrderedColorGenerator nightGenerator(
    OrderedColorGenerator::Order::RGB,
    0.1, 0.1,
    0.000001, 0.0000005,
    0.000000, 0.0000000015);

SmoothColor dayColor();
SmoothColor twilightColor();
SmoothColor nightColor();
SmoothColor randomColor();

int getTimeOfDay(time_t time);
void networkingCode(void *pvParameters);

#endif