#include <FastLED.h>
#include "constants.h"
#include "dithering.h"


int r_dithering_offset = 0;
int g_dithering_offset = (DITHER_LEVEL / 3);
int b_dithering_offset = (DITHER_LEVEL / 3) * 2.0;
int frame_array[DITHER_LEVEL];
CRGB leds[NUM_LEDS];

int frame = 0;
int actual_frame = 0;
int count = 0;
void randomize(int arr[], int n) {
  for (int i = n - 1; i > 0; i--)
  {
    int j = random(0, n);
    swap(&arr[i], &arr[j]);
  }
}
void initDither() {
  FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
  for (int i = 0; i < DITHER_LEVEL; i++) {
    frame_array[i] = i;
  }
  randomize(frame_array, DITHER_LEVEL);
}

inline int ditherSingle(int idx, int channel) {
  return channel / DITHER_LEVEL + (int) (channel % DITHER_LEVEL > (idx + frame) % DITHER_LEVEL);
}

inline void dithering(int idx, int r, int g, int b) {
  leds[idx].setRGB(ditherSingle((idx + r_dithering_offset) % DITHER_LEVEL, r), ditherSingle((idx + g_dithering_offset) % DITHER_LEVEL, g), ditherSingle((idx + b_dithering_offset) % DITHER_LEVEL, b));
}

inline int getIdx(int r, int c) {
  return r * COL + c;
}

void swap (int *a, int *b)
{
  int temp = *a;
  *a = *b;
  *b = temp;
}

void frame_set_color(int red, int green, int blue) {
  for (int r = 0; r < ROW; r++) {
    for (int c = 0; c < COL; c++) {
      dithering(getIdx(r, c), red, green, blue);
    }
  }
//  delay(1000);
//  if ((actual_frame + 1) % DITHER_LEVEL == 0) {
//    count++;
//    if (count % 100000 == 0) {
//      count = 0;
//      swap(&frame_array[random(0, DITHER_LEVEL)], &frame_array[random(0, DITHER_LEVEL)]);
//    }
//  }
  actual_frame = (actual_frame + 1) % DITHER_LEVEL;
  frame = frame_array[actual_frame];
}
