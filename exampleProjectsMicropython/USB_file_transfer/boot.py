#This optional boot.py file can be included on device to configure stuff a single time
import sys
import usb.device
from usb.device.cdc import CDCInterface

bulk_cdc = CDCInterface()
bulk_cdc.init(timeout=0)  # zero timeout makes this non-blocking, suitable for os.dupterm()

# pass builtin_driver=True so that we get the built-in USB-CDC alongside,
# if it's available.
usb.device.get().init(bulk_cdc, builtin_driver=True)
