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
SmoothColor oldColor(0.0);

  SmoothColor::SmoothColor(double brightness){
    this->red = brightness;
    this->green = brightness;
    this->blue =brightness;
  }
  SmoothColor::SmoothColor(double red, double blue, double green){
    this->red = red;
    this->green = green;
    this->blue =blue;
  }

  SmoothColor::SmoothColor(double red, double blue, double green, double brightness): SmoothColor(red, blue, green){
    this->normalizeColor(brightness);
  }
  void SmoothColor::normalizeColor(double brightness){
    double normFactor = brightness / (red + green + blue) / 3;
    red = red * normFactor;
    green = green * normFactor;
    blue = blue * normFactor;
  }


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



void turnOff() {
  frame_set_color(0, 0, 0);
  FastLED.show();
}


void breath_iteration(SmoothColor color, int array_len, float* brightness_array, int offset) {
  for (int i = 0; i < array_len; ++i) {
    double increasing = min((i + offset) * 3.0, smoothness_pts) / smoothness_pts;
    //    double decreasing = max((double)(smoothness_pts - (i + offset) * 3.0) / smoothness_pts, 0.0);
    double smooth_r = color.red * increasing + oldColor.red * (1.0 - increasing);
    double smooth_g = color.green * increasing + oldColor.green * (1.0 - increasing);
    double smooth_b = color.blue * increasing + oldColor.blue * (1.0 - increasing);
    double smoothed_cached_brightness = brightness_array[i] / (smooth_r + smooth_g + smooth_b) / 3.0;

    int red = (int) (smooth_r * smoothed_cached_brightness);
    int green = (int) (smooth_g * smoothed_cached_brightness);
    int blue = (int) (smooth_b * smoothed_cached_brightness);
    frame_set_color(red, green, blue);
    FastLED.show();
  }

}

void breathe(SmoothColor color) {
  breath_iteration(color, STATIC_SMOOTHNESS, static_cached_brightness, 0);
  breath_iteration(color, dynamic_length, dynamic_cached_brightness, STATIC_SMOOTHNESS);
  oldColor = color;
}
