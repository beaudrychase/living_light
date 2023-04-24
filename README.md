# living_light

A meditative smart light that follows the solar cycle. While active the light 'breathes' by slowly pulsating in brightness at the rate of slow meditative breathing. The light changes color and intensity based on the sun's position, transitioning from bright blues during the day to dim amber at night.

## Platform

This firmware is designed to run on a [ESP32-WROOM-32](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf) that drives a 8x8 Tri-color [DotStar LED](https://cdn-shop.adafruit.com/product-files/2734/2734%20data.pdf) grid.

## Infrastructure

This firmware relies on the REST API defined in [sunrise-sunset-api](https://github.com/beaudrychase/sunrise-sunset-api).

