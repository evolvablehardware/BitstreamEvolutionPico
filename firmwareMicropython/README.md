# pico-ice-micropython
Micropython for pico-ice and pico2-ice from tinyVision.ai

# To build
For riscv, you may add `-DPICO_GCC_TRIPLE=riscv64-unknown-elf` to the cmake command if your 'riscv64' compiler supports rp2350/rv32.

- `git submodule update --init lib/micropython`
- `git submodule update --init lib/pico-ice-mpy-module`
- `cd lib/pico-ice-mpy-module && git submodule update --init pico-ice-sdk && cd ../..`
- `make -C lib/micropython/mpy-cross -j4`
- `make -C lib/micropython/ports/rp2 submodules`
- `cd boards/PICO_ICE`
- `mkdir build && cd build`
- `cmake -DPICO_BOARD=pico2_ice ..` or `cmake -DPICO_BOARD=pico_ice ..` or `cmake -DPICO_BOARD=pico2_ice -DPICO_PLATFORM=rp2350-riscv ..`
- `make -j8`

## Troubleshooting

- "Cannot find function pico_find_compiler_with_triples"
This is a spurious pico-sdk error, if it happens, leave and delete the build directory, close your terminal, and repeat the process from mkdir.

# To use
The API to use the FPGA is as follow:
- `ice` is the microypthon module: `import ice`
- `fpga` is the class provided by the module to manage the ICE40 FPGA `ice.fpga(...)`
- `fpga` provides the methods `stop()`, `start()`, and `cram(file)`

## How to program a bitstream to the FPGA RAM

- The Pin class is imported from the machine module to provide interfacing with the hardware.
- The module is imported
- The fpga type is configured with the approriate pins for the device, as well as the frequency (in MHz) that the FPGA will run at.
- The bitstream file is opened, in byte (as opposed to text) mode (`b`), to read (`r`).
- The fpga is brought out of reset with `fpga.start()`
- The bitstream is loaded into the FPGA using `fpga.cram(file)` with the previously opened file as argument.

### On pico-ice:

```python
from machine import Pin
import ice
fpga = ice.fpga(cdone=Pin(26), clock=Pin(24), creset=Pin(27), cram_cs=Pin(9), cram_mosi=Pin(8), cram_sck=Pin(10), frequency=48)
file = open("bitstream.bin", "br")
fpga.start()
fpga.cram(file)
```

### On pico2-ice:

```python
from machine import Pin
import ice
fpga = ice.fpga(cdone=Pin(40), clock=Pin(21), creset=Pin(31), cram_cs=Pin(5), cram_mosi=Pin(4), cram_sck=Pin(6), frequency=48)
file = open("bitstream.bin", "br")
fpga.start()
fpga.cram(file)
```

## How to program a bistream to the FPGA Flash (persistent bitstream)

### On pico-ice:

```python
from machine import Pin
import ice
file = open("bitstream.bin", "br")
flash = ice.flash(miso=Pin(8), mosi=Pin(11), sck=Pin(10), cs=Pin(9))
flash.erase(4096) # Optional
flash.write(file)
# Optional
fpga = ice.fpga(cdone=Pin(26), clock=Pin(24), creset=Pin(27), cram_cs=Pin(9), cram_mosi=Pin(8), cram_sck=Pin(10), frequency=48)
fpga.start()
```

### On pico2-ice:

```python
from machine import Pin
import ice
file = open("bitstream.bin", "br")
flash = ice.flash(miso=Pin(4), mosi=Pin(7), sck=Pin(6), cs=Pin(5))
flash.erase(4096) # Optional
flash.write(file)
# Optional
fpga = ice.fpga(cdone=Pin(40), clock=Pin(21), creset=Pin(31), cram_cs=Pin(5), cram_mosi=Pin(4), cram_sck=Pin(6), frequency=48)
fpga.start()
```
