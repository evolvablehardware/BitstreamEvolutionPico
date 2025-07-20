import sys
from machine import Pin


red_led = Pin(LED_R, Pin.OUT)
green_led = Pin(LED_G, Pin.OUT)
blue_led = Pin(LED_B, Pin.OUT)

print("Ready!")

buffer = ""

while True:
    char = sys.stdin.read(1)
    
    if char == "\r" or char =="\n":
        print()
        print("Received:", buffer)
        buffer = buffer.upper()
        
        if buffer in {"RED", "R"}:
            red_led.toggle()
            print("Toggled Red LED")
        elif buffer in {"GREEN", "G"}:
            green_led.toggle()
            print("Toggled Green LED")
        elif buffer in {"BLUE", "B"}:
            blue_led.toggle()
            print("Toggled Blue LED")
        else:
            print("Unknown command. Valid commands: (R)ed, (G)reen, (B)lue")
        buffer = ""
    else:
        buffer += char
        print(char,end='')