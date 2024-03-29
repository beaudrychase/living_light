#ifndef MAIN
#define MAIN
#pragma once

#include <TimeLib.h>

#include <Classes/Breath.h>
#include <Classes/TimeManager.h>
#include <Classes/OrderedColorGenerator.h>
#include <constants.h>
#include <secrets.h>

volatile bool lightOn = true;
volatile bool randomModeOn = false;
volatile int vol_breath_seconds = 0;
volatile TimeManager::DayStatus  dayStatus = TimeManager::DayStatus::Night;
int breath_seconds = 0;

TaskHandle_t networkingTask;
TimeManager *timeManager;

Breath breath = Breath();
OrderedColorGenerator midDayGenerator(
    OrderedColorGenerator::Order::BGR,
    0.04, 0.04,
    0.025, 0.035,
    0.020, 0.035);
OrderedColorGenerator dayGenerator(
    OrderedColorGenerator::Order::BGR,
    0.04, 0.05,
    0.030, 0.04,
    0.030, 0.04);
OrderedColorGenerator horizonGenerator(
    OrderedColorGenerator::Order::RBG,
    0.04, 0.04,
    0.002, 0.013,
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