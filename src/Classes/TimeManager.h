#include <TimeLib.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#include <secrets.h>

class TimeManager {
    public:
    enum class DayStatus {MidDay, Day, Horizon, Twilight, Night, DeepNight};

    private:
    time_t currentTime;
    time_t lastTimeUpdated = 0;
    int currentDay;
    time_t sunriseTime = 0;
    time_t sunsetTime = 0;

    time_t twilightBeginTime = 0; // time when morning twilight begins before sunrise
    time_t twilightEndTime = 0; // time when night twilight ends after sunset
    int gmtOffset;
    const String setCurrentTimeUrl = "http://api.timezonedb.com/v2/get-time-zone?key="
        + String(TIMEZONE_API_KEY)
        + "&format=json&by=position&lat="
        + String(LATITUDE) 
        + "&lng="
        + String(LONGITUDE);

    static time_t timeFromDaylightString(const char* daylightString);
    static int getTimeOfDay(time_t time);

    public:
    TimeManager();
    void updateForNewDay();
    void setCurrentTime();
    void fetchDaylightInfo();
    DayStatus getDayStatus();
};

