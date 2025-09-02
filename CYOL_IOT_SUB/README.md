# CYOL_IOT_SUB Project Structure

## Overview

This Arduino project is the **SUB** (sensor node) version of the CYOL_IOT system, organized into modular components for better maintainability and reusability. This is a sensor node that connects to the main hub.

## Folder Structure

```
CYOL_IOT_SUB/
├── CYOL_IOT_SUB.ino          # Main Arduino sketch file (sensor node)
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
    └── DeviceBinding/
        ├── DeviceBinding.h   # Device ID binding/management header
        └── DeviceBinding.cpp # Device ID binding/management implementation
```

## Module Descriptions

### LoRa Module (`modules/LoRa/`)

- Handles LoRa radio setup and configuration
- Provides functions for sending and receiving packets
- Optimized for maximum range communication
- Same as main hub but configured as sensor node

### Soil Sensor Module (`modules/SoilSensor/`)

- Reads analog soil moisture values
- Provides error handling for sensor detection
- Returns -1 for sensor errors, moisture value otherwise
- Includes Serial output for debugging

### EEPROM Manager (`modules/EEPROM/`)

- Basic EEPROM read/write operations
- String storage and retrieval functions
- EEPROM clearing utilities
- Shared with main hub

### LED Control (`modules/LEDControl/`)

- LED pattern management for device status indication
- Bootup sequence patterns
- General purpose LED blinking functions
- Shared with main hub

### Device Binding (`modules/DeviceBinding/`)

- **SUB-specific**: Device ID management for sensor nodes
- Handles binding to main hub
- Single device ID storage (not array like main hub)
- Sensor data transmission with proper formatting
- Confirmation protocol for successful binding

## Key Differences from Main Hub

### Device Binding Behavior

- **Main Hub**: Manages multiple bound sensor IDs in array
- **SUB Node**: Stores single device ID assigned by hub
- **SUB Node**: Sends sensor data to hub, doesn't receive from other sensors
- **SUB Node**: Implements confirmation protocol during binding

### Data Flow

1. **Binding Mode**: Receives ID from main hub
2. **Normal Mode**: Reads sensors and transmits data to hub
3. **Confirmation**: Confirms successful binding with hub

### Sensor Data Format

Transmits data in format: `deviceID:soil=XXX,air=XXX,nValue=XXX,pValue=XXX,kValue=XXX`

## Pin Definitions

- **DO_PIN (PB3)**: Soil sensor digital output
- **AO_PIN (PA1)**: Soil sensor analog output
- **PUSH_BTN_PIN (PB5)**: Button for binding mode
- **LED_PIN (PC13)**: Status LED
- **SS (PA4)**: LoRa SPI select
- **RST (PB1)**: LoRa reset
- **DIO0 (PB0)**: LoRa interrupt

## Usage

1. **Binding Mode**: Hold button to enter binding mode and receive ID from hub
2. **Normal Operation**: Automatically reads sensors and transmits data
3. **LED Indicators**: Shows binding status and transmission activity

## Benefits of This Structure

- **Modularity**: Each feature is self-contained
- **Maintainability**: Easy to update individual components
- **Reusability**: Modules can be shared between main hub and sensor nodes
- **Clarity**: Main file shows program flow clearly
- **Scalability**: Easy to add new sensor types

## Development Notes

- All modules include proper header guards
- External dependencies are clearly defined
- Global variables are declared as `extern` in headers where needed
- Cross-module dependencies use relative paths
- Compatible with main CYOL_IOT hub system
