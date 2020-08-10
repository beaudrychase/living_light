#include <WiFi.h>

#define WIFI_SSID "guest-wifi"
#define WIFI_PW "sudomod1"

void setup() {
  Serial.begin(115200);

  // Disconnect wifi before beginning (fixes some connection issues I've had)
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PW);

  // Wait for wifi to connect
  Serial.println("Connecting to wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }

  // We should be connected at this point, so print the ip address we were assigned
  Serial.println("Connected!");
  Serial.println(WiFi.localIP());

}

void loop() {
  // put your main code here, to run repeatedly:

}
