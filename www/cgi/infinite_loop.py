#!/usr/bin/env python3

import time

print("Content-Type: text/plain\r")
print("\r")

# Infinite loop to test CGI timeout
while True:
    time.sleep(1)
