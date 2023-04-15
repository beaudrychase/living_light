
#include <Arduino.h>
#include <Classes/SmoothColor.h>
#include <Classes/DitherManager.h>
#include <constants.h>

class Breath
{
private:
    double _smoothness = SMOOTHNESS;
    static int _breathLength;
    /*
    Due to memory restrictions of the ESP32 the CachedBrightness array is split into two parts. The static one is defined in static memory while the dynamic
    one is dynamically allocated at runtime. Functionally they act like two halves of a single array where static + dynamic
    */
    float _staticCachedBrightness[STATIC_SMOOTHNESS];
    float *_dynamicCachedBrightness;
    const int _dynamicLength = SMOOTHNESS - STATIC_SMOOTHNESS;
    const double _colorGammaCorrection = 1.5;
    const double _gam = 0.13; // affects the width of peak (more or less darkness)
    const double _beta = 0.5; // shifts the gaussian to be symmetric
    SmoothColor oldColor = SmoothColor(0.0);
    DitherManager _ditherManager = DitherManager();
    double const _blue = exp(-(_beta * _beta) / (2 * _colorGammaCorrection * _gam * _gam));
    double const _red = -1.0 / (2 * _colorGammaCorrection * SMOOTHNESS * SMOOTHNESS * _gam * _gam);
    double const _yellow = _beta / (_colorGammaCorrection * SMOOTHNESS * _gam * _gam);
    inline float computeBrightnessVal(int step, double total_steps)
    {
            return (4 * DITHER_LEVEL) + (251.0 * DITHER_LEVEL * _blue * exp(_red * step * step + _yellow * step));
            // return (4 * DITHER_LEVEL) + (251.0 * DITHER_LEVEL * pow((exp(-(pow(((((double)step) / total_steps) - _beta) / _gam, 2.0)) / 2.0)), 1.0 / _colorGammaCorrection));
    }
    void breathIteration(SmoothColor color, int arrayLen, float *brightnessArray, int offset);

public:
    Breath();
    void breathe(SmoothColor color);
    void turnOff();
};