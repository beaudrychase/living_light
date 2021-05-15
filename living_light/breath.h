



class SmoothColor {
public:
  double red;
  double blue;
  double green;
  SmoothColor(double brightness = 0.0);
  SmoothColor(double red, double blue, double green);
  SmoothColor(double red, double blue, double green, double brightness);
  void normalizeColor(double brightness);
  
};

extern void breathe(SmoothColor color);
extern void initBreathe();
extern void turnOff();
extern void updateBreathLength(int new_len_sec);

void breath_iteration(SmoothColor color, int array_len, float* brightness_array, int offset);
float computeBrightnessVal(int step, double total_steps);
