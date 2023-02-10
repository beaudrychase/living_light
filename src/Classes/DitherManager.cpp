#include <Classes/DitherManager.h>

CRGB _leds[NUM_LEDS];

void DitherManager::randomize(int arr[], int n)
{
  for (int i = n - 1; i > 0; i--)
  {
    int j = random(0, n);
    swap(&arr[i], &arr[j]);
  }
}

DitherManager::DitherManager()
{
  FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, BGR>(_leds, NUM_LEDS);
  for (int i = 0; i < DITHER_LEVEL; i++)
  {
    _frameArray[i] = i;
  }
  randomize(_frameArray, DITHER_LEVEL);
  for (int i = 0; i < NUM_LEDS; i++)
  {
    _idxArray[i] = i;
  }
  randomize(_idxArray, NUM_LEDS);
}

inline int DitherManager::ditherSingle(int idx, int brightness, int high_frames)
{
  return brightness + (int)(high_frames > (idx + _frame) % DITHER_LEVEL);
}

inline void DitherManager::dithering(int idx, int r, int r_high_frames, int g, int g_high_frames, int b, int b_high_frames)
{
  _leds[idx].setRGB(
      ditherSingle(_idxArray[idx], r, r_high_frames),
      ditherSingle(_idxArray[(idx + _greenOffset) % DITHER_LEVEL], g, g_high_frames),
      ditherSingle(_idxArray[(idx + _blueOffset) % DITHER_LEVEL], b, b_high_frames));
  // _leds[idx].setRGB(1,1,1);
}

void DitherManager::swap(int *a, int *b)
{
  int temp = *a;
  *a = *b;
  *b = temp;
}

void DitherManager::setColor(int red, int green, int blue)
{
  int red_base = red / DITHER_LEVEL;
  int red_high_frames = red % DITHER_LEVEL;
  int green_base = green / DITHER_LEVEL;
  int green_high_frames = green % DITHER_LEVEL;
  int blue_base = blue / DITHER_LEVEL;
  int blue_high_frames = blue % DITHER_LEVEL;
  for (int i = 0; i < NUM_LEDS; i++)
  {
    dithering(i, red_base, red_high_frames, green_base, green_high_frames, blue_base, blue_high_frames);
  }
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
  pCur->showLeds();
}