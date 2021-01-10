extern void initDither();
extern void frame_set_color(int red, int green, int blue);
void swap (int *a, int *b);
void randomize(int arr[], int n);
inline void dithering(int idx, int r, int g, int b);
inline int ditherSingle(int idx, int channel);
inline int getIdx(int r, int c);
void swap (int *a, int *b);
