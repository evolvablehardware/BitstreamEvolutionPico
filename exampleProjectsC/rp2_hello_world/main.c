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

// libc
#include <stdio.h>
#include <stdbool.h>

// pico-sdk
#include "pico/stdlib.h"
#include "boards.h"

// tinyusb
#include "tusb.h"

#include "pico/stdio.h"
#include "ice_cram.h"
#include "ice_fpga.h"
#include "ice_led.h"

#ifdef PICO_TIME_DEFAULT_ALARM_POOL_DISABLED
#undef PICO_TIME_DEFAULT_ALARM_POOL_DISABLED
#endif

uint8_t bitstream[] = {
#include "bitstream.h"
};

int LED_R = 4;
int LED_G = 5;
int LED_B = 6;
int PURPLE_OUTPUT = 7;
uint64_t last_change;

void gpio_callback(uint gpio, uint32_t events) {
    uint64_t current_time = time_us_64();
    uint64_t duration = current_time - last_change;
    if (events == GPIO_IRQ_EDGE_RISE) {
        printf("Purple turned on. Was off for %llu microseconds\r\n", duration);
    } else if (events == GPIO_IRQ_EDGE_FALL) {
        printf("Purple turned off. Was on for %llu microseconds\r\n", duration);
    }
    last_change = current_time;
}

int main(void) {
    // intit things for //printf
    tusb_init();
    stdio_init_all();

    // program fpga
    ice_led_init();
	ice_fpga_init(FPGA_DATA, 48);
    ice_fpga_start(FPGA_DATA);
    // Write the whole bitstream to the FPGA CRAM
    ice_cram_open(FPGA_DATA);
    ice_cram_write(bitstream, sizeof(bitstream));
    ice_cram_close();

    // set up pins to control leds
    gpio_init(LED_R);
    gpio_set_dir(LED_R, GPIO_OUT);
    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);
    gpio_init(LED_B);
    gpio_set_dir(LED_B, GPIO_OUT);

    // enable interrupt handler
    gpio_set_irq_enabled_with_callback(PURPLE_OUTPUT, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    // loop through different colors
    last_change = time_us_64();
    while (true) {
        tud_task();
        printf("hello world\r\n");
        gpio_put(LED_B, 0);
        sleep_ms(500);
        gpio_put(LED_G, 1);
        sleep_ms(500);
        gpio_put(LED_R, 0);
        sleep_ms(500);
        gpio_put(LED_B, 1);
        sleep_ms(500);
        gpio_put(LED_G, 0);
        sleep_ms(500);
        gpio_put(LED_R, 1);
        sleep_ms(500);
    }
    return 0;
}
