from machine import Pin
import ice
fpga = ice.fpga(cdone=Pin(ICE_DONE), clock=Pin(ICE_CLK), creset=Pin(ICE_RST), cram_cs=Pin(ICE_SS), cram_mosi=Pin(ICE_SI), cram_sck=Pin(ICE_SCK), frequency=48)
file = open("bitstream.bin", "br")
fpga.start()
fpga.cram(file)