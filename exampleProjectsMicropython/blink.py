from machine import Pin
import time



red_led = Pin(LED_R, Pin.OUT)
green_led = Pin(LED_G, Pin.OUT)
blue_led = Pin(LED_B, Pin.OUT)


red_led.value(1)
green_led.value(1)
blue_led.value(1)


while(True):
    red_led.value(0)
    time.sleep_ms(750)
    red_led.value(1)
    green_led.value(0)
    time.sleep_ms(750)
    green_led.value(1)
    blue_led.value(0)
    time.sleep_ms(750)
    blue_led.value(1)
    
