#include <Classes/SmoothColor.h>
#include <ISmoothColorGenerator.h>
#include <Arduino.h>
enum Color {red, green, blue};

class OrderedColorGenerator: ISmoothColorGenerator{
private:
    Color _order[3];
    int _redMin;
    int _redMax;
    int _greenMin;
    int _greenMax;
    int _blueMin;
    int _blueMax;
    int denom = __INT32_MAX__;
    int convertFloat(double value);

public:
    OrderedColorGenerator(Color order[3], double redMin, double redMax, double greenMin, double greenMax, double blueMin, double blueMax);
    SmoothColor generateColor(double brightness);
};
