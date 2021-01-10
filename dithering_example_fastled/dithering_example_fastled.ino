#include <FastLED.h>
#include <SPI.h> 
#include <Wire.h>

#define SMOOTHNESS 18000
#define DITHER_LEVEL 34
#define ROW 8
#define COL 8
#define DATA_PIN 18
#define CLOCK_PIN 5
#define NUM_LEDS 64

CRGB leds[NUM_LEDS];



const float smoothness_pts = SMOOTHNESS;//larger=slower change in brightness
int cached_brightness[SMOOTHNESS];
//int cached_brightness2[SMOOTHNESS];
double color_gamma_correction = 1.5;
double gam = 0.12; // affects the width of peak (more or less darkness)
double gam2 = 0.10;
double beta = 0.5; // shifts the gaussian to be symmetric
int r_dithering_offset = 0;
int g_dithering_offset = (DITHER_LEVEL / 3);
int b_dithering_offset = (DITHER_LEVEL / 3) * 2.0;

int frame_array[DITHER_LEVEL];

inline int getIdx(int r, int c){
  return r * COL + c;
}

int frame = 0;
int actual_frame = 0;
inline void dithering(int idx, int r, int g, int b){  
  leds[idx].setRGB(ditherSingle((idx + r_dithering_offset)%DITHER_LEVEL, r), ditherSingle((idx + g_dithering_offset)%DITHER_LEVEL, g), ditherSingle((idx + b_dithering_offset)%DITHER_LEVEL, b));
//  leds[idx].setRGB(ditherSingle(idx, r), ditherSingle(idx, g), ditherSingle(idx, b));
 
}

inline int ditherSingle(int idx, int channel){
//  
//  if (channel % DITHER_LEVEL <= (idx + frame) % DITHER_LEVEL){
//    return channel / DITHER_LEVEL;
//  }
//  return channel / DITHER_LEVEL + 1;
    return channel / DITHER_LEVEL + (int) (channel % DITHER_LEVEL > (idx + frame) % DITHER_LEVEL);

  
}
int count = 0;
void breathe(double r, double g, double b){   
    for(int i = 0; i < smoothness_pts; ++i){
      int red = (int) (r * cached_brightness[i]);
      int green = (int) (g * cached_brightness[i]);
      int blue = (int) (b * cached_brightness[i]);
//      
      for(int r = 0; r < ROW; r++){
        for(int c = 0; c < COL; c++){
          dithering(getIdx(r, c), red, green, blue);
        }
      }
      FastLED.show();
//      delay(100);
      if ((actual_frame + 1) % DITHER_LEVEL == 0){
        count++;
        if (count % 1000 == 0){
          count = 0;
          swap(&frame_array[random(0, DITHER_LEVEL)], &frame_array[random(0, DITHER_LEVEL)]);  
        }
        
//        randomize(frame_array, DITHER_LEVEL);
      }
//      delay(100);
      actual_frame = (actual_frame + 1) % DITHER_LEVEL;
      frame = frame_array[actual_frame];
  }
}

void randomize(int arr[], int n) {
  
  for (int i = n-1; i > 0; i--)
    {
        int j = random(0, n);
        swap(&arr[i], &arr[j]);
    }
}

void swap (int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}


void setup() {
  FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
  Serial.begin(115200);
  randomSeed(analogRead(0));
  for (int i = 0; i < DITHER_LEVEL; i++) {
    frame_array[i] = i;
  }
  randomize(frame_array, DITHER_LEVEL);
  
//  for (int i = 0; i < SMOOTHNESS; ++i){
//    cached_brightness[i] = (int) 255.0* DITHER_LEVEL*(exp(-(pow(((i/smoothness_pts)-beta)/gam,2.0))/2.0));
//  }
  //gamma correction
  for (int i = 0; i < SMOOTHNESS; ++i){
//    cached_brightness[i] = (int) (255.0 * DITHER_LEVEL * pow(cached_brightness[i] / (255.0 * DITHER_LEVEL), 1.0 / color_gamma_correction));
      cached_brightness[i] = (int) (255.0 * DITHER_LEVEL * pow(255.0* DITHER_LEVEL*(exp(-(pow(((i/smoothness_pts)-beta)/gam,2.0))/2.0)) / (255.0 * DITHER_LEVEL), 1.0 / color_gamma_correction));
  }

  
//  for (int i = 0; i < SMOOTHNESS; ++i){
//    cached_brightness2[i] = (int) 255.0* DITHER_LEVEL*(exp(-(pow(((i/smoothness_pts)-beta)/gam2,2.0))/2.0));
//  }
}
void loop() {
  double rand1 = random(4000, 30000) / 100000.0;
  double rand2 = random(1500, 20000) / 100000.0;
  double rand3 = random(500, 10000) / 100000.0;
  double high = max(rand1, max(rand2, rand3));
  double low = min(rand1, min(rand2, rand3));
  double med = max(min(rand1, rand2), min(rand2, rand3));
//  
  double r = high;
  double g = med;
  double b = low;
//    double r = 0.12;
//    double g = 0.03;
//    double b = 0.00;
    
//  Serial.print(r);
  breathe(r,g,b);
  
}
