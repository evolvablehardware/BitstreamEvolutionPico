# rp2_cram

Program an `rgb_blink.bin` bitstream, prior embedded into a code array (see `rgb_blink.py` and `rgb_blink.h` to the FPGA CRAM.


# Requirements 
Requires a a specialized terminal such as tio 

## install tio
apt install tio

## demo
run 

tio /dev/ttyACM0

note: (suggestion to tab complete at /de/v/ttyA to see available devices in case your differs or is not available for some reason)

## Expected Behavior
Upon running the tio command about the device should behave as follows:
blink green light every 0.5 seconds

output timing data:

```
Connected :)
Attempted Flash, timing data: 
initTime (ns)   : 37000
startTime (ns)  : 8000
openTime (ns)   : 1456000
writeTime (ns)  : 4000
closeTime (ns)  : 55000
 Hello from CDC!
Cyclic flashing total time (us) : 1414
    Measured with picoSDK (us)  : 1422
Hello from CDC!
...
repeats timing data indefinitely
```

In order to stop tio, you must use the command:

control-t q

(Control-c cannot be used as it could be interpretted as an interrupt)

Upon stopping, a red light should turn on and hold, no more blinking green light.