#include <Time.h>
#include <TimeLib.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include "gradient.h"

#define DEBUG_MODE 0

// Put your latitude and longitude here
String LATITUDE = "30.267153";
String LONGITUDE = "-97.743057";

// Put your API key for timezonedb.com here
String TIMEZONE_API_KEY = "5ANOLCRSRKFE";

#define PIXELS_PIN  15
#define PIXELS_COUNT  40

#define WIFI_SSID "p/g"
#define WIFI_PW "#0Ne23456789"

time_t currentTime;
int currentDay;
time_t sunriseTime = 0;
time_t sunsetTime = 0;

NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod> pixels(PIXELS_COUNT, PIXELS_PIN);
NeoPixelAnimator animation(1);

RgbwColor currentColor;
RgbwColor nextColor;

int currentX = -1; // Set this to -1 so it will be 0 the first time it's run.
int currentY = 0;

#if DEBUG_MODE
int pixelFadeTime = 100;
#else
int pixelFadeTime = 1250;
#endif
int totalFadeTime = (pixelFadeTime * GRADIENT_WIDTH * 2 * GRADIENT_HEIGHT) / 1000;

int gmtOffset;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  pixels.Begin();
  setColor(RgbwColor(0, 0, 0, 0));
}


void connectToWifi() {
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PW);

  Serial.println("Connecting to wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }
  
  Serial.println("Connected to wifi!");
}

void setColor(RgbwColor color) {
  for (int i = 0; i < PIXELS_COUNT; i++) {
    pixels.SetPixelColor(i, color);
  }
}

void animationCallback(const AnimationParam& param) {
  RgbwColor updatedColor = RgbwColor::LinearBlend(
      currentColor,
      nextColor,
      param.progress);

  setColor(updatedColor);
}

void setCurrentTime() {
    HTTPClient http;
    
    // Build the url for the timezonedb API call
    String url = "http://api.timezonedb.com/v2/get-time-zone?key=" + TIMEZONE_API_KEY + "&format=json&by=position&lat=" + LATITUDE + "&lng=" + LONGITUDE;

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
          currentDay = day(now());
        }
      }
    } else {
      Serial.println("Error while fetching current time...");
    }

    // Free resources in use by http client
    http.end();
}

// Expects string from sunrise-sunset service in this format: 2018-06-21T11:00:51+00:00
time_t timeFromDaylightString(const char* daylightString) {
  // Variables to hold date components
  int yr, mnth, dy, hr, mn, sec; 

  // Parse input string into date components
  sscanf(daylightString, "%d-%d-%dT%d:%d:%d", &yr, &mnth, &dy, &hr, &mn, &sec); 

  // Create tmElements_t struct from components
  tmElements_t timeElements = { sec, mn, hr, weekday(), dy, mnth, yr - 1970 }; 

  // Make time_t out of tmElements_t struct and return it
  return makeTime(timeElements);  
}

