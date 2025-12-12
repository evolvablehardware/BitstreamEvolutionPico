
#pragma once
// actual definition is handled by preprocessor flags (-DPICO_BOARD=pico2_ice)
// This makes vs code linting not tweak out though which is nice
#ifndef FPGA_DATA
#define FPGA_DATA pico2_fpga
#endif
#include "pico/time.h"
#include "ice_fpga.h"
#include "ice_cram.h"

struct FlashTimePacket
{
    long long initTime;
    long long startTime;
    long long openTime;
    long long writeTime;
    long long closeTime;
};

FlashTimePacket benchmarkFlashTime(const uint8_t *bitstream, uint32_t size);
long long checkTotalFlashTime(const uint8_t *bitstream, uint32_t size);