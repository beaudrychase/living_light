#include <FastLED.h>
#include "dithering.h"
#include "constants.h"

const float smoothness_pts = SMOOTHNESS;//larger=slower change in brightness
int cached_brightness[SMOOTHNESS];
double color_gamma_correction = 1.5;
double gam = 0.12; // affects the width of peak (more or less darkness)
double gam2 = 0.10;
double beta = 0.5; // shifts the gaussian to be symmetric

void initBreathe(){
  initDither();
  for (int i = 0; i < SMOOTHNESS; ++i) {
    cached_brightness[i] = (int) (255.0 * DITHER_LEVEL * pow(255.0 * DITHER_LEVEL * (exp(-(pow(((i / smoothness_pts) - beta) / gam, 2.0)) / 2.0)) / (255.0 * DITHER_LEVEL), 1.0 / color_gamma_correction));
  }
}

void breathe(double r, double g, double b) {
  for (int i = 0; i < smoothness_pts; ++i) {
    int red = (int) (r * cached_brightness[i]);
    int green = (int) (g * cached_brightness[i]);
    int blue = (int) (b * cached_brightness[i]);
    frame_set_color(red, green, blue);
    FastLED.show();
  }
}
