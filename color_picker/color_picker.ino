#include <WiFi.h>
#include <NeoPixelBus.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

#include "color_picker.h"

#define PIXELS_PIN  12
#define PIXELS_COUNT  40

#define WIFI_SSID "p/g"
#define WIFI_PW "#0Ne23456789"

// Variables to store the most recent color sent to us by the client (i.e a web browser)
int red = 255;
int green = 255;
int blue = 255;

// Variables to store the current color of the pixels (we will compare with above to see if we need to update the color)
int currentRed = red;
int currentGreen = green;
int currentBlue = blue;

// The server object that will listen on port 80 and respond to requests from web browsers
AsyncWebServer server(80);

NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod> pixels(PIXELS_COUNT, PIXELS_PIN);

void setup() {
  Serial.begin(115200);
  pixels.Begin();

  // Disconnect wifi before beginning (fixes some connection issues I've had)
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PW);

  // Connect to wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Makes it so you can find the esp32 on your network by visiting pixels.local/ in your browser rather than an ip address
  MDNS.begin("pixels");
  
  Serial.println(WiFi.localIP());

  // On a GET request, respond with the HTML string found in color_picker.h
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/html", HTML);
  });

  // On a POST request, grab the red, green and blue values that were sent and save them
  server.on("/", HTTP_POST, [](AsyncWebServerRequest * request) {
    AsyncWebParameter *redParam = request->getParam("red", true);
    AsyncWebParameter *greenParam = request->getParam("green", true);
    AsyncWebParameter *blueParam = request->getParam("blue", true);

    red = redParam->value().toInt();
    green = greenParam->value().toInt();
    blue = blueParam->value().toInt();
  });

  // Start the server
  server.begin();
}

void loop() {
  // If the color has changed, then update the pixel colors and call Show()
  if (currentRed != red || currentGreen != green || currentBlue != blue) {
    currentRed = red;
    currentGreen = green;
    currentBlue = blue;
    for (int i = 0; i < PIXELS_COUNT; i++) {
      pixels.SetPixelColor(i, RgbwColor(currentRed, currentGreen, currentBlue, 0));
    }
    pixels.Show();
  }

  delay(20);
}
