from serial import Serial

s = Serial("/dev/ttyACM1", 115200, timeout=2)

while True:
    s.reset_input_buffer()
    s.reset_output_buffer()
    to_send = input("Character to send: ")
    byte_to_send = bytes(to_send, "utf-8")
    s.write(byte_to_send)
    print(f"Sent: {byte_to_send}")
    print(f"Received: {s.read(1)}")
    print()