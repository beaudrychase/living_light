#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <Wire.h>
#include "Adafruit_VEML7700.h"

Adafruit_VEML7700 veml = Adafruit_VEML7700();
#define SMOOTHNESS 5100
#define DITHER_LEVEL 16
#define ROW 5
#define COL 8
const uint8_t PixelCount = ROW * COL;
const uint8_t PixelPin = 15;
const float smoothness_pts = SMOOTHNESS;//larger=slower change in brightness
int cached_brightness[SMOOTHNESS];
int cached_brightness2[SMOOTHNESS];
float gam = 0.165; // affects the width of peak (more or less darkness)
float gam2 = 0.10;
float beta = 0.5; // shifts the gaussian to be symmetric
float light_adjustment = 1.0;
NeoPixelBus<NeoGrbwFeature, NeoEsp32I2s1800KbpsMethod> strip(PixelCount, PixelPin); // Pin added for Uno
NeoGamma<NeoGammaTableMethod> colorgamma;

inline int getIdx(int r, int c){
  return r * COL + c;
}

int frame = 0;
inline void dithering(int idx, int r, int g, int b, int w){
  strip.SetPixelColor(idx, RgbwColor(ditherSingle(idx, r), ditherSingle(idx, g), ditherSingle(idx, b), ditherSingle(idx, w)));
}

inline int ditherSingle(int idx, int channel){
//  int granularBrightness = channel % DITHER_LEVEL;
//  int offset = (idx + frame) % DITHER_LEVEL;
//  int lowerBrightness = channel / DITHER_LEVEL;
  
  if (channel % DITHER_LEVEL <= (idx + frame) % DITHER_LEVEL){
    return channel / DITHER_LEVEL;
  }
  return channel / DITHER_LEVEL + 1;
  
}

void breathe(float r, float g, float b, float w){   
    for(int i = 0; i < smoothness_pts; ++i){
      int red = (int) (r * cached_brightness2[i]);
      int green = (int) (g * cached_brightness2[i]);
      int blue = (int) (b * cached_brightness2[i]);
      int white = (int) (w * cached_brightness[i]);
      for(int r = 0; r < ROW; r++){
        for(int c = 0; c < COL; c++){
          int rowDif = r - ROW;
          int colDif = c - COL/2;
          float filter = 1.0 / (2 << c + 1);
          dithering(getIdx(r, c), red, green, blue, white);
        }
      }
      strip.Show();
      frame = (frame + 1) % DITHER_LEVEL;
  }
}


void setup() {
  Serial.begin(115200);
  if (!veml.begin()) {
    Serial.println("Sensor not found");
    while (1);
  }
  veml.setGain(VEML7700_GAIN_1);
  veml.setIntegrationTime(VEML7700_IT_25MS);
  randomSeed(analogRead(0));
  for (int i = 0; i < SMOOTHNESS; ++i){
    cached_brightness[i] = (int) 255.0* DITHER_LEVEL*(exp(-(pow(((i/smoothness_pts)-beta)/gam,2.0))/2.0));
  }
  for (int i = 0; i < SMOOTHNESS; ++i){
    cached_brightness2[i] = (int) 255.0* DITHER_LEVEL*(exp(-(pow(((i/smoothness_pts)-beta)/gam2,2.0))/2.0));
  }
  strip.Begin();
  strip.ClearTo(RgbwColor(0, 0, 0, 0));
  strip.Show();
}
void loop() {
  float r = light_adjustment * 1.0;
  float g = random(1, 500) / 500.0;
  float b = random(1, 500) / 500.0;
  float w = light_adjustment * 0.27;
//  Serial.print(r);
  breathe(r,0,0, w);
  light_adjustment = min(0.1 + (0.9 / 60.0 * veml.readLuxNormalized()), 1.0);
  
}
