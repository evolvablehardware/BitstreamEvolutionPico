import sys
from machine import Pin
import ice
import time


#Set RGB pins to High-Z state
red_led = Pin(LED_R, Pin.IN)
green_led = Pin(LED_G, Pin.IN)
blue_led = Pin(LED_B, Pin.IN)

fpga = ice.fpga(cdone=Pin(ICE_DONE), clock=Pin(ICE_CLK), creset=Pin(ICE_RST), cram_cs=Pin(ICE_SS), cram_mosi=Pin(ICE_SI), cram_sck=Pin(ICE_SCK), frequency=48)


print("Ready!")

buffer = ""

while True:
    char = sys.stdin.read(1)
    
    if char == "\r" or char =="\n":
        print()
        print("Received:", buffer)
        buffer = buffer.upper()
        
        if buffer in {"RAM", "R"}:
            start_t = time.ticks_us()
        
            load_t = time.ticks_us()
            load_dur = time.ticks_diff(load_t,start_t)
            fpga.start()

            bram_test = open("bram_test.bin","br")
            fpga.cram(bram_test)
            image1_t = time.ticks_us()
            image1_dur = time.ticks_diff(image1_t, load_t)
            
            dsp_test = open("dsp_test.bin","br")                        
            fpga.cram(dsp_test)
            image2_t = time.ticks_us()
            image2_dur = time.ticks_diff(image2_t, image1_t)
            
            spram_test = open("spram_test.bin","br")
            fpga.cram(spram_test)
            image3_t = time.ticks_us()
            image3_dur = time.ticks_diff(image3_t, image2_t)
                        
            up5k_riscv = open("up5k_riscv.bin","br")
            fpga.cram(up5k_riscv)
            image4_t = time.ticks_us()
            image4_dur = time.ticks_diff(image4_t, image3_t)
            
            print("Benchmark complete.")
            print(f"Load time: {load_dur} us")
            print(f"BRAM image time: {image1_dur} us")
            print(f"DSP image time: {image2_dur} us")
            print(f"SPRAM image time: {image3_dur} us")
            print(f"RISC-V image time: {image4_dur} us")

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