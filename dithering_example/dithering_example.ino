#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <Wire.h>

#define SMOOTHNESS 5100
#define DITHER_LEVEL 16
#define ROW 5
#define COL 8
const uint8_t PixelCount = ROW * COL;
const uint8_t PixelPin = 15;
const uint8_t AnimationChannels = 1;
const float smoothness_pts = SMOOTHNESS;//larger=slower change in brightness
int cached_brightness[SMOOTHNESS]; 
float gam = 0.17; // affects the width of peak (more or less darkness)
float beta = 0.5; // shifts the gaussian to be symmetric
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


void setup() {
  for (int i = 0; i < SMOOTHNESS; ++i){
    cached_brightness[i] = (int) 255.0* DITHER_LEVEL*(exp(-(pow(((i/smoothness_pts)-beta)/gam,2.0))/2.0));
  }
  strip.Begin();
  strip.ClearTo(RgbwColor(0, 0, 0, 0));
  strip.Show();
}
void loop() {
//  for(int i = 0; i < smoothness_pts; ++i){
////    float pwm_val = 255.0* DITHER_LEVEL*(exp(-(pow(((i/smoothness_pts)-beta)/gam,2.0))/2.0));
//    
//    for(int j = 0; j < 40 ; j++){
//      dithering(j , cached_brightness[i], 0, 0, cached_brightness[i]/ 16);
//    }
//    strip.Show();
//    frame = (frame + 1) % DITHER_LEVEL;
//  }

  for(int i = 0; i < smoothness_pts; ++i){
//    float pwm_val = 255.0* DITHER_LEVEL*(exp(-(pow(((i/smoothness_pts)-beta)/gam,2.0))/2.0));
    
    for(int j = 0; j < 40 ; j++){
      dithering(j , 0, cached_brightness[i], cached_brightness[i], cached_brightness[i]);
    }
    strip.Show();
    frame = (frame + 1) % DITHER_LEVEL;
  }
}
