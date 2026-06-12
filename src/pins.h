#pragma once

// =====================================================================
// Pin map for the Seeed XIAO ESP32-C6 board.
// Single source of truth for every hardware pin used by the firmware.
// =====================================================================

// --- IMU (BNO055, I2C @ 0x29) — shares the default Wire bus ---
#define IMU_SDA D4
#define IMU_SCL D5

// --- GPS (UART) ---
#define GPS_RX D7
#define GPS_TX D6

// --- LoRa (SPI / FSPI) ---
#define LORA_NSS D1
// DIO0 is not wired to an MCU pin; RX is polled (parsePacket), so no DIO0
// interrupt is used. Pass -1 as the LoRa DIO0 argument.
#define LORA_DIO0 -1
// Hardware RST line is not wired to an MCU pin; reset is pulsed via the
// I/O extender (see EXT_LORA_RST). Pass -1 as the LoRa RST argument.
#define LORA_RST -1

// --- LED rings (FastLED, WS2812) ---
#define LED_DATA D3

// --- Battery voltage sense (ADC) ---
#define BATTERY_ADC D2

// --- PCF8574 I/O extender logical pins (drive MOSFET power gates) ---
#define EXT_GPS_POWER 0 // MOSFET that powers the GPS module
#define EXT_IMU_POWER 2 // MOSFET that powers the IMU module
#define EXT_LORA_RST 3  // LoRa reset line routed through the extender
#define EXT_BUTTON 7

// --- PCF8574 interrupt (INT) output, open-drain / active-low ---
#define EXT_INT D0 // Goes low on any extender input change (button)
