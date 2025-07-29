# NOT WORKING, DO NOT FLASH
# 
# 
# To run this example:
#
# 1. Make sure `usb-device-cdc` is installed via: mpremote mip install usb-device-cdc
#
# There's a way to include it in the overall image,
# but that will have to come later
# Modified from https://github.com/micropython/micropython-lib/blob/master/micropython/usb/examples/device/cdc_repl_example.py

import sys
import time
from boot import bulk_cdc


print("Booted, checking that new new cdc is open...")

# wait for host enumerate as a CDC device...
while not bulk_cdc.is_open():
    time.sleep_ms(100)
    print("notopen")

print("bulkcdc ready")

buffer = ""
bulk_buff = bytearray(64)
while True:
    char = sys.stdin.read(1)
    
    if char == "\r" or char =="\n":
        print()
        print("Received:", buffer)
        buffer = buffer.upper()
        
        if buffer in {"READ", "R"}:
            num_bytes_received = bulk_cdc.readinto(bulk_buff)
            if num_bytes_received:
                print("Received on bulk CDC:", bulk_buff)
            else:
                print("Attempted to read bulk CDC buffer, Buffer empty")
        elif buffer in {"WRITE", "W"}:
            bulk_cdc.write(b"Hello from the extra CDC interface!\n")
            print("Wrote to bulk CDC")
        else:
            print("""Unknown command. Valid commands:
                  (R)ead 64 bytes from bulk CDC buffer 
                  (W)rite to bulk CDC""")
        buffer = ""
    else:
        buffer += char
        print(char,end='')

# Note: This example doesn't wait for the host to access the new CDC port,
# which could be done by polling cdc.dtr, as this will block the REPL
# from resuming while this code is still executing.

