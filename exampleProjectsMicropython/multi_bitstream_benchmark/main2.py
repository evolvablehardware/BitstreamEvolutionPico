import sys
from machine import Pin
from machine import freq
import ice
import time
import gc

#slight overclock to saturate the bitbanged SPI
freq(180_000_000)

#Set RGB pins to High-Z state
red_led = Pin(LED_R, Pin.IN)
green_led = Pin(LED_G, Pin.IN)
blue_led = Pin(LED_B, Pin.IN)

fpga = ice.fpga(cdone=Pin(ICE_DONE), 
                clock=Pin(ICE_CLK), 
                creset=Pin(ICE_RST), 
                cram_cs=Pin(ICE_SS), 
                cram_mosi=Pin(ICE_SI), 
                cram_sck=Pin(ICE_SCK), 
                frequency=48)

print("Ready!")

buffer = ""

#implementation for testing
def flash_and_time(filename, mem_log):
    t0 = time.ticks_us()
    with open(filename, "rb") as f:
        fpga.cram(f)
        gc.collect()
    t1 = time.ticks_us()          
    mem_after = gc.mem_free()
    image_dur = time.ticks_diff(t1,t0)
    mem_log.append((filename, mem_after))
    return image_dur


def flash_with_collection(filename):
    with open(filename,"rb") as f:
        fpga.cram(f)
        gc.collect()
        

while True:
    char = sys.stdin.read(1)
    
    if char == "\r" or char =="\n":
        print()
        print("Received:", buffer)
        buffer = buffer.upper()
        
        if buffer in {"RAM", "R"}:
            fpga.start()
            start_t = time.ticks_us()
            mem_log = []

            # --- BRAM ---
            image1_dur = flash_and_time("bram_test.bin",mem_log)


            # --- DSP ---
            image2_dur = flash_and_time("dsp_test.bin",mem_log)

            # --- SPRAM ---
            image3_dur = flash_and_time("spram_test.bin",mem_log)

            # --- RISC-V repeated 5 times ---
            riscv_times = []
            for i in range(5):
                filename = f"up5k_riscv{i+1}.bin"
                riscv_times.append(flash_and_time(filename,mem_log))



            # --- Benchmark Results ---
            print("Benchmark complete.")
            print(f"BRAM image time: {image1_dur} us")
            print(f"DSP image time: {image2_dur} us")
            print(f"SPRAM image time: {image3_dur} us")
            print("RISC-V image times (5 runs):")
            for i, t in enumerate(riscv_times):
                print(f"  Run {i+1}: {t} us")
            print()
            
            # --- Memory Usage Log ---
            print("Free memory on pico after each cram flash:")
            for label, after in mem_log:
                print(f"{label}: {after}")

        elif buffer in {"F", "FLASH"}:
            print("not yet implemented")
            pass
        else:
            print("""Unknown command. 
                  Valid commands: 
                  '(R)AM': benchmarks time to flash FPGA with image from Pico memory, 
                  '(F)lash': benchmakrs time to flash FPGA from Pico spi flash""")
        buffer = ""
    else:
        buffer += char
        print(char,end='')