#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <Wire.h>
#include "Adafruit_VEML7700.h"

Adafruit_VEML7700 veml = Adafruit_VEML7700();

const uint8_t PixelCount = 40;
const uint8_t PixelPin = 12;
const uint8_t AnimationChannels = 1;
const float smoothness_pts = 230;//larger=slower change in brightness
float cached_brightness[230]; 
float g = 0.14; // affects the width of peak (more or less darkness)
float beta = 0.5; // shifts the gaussian to be symmetric
NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin); // Pin added for Uno
NeoGamma<NeoGammaEquationMethod> colorGamma;
//void SetRandomSeed()
//{
//    uint32_t seed;
//
//    // random works best with a seed that can use 31 bits
//    // analogRead on a unconnected pin tends toward less than four bits
//    seed = analogRead(0);
//    delay(1);
//
//    for (int shifts = 3; shifts < 31; shifts += 3)
//    {
//        seed ^= analogRead(0) << shifts;
//        delay(1);
//    }
//
//    randomSeed(seed);
//}

inline float calculate_brightness(int ii) {
  return 255.0*(exp(-(pow(((ii/smoothness_pts)-beta)/g,2.0))/2.0));
}

void setup() {

  strip.Begin();
  strip.ClearTo(RgbwColor(0, 0, 0, 0));
  strip.Show();
}
float brightness_multiplier = 1;
void loop() {
  strip.Show();
  strip.ClearTo(RgbwColor(0,0,0,1));
  strip.Show();
  strip.ClearTo(RgbwColor(0,0,0,0));
  strip.Show();
  strip.ClearTo(RgbwColor(0,0,0,0));
  strip.Show();
  strip.ClearTo(RgbwColor(0,0,0,0));
  strip.Show();
  strip.ClearTo(RgbwColor(0,0,0,0));
  strip.Show();
  strip.ClearTo(RgbwColor(0,0,0,0));
  strip.Show();
  strip.ClearTo(RgbwColor(0,0,0,0));
  strip.Show();
  strip.ClearTo(RgbwColor(0,0,0,0));
  strip.Show();
  strip.ClearTo(RgbwColor(0,0,0,0));
  strip.Show();
  strip.ClearTo(RgbwColor(0,0,0,0));
  strip.Show();
  strip.ClearTo(RgbwColor(0,0,0,0));
//  for (int ii=0;ii<smoothness_pts;ii++){
//    strip.ClearTo(colorGamma.Correct(RgbwColor(cached_brightness[ii] * brightness_multiplier, 0, 0, cached_brightness[ii] * brightness_multiplier)));
//    strip.Show();
//    delay(10);
//  }

  
}
