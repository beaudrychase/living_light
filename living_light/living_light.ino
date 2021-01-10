#include <FastLED.h>
#include <SPI.h>
#include <Wire.h>
#include "constants.h"
#include "dithering.h"

const float smoothness_pts = SMOOTHNESS;//larger=slower change in brightness
int cached_brightness[SMOOTHNESS];
double color_gamma_correction = 1.5;
double gam = 0.12; // affects the width of peak (more or less darkness)
double gam2 = 0.10;
double beta = 0.5; // shifts the gaussian to be symmetric

void breathe(double r, double g, double b) {
  for (int i = 0; i < smoothness_pts; ++i) {
    int red = (int) (r * cached_brightness[i]);
    int green = (int) (g * cached_brightness[i]);
    int blue = (int) (b * cached_brightness[i]);
    frame_set_color(red, green, blue);
    FastLED.show();
  }
}

void setup() {
  initDither();
  Serial.begin(115200);
  randomSeed(analogRead(0));
  //gamma correction
  for (int i = 0; i < SMOOTHNESS; ++i) {
    cached_brightness[i] = (int) (255.0 * DITHER_LEVEL * pow(255.0 * DITHER_LEVEL * (exp(-(pow(((i / smoothness_pts) - beta) / gam, 2.0)) / 2.0)) / (255.0 * DITHER_LEVEL), 1.0 / color_gamma_correction));
  }
}
void loop() {
  double rand1 = random(4000, 30000) / 100000.0;
  double rand2 = random(1500, 20000) / 100000.0;
  double rand3 = random(500, 10000) / 100000.0;
  double high = max(rand1, max(rand2, rand3));
  double low = min(rand1, min(rand2, rand3));
  double med = max(min(rand1, rand2), min(rand2, rand3));
  double r = high;
  double g = med;
  double b = low;
  breathe(r, g, b);

}
