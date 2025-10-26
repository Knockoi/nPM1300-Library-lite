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

### GPIO Functions

```cpp
bool setGpioMode(uint8_t pin, uint8_t mode)
```
Set GPIO mode (pin: 0-4). Available modes:
- `NPM1300_GPIO_INPUT` (0) - Input
- `NPM1300_GPIO_OUT_LOGIC1` (8) - Output HIGH
- `NPM1300_GPIO_OUT_LOGIC0` (9) - Output LOW
- `NPM1300_GPIO_OUT_IRQ` (5) - Interrupt output
- `NPM1300_GPIO_EVENT_RISE` (3) - Rising edge event
- `NPM1300_GPIO_EVENT_FALL` (4) - Falling edge event

```cpp
bool setGpioOutput(uint8_t pin, bool state)
```
Set GPIO output HIGH or LOW.

```cpp
bool getGpioInput(uint8_t pin)
```
Read GPIO input state.

```cpp
bool setGpioPullUp(uint8_t pin, bool enable)
bool setGpioPullDown(uint8_t pin, bool enable)
```
Enable or disable pull-up/pull-down resistors.

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

### GPIO Control

```cpp
#include <NPM1300.h>

NPM1300 pmic;

void setup() {
    pmic.begin();
    
    // GPIO0 as output (LED)
    pmic.setGpioMode(0, NPM1300_GPIO_OUT_LOGIC0);
    
    // GPIO1 as input (Button) with pull-up
    pmic.setGpioMode(1, NPM1300_GPIO_INPUT);
    pmic.setGpioPullUp(1, true);
    
    // GPIO2 as interrupt output
    pmic.setGpioMode(2, NPM1300_GPIO_OUT_IRQ);
}

void loop() {
    // Read button
    bool buttonPressed = !pmic.getGpioInput(1);
    
    if (buttonPressed) {
        // Toggle LED
        static bool led = false;
        led = !led;
        pmic.setGpioOutput(0, led);
        delay(200);  // Debounce
    }
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

The library uses 16-bit register addresses (Base Address + Offset) according to the nPM1300 datasheet:

**BUCK Registers (Base: 0x0400)**
- BUCK1ENASET: 0x0400
- BUCK1NORMVOUT: 0x0408
- BUCK2NORMVOUT: 0x040A
- BUCKSWCTRLSEL: 0x040F

**Charger Registers (Base: 0x0300)**
- BCHGENABLESET: 0x0304
- BCHGISETMSB: 0x0308
- BCHGVTERM: 0x030C
- BCHGCHARGESTATUS: 0x0334

**ADC Registers (Base: 0x0500)**
- TASK_VBAT_MEAS: 0x0500
- ADC_VBAT_MSB: 0x0511
- ADC_GP0_LSBS: 0x0515

### Voltage Conversion

BUCK voltage range is 1.0V to 3.3V with 0.1V steps:
- Register value 0 = 1.0V
- Register value 23 = 3.3V
- Formula: Register Value = (Voltage - 1.0) / 0.1

### Charge Current Configuration

Charging current range: 32 mA to 800 mA with 2 mA steps
- BCHGISETMSB = Current_mA / 2
- BCHGISETLSB = Current_mA % 2

Example: 100 mA → ISETMSB = 50, ISETLSB = 0

### Charge Voltage Options

Termination voltage options (BCHGVTERM register):
- 3.50V, 3.55V, 3.60V, 3.65V
- 4.00V to 4.45V in 50mV steps

### ADC Conversion

The nPM1300 uses a 10-bit ADC with different voltage ranges:
- VBAT: (ADC_value * 5000) / 1024 mV (0-5V range)
- VBUS: (ADC_value * 7500) / 1024 mV (0-7.5V range)
- VSYS: (ADC_value * 6375) / 1024 mV (0-6.375V range)

ADC conversion time: approximately 250 microseconds

### Battery SOC

Note: The nPM1300 does not have built-in State of Charge calculation. The `readSOC()` function provides a simple voltage-based estimation. For accurate SOC measurement, use the Nordic nRF Connect SDK fuel gauge algorithm with the nPM1300.

### I2C Communication

I2C address: 0x6B (7-bit)

Register access uses 16-bit addressing:
```
[Slave Address][High Byte][Low Byte][Data]
     0x6B         0x04        0x08     0x17
```

### BUCK Control Sequence

To change BUCK voltage:
1. Write voltage value to BUCK1NORMVOUT/BUCK2NORMVOUT
2. Set BUCKSWCTRLSEL register to enable software control
3. The library handles this automatically in `writeBuck1()` and `writeBuck2()`

### GPIO Configuration

The nPM1300 has 5 GPIO pins (GPIO0-GPIO4) that can be configured for various functions:

**GPIO Modes:**
- Input (with pull-up/pull-down)
- Output (logic high/low)
- Interrupt output
- Edge detection (rising/falling)
- BUCK control
- Load switch control

**GPIO Register Base:** 0x0600

Each GPIO has individual registers for:
- Mode (offset 0x00-0x04)
- Drive strength (offset 0x05-0x09)
- Pull-up enable (offset 0x0A-0x0E)
- Pull-down enable (offset 0x0F-0x13)
- Open drain (offset 0x14-0x18)
- Debounce (offset 0x19-0x1D)

GPIO status can be read from register 0x061E.

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
