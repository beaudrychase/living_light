#include <Classes/DitherManager.h>

CRGB _leds[NUM_LEDS];

void DitherManager::randomize(int arr[], int n) {
  for (int i = n - 1; i > 0; i--)
  {
    int j = random(0, n);
    swap(&arr[i], &arr[j]);
  }
}

DitherManager::DitherManager(){
    FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, BGR>(_leds, NUM_LEDS);
    for (int i = 0; i < DITHER_LEVEL; i++) {
        _frameArray[i] = i;
    }
    randomize(_frameArray, DITHER_LEVEL);
}

inline int DitherManager::ditherSingle(int idx, int channel) {
  return channel / DITHER_LEVEL + (int) (channel % DITHER_LEVEL > (idx + _frame) % DITHER_LEVEL);
}

inline void DitherManager::dithering(int idx, int r, int g, int b) {
    _leds[idx].setRGB(
        ditherSingle((idx + _redOffset) % DITHER_LEVEL, r),
        ditherSingle((idx + _greenOffset) % DITHER_LEVEL, g),
        ditherSingle((idx + _blueOffset) % DITHER_LEVEL, b)
        );
    // _leds[idx].setRGB(1,1,1);
}



void DitherManager::swap (int *a, int *b)
{
  int temp = *a;
  *a = *b;
  *b = temp;
}

void DitherManager::setColor(int red, int green, int blue) {
    for (int i = 0; i < LED_NUM; i++) {
      dithering(i, red, green, blue);
    }
//   Serial.println(red /DITHER_LEVEL);
//  delay(1000);
//  if ((actual_frame + 1) % DITHER_LEVEL == 0) {
//    count++;
//    if (count % 100000 == 0) {
//      count = 0;
//      swap(&frame_array[random(0, DITHER_LEVEL)], &frame_array[random(0, DITHER_LEVEL)]);
//    }
//  }
  _actualFrame = (_actualFrame + 1) % DITHER_LEVEL;
  _frame = _frameArray[_actualFrame];
//   FastLED.show();
  pCur->showLeds();
}