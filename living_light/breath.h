extern void breathe(double r, double g, double b);
extern void initBreathe();
extern void normalizeColor(double brightness, double &r, double &g, double &b);
extern void turnOff();
extern void updateBreathLength(int new_len_sec);

void breath_iteration(double r, double g, double b, int array_len, float* brightness_array, int offset);
float computeBrightnessVal(int step, double total_steps);
