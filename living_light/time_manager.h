extern void setCurrentTime();
extern void fetchDaylightInfo();
time_t timeFromDaylightString(const char* daylightString);
extern time_t lastTimeUpdated;
extern time_t sunriseTime;
extern time_t sunsetTime;
extern time_t twilightBeginTime;
extern time_t twilightEndTime;
extern int currentDay;
