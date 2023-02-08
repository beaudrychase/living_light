#include "SmoothColor.h"

SmoothColor::SmoothColor(double brightness)
{
  this->red = brightness;
  this->green = brightness;
  this->blue = brightness;
}
SmoothColor::SmoothColor(double red, double green, double blue)
{
  this->red = red;
  this->green = green;
  this->blue = blue;
}

SmoothColor::SmoothColor(double red, double green, double blue, double brightness) : SmoothColor(red, green, blue)
{
  this->normalizeColor(brightness);
}
void SmoothColor::normalizeColor(double brightness)
{
  double normFactor = (red + green + blue) / 3.0;
  // red = (red * normFactor) * brightness;
  // green = (green * normFactor) * brightness;
  // blue = (blue * normFactor) * brightness;
  red = red * brightness;
  green = green * brightness;
  blue = blue * brightness;
}
