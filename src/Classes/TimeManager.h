#ifndef TIMEMANAGER
#define TIMEMANAGER
#pragma once
#include <TimeLib.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include <secrets.h>

class TimeManager
{
public:
    enum class DayStatus
    {
        MidDay,
        Day,
        Horizon,
        CivilTwilight,
        NauticalTwilight,
        AstonomicalTwilight,
        Night
    };

private:
    time_t _currentTime;
    time_t _lastTimeUpdated = 0;

    time_t _nightEnd = 0;
    time_t _nauticalDawn = 0;
    time_t _dawn = 0; // time when morning twilight begins before sunrise
    time_t _sunrise = 0;
    time_t _sunriseEnd = 0;
    time_t _goldenHourEnd = 0;
    time_t _midDayBegin = 0;
    time_t _solarNoon = 0;
    time_t _midDayEnd = 0;
    time_t _goldenHour = 0;
    time_t _sunsetStart = 0;
    time_t _sunset = 0;
    time_t _dusk = 0;   // time when night twilight ends after sunset
    time_t _nauticalDusk = 0;
    time_t _night = 0;
    time_t _nadir = 0;

    int _currentDay;

    const String _getSolarTimingsUrl = "https://sun-api.beaudrychase.xyz/time?lat=" + String(LATITUDE) + "&long=" + String(LONGITUDE);
    const String _apiKeyName = "x-api-key";
    const char* _apiCertificate = \
        "-----BEGIN CERTIFICATE-----\n" \
        "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\n" \
        "-----END CERTIFICATE-----\n";

    static time_t
    timeFromDaylightString(const char *daylightString);
    static time_t getTimeOfDay(time_t time);
    static void printTime(time_t time);
    void setDayTimes();
    void setCurrentTime(const char* timeString);


public:
    TimeManager();
    void printTimes();
    void updateForNewDay();
    void fetchDaylightInfoAndTime();
    DayStatus getDayStatus();    
};

#endif
