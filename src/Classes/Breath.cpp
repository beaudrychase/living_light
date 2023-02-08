#include <Classes/Breath.h>

Breath::Breath()
{
  _ditherManager = DitherManager();
  _dynamicCachedBrightness = (float *)malloc((_dynamicLength) * sizeof(float));
  for (int i = 0; i < STATIC_SMOOTHNESS; ++i)
  {
    _staticCachedBrightness[i] = computeBrightnessVal(i, _smoothness);
  }
  for (int i = 0; i < _dynamicLength; ++i)
  {
    _dynamicCachedBrightness[i] = computeBrightnessVal(i + STATIC_SMOOTHNESS, _smoothness);
  }
}

void Breath::turnOff()
{
  _ditherManager.setColor(0, 0, 0);
}

void Breath::breathIteration(SmoothColor color, int arrayLen, float *brightnessArray, int offset)
{
  for (int i = 0; i < arrayLen; ++i)
  {
    double increasing = min((i + offset) * 3.0, _smoothness) / _smoothness;
    double smooth_r = color.red * increasing + oldColor.red * (1.0 - increasing);
    double smooth_g = color.green * increasing + oldColor.green * (1.0 - increasing);
    double smooth_b = color.blue * increasing + oldColor.blue * (1.0 - increasing);
    _ditherManager.setColor(
        (int)(smooth_r * brightnessArray[i]),
        (int)(smooth_g * brightnessArray[i]),
        (int)(smooth_b * brightnessArray[i]));
  }
}

void Breath::breathe(SmoothColor color)
{
  breathIteration(color, STATIC_SMOOTHNESS, _staticCachedBrightness, 0);
  breathIteration(color, _dynamicLength, _dynamicCachedBrightness, STATIC_SMOOTHNESS);
  oldColor = color;
}