#!/bin/bash

~/.platformio/penv/bin/platformio run --target upload --target monitor --environment upesy_wroom_c6 --upload-port /dev/cu.usbmodem101 --monitor-port /dev/cu.usbmodem101
