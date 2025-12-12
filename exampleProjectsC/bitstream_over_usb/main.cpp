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

#include "constants.hpp"
#include "ice_usb.h"
#include "statusLEDs.hpp"
#include "flashBenchmark.hpp"

// actual definition is handled by preprocessor flags (-DPICO_BOARD=pico2_ice)
// This makes vs code linting not tweak out though which is nice
#ifndef FPGA_DATA
#define FPGA_DATA pico2_fpga
#endif

enum STATES
{
    INIT,
    WAIT_FOR_USB_CONNECTION,
    USB_CONNECTED,
    USB_DISCONNECTED,
    WAIT_FOR_BITSTREAM_TRANSFER,
    TRANSFER_BITSTREAM,
    FLASH_FPGA,
    TAKE_MEASUREMENTS,
    IDLE,
    UNKNOWN
};


struct MeasurementPacket
{
    long long measurement1;
    float measurement2;
} __attribute__((packed)); // Ensure no padding is added by the compiler


//returns true if measurement successful
bool takeMeasurements(MeasurementPacket *measurementPacket)
{
    // Just a dummy function for now
    measurementPacket->measurement1 = 42;
    measurementPacket->measurement2 = 3.14f;
    return true;
}

// NOT FOR PICO1-ice board
int main(void)
{
    absolute_time_t startTime;
    uint8_t bitstream[Constants::BITSTREAM_LENGTH_BYTES] = {};
    uint32_t numReceivedBitstreamBytes = 0;
    enum STATES currentState = INIT;
    enum STATES previousState = INIT;
    int numBytesAvailable;
    absolute_time_t bitstreamStartTime;

    while (1)
    {
        tud_task(); // tinyusb device task
        switch (currentState)
        {
        case INIT:
            status_leds_init();
            ice_usb_init();
            stdio_init_all();

            startTime = get_absolute_time();
            currentState = WAIT_FOR_USB_CONNECTION;
            status_leds_config(LED_Colors::YELLOW, 1);
            while (!tud_cdc_connected())
            {
                tud_task();
                sleep_ms(10);
            }
            tud_cdc_write_str("USB Connected :)\r\n");
            tud_cdc_n_write_flush(0);
            currentState = USB_CONNECTED;

            status_leds_config(LED_Colors::GREEN, 1);

            startTime = get_absolute_time();
            previousState = currentState;
            break;

        case USB_CONNECTED:
            // connected, turn on green led, move to wait for bitstream transfer
            if (previousState != currentState && previousState != INIT)
            {
                status_leds_config(LED_Colors::GREEN, 1);
                tud_cdc_n_write_str(0, "USB Reconnected :)\r\n");
                tud_cdc_n_write_flush(0);
                previousState = currentState;
            }
            currentState = WAIT_FOR_BITSTREAM_TRANSFER;
            break;

        case USB_DISCONNECTED:
            // usb disconnected, blink red led until reconnected
            if (!tud_cdc_connected())
            {
                if (previousState != currentState)
                {
                    status_leds_config(LED_Colors::RED, 0.5);
                    tud_cdc_n_write_str(0, "USB Disconnected :(\r\n");
                    previousState = currentState;
                }
            }
            // reconnected
            else
            {
                previousState = currentState;
                currentState = USB_CONNECTED;
            }

            break;

        case WAIT_FOR_BITSTREAM_TRANSFER:
            // enter this state once usb is connected, blink green led slowly until bitstream transfer starts
            if (previousState != currentState)
            {
                status_leds_config(LED_Colors::GREEN, 0.5);
                tud_cdc_n_write_str(0, "Waiting for bitstream transfer\r\n");
                tud_cdc_n_write_flush(0);
                previousState = currentState;
            }
            if (tud_cdc_available())
            {
                tud_cdc_n_write_str(0, "Bitstream transfer started\r\n");
                tud_cdc_n_write_flush(0);
                currentState = TRANSFER_BITSTREAM;
            }
            if (!tud_cdc_connected())
            {
                currentState = USB_DISCONNECTED;
            }
            break;

        case TRANSFER_BITSTREAM:
            // Receive bitstream

            if (previousState != currentState)
            {
                numReceivedBitstreamBytes = 0;
                bitstreamStartTime = get_absolute_time();
                status_leds_config(LED_Colors::MAGENTA, 20);

                tud_cdc_n_write_str(0, "Receiving bitstream\r\n");
                tud_cdc_n_write_flush(0);
                previousState = currentState;
            }

            if (!tud_cdc_connected())
            {
                previousState = currentState;
                currentState = USB_DISCONNECTED;
                break;
            }

            if (absolute_time_diff_us(bitstreamStartTime, get_absolute_time()) > Constants::USB_WATCHDOG_TIMEOUT_US)
            {
                char buf[64];
                snprintf(buf,
                         64,
                         "Watchdog timeout, %lu bytes received of %lu\r\n",
                         (unsigned long)numReceivedBitstreamBytes,
                         (unsigned long)Constants::BITSTREAM_LENGTH_BYTES);

                tud_cdc_n_write_str(0, buf);
                tud_cdc_n_write_flush(0);
                previousState = currentState;
                currentState = WAIT_FOR_BITSTREAM_TRANSFER;
            }

            numBytesAvailable = tud_cdc_n_available(0);
            if (numBytesAvailable > 0)
            {
                int numToRead = numBytesAvailable;
                if (numReceivedBitstreamBytes + numBytesAvailable > Constants::BITSTREAM_LENGTH_BYTES)
                {
                    numToRead = Constants::BITSTREAM_LENGTH_BYTES - numReceivedBitstreamBytes;
                }
                uint32_t count = tud_cdc_n_read(0, &bitstream[numReceivedBitstreamBytes], numToRead);
                numReceivedBitstreamBytes += count;
            }
            if (numReceivedBitstreamBytes >= Constants::BITSTREAM_LENGTH_BYTES)
            {
                char buf[64];
                snprintf(buf,
                         64,
                         "Received bitstream in %lu us :)\r\n",
                         (unsigned long)absolute_time_diff_us(bitstreamStartTime, get_absolute_time()));
                tud_cdc_n_write_str(0, buf);
                tud_cdc_n_write_flush(0);
                previousState = currentState;
                currentState = FLASH_FPGA;
            }
            break;

        case FLASH_FPGA:
            // Flash FPGA with received bitstream
            if (previousState != currentState)
            {
                status_leds_config(LED_Colors::MAGENTA, 20);
                tud_cdc_n_write_str(0, "Flashing FPGA\r\n");
                tud_cdc_n_write_flush(0);
                previousState = currentState;
            }

            {
                char buf[128];
                FlashTimePacket flashTimes = benchmarkFlashTime(bitstream, Constants::BITSTREAM_LENGTH_BYTES);
                snprintf(buf,
                         128,
                         "FPGA Flash times (us): init %lld, start %lld, open %lld, write %lld, close %lld\r\n",
                         flashTimes.initTime,
                         flashTimes.startTime,
                         flashTimes.openTime,
                         flashTimes.writeTime,
                         flashTimes.closeTime);
                tud_cdc_n_write_str(0, buf);
                tud_cdc_n_write_flush(0);
            }
            currentState = IDLE;
            break;

        case TAKE_MEASUREMENTS:
            // Take measurements with FPGA
            if (previousState != currentState)
            {
                status_leds_config(LED_Colors::CYAN, 10);
                tud_cdc_n_write_str(0, "Taking measurements\r\n");
                tud_cdc_n_write_flush(0);
                previousState = currentState;
            }

            {
                char buf[128];
                MeasurementPacket measurementPacket;
                bool success = takeMeasurements(&measurementPacket);
                snprintf(buf,
                         128,
                         "Measurement results: %lld, %f\r\n",
                         measurementPacket.measurement1,
                         measurementPacket.measurement2);
                tud_cdc_n_write_str(0, buf);
                tud_cdc_n_write_flush(0);
            }
            currentState = IDLE;
            break;

        case IDLE:
            // Chill here for now
            if (previousState != currentState)
            {
                status_leds_config(LED_Colors::WHITE, 0.5);
                tud_cdc_n_write_str(0, "IDLE\r\n");
                tud_cdc_n_write_flush(0);
                previousState = currentState;
            }
            if (!tud_cdc_connected())
            {
                previousState = currentState;
                currentState = USB_DISCONNECTED;
            }
            break;

        default:
            if (previousState != currentState)
            {
                status_leds_config(LED_Colors::RED, 1);
                startTime = get_absolute_time();
                currentState = UNKNOWN;
                previousState = currentState;
                tud_cdc_n_write_str(0, "UNKNOWN STATE, WAITING 2 SECONDS THEN ENTERING INIT\r\n");
                tud_cdc_n_write_flush(0);
            }

            if (absolute_time_diff_us(startTime, get_absolute_time()) > Constants::UNKNOWN_STATE_PING_US)
            {
                currentState = INIT;
            }

            break;
        }
    }
    return 0;
}
