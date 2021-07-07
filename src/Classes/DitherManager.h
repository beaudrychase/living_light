#include <FastLED.h>
#include <constants.h>


class DitherManager {
    private:
    static const int _redOffset = 0;
    static const int _greenOffset = (DITHER_LEVEL / 3);
    static const int _blueOffset = (DITHER_LEVEL / 3) * 2.0;
    int _frameArray[DITHER_LEVEL];
    

    int _frame = 0;
    int _actualFrame = 0;
    int _count = 0;

    void swap (int *a, int *b);
    void randomize(int arr[], int n);
    inline void dithering(int idx, int r, int g, int b);
    inline int ditherSingle(int idx, int channel);
    inline int getIdx(int r, int c);

    public:
    DitherManager();
    void setColor(int red, int green, int blue);
    // CRGB _leds[64];
};