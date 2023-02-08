#include <Classes/OrderedColorGenerator.h>

int OrderedColorGenerator::convertFloat(double value)
{
    return (int)(value * denom);
}

OrderedColorGenerator::OrderedColorGenerator(OrderedColorGenerator::Order order, double redMin, double redMax, double greenMin, double greenMax, double blueMin, double blueMax)
{
    _order = order;
    _redMin = convertFloat(redMin);
    _redMax = convertFloat(redMax);
    _greenMin = convertFloat(greenMin);
    _greenMax = convertFloat(greenMax);
    _blueMin = convertFloat(blueMin);
    _blueMax = convertFloat(blueMax);
}

SmoothColor OrderedColorGenerator::generateColor(double brightness)
{
    double rand[3];
    rand[0] = static_cast<double>(random(_redMin, _redMax)) / denom;
    rand[1] = static_cast<double>(random(_greenMin, _greenMax)) / denom;
    rand[2] = static_cast<double>(random(_blueMin, _blueMax)) / denom;
    double ordered[3];
    ordered[0] = max(rand[0], max(rand[1], rand[2]));
    ordered[1] = max(min(rand[0], rand[1]), min(rand[1], rand[2]));
    ordered[2] = min(rand[0], min(rand[1], rand[2]));
    switch (_order)
    {
    case Order::RGB:
        return SmoothColor(ordered[0], ordered[1], ordered[2], brightness);
    case Order::RBG:
        return SmoothColor(ordered[0], ordered[2], ordered[1], brightness);
    case Order::GRB:
        return SmoothColor(ordered[1], ordered[0], ordered[2], brightness);
    case Order::GBR:
        return SmoothColor(ordered[2], ordered[0], ordered[1], brightness);
    case Order::BRG:
        return SmoothColor(ordered[1], ordered[2], ordered[0], brightness);
    default:
        return SmoothColor(ordered[2], ordered[1], ordered[0], brightness);
    }
}
