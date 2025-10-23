# nPM1300-Library-lite
The nPM1300 library is a lightweight C++ library for Nordic’s nPM1300 power management IC. It offers power control, battery management, and GPIO features, and works with both Arduino and nRF SDK. This makes it easy to integrate the nPM1300 into various embedded projects.

## How it work?

## 1. What Is I²C?

**I²C (Inter-Integrated Circuit)** is a **two-wire serial communication protocol** commonly used to connect a **microcontroller (master)** with **peripheral ICs (slaves)** such as sensors, PMICs, or EEPROMs.

| Line | Function | Description |
|------|-----------|-------------|
| **SDA** | Serial Data | Bi-directional data line |
| **SCL** | Serial Clock | Clock signal controlled by the master |

### I²C Key Features
- Two wires only (SDA & SCL)  
- Multi-device shared bus (each has a unique 7-bit address)  
- Master–slave architecture  
- Byte-based data transmission with ACK/NACK signaling  

---

## 2. I²C Usage Inside a Library

Taking an example of a PMIC driver (e.g., **nPM1300**), a library typically uses **Arduino’s `TwoWire` class** to handle I²C communication.

### Example: Class Structure

```cpp
// NPM1300.h
class NPM1300 {
public:
    bool begin();
    uint16_t readVBAT();
    uint8_t readSOC();
private:
    TwoWire *_i2c;  // Pointer to the I2C interface
};
```

```cpp
// NPM1300.cpp
NPM1300::NPM1300(TwoWire &wirePort) {
    _i2c = &wirePort;  // Store the I2C port reference
}

bool NPM1300::begin() {
    _i2c->begin();     // Initialize I²C
    return isConnected();
}
```

---

## 3. Basic I²C Operations

Every I²C library function is ultimately built upon **three fundamental operations**:

### 3.1 Write to a Register

```cpp
bool NPM1300::writeRegister(uint8_t reg, uint8_t value) {
    _i2c->beginTransmission(NPM1300_I2C_ADDR);
    _i2c->write(reg);      // Target register address
    _i2c->write(value);    // Data to be written
    return (_i2c->endTransmission() == 0);
}
```

📡 **Bus Activity:**
```
[START] → [0x6B + Write] → [Register Addr] → [Data] → [STOP]
```

---

### 3.2 Read from a Register

```cpp
bool NPM1300::readRegister(uint8_t reg, uint8_t *value) {
    // Step 1: Specify which register to read
    _i2c->beginTransmission(NPM1300_I2C_ADDR);
    _i2c->write(reg);
    if (_i2c->endTransmission() != 0) return false;

    // Step 2: Request 1 byte of data
    if (_i2c->requestFrom(NPM1300_I2C_ADDR, (uint8_t)1) != 1) return false;

    // Step 3: Read the received byte
    *value = _i2c->read();
    return true;
}
```

📡 **Bus Activity:**
```
[START] → [0x6B + Write] → [Register Addr] → [STOP]
[RESTART] → [0x6B + Read] → [Data Byte] → [STOP]
```

---

### 3.3 Read Multiple Registers

```cpp
bool NPM1300::readRegisters(uint8_t reg, uint8_t *buffer, uint8_t len) {
    _i2c->beginTransmission(NPM1300_I2C_ADDR);
    _i2c->write(reg);
    if (_i2c->endTransmission() != 0) return false;

    if (_i2c->requestFrom(NPM1300_I2C_ADDR, len) != len) return false;

    for (uint8_t i = 0; i < len; i++) {
        buffer[i] = _i2c->read();
    }
    return true;
}
```

📡 **Example Bus Sequence:**
```
[START] → [0x6B + Write] → [0x32] → [STOP]
[RESTART] → [0x6B + Read] → [0x0F] [0x0A] → [STOP]
```

---

## 4. Practical Example – Reading Battery Voltage

```cpp
uint16_t NPM1300::readVBAT() {
    uint8_t buffer[2];
    if (readRegisters(0x32, buffer, 2)) {
        return (buffer[0] << 8) | buffer[1]; // Merge high + low bytes
    }
    return 0;
}
```

