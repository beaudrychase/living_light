#include <Classes/Breath.h>

Breath::Breath(){
    _ditherManager = DitherManager();
    _dynamicCachedBrightness = (float *) malloc((_dynamicLength) * sizeof(float));
    for (int i = 0; i < STATIC_SMOOTHNESS; ++i) {
        _staticCachedBrightness[i] = computeBrightnessVal(i, _smoothness);
    }
    for (int i = 0; i < _dynamicLength; ++i) {
        _dynamicCachedBrightness[i] = computeBrightnessVal(i + STATIC_SMOOTHNESS, _smoothness);
    }
}

void Breath::turnOff() {
  _ditherManager.setColor(0, 0, 0);
}

void Breath::breathIteration(SmoothColor color, int arrayLen, float* brightnessArray, int offset) {
  for (int i = 0; i < arrayLen; ++i) {
    double increasing = min((i + offset) * 3.0, _smoothness) / _smoothness;
    //    double decreasing = max((double)(smoothness_pts - (i + offset) * 3.0) / smoothness_pts, 0.0);
    double smooth_r = color.red * increasing + oldColor.red * (1.0 - increasing);
    double smooth_g = color.green * increasing + oldColor.green * (1.0 - increasing);
    double smooth_b = color.blue * increasing + oldColor.blue * (1.0 - increasing);
    // double smoothed_cached_brightness = brightnessArray[i] / (smooth_r + smooth_g + smooth_b) / 3.0;
    double smoothed_cached_brightness = brightnessArray[i];

    int red = (int) (smooth_r * smoothed_cached_brightness);
    int green = (int) (smooth_g * smoothed_cached_brightness);
    int blue = (int) (smooth_b * smoothed_cached_brightness);
    _ditherManager.setColor(red, green, blue);
  }

}

void Breath::breathe(SmoothColor color) {
  breathIteration(color, STATIC_SMOOTHNESS, _staticCachedBrightness, 0);
  breathIteration(color, _dynamicLength, _dynamicCachedBrightness, STATIC_SMOOTHNESS);
  oldColor = color;
}