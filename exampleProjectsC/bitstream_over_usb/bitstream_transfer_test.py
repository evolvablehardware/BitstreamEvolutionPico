import serial
import time
import os
import threading
import sys

# ==============================
# Configuration
# ==============================
PORT = "/dev/ttyACM0"   # Change to COMx on Windows
BAUD = 115200            # ignored by TinyUSB but needed by pyserial
FILENAME = "blinky_bitstream.bin"
CHUNK_SIZE = 512         # bytes per write
INTER_CHUNK_DELAY = 0.00001  # seconds
SHOW_RX = True           # print incoming text from device
# ==============================

def read_from_device(ser: serial.Serial):
    """Thread that continuously reads from the Pico and prints output."""
    while ser.is_open:
        try:
            data = ser.read(ser.in_waiting or 1)
            if data:
                # Print raw text to stdout (decode safely)
                sys.stdout.write(data.decode(errors='replace'))
                sys.stdout.flush()
        except serial.SerialException:
            break
        except OSError:
            break

def main():
    if not os.path.exists(FILENAME):
        print(f"Error: file '{FILENAME}' not found.")
        return

    # Read entire binary file
    with open(FILENAME, "rb") as f:
        data = f.read()
    data_len = len(data)
    print(f"File size: {data_len} bytes")

    # Open serial connection
    ser = serial.Serial(PORT, BAUD, timeout=0.1)
    time.sleep(2)  # give Pico time to enumerate and reset

    # Start background thread for reading device output
    if SHOW_RX:
        reader_thread = threading.Thread(target=read_from_device, args=(ser,), daemon=True)
        reader_thread.start()

    print("Starting transfer...")
    # Send the file in chunks
    for i in range(0, data_len, CHUNK_SIZE):
        chunk = data[i:i+CHUNK_SIZE]
        ser.write(chunk)
        ser.flush()
        time.sleep(INTER_CHUNK_DELAY)

        # optional progress bar
        progress = (i + len(chunk)) / data_len * 100
        print(f"\rProgress: {progress:6.2f}%", end="", flush=True)

    print("\nTransfer complete. Waiting for device response...")

    # Give Pico time to finish responding
    time.sleep(2)

    ser.close()
    print("Closed serial port.")

if __name__ == "__main__":
    main()