**I²C Bus Trace:**
```
[START] → 0x6B(W) → 0x32 → [STOP]
[RESTART] → 0x6B(R) → [High Byte] [Low Byte] → [STOP]
```

Result → `0x0F0A` = **3850 mV**

---

## 5. Internal Abstraction Layers

| Layer | Function | Example |
|-------|-----------|----------|
| **User API** | Simplified interface | `readVBAT()` |
| **Driver Logic** | Converts values, calculates register mappings | `voltageToRegValue()` |
| **I²C Access** | Low-level read/write implementation | `readRegister()`, `writeRegister()` |

This layered approach isolates hardware details while keeping user APIs simple and consistent.

---

## 6. Device Addressing and Registers

```cpp
#define NPM1300_I2C_ADDR            0x6B  // Device address
#define NPM1300_BUCK1_VOUT_SEL      0x20  // Voltage control register
#define NPM1300_BAT_SOC             0x42  // Battery state-of-charge
```

Each register corresponds to a **specific control or measurement parameter**.  
Consult the **datasheet** to obtain all register maps and bit fields.

---

## 7. Data Types Used in I²C Drivers

### 7.1 Fixed-Width Integer Types

| Type | Size | Range | Typical Usage |
|------|------|--------|----------------|
| `uint8_t` | 1 byte | 0–255 | I²C registers, small counters |
| `int8_t` | 1 byte | −128–127 | Signed byte data |
| `uint16_t` | 2 bytes | 0–65535 | Voltage (mV), time (ms) |
| `int16_t` | 2 bytes | −32768–32767 | Signed current values |
| `uint32_t` | 4 bytes | 0–4,294,967,295 | Timestamp, uptime |

### 7.2 Why Use `uint8_t`?

1. **Memory efficiency** — saves RAM on small MCUs.  
2. **Register matching** — I²C transfers data 1 byte at a time.  
3. **Platform consistency** — always 8 bits, unlike `int`.  
4. **Predictable data size** — critical for low-level protocols.  

Example:
```cpp
uint8_t reg = 0x20;   // Register address
uint8_t value = 0xFF; // One-byte data
```

---

## 8. Device Connectivity Check

```cpp
bool NPM1300::isConnected() {
    _i2c->beginTransmission(NPM1300_I2C_ADDR);
    return (_i2c->endTransmission() == 0); // 0 = ACK received
}
```

Used to verify that the target device is present and responding on the bus.

---

## 9. Bit and Byte Manipulation

16-bit data from two registers:
```cpp
uint8_t high = 0x0F;
uint8_t low  = 0x0A;
uint16_t value = (high << 8) | low; // Combine into 0x0F0A
```

---

## 10. Summary

| Concept | Description |
|----------|--------------|
| **I²C Address** | 7-bit identifier for each slave device |
| **Register Address** | Memory location inside the IC |
| **Data Transmission** | Byte by byte, MSB first |
| **Library Role** | Abstraction layer hiding protocol details |
| **Data Types** | Use `uint8_t`, `uint16_t` for consistency |
| **Error Checking** | Return `false` if `endTransmission()` or `requestFrom()` fails |

---

## 11. I²C Communication Flow Overview

```
User → pmic.readVBAT()
        ↓
Driver API → getVbatVoltage()
        ↓
Low-Level I²C → readRegisters(0x32, buffer, 2)
        ↓
Bus Operations:
  beginTransmission(0x6B)
  write(0x32)
  endTransmission()
  requestFrom(0x6B, 2)
  buffer[0] = read()
  buffer[1] = read()
        ↓
Data Conversion → (buffer[0] << 8) | buffer[1]
        ↓
Return Result → 3850 mV
```

---

## 12. Final Takeaways

- **I²C acts as the bridge** between MCU and peripheral ICs.  
- Libraries wrap I²C operations into **human-readable functions**.  
- Using `uint8_t` ensures **data precision and platform independence**.  
- Register-level access remains **transparent yet structured**.  
- Abstraction layers help developers **focus on function, not protocol.**
