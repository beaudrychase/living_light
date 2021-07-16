#include <TimeLib.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#include <secrets.h>

class TimeManager {
    public:
    enum class DayStatus {MidDay, Day, Horizon, CivilTwilight, NauticalTwilight, AstonomicalTwilight, Night};

    private:
    time_t _currentTime;
    time_t _lastTimeUpdated = 0;
    int _currentDay;
    time_t _sunrise = 0;
    time_t _sunset = 0;

    time_t _midDayBegin = 0;
    time_t _midDayEnd = 0;

    time_t _horizonBegin = 0;
    time_t _horizonEnd = 0;

    time_t _civilTwilightBegin = 0; // time when morning twilight begins before sunrise
    time_t _civilTwilightEnd = 0; // time when night twilight ends after sunset

    time_t _nauticalTwilightBegin = 0;
    time_t _nauticalTwilightEnd = 0;

    time_t _astronomicalTwilightBegin = 0;
    time_t _astronomicalTwilightEnd = 0;
    int gmtOffset;
    const String setCurrentTimeUrl = "http://api.timezonedb.com/v2/get-time-zone?key="
        + String(TIMEZONE_API_KEY)
        + "&format=json&by=position&lat="
        + String(LATITUDE) 
        + "&lng="
        + String(LONGITUDE);

    static time_t timeFromDaylightString(const char* daylightString);
    static time_t getTimeOfDay(time_t time);
    static void printTime(time_t time);
    void setDayTimes();
    public:
    TimeManager();
    void printTimes();
    void updateForNewDay();
    void setCurrentTime();
    void fetchDaylightInfo();
    DayStatus getDayStatus();
};

