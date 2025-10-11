
#pragma once

enum LEDS {
    RED_LED,
    GREEN_LED,
    BLUE_LED,
    length
};

static const auto blinkSpeed_hz = 2.;
static const auto blinkPeriod_us = (long long)((1. / blinkSpeed_hz) * 1'000'000);
