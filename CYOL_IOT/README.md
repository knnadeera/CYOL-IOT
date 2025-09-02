# CYOL_IOT Project Structure

## Overview

This Arduino project is organized into modular components for better maintainability and reusability.

## Folder Structure

```
CYOL_IOT/
├── CYOL_IOT.ino              # Main Arduino sketch file
├── README.md                 # This documentation
└── modules/                  # All module files organized by functionality
    ├── LoRa/
    │   ├── LoRaModule.h      # LoRa communication header
    │   └── LoRaModule.cpp    # LoRa communication implementation
    ├── SoilSensor/
    │   ├── SoilSensor.h      # Soil moisture sensor header
    │   └── SoilSensor.cpp    # Soil moisture sensor implementation
    ├── EEPROM/
    │   ├── EEPROMManager.h   # EEPROM management header
    │   └── EEPROMManager.cpp # EEPROM management implementation
    ├── LEDControl/
    │   ├── LEDControl.h      # LED control header
    │   └── LEDControl.cpp    # LED control implementation
    ├── SIM7600/
    │   ├── SIM7600Module.h   # GSM/HTTP communication header
    │   └── SIM7600Module.cpp # GSM/HTTP communication implementation
    └── Binding/
        ├── BindingManager.h  # Device binding/ID management header
        └── BindingManager.cpp# Device binding/ID management implementation
```

## Module Descriptions

### LoRa Module (`modules/LoRa/`)

- Handles LoRa radio setup and configuration
- Provides functions for sending and receiving packets
- Optimized for maximum range communication

### Soil Sensor Module (`modules/SoilSensor/`)

- Reads analog soil moisture values
- Provides error handling for sensor detection
- Returns -1 for sensor errors, moisture value otherwise

### EEPROM Manager (`modules/EEPROM/`)

- Basic EEPROM read/write operations
- String storage and retrieval functions
- EEPROM clearing utilities

### LED Control (`modules/LEDControl/`)

- LED pattern management for device status indication
- Bootup sequence patterns
- General purpose LED blinking functions

### SIM7600 Module (`modules/SIM7600/`)

- GSM module communication
- HTTP POST request handling
- AT command processing
- Network connectivity management

### Binding Manager (`modules/Binding/`)

- Device ID management and storage
- LoRa device binding procedures
- Data parsing and validation
- EEPROM-based ID persistence
- JSON payload creation for API communication

## Pin Definitions

- **DO_PIN (PB3)**: Soil sensor digital output
- **AO_PIN (PA1)**: Soil sensor analog output
- **PUSH_BTN_PIN (PB5)**: Button for binding mode
- **LED_PIN (PC13)**: Status LED
- **SS (PA4)**: LoRa SPI select
- **RST (PB1)**: LoRa reset
- **DIO0 (PB0)**: LoRa interrupt
- **SIM7600_RX (PA3)**: SIM7600 receive
- **SIM7600_TX (PA2)**: SIM7600 transmit

## Usage

The main .ino file contains only the high-level program flow:

1. Hardware initialization in `setup()`
2. Main program loop with sensor reading and communication
3. Module-specific functions are called as needed

## Benefits of This Structure

- **Modularity**: Each feature is self-contained
- **Maintainability**: Easy to update individual components
- **Reusability**: Modules can be used in other projects
- **Clarity**: Main file shows program flow clearly
- **Scalability**: Easy to add new modules

## Development Notes

- All modules include proper header guards
- External dependencies are clearly defined
- Global variables are declared as `extern` in headers where needed
- Cross-module dependencies use relative paths
