#include "flashBenchmark.hpp"

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
