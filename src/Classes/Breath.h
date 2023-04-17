
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
    const double _gam = 0.02535; // affects the width of peak (more or less darkness)
    const double _beta = 0.5; // shifts the gaussian to be symmetric
    SmoothColor oldColor = SmoothColor(0.0);
    DitherManager _ditherManager = DitherManager();
    float const _c1 = exp(-(_beta * _beta) / (2 * _gam));
    float const _c2 = -1.0 / (2 * SMOOTHNESS * SMOOTHNESS * _gam);
    float const _c3 = _beta / (SMOOTHNESS * _gam);
    void breathIteration(SmoothColor color, int steps);

public:
    Breath();
    void breathe(SmoothColor color);
    void turnOff();
};