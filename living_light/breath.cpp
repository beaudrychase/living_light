#include <FastLED.h>
#include "breath.h"
#include "dithering.h"
#include "constants.h"

double smoothness_pts = SMOOTHNESS;//larger=slower change in brightness
int breath_length = SMOOTHNESS;
float  static_cached_brightness[STATIC_SMOOTHNESS];
float* dynamic_cached_brightness;
int dynamic_length = SMOOTHNESS - STATIC_SMOOTHNESS;
double color_gamma_correction = 1.5;
double gam = 0.13; // affects the width of peak (more or less darkness)
double beta = 0.5; // shifts the gaussian to be symmetric

double fade_amount = 0.75 * smoothness_pts;
double oldR = 0.1;
double oldG = 0.1;
double oldB = 0.1;

void initBreathe() {
  //  Serial.println(ESP.getFreeHeap());
  dynamic_cached_brightness = (float *) malloc((dynamic_length) * sizeof(float));
  initDither();
  for (int i = 0; i < STATIC_SMOOTHNESS; ++i) {
    static_cached_brightness[i] = computeBrightnessVal(i, smoothness_pts);
  }
  for (int i = 0; i < dynamic_length; ++i) {
    dynamic_cached_brightness[i] = computeBrightnessVal(i + STATIC_SMOOTHNESS, smoothness_pts);
  }
}

inline float computeBrightnessVal(int step, double total_steps) {
  return (255.0 * DITHER_LEVEL * pow((exp(-(pow(((((double)step) / total_steps) - beta) / gam, 2.0)) / 2.0)), 1.0 / color_gamma_correction));
}

void updateBreathLength(int new_len_sec) {
  breath_length = new_len_sec * 1500;
  smoothness_pts = ((double) breath_length);
  if (breath_length > STATIC_SMOOTHNESS) {
    for (int i = 0; i < STATIC_SMOOTHNESS; i++) {
      static_cached_brightness[i] = computeBrightnessVal(i, smoothness_pts);
    }
    for (int i = STATIC_SMOOTHNESS; i < breath_length; ++i) {
      dynamic_cached_brightness[i - STATIC_SMOOTHNESS] = computeBrightnessVal(i, smoothness_pts);
    }
  } else {
    for (int i = 0; i < breath_length; ++i) {
      static_cached_brightness[i] = computeBrightnessVal(i, smoothness_pts);
    }

  }
}

void normalizeColor(double brightness, double &r, double &g, double &b) {
  double normFactor = brightness / (r + g + b) / 3;
  r = r * normFactor;
  g = g * normFactor;
  b = b * normFactor;

}

void turnOff() {
  frame_set_color(0, 0, 0);
  FastLED.show();
}


void breath_iteration(double r, double g, double b, int array_len, float* brightness_array, int offset) {
  for (int i = 0; i < array_len; ++i) {
    double increasing = min((i + offset) * 3.0, smoothness_pts) / smoothness_pts;
    //    double decreasing = max((double)(smoothness_pts - (i + offset) * 3.0) / smoothness_pts, 0.0);
    double smooth_r = r * increasing + oldR * (1.0 - increasing);
    double smooth_g = g * increasing + oldG * (1.0 - increasing);
    double smooth_b = b * increasing + oldB * (1.0 - increasing);
    double smoothed_cached_brightness = brightness_array[i] / (smooth_r + smooth_g + smooth_b) / 3.0;

    int red = (int) (smooth_r * smoothed_cached_brightness);
    int green = (int) (smooth_g * smoothed_cached_brightness);
    int blue = (int) (smooth_b * smoothed_cached_brightness);
    frame_set_color(red, green, blue);
    FastLED.show();
  }

}

void breathe(double r, double g, double b) {
  breath_iteration(r, g, b, STATIC_SMOOTHNESS, static_cached_brightness, 0);
  breath_iteration(r, g, b, dynamic_length, dynamic_cached_brightness, STATIC_SMOOTHNESS);
  oldR = r;
  oldG = g;
  oldB = b;
}
