#include "statusLEDs.hpp"

void status_leds_init()
{
    ice_led_init();
}

void status_leds_stop()
{
    cancel_repeating_timer(&timer);
    ice_led_red(false);
    ice_led_green(false);
    ice_led_blue(false);
}

bool _status_led_toggle_handler(struct repeating_timer *rt)
{
    static bool led_status = true;
    if (rt->user_data == nullptr)
        return false;

    StatusLEDStates StatusLEDState = *(StatusLEDStates *)(rt->user_data);
    if (StatusLEDState.startup)
    {
        led_status = false;
        StatusLEDState.startup = false;
    }

    led_status = !led_status;
    switch (StatusLEDState.current_color)
    {
    case LED_Colors::RED:
        ice_led_red(led_status);
        break;
    case LED_Colors::GREEN:
        ice_led_green(led_status);
        break;
    case LED_Colors::YELLOW:
        ice_led_red(led_status);
        ice_led_green(led_status);
        break;
    case LED_Colors::BLUE:
        ice_led_blue(led_status);
        break;
    case LED_Colors::MAGENTA:
        ice_led_red(led_status);
        ice_led_blue(led_status);
        break;
    case LED_Colors::CYAN:
        ice_led_green(led_status);
        ice_led_blue(led_status);
        break;
    case LED_Colors::WHITE:
        ice_led_red(led_status);
        ice_led_green(led_status);
        ice_led_blue(led_status);
        break;
    default:
        ice_led_green(false);
        ice_led_red(false);
        ice_led_blue(false);
        led_status = false;
        break;
    }
    return true;
}

/**
 * @brief Starts/restarts the status LED blinking with new color and frequency, current pattern is cancelled
 * @param color color of the LED
 * @param blinkFrequency_hz frequency of blinking in Hz
 *
 * @return void
 */
void status_leds_config(LED_Colors color, double blinkFrequency_hz)
{
    static StatusLEDStates StatusLEDState;
    StatusLEDState.startup = true;
    StatusLEDState.current_color = color;

    status_leds_stop();
    const auto togglePeriod_us = (int64_t)((1'000'000 / blinkFrequency_hz) / 2);

    add_repeating_timer_us(togglePeriod_us, _status_led_toggle_handler, &StatusLEDState, &timer);
}
