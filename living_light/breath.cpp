#include <FastLED.h>
#include "dithering.h"
#include "constants.h"

const double smoothness_pts = SMOOTHNESS;//larger=slower change in brightness
float* cached_brightness;
double color_gamma_correction = 1.5;
double gam = 0.14; // affects the width of peak (more or less darkness)
double beta = 0.5; // shifts the gaussian to be symmetric

double fade_amount = 0.75 * smoothness_pts;
double oldR = 0.0;
double oldG = 0.0;
double oldB = 0.0;

void initBreathe() {
//  Serial.println(ESP.getFreeHeap());
  cached_brightness = (float *) malloc(SMOOTHNESS* sizeof(float));
//  Serial.println(ESP.getFreeHeap());
  initDither();
  for (int i = 0; i < SMOOTHNESS; ++i) {
    cached_brightness[i] = (255.0 * DITHER_LEVEL * pow((exp(-(pow(((((double)i) / smoothness_pts) - beta) / gam, 2.0)) / 2.0)), 1.0 / color_gamma_correction));
    //    Serial.println("(((double)i) / smoothness_pts) - beta) / gam");
    //    Serial.println(" ");
    //    Serial.println("i:");
    //    Serial.println(i);
    //    Serial.println("1:");
    //    Serial.println(((((double)i) / smoothness_pts) - beta) / gam, 5);
    ////    Serial.println("(pow(((((double)i) / smoothness_pts) - beta) / gam, 2.0))");
    //    Serial.println("2:");
    //    Serial.println((pow(((((double)i) / smoothness_pts) - beta) / gam, 2.0)),5);
    ////    Serial.println("pow(255.0 * DITHER_LEVEL * (exp(-(pow(((((double)i) / smoothness_pts) - beta) / gam, 2.0)) / 2.0)) / (255.0 * DITHER_LEVEL), 1.0 / color_gamma_correction)");
    //    Serial.println("3:");
    //    Serial.println(pow(255.0 * DITHER_LEVEL * (exp(-(pow(((((double)i) / smoothness_pts) - beta) / gam, 2.0)) / 2.0)) / (255.0 * DITHER_LEVEL), 1.0 / color_gamma_correction),5);
    ////
    //    Serial.println(cached_brightness[i], 5);
    //    Serial.println((255.0 * DITHER_LEVEL * pow(255.0 * DITHER_LEVEL * (exp(-(pow(((i / smoothness_pts) - beta) / gam, 2.0)) / 2.0)) / (255.0 * DITHER_LEVEL), 1.0 / color_gamma_correction)),5);
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

void breathe(double r, double g, double b) {
  for (int i = 0; i < smoothness_pts; ++i) {
    double increasing = min((double) (i * 3.0) / smoothness_pts, 1.0);
    double decreasing = max((double)(smoothness_pts - i * 3.0) / smoothness_pts, 0.0);
    double smooth_r = r * increasing + oldR * decreasing;
    double smooth_g = g * increasing + oldG * decreasing;
    double smooth_b = b * increasing + oldB * decreasing;
    double smoothed_cached_brightness = cached_brightness[i] / (smooth_r + smooth_g + smooth_b) / 3.0;

    int red = (int) (smooth_r * smoothed_cached_brightness);
    int green = (int) (smooth_g * smoothed_cached_brightness);
    int blue = (int) (smooth_b * smoothed_cached_brightness);
    //
    //    int red = (int) ((r * increasing + oldR * decreasing) * cached_brightness[i]);
    //    int green = (int) ((g * increasing + oldG * decreasing) * cached_brightness[i]);
    //    int blue = (int) ((b * increasing + oldB * decreasing) * cached_brightness[i]);
    frame_set_color(red, green, blue);
    FastLED.show();
  }
//  Serial.println(ESP.getFreeHeap());
  oldR = r;
  oldG = g;
  oldB = b;
}