void fetchDaylightInfo() {
    HTTPClient http;

    // Build the url for the sunrise-sunset API call
    String url = "https://api.sunrise-sunset.org/json?lat=" + LATITUDE + "&lng=" + LONGITUDE + "&date="+String(year())+"-"+String(month())+"-"+String(day())+"&formatted=0";

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

          // Grab sunrise time from results
          const char * sunriseParsed = results["sunrise"];
          
          // Adjust for GMT offset and subtract half of the fade time, so it starts changing before the actual time
          sunriseTime = timeFromDaylightString(sunriseParsed) + gmtOffset - (long)((float)totalFadeTime/2.0); 
          
          // Grab sunset time from results
          const char * sunsetParsed = results["sunset"];
          // Adjust for GMT offset and subtract half of the fade time, so it starts changing before the actual time
          sunsetTime = timeFromDaylightString(sunsetParsed) + gmtOffset - (long)((float)totalFadeTime/2.0);

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

// Calculates the y position on the gradient that we should currently be at
void updateYPosition() {
  // Default to night time (i.e the bottom of the gradient), in case for whatever reason none of the below conditions are met
  int yPosition = GRADIENT_HEIGHT - 1;

  // If we're past sunset time on the current day
  if (currentTime > sunsetTime) {
    // If we're past sunset time but not yet done fading to night time
    if (currentTime < sunsetTime + totalFadeTime) {
      // Calculate the y position based on the percentage of the way through the fade interval that the current time is
      float percentOfFade = ((float)(currentTime - sunsetTime)/(float)totalFadeTime);
      yPosition = percentOfFade * ((float)GRADIENT_HEIGHT-1);

      // Print a bunch of stuff.
      Serial.print("Fading for sunset: ");
      Serial.print(currentTime);
      Serial.print(", ");
      Serial.print(sunsetTime);
      Serial.print(", ");
      Serial.print(totalFadeTime);
      Serial.print(", ");
      Serial.print(percentOfFade);
      Serial.print(", ");
      Serial.println(yPosition);
    } else {
      // Else it's night time and not yet time for sunrise, so stay at the bottom of the gradient
      yPosition = GRADIENT_HEIGHT - 1;
      Serial.print("Waiting for sunrise... ");
      Serial.print((sunriseTime - currentTime)/60);
      Serial.println(" minutes left.");
    }

    // If we're past sunrise time
  } else if (currentTime > sunriseTime) {
    // If we're past sunrise but not yet done fading to daytime
    if (currentTime < sunriseTime + totalFadeTime) {
      // Calculate the y position based on the percentage of the way through the fade interval that the current time is
      float percentOfFade = ((float)(currentTime - sunriseTime)/(float)totalFadeTime);
      yPosition = percentOfFade * ((float)GRADIENT_HEIGHT-1);
      yPosition = GRADIENT_HEIGHT - yPosition;

      // Print a bunch of stuff.
      Serial.print("Fading for sunrise: ");
      Serial.print(currentTime);
      Serial.print(", ");
      Serial.print(sunriseTime);
      Serial.print(", ");
      Serial.print(totalFadeTime);
      Serial.print(", ");
      Serial.print(percentOfFade);
      Serial.print(", ");
      Serial.println(yPosition);
    } else {
      // Else it's daytime and we're waiting for sunset, so stay at the top of the gradient
      Serial.print("Waiting for sunset... ");
      Serial.print((sunsetTime - currentTime)/60);
      Serial.println(" minutes left.");
      yPosition = 0;
    }
  // Else it's it's the following day but too early for sunrise
  } else {
    Serial.print("Waiting for sunrise... ");
    Serial.print((sunriseTime - currentTime)/60);
    Serial.println(" minutes left.");
  }

  // Make sure our math didn't put us outside of the gradient array
  if (yPosition > GRADIENT_HEIGHT - 1) {
    yPosition = GRADIENT_HEIGHT - 1;
  } else if (yPosition < 0) {
    yPosition = 0;
  }

  currentY = yPosition;
}

// Calculates the x position for sweeping back and forth on the current row of pixels in the gradient
void updateXPosition() {
  // Increment current x position
  currentX++;

  // currentX will go from 0 to 2*GRADIENT_WIDTH (easier math for making it go back and forth on the same row).
  // So if currentX has gone past that point, reset to 0 (that's where we would be anyway) and recalculate the Y posistion
  if (currentX > GRADIENT_WIDTH * 2 - 2) {
    currentX = 0;
    updateYPosition();
  }

  // If currentX is past GRADIENT_WIDTH then we're supposed to be sweeping back the other way to the left side of the gradient
  // This accounts for that
  int xPosition = currentX;
  if (currentX >= GRADIENT_WIDTH - 1) {
    xPosition = GRADIENT_WIDTH - (currentX - GRADIENT_WIDTH) - 2;
  }

  Serial.print("xPosition: ");
  Serial.println(xPosition);

  // The gradient array is a 1-dimensional array, with R,G,B values repeated.  This math converts the currentX and currentY values
  // into the appropriate 1-dimensional array index.
  int arrayPos = (currentY * GRADIENT_WIDTH + xPosition) * 3;

  // Save current color so we know what values we are fading between
  currentColor = pixels.GetPixelColor(0);
  // Get next color from the gradient array
  nextColor = RgbwColor(gradient[arrayPos], gradient[arrayPos+1], gradient[arrayPos+2], 0);
  // Tell the animation object to animate using the pixelFadeTime defined at the top, and the animationCallback function we defined
  animation.StartAnimation(0, pixelFadeTime, animationCallback);
}

void loop() {
  // If it's a new day or we haven't fetched daylight info yet
  if (currentDay != day(now()) || sunriseTime == 0) {
    Serial.println("New day, fetching new date/time/daylight info...");
    // Connect to wifi if we aren't already
    if (WiFi.status() != WL_CONNECTED) {
      connectToWifi();
    }

    // Fetch current time
    setCurrentTime();

    // Fetch daylight info
    fetchDaylightInfo();

    // Disconnect/disable wifi so it doesn't interfere with our pixels.
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
  }

  // Save current time for use in x/y position calculations
  currentTime = now();

  // If we're in the middle of fading between two pixel colors, tell the animator to update itself and show the new values
  if (animation.IsAnimating()) {
    animation.UpdateAnimations();
    pixels.Show();
    
  // Else we're done fading between two pixel colors, calculate the next pixel color to fade to
  } else {
    updateXPosition();
  }
}
