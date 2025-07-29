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

#include <stdio.h>

// pico-sdk
#include "pico/stdio.h"
#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"

#include "boards.h"
#include "pico/stdlib.h"

// pico-ice-sdk
#include "ice_usb.h"
#include "ice_fpga.h"
#include "ice_cram.h"
#include "ice_led.h"

#define UART_TX_PIN 0
#define UART_RX_PIN 1

uint8_t bitstream[] = {
#include "bitstream.h"
};

int main(void) {
    // Enable the UART
    uart_init(uart0, 115200);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Configure the piping as configured in <tusb_config.h>
    ice_usb_init();

    ice_led_init();
	ice_fpga_init(FPGA_DATA, 48);
    ice_fpga_start(FPGA_DATA);

    // Write the whole bitstream to the FPGA CRAM
    ice_cram_open(FPGA_DATA);
    ice_cram_write(bitstream, sizeof(bitstream));
    ice_cram_close();

    // reset state machine
    int pin = 2;
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, 0);
    sleep_ms(100);
    gpio_put(pin, 1);
    
    while (true) {
        tud_task();
    }
    return 0;
}
