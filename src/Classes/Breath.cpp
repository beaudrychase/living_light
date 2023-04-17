#include <Classes/Breath.h>

Breath::Breath()
{
  _ditherManager = DitherManager();
}

void Breath::turnOff()
{
  _ditherManager.setColor(0, 0, 0);
}

void Breath::breathIteration(SmoothColor color, int steps)
{
  float triple_smooth = 3.0 / _smoothness;
  for (float i = 0; i < steps; i += 1.0)
  {
    float brightness = (10.0f * DITHER_LEVEL) + (245.0f * DITHER_LEVEL * _c1 * exp(_c2 * i * i + _c3 * i));
    float increasing = min(i * triple_smooth, 1.0f);
    float decreasing = 1.0f - increasing;
    int smooth_r = (color.red * increasing + oldColor.red * decreasing) * brightness;
    int smooth_g = (color.green * increasing + oldColor.green * decreasing) * brightness;
    int smooth_b = (color.blue * increasing + oldColor.blue * decreasing) * brightness;
    _ditherManager.setColor(
        smooth_r,
        smooth_g,
        smooth_b);
    _ditherManager.setColor(
        smooth_r,
        smooth_g,
        smooth_b);
    // _ditherManager.setColor(
    //     smooth_r,
    //     smooth_g,
    //     smooth_b);
  }
}

void Breath::breathe(SmoothColor color)
{
  breathIteration(color, SMOOTHNESS);
  oldColor = color;
}