from serial import Serial

s = Serial("/dev/ttyACM0", 115200, timeout=2)

while True:
    to_send = input("Character to send: ")
    byte_to_send = bytes(to_send, "utf-8")
    s.write(byte_to_send)
    print(f"Sent: {byte_to_send}")
    print(f"Received: {s.readline()}")