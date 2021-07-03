#include <constants.h>
#include <Arduino.h>
#include <Classes/SmoothColor.h>
#include <FastLED.h>
#include "dithering.h"

class Breath {
private:
    double _smoothness = SMOOTHNESS;
    int _breathLength;
    static float _staticCachedBrightness[STATIC_SMOOTHNESS];
    float* _dynamicCachedBrightness;
    int _dynamicLength = SMOOTHNESS - STATIC_SMOOTHNESS;
    double _colorGammaCorrection = 1.5;
    double _gam = 0.13; // affects the width of peak (more or less darkness)
    double _beta = 0.5; // shifts the gaussian to be symmetric
    SmoothColor oldColor = SmoothColor(0.0);
    inline float computeBrightnessVal(int step, double total_steps) {
        return (255.0 * DITHER_LEVEL * pow((exp(-(pow(((((double)step) / total_steps) - _beta) / _gam, 2.0)) / 2.0)), 1.0 / _colorGammaCorrection));
    }
    void breathIteration(SmoothColor color, int arrayLen, float* brightnessArray, int offset);
public:
    Breath();
    void breathe(SmoothColor color);
    void turnOff();

};