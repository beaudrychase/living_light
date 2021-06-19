class SmoothColor {
public:
  double red;
  double blue;
  double green;
  SmoothColor(double brightness = 0.0);
  SmoothColor(double red, double green, double blue);
  SmoothColor(double red, double green, double blue, double brightness);
  void normalizeColor(double brightness);
};