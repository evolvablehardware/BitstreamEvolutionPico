/*
 * MIT License
 *
 * Copyright (c) 2023 tinyVision.ai
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "boards.h"
#include "ice_cram.h"
#include "ice_fpga.h"
#include "ice_led.h"

uint8_t bitstream[] = {
#include "bitstream.h"
};

#include "ice_usb.h"

// actual definition is handled by preprocessor flags (-DPICO_BOARD=pico2_ice)
// This makes vs code linting not tweak out though which is nice
#ifndef FPGA_DATA
#define FPGA_DATA pico2_fpga
#endif

static const auto blinkSpeed_hz = 2.;
static const auto blinkPeriod_us = (long long)((1. / blinkSpeed_hz) * 1'000'000);

// bool led_toggle_timer_callback(goober goober)
// {

// }

// enum LEDS
// {
//     RED_LED,
//     GREEN_LED,
//     BLUE_LED,
//     length
// };

// bool led_toggle(enum LEDS led)
// {
//     static bool redState = false;
//     static bool greenState = false;
//     static bool blueState = false;

//     switch (led)
//     {
//     case RED_LED:
//         ice_led_red(redState);
//         break;
//     case GREEN_LED:
//         ice_led_green(greenState);
//         break;
//     case BLUE_LED:
//         ice_led_blue(blueState);
//         break;
//     default:
//         return false;
//     }
// }

struct FlashTimePacket
{
    long long initTime;
    long long startTime;
    long long openTime;
    long long writeTime;
    long long closeTime;
};

FlashTimePacket benchmarkFlashTime(const uint8_t *bitstream, uint32_t size)
{
    FlashTimePacket flashTimeResults;
    auto t1 = get_absolute_time();
    if (ice_fpga_init(FPGA_DATA, 48) != 0)
        flashTimeResults.initTime = -1;
    else
        flashTimeResults.initTime = absolute_time_diff_us(t1, get_absolute_time());

    t1 = get_absolute_time();
    if (ice_fpga_start(FPGA_DATA) != 0)
        flashTimeResults.startTime = -1;
    else
        flashTimeResults.startTime = absolute_time_diff_us(t1, get_absolute_time());

    t1 = get_absolute_time();
    if (ice_cram_open(FPGA_DATA) != true)
        flashTimeResults.openTime = -1;
    else
        flashTimeResults.openTime = absolute_time_diff_us(t1, get_absolute_time());

    t1 = get_absolute_time();
    if (ice_cram_write(bitstream, size) != 0)
        flashTimeResults.writeTime = -1;
    else
        flashTimeResults.writeTime = absolute_time_diff_us(t1, get_absolute_time());

    t1 = get_absolute_time();
    if (ice_cram_close() != true)
        flashTimeResults.closeTime = -1;
    else
        flashTimeResults.closeTime = absolute_time_diff_us(t1, get_absolute_time());

    return flashTimeResults;
}

/**
 * @brief Runs and benchmarks the full flashing sequence
 * @param bitstream address of the bitstream
 * @param size length of bitstream
 *
 * @return time in (us) on success, negative on fail
 */
long long checkTotalFlashTime(const uint8_t *bitstream, uint32_t size)
{
    absolute_time_t t1 = get_absolute_time();
    if (ice_fpga_init(FPGA_DATA, 48) != 0)
        return -1;
    if (ice_fpga_start(FPGA_DATA) != 0)
        return -1;
    if (ice_cram_open(FPGA_DATA) != true)
        return -1;
    if (ice_cram_write(bitstream, size) != 0)
        return -1;
    if (ice_cram_close() != true)
        return -1;
    return absolute_time_diff_us(t1, get_absolute_time());
}

bool;

// NOT FOR PICO1-ice board
int main(void)
{
    absolute_time_t startTime;

    ice_led_init();
    bool green_status = true;
    bool red_status = false;
    ice_led_green(green_status);

    ice_usb_init();
    stdio_init_all();

    red_status = true;
    ice_led_red(red_status);
    startTime = get_absolute_time();
    while (!tud_cdc_connected())
    {
        tud_task();
        sleep_ms(10);
    }
    tud_cdc_write_str("Connected :)\r\n");
    red_status = false;
    ice_led_red(red_status);

    tud_cdc_n_write_str(0, "Initialized, starting FPGA Flash\r\n");
    tud_cdc_n_write_flush(0);

    long long flashDuration_us = checkTotalFlashTime(bitstream, sizeof(bitstream));
    printf("Flash time (us) : %lld\r\n", flashDuration_us);

    startTime = get_absolute_time();
    // add_repeating_timer_us(blinkPeriod_us / 2, );
    while (1)
    {
        if (absolute_time_diff_us(startTime, get_absolute_time()) > 500'000)
        {
            if (tud_cdc_connected())
            {
                green_status = !green_status;
                ice_led_green(green_status);

                flashDuration_us = checkTotalFlashTime(bitstream, sizeof(bitstream));

                auto flashTimeResults = benchmarkFlashTime(bitstream, sizeof(bitstream));

                if (flashDuration_us < 0)
                {
                    tud_cdc_write_str("Error while attempting to flash repeatedly\r\n");
                }
                else
                {

                    printf("Flash time (us) : %lld\r\n", flashDuration_us);
                    tud_cdc_n_write_flush(0);
                    printf("Alternate benchmark, timing data: \r\n"
                           "initTime (us)   : %lld\r\n"
                           "startTime (us)  : %lld\r\n"
                           "openTime (us)   : %lld\r\n"
                           "writeTime (us)  : %lld\r\n"
                           "closeTime (us)  : %lld\r\n",
                           flashTimeResults.initTime, flashTimeResults.startTime, flashTimeResults.openTime, flashTimeResults.writeTime, flashTimeResults.closeTime);
                    tud_cdc_n_write_flush(0);
                }
            }
            else
            {
                green_status = false;
                red_status = !red_status;
                ice_led_red(red_status);

            }
            startTime = get_absolute_time();
        }
    }
    return 0;
}
