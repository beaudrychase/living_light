#include <NeoPixelBus.h>

// Define the pin number, and number of pixels we'll be using
#define PIXELS_PIN 14
#define PIXELS_COUNT 5

// Create NeoPixelBus object, tell it what type of pixels we're using, what speed
// to use in talking to them, and the number of pixels and pin number
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> pixels(PIXELS_COUNT, PIXELS_PIN);

void setup() {
  // Tell the NeoPixelBus object to set itself up
  pixels.Begin();

  // Set all the pixels to green
  for (int i = 0; i < PIXELS_COUNT; i++) {
    pixels.SetPixelColor(i, RgbColor(0, 255, 0));
  }

  // Show the updated color (new colors won't show up until you call this function)
  pixels.Show();
}

void loop() {
  // put your main code here, to run repeatedly:

}
