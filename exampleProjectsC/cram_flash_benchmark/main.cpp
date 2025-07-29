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

#include <chrono>
#include <utility>
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "boards.h"
#include "ice_cram.h"
#include "ice_fpga.h"
#include "ice_led.h"
#include "pico/time.h"

// pico-ice-sdk
#include "ice_usb.h"

// At compiletime, the CMake will handle generating the .bin file and concatenate it in here
uint8_t bitstream[] = {
#include "bitstream.h"
};

// actual definition is handled by preprocessor flags (-DPICO_BOARD=pico2_ice)
// This makes vs code linting not tweak out though which is nice
#ifndef FPGA_DATA
#define FPGA_DATA pico2_fpga
#endif

template <typename F, typename... Args> // creastes a function type template
int funcTimeNanoseconds(F func, Args &&...args)
{
    auto t1 = std::chrono::high_resolution_clock::now();
    func(std::forward<Args>(args)...);
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
    return static_cast<int>(duration.count());
}

struct FlashTimePacket
{
    int initTime;
    int startTime;
    int openTime;
    int writeTime;
    int closeTime;
};

FlashTimePacket benchmarkFlashTime(const uint8_t *bitstream, uint32_t size)
{
    FlashTimePacket flashTimeResults;
    flashTimeResults.initTime = funcTimeNanoseconds(ice_fpga_init, FPGA_DATA, 48);
    flashTimeResults.startTime = funcTimeNanoseconds(ice_fpga_start, FPGA_DATA);
    flashTimeResults.openTime = funcTimeNanoseconds(ice_cram_open, FPGA_DATA);
    flashTimeResults.writeTime = funcTimeNanoseconds(ice_cram_write, bitstream, size);
    flashTimeResults.closeTime = funcTimeNanoseconds(ice_cram_close);
    return flashTimeResults;
}

int checkTotalFlashTime(const uint8_t *bitstream, uint32_t size)
{
    auto t1 = std::chrono::high_resolution_clock::now();
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

    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
    return static_cast<int>(duration.count());
}

int main(void)
{
    // Configure the piping as configured in <tusb_config.h>
    ice_usb_init();
    ice_led_init();
    stdio_init_all();

    ice_led_red(true);
    while (!tud_cdc_connected())
    {
        tud_task();
        sleep_ms(10);
    }
    ice_led_red(false);
    tud_cdc_write_str("Connected :)\r\n");

    auto greenState = true;
    ice_led_green(greenState);

    // Write the whole bitstream to the FPGA CRAM
    auto flashTime = benchmarkFlashTime(bitstream, sizeof(bitstream));
    printf("Attempted Flash, timing data: \r\n"
           "initTime (ns)   : %d\r\n"
           "startTime (ns)  : %d\r\n"
           "openTime (ns)   : %d\r\n"
           "writeTime (ns)  : %d\r\n"
           "closeTime (ns)  : %d\r\n ",
           flashTime.initTime, flashTime.startTime, flashTime.openTime, flashTime.writeTime, flashTime.closeTime);

    auto startTime = get_absolute_time();
    while (1)
    {
        tud_task();
        if (absolute_time_diff_us(startTime, get_absolute_time()) > 1'000'000)
        {

            if (tud_cdc_connected())
            {
                greenState = !greenState;
                ice_led_green(greenState);
                ice_led_red(false);
                tud_cdc_write_str("Hello from CDC!\r\n");
                auto flashStartTime = get_absolute_time();
                auto totalFlashTime = checkTotalFlashTime(bitstream, sizeof(bitstream));
                auto flashTimeDuration_us = absolute_time_diff_us(flashStartTime, get_absolute_time());
                if (totalFlashTime < 0)
                {
                    printf("Error while attempting to flash repeatedly\r\n");
                }
                else
                {
                    printf("Cyclic flashing total time (us) : %d\r\n", totalFlashTime);
                    printf("    Measured with picoSDK (us)  : %lld\r\n", flashTimeDuration_us);
                }
                tud_cdc_write_flush();
            }
            else
            {
                greenState = false;
                ice_led_green(greenState);
                ice_led_red(true);
                printf("Disconnected from cdc");
            }
            startTime = get_absolute_time();
        }
    }
    return 0;
}