#include <Classes/TimeManager.h>

WiFiClientSecure wifiClient;

TimeManager::DayStatus TimeManager::getDayStatus()
{
  time_t nowTime = getTimeOfDay(now());
  if (nowTime <= getTimeOfDay(_nightEnd))
  {
    return DayStatus::Night;
  }
  else if (nowTime <= getTimeOfDay(_nauticalDawn))
  {
    return DayStatus::AstonomicalTwilight;
  }
  else if (nowTime <= getTimeOfDay(_dawn))
  {
    return DayStatus::NauticalTwilight;
  }
  else if (nowTime <= getTimeOfDay(_sunrise))
  {
    return DayStatus::CivilTwilight;
  }
  else if (nowTime <= getTimeOfDay(_goldenHourEnd))
  {
    return DayStatus::Horizon;
  }
  else if (nowTime <= getTimeOfDay(_midDayBegin))
  {
    return DayStatus::Day;
  }
  else if (nowTime <= getTimeOfDay(_midDayEnd))
  {
    return DayStatus::MidDay;
  }
  else if (nowTime <= getTimeOfDay(_goldenHour))
  {
    return DayStatus::Day;
  }
  else if (nowTime <= getTimeOfDay(_sunset))
  {
    return DayStatus::Horizon;
  }
  else if (nowTime <= getTimeOfDay(_dusk))
  {
    return DayStatus::CivilTwilight;
  }
  else if (nowTime <= getTimeOfDay(_nauticalDusk))
  {
    return DayStatus::NauticalTwilight;
  }
  else if (nowTime <= getTimeOfDay(_night))
  {
    return DayStatus::AstonomicalTwilight;
  }
  return DayStatus::Night;
}

void TimeManager::updateForNewDay()
{
  if ( numberOfHours(now() - _lastTimeUpdated) >= 24 || _sunrise == 0)
  {
    fetchDaylightInfoAndTime();
  }
}

void TimeManager::setCurrentTime(const char* timeString)
{
  // Give the Time library the current time

  // Set the current date
  time_t current_time = timeFromDaylightString(timeString);
  setTime(current_time);
  _currentDay = day(now());
  _lastTimeUpdated = now();
}

void TimeManager::fetchDaylightInfoAndTime()
{
  wifiClient.setCACert(_apiCertificate);
  HTTPClient https;

  https.addHeader(_apiKeyName, SUNSET_API_KEY);

  // Build the url for the sunrise-sunset API call
  String url = _getSolarTimingsUrl;

  // Start the http client
  
  https.begin(wifiClient, url);

  // Make the request
  int httpCode = https.GET();
  Serial.println(httpCode);

  // Check for status code
  if (httpCode > 0)
  {
    // Get/print payload from http response
    String payload = https.getString();
    Serial.println(payload);

    // Check if request was successful with 200 status
    if (httpCode == 200)
    {
      // Size of JSON buffer for the response this service gives (as given by https://arduinojson.org/v5/assistant/)
      const size_t bufferSize = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(10) + 1000;

      // Create JSON buffer with the size above
      DynamicJsonDocument doc(bufferSize);

      // Parse response from http request
      auto error = deserializeJson(doc, payload);

      // If parsing failed for some reason, print an error message
      if (error)
      {
        Serial.println("Parsing failed");
        Serial.println(error.c_str());
        delay(5000);
      }
      else
      {
        setCurrentTime(doc["now"]);

        const char *nadirParsed = doc["nadir"];
        _nadir = timeFromDaylightString(nadirParsed);
        const char *nightEndParsed = doc["nightEnd"];
        _nightEnd = timeFromDaylightString(nightEndParsed);
        const char *nauticalDawnParsed = doc["nauticalDawn"];
        _nauticalDawn = timeFromDaylightString(nauticalDawnParsed);
        const char *dawnParsed = doc["dawn"];
        _dawn = timeFromDaylightString(dawnParsed);
        const char *sunriseParsed = doc["sunrise"];
        _sunrise = timeFromDaylightString(sunriseParsed);
        const char *sunriseEndParsed = doc["sunriseEnd"];
        _sunriseEnd = timeFromDaylightString(sunriseEndParsed);
        const char *goldenHourEndParsed = doc["goldenHourEnd"];
        _goldenHourEnd = timeFromDaylightString(goldenHourEndParsed);
        const char *solarNoonParsed = doc["solarNoon"];
        _solarNoon = timeFromDaylightString(solarNoonParsed);
        const char *goldenHourParsed = doc["goldenHour"];
        _goldenHour = timeFromDaylightString(goldenHourParsed);
        const char *sunsetStartParsed = doc["sunsetStart"];
        _sunsetStart = timeFromDaylightString(sunsetStartParsed);
        const char *sunsetParsed = doc["sunset"];
        _sunset = timeFromDaylightString(sunsetParsed);
        const char *duskParsed = doc["dusk"];
        _dusk = timeFromDaylightString(duskParsed);
        const char *nauticalDuskParsed = doc["nauticalDusk"];
        _nauticalDusk = timeFromDaylightString(nauticalDuskParsed);
        const char *nightParsed = doc["night"];
        _night = timeFromDaylightString(nightParsed);

// If in debug mode, make sunrise time 10 seconds from now, and sunset time 30 seconds after sunrise has finished fading
#if DEBUG_MODE
        sunriseTime = now() + 10;
        sunsetTime = sunriseTime + totalFadeTime + 30;
#endif
      }
    }
  }
  else
  {
    Serial.println("Error on HTTP request");
  }

  // Free resources in use by http client
  https.end();
  setDayTimes();
  printTimes();
}

void TimeManager::setDayTimes()
{
  _midDayBegin = (_solarNoon) - (getTimeOfDay(_sunsetStart) - getTimeOfDay(_sunset));
  _midDayEnd = (_solarNoon) + (getTimeOfDay(_sunsetStart) - getTimeOfDay(_sunset));
}

time_t TimeManager::getTimeOfDay(time_t time)
{
  tmElements_t timeElements = {second(time), minute(time), hour(time), 0, 0, 0, 0};
  return makeTime(timeElements);
}

time_t TimeManager::timeFromDaylightString(const char *daylightString)
{
  // Variables to hold date components
  int yr, mnth, dy, hr, mn, sec;

  // Parse input string into date components
  Serial.println(daylightString);
  sscanf(daylightString, "%d-%d-%dT%d:%d:%d", &yr, &mnth, &dy, &hr, &mn, &sec);

  // Create tmElements_t struct from components
  tmElements_t timeElements = {sec, mn, hr, weekday(), dy, mnth, yr - 1970};

  // Make time_t out of tmElements_t struct and return it
  return makeTime(timeElements);
}

void TimeManager::printTimes()
{
  time_t timeArray[13] = {
      _lastTimeUpdated,
      _nightEnd,
      _nauticalDawn,
      _dawn,
      _sunrise,
      _goldenHourEnd,
      _midDayBegin,
      _midDayEnd,
      _goldenHour,
      _sunset,
      _dusk,
      _nauticalDusk,
      _night};
  for (int i = 0; i < 12; i++)
  {
    Serial.println(i);
    printTime(timeArray[i]);
  }
}

void TimeManager::printTime(time_t time)
{
  Serial.print(year(time));
  Serial.print("-");
  Serial.print(month(time));
  Serial.print("-");
  Serial.print(day(time));
  Serial.print(" ");
  Serial.print(hour(time));
  Serial.print(":");
  Serial.print(minute(time));
  Serial.print(":");
  Serial.print(second(time));
  Serial.println(" ");
}

TimeManager::TimeManager()
{
}