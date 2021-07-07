#include <Classes/TimeManager.h>

TimeManager::DayStatus TimeManager::getDayStatus(){
    time_t nowTime = getTimeOfDay(now());
    if (nowTime <= getTimeOfDay(_civilTwilightBegin)){
        return DayStatus::Night;
    } else if (nowTime <= getTimeOfDay(_sunrise)){
        return DayStatus::Twilight;
    } else if (nowTime <= getTimeOfDay(_sunset)){
        return DayStatus::Day;
    } else if (nowTime <= getTimeOfDay(_civilTwilightEnd)){
        return DayStatus::Twilight;
    }
    return DayStatus::Night;

}

void TimeManager::updateForNewDay(){
    if (_currentDay != day(now()) || _sunrise == 0) {
      setCurrentTime();
      fetchDaylightInfo();
    }
    if (day(_sunset) != day(now())) {
      setCurrentTime();
      fetchDaylightInfo();
    }
}

TimeManager::TimeManager(){
}

void TimeManager::setCurrentTime(){
    HTTPClient http;
    
    // Build the url for the timezonedb API call
//    String url = "http://api.timezonedb.com/v2/get-time-zone?key=" + String(TIMEZONE_API_KEY) + "&format=json&by=position&lat=" + LATITUDE + "&lng=" + LONGITUDE;

    // Start the http client
    http.begin(setCurrentTimeUrl); 

    // Make the request
    int httpCode = http.GET();

    // Check for status code
    if (httpCode > 0) { 
      // Get/print payload from http response
      String payload = http.getString();
//      Serial.println(payload);

      // Check if request was successful with 200 status
      if (httpCode == 200) {
        // Size of JSON buffer for the response this service gives (as given by https://arduinojson.org/v5/assistant/)
        const size_t bufferSize = JSON_OBJECT_SIZE(13) + 270;
        
        // Create JSON buffer with the size above
        DynamicJsonDocument doc(bufferSize);

        // Parse response from http request
        auto error = deserializeJson(doc, payload);

        // If parsing failed for some reason, print an error message
        if (error) {
          Serial.println("Parsing failed");
          delay(5000);
        } else {
          
          // Grab formatted current time parsed JSON response (just for printing)
          const char * parsedTime = doc["formatted"];

          // Grab timestamp for current time from parsed JSON
          long timestamp = doc["timestamp"];

          // Set GMT offset (i.e timezone) from parsed JSON
          gmtOffset = doc["gmtOffset"];

          // Print out all of that for debugging
          Serial.print("parsedTime: ");
          Serial.println(parsedTime);
          Serial.print("gmtOffset: ");
          Serial.println(gmtOffset);
          Serial.print("timestamp: ");
          Serial.println(timestamp);

          // Give the Time library the current time
          setTime(timestamp);

          // Set the current date
          _currentDay = day(now());
          _lastTimeUpdated = now();
          Serial.println(now());
        }
      }
    } else {
      
      Serial.println("Error while fetching current time...");
    }

    // Free resources in use by http client
    http.end();
}

void TimeManager::fetchDaylightInfo(){
    HTTPClient http;

    // Build the url for the sunrise-sunset API call
    String url = "https://api.sunrise-sunset.org/json?lat=" + String(LATITUDE) + "&lng=" + String(LONGITUDE) + "&date="+String(year())+"-"+String(month())+"-"+String(day())+"&formatted=0";

    // Start the http client
    http.begin(url);

    // Make the request
    int httpCode = http.GET();

    // Check for status code
    if (httpCode > 0) {
      // Get/print payload from http response
      String payload = http.getString();
      Serial.println(payload);

      // Check if request was successful with 200 status
      if (httpCode == 200) {
        // Size of JSON buffer for the response this service gives (as given by https://arduinojson.org/v5/assistant/)
        const size_t bufferSize = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(10) + 500;

        // Create JSON buffer with the size above
        DynamicJsonDocument doc(bufferSize);

        // Parse response from http request
        auto error = deserializeJson(doc, payload);
        
        // If parsing failed for some reason, print an error message
        if (error) {
          Serial.println("Parsing failed");
          Serial.println(error.c_str());
          delay(5000);
        } else {
          // Get results from parsed JSON
          const JsonObject& results = doc["results"];

          //Grab twilight begin time
          const char * twilightBeginParsed = results["civil_twilight_begin"];
          const char * twilightEndParsed = results["civil_twilight_end"];
          _civilTwilightBegin = timeFromDaylightString(twilightBeginParsed) + gmtOffset;
          _civilTwilightEnd = timeFromDaylightString(twilightEndParsed) + gmtOffset;

          // Grab sunrise time from results
          const char * sunriseParsed = results["sunrise"];
          
          // Adjust for GMT offset and subtract half of the fade time, so it starts changing before the actual time
          _sunrise = timeFromDaylightString(sunriseParsed) + gmtOffset; 
          
          // Grab sunset time from results
          const char * sunsetParsed = results["sunset"];
          // Adjust for GMT offset and subtract half of the fade time, so it starts changing before the actual time
          _sunset = timeFromDaylightString(sunsetParsed) + gmtOffset;

          // If in debug mode, make sunrise time 10 seconds from now, and sunset time 30 seconds after sunrise has finished fading
          #if DEBUG_MODE
            sunriseTime = now() + 10; 
            sunsetTime = sunriseTime + totalFadeTime + 30; 
          #endif
        }
      }

    } else {
      Serial.println("Error on HTTP request");
    }

    // Free resources in use by http client
    http.end();
}

time_t TimeManager::getTimeOfDay(time_t time){
    tmElements_t timeElements = { second(time), minute(time), hour(time), 0, 0, 0, 0};
    return makeTime(timeElements);
}

time_t TimeManager::timeFromDaylightString(const char* daylightString) {
  // Variables to hold date components
  int yr, mnth, dy, hr, mn, sec; 

  // Parse input string into date components
  sscanf(daylightString, "%d-%d-%dT%d:%d:%d", &yr, &mnth, &dy, &hr, &mn, &sec); 

  // Create tmElements_t struct from components
  tmElements_t timeElements = { sec, mn, hr, weekday(), dy, mnth, yr  - 1970 }; 

  // Make time_t out of tmElements_t struct and return it
  return makeTime(timeElements);  
}
