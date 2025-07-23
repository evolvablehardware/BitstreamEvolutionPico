from machine import Pin
import ice
import gc

#necessary to maintain speed after flashes
def flash_with_collection(filename):
    with open(filename,"rb") as f:
        fpga.cram(f)
        gc.collect()

fpga = ice.fpga(cdone=Pin(ICE_DONE), 
                clock=Pin(ICE_CLK), 
                creset=Pin(ICE_RST), 
                cram_cs=Pin(ICE_SS), 
                cram_mosi=Pin(ICE_SI), 
                cram_sck=Pin(ICE_SCK), 
                frequency=48)

fpga.start()
flash_with_collection("leds.bin")