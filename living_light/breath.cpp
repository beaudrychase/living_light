#include <FastLED.h>
#include "dithering.h"
#include "constants.h"

const float smoothness_pts = SMOOTHNESS;//larger=slower change in brightness
int cached_brightness[SMOOTHNESS];
double color_gamma_correction = 1.5;
double gam = 0.16; // affects the width of peak (more or less darkness)
double beta = 0.5; // shifts the gaussian to be symmetric

double fade_amount = 0.75 * (double) smoothness_pts;
double oldR = 0.0;
double oldG = 0.0;
double oldB = 0.0;

void initBreathe(){
  initDither();
  for (int i = 0; i < SMOOTHNESS; ++i) {
    cached_brightness[i] = (int) (255.0 * DITHER_LEVEL * pow(255.0 * DITHER_LEVEL * (exp(-(pow(((i / smoothness_pts) - beta) / gam, 2.0)) / 2.0)) / (255.0 * DITHER_LEVEL), 1.0 / color_gamma_correction));
  }
}

void normalizeColor(double brightness, double &r, double &g, double &b) {
  double normFactor = brightness / (r + g + b) / 3;
  r = r * normFactor;
  g = g * normFactor;
  b = b * normFactor;
  
}

void turnOff(){
  frame_set_color(0, 0, 0);
  FastLED.show();
}

void breathe(double r, double g, double b) {
  for (int i = 0; i < smoothness_pts; ++i) {
    double increasing = min((double) (i * 3.0) / (double) smoothness_pts, 1.0);
    double descreasing = max((double)(smoothness_pts - i * 3.0) / (double) smoothness_pts, 0.0);
    
    int red = (int) ((r * increasing + oldR * descreasing) * cached_brightness[i]);
    int green = (int) ((g * increasing + oldG * descreasing) * cached_brightness[i]);
    int blue = (int) ((b * increasing + oldB * descreasing) * cached_brightness[i]);
    frame_set_color(red, green, blue);
    FastLED.show();
  }
  oldR = r;
  oldG = g;
  oldB = b;
}
