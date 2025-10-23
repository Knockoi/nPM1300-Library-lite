# NPM1300 Arduino Library

A lightweight Arduino library for the Nordic nPM1300 Power Management IC.

## Features

- BUCK1 and BUCK2 voltage control (1.0V - 3.3V)
- Battery voltage, current, and temperature monitoring
- State of Charge (SOC) reading
- Charging control and status
- Simple Arduino-style API
- I2C communication

## Installation

### Arduino IDE

1. Download this repository as a ZIP file
2. In Arduino IDE, go to `Sketch` > `Include Library` > `Add .ZIP Library`
3. Select the downloaded ZIP file
4. Restart Arduino IDE

### Manual Installation

1. Download this repository
2. Copy the `NPM1300` folder to your Arduino libraries directory:
   - Windows: `Documents/Arduino/libraries/`
   - Mac: `Documents/Arduino/libraries/`
   - Linux: `~/Arduino/libraries/`
3. Restart Arduino IDE

## Hardware Connection

Connect nPM1300 to Arduino via I2C:

- SDA to Arduino SDA pin
- SCL to Arduino SCL pin
- GND to Arduino GND
- Default I2C address: 0x6B

## Quick Start

```cpp
#include <NPM1300.h>

NPM1300 pmic;

void setup() {
    Serial.begin(115200);
    
    // Initialize nPM1300
    if (!pmic.begin()) {
        Serial.println("Failed to initialize NPM1300");
        while (1);
    }
    
    // Configure BUCK outputs
    pmic.writeBuck1(3.3);  // Set BUCK1 to 3.3V
    pmic.writeBuck2(1.8);  // Set BUCK2 to 1.8V
}

void loop() {
    // Read battery voltage
    uint16_t voltage = pmic.readVBAT();
    Serial.print("Battery: ");
    Serial.print(voltage);
    Serial.println(" mV");
    
    // Read battery percentage
    uint8_t soc = pmic.readSOC();
    Serial.print("SOC: ");
    Serial.print(soc);
    Serial.println(" %");
    
    delay(1000);
}
```

## API Reference

### Initialization

```cpp
bool begin()
```
Initialize the nPM1300. Automatically configures BUCK1 and BUCK2 to 3.3V.

Returns `true` if successful, `false` otherwise.

### Reading Functions (Simplified)

```cpp
uint16_t readVBAT()
```
Read battery voltage in millivolts (mV).

```cpp
uint16_t readVBUS()
```
Read USB input voltage in millivolts (mV).

```cpp
uint16_t readVSYS()
```
Read system voltage in millivolts (mV).

```cpp
int16_t readCurrent()
```
Read battery current in milliamperes (mA). Positive value indicates charging, negative indicates discharging.

```cpp
uint8_t readSOC()
```
Read battery State of Charge as a percentage (0-100%).

```cpp
int8_t readTemp()
```
Read battery temperature in degrees Celsius.

### Writing Functions (Simplified)

```cpp
bool writeBuck1(float voltage)
```
Set BUCK1 output voltage. Valid range: 1.0V to 3.3V.

```cpp
bool writeBuck2(float voltage)
```
Set BUCK2 output voltage. Valid range: 1.0V to 3.3V.

```cpp
bool writeChargeCurrent(uint16_t mA)
```
Set charging current in milliamperes. Maximum: 992mA.

```cpp
bool writeChargeVoltage(float voltage)
```
Set charge termination voltage.

### Control Functions

```cpp
bool enableBuck1()
bool enableBuck2()
bool disableBuck1()
bool disableBuck2()
```
Enable or disable BUCK converters.

```cpp
bool enableCharger()
bool disableCharger()
```
Enable or disable battery charging.

### Status Functions

```cpp
bool isCharging()
```
Returns `true` if battery is currently charging.

```cpp
bool isBatteryFull()
```
Returns `true` if battery is fully charged.

```cpp
bool isConnected()
```
Check if nPM1300 is connected via I2C.

## Examples

### Basic Reading

```cpp
#include <NPM1300.h>

NPM1300 pmic;

void setup() {
    Serial.begin(115200);
    pmic.begin();
}

void loop() {
    Serial.print("Voltage: ");
    Serial.print(pmic.readVBAT());
    Serial.println(" mV");
    
    Serial.print("Current: ");
    Serial.print(pmic.readCurrent());
    Serial.println(" mA");
    
    Serial.print("SOC: ");
    Serial.print(pmic.readSOC());
    Serial.println(" %");
    
    delay(2000);
}
```

### BUCK Configuration

```cpp
#include <NPM1300.h>

NPM1300 pmic;

void setup() {
    Serial.begin(115200);
    pmic.begin();
    
    // Set different voltages for BUCK1 and BUCK2
    pmic.writeBuck1(3.3);  // 3.3V for main system
    pmic.writeBuck2(1.8);  // 1.8V for sensors
    
    Serial.println("BUCK voltages configured");
}

void loop() {
    // Your code here
}
```

### Charging Control

```cpp
#include <NPM1300.h>

NPM1300 pmic;

void setup() {
    Serial.begin(115200);
    pmic.begin();
    
    // Configure charging
    pmic.writeChargeCurrent(100);   // 100mA charge current
    pmic.writeChargeVoltage(4.2);   // 4.2V termination voltage
    pmic.enableCharger();
    
    Serial.println("Charger configured");
}

void loop() {
    if (pmic.isCharging()) {
        Serial.println("Charging...");
    } else if (pmic.isBatteryFull()) {
        Serial.println("Battery full");
    } else {
        Serial.println("Not charging");
    }
    
    delay(1000);
}
```

## Advanced API

For users who prefer more descriptive function names, the library also provides:

- `getVbatVoltage()` instead of `readVBAT()`
- `getBatCurrent()` instead of `readCurrent()`
- `getBatterySOC()` instead of `readSOC()`
- `setBuck1Voltage()` instead of `writeBuck1()`
- `setChargeCurrent()` instead of `writeChargeCurrent()`

Both naming styles are fully supported.

## Technical Notes

### Register Addresses

The library uses the following default register addresses. Adjust according to your nPM1300 datasheet:

- BUCK1_VOUT_SEL: 0x20
- BUCK2_VOUT_SEL: 0x21
- VBAT_VOLTAGE: 0x32
- BAT_SOC: 0x42

### Voltage Conversion

BUCK voltage range is 1.0V to 3.3V with 0.1V steps. The conversion formula:

```
Register Value = (Voltage - 1.0) / 0.1
```

### I2C Address

Default I2C address is 0x6B. To change it, modify `NPM1300_I2C_ADDR` in `NPM1300.h`.

## Compatibility

- Arduino Uno
- Arduino Nano
- Arduino Mega
- ESP32
- ESP8266
- nRF52 series
- Any Arduino-compatible board with I2C support

## License

MIT License

## Contributing

Contributions are welcome. Please open an issue or submit a pull request.

## Support

For questions or issues, please open an issue on GitHub.

## Changelog

### Version 1.0.0
- Initial release
- BUCK1 and BUCK2 control
- Battery monitoring
- Charging control
- Simplified API
