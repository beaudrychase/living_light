#include <Classes/OrderedColorGenerator.h>

int OrderedColorGenerator::convertFloat(double value){
    return (int) (value * denom);
}

OrderedColorGenerator::OrderedColorGenerator(Color order[3], double redMin, double redMax, double greenMin, double greenMax, double blueMin, double blueMax){
    _order[0] = order[0];
    _order[1] = order[1];
    _order[2] = order[2];
    if ((_order[0] != red && _order[1] != red && _order[2] != red) ||
        (_order[0] != green && _order[1] != green && _order[2] != green) ||
        (_order[0] != blue && _order[1] != blue && _order[2] != blue)) {
            throw 100;
        }
    _redMin = convertFloat(redMin);
    _redMax = convertFloat(redMax);
    _greenMin = convertFloat(greenMin);
    _greenMax = convertFloat(greenMax);
    _blueMin = convertFloat(blueMin);
    _blueMax = convertFloat(blueMax);
}

SmoothColor OrderedColorGenerator::generateColor(double brightness){
    int rand[3];
    rand[0] = random(_redMin, _redMax);
    rand[1] = random(_greenMin, _greenMax);
    rand[2] = random(_blueMin, _blueMax);
    int ordered[3];
    ordered[0] = max(rand[0], max(rand[1], rand[2]));
    ordered[1] = min(rand[0], min(rand[1], rand[2]));
    ordered[2] = max(min(rand[0], rand[1]), min(rand[1], rand[2]));
    double rgb[3] = {0.0,0.0,0.0};
    for(int i = 0; i < 3; i++){
        switch(_order[i]){
            case red:
                rgb[0] = (double)ordered[i] / denom;
                break;
            case green:
                rgb[1] = (double)ordered[i] / denom;
                break;
            case blue:
                rgb[2] = (double)ordered[i] / denom;
                break;
        }
    }
    return SmoothColor(rgb[0], rgb[1], rgb[2], brightness);

}