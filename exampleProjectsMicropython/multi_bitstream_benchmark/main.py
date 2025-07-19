import sys

print("Ready!")

while True:
    mesg = sys.stdin.readline().strip()
    print(f"Received {mesg}")
    
    mesg = mesg.upper()
    
    if mesg == "RED":
        print("Toggled Red LED")
    elif mesg == "BLUE":
        print("Toggled Blue LED")
    elif mesg == "GREEN":
        print("Toggled Green LED")
    else:
        print("Unknown command. Valid commands: Red, Blue, Green")