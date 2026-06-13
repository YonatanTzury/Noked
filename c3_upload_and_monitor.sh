#!/bin/bash

~/.platformio/penv/bin/platformio run --target upload --target monitor --environment upesy_wroom --upload-port /dev/cu.usbmodem2101 --monitor-port /dev/cu.usbmodem2101
