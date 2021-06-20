#include <FastLED.h>
#include "dithering.h"
#include <constants.h>
#include "Classes/SmoothColor.h"


extern void breathe(SmoothColor color);
extern void initBreathe();
extern void turnOff();
extern void updateBreathLength(int new_len_sec);

void breath_iteration(SmoothColor color, int array_len, float* brightness_array, int offset);
float computeBrightnessVal(int step, double total_steps);
