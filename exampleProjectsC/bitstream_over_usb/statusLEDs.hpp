
#pragma once
#include "pico/stdlib.h"
#include "ice_led.h"
enum class LED_Colors
{
    OFF,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    length
};

struct StatusLEDStates
{
    LED_Colors current_color = LED_Colors::OFF;
    bool startup = true;
};

static repeating_timer_t timer;

void status_leds_init();
void status_leds_stop();
bool status_led_toggle_handler(struct repeating_timer *rt);
void status_leds_config(LED_Colors color, double blinkFrequency_hz);
