# nPM1300-Library-lite
The nPM1300 library is a lightweight C++ library for Nordic’s nPM1300 power management IC. It offers power control, battery management, and GPIO features, and works with both Arduino and nRF SDK. This makes it easy to integrate the nPM1300 into various embedded projects.

## How it work?
### What Is I²C?

**Master (Arduino)** ←→ **Slave (nPM1300)**  
```
SDA (Data Line)
SCL (Clock Line)
```

**Two-wire communication:**
- `SDA` – Data line  
- `SCL` – Clock line  

**Master–Slave architecture:**
- Arduino = Master  
- nPM1300 = Slave  

**Address identification:**
- Each device has a unique I²C address (e.g., `0x6B`)

---

## 🔧 I²C Structure in the NPM1300 Library

### 1. Initialize I²C

```cpp
// NPM1300.h
class NPM1300 {
private:
    TwoWire *_i2c;  // Pointer to the Wire object
};

// NPM1300.cpp
NPM1300::NPM1300(TwoWire &wirePort) {
    _i2c = &wirePort;  // Store the Wire object pointer
}

bool NPM1300::begin() {
    _i2c->begin();  // Start I²C communication
    
    if (!isConnected()) {
        return false;  // Check if the device is present
    }
    
    // Additional initialization...
}
```

**Why use `TwoWire`?**
- `Wire` is the global Arduino I²C object.  
- `TwoWire` is its class type.  
- Some boards have multiple I²C ports (`Wire`, `Wire1`, `Wire2`).  
- Using a pointer allows flexible I²C selection.

---

##  Three Core I²C Operations

### 1️ Write to Register

```cpp
bool NPM1300::writeRegister(uint8_t reg, uint8_t value) {
    // Step 1: Start transmission
    _i2c->beginTransmission(NPM1300_I2C_ADDR);  // 0x6B
    
    // Step 2: Write register address
    _i2c->write(reg);     // e.g., 0x20 (BUCK1_VOUT_SEL)
    
    // Step 3: Write value
    _i2c->write(value);   // e.g., 0x17 (represents 3.3V)
    
    // Step 4: End transmission
    return (_i2c->endTransmission() == 0);  // 0 = Success
}
```

**Actual Usage:**
```cpp
pmic.writeBuck1(3.3);
 ↓
setBuck1Voltage(3.3);
 ↓
writeRegister(0x20, 0x17);
 ↓
I²C Bus: [START] → [0x6B] → [0x20] → [0x17] → [STOP]
```

---

### 2️ Read from Register

```cpp
bool NPM1300::readRegister(uint8_t reg, uint8_t *value) {
    // Step 1: Tell the device which register to read
    _i2c->beginTransmission(NPM1300_I2C_ADDR);
    _i2c->write(reg);
    if (_i2c->endTransmission() != 0) {
        return false;
    }
    
    // Step 2: Request 1 byte
    if (_i2c->requestFrom(NPM1300_I2C_ADDR, (uint8_t)1) != 1) {
        return false;
    }
    
    // Step 3: Read received data
    *value = _i2c->read();
    return true;
}
```

**Actual Usage:**
```cpp
uint8_t soc = pmic.readSOC();
 ↓
getBatterySOC();
 ↓
readRegister(0x42, &value);
 ↓
I²C Bus:
  [START] → [0x6B] → [0x42] → [STOP]
  [START] → [0x6B] → [READ DATA] → [STOP]
```

---

### 3️ Read Multiple Registers

```cpp
bool NPM1300::readRegisters(uint8_t reg, uint8_t *buffer, uint8_t len) {
    // Step 1: Specify starting register
    _i2c->beginTransmission(NPM1300_I2C_ADDR);
    _i2c->write(reg);
    if (_i2c->endTransmission() != 0) {
        return false;
    }
    
    // Step 2: Request multiple bytes
    if (_i2c->requestFrom(NPM1300_I2C_ADDR, len) != len) {
        return false;
    }
    
    // Step 3: Read in a loop
    for (uint8_t i = 0; i < len; i++) {
        buffer[i] = _i2c->read();
    }
    return true;
}
```

**Actual Usage:**
```cpp
uint16_t voltage = pmic.readVBAT();
 ↓
getVbatVoltage();
 ↓
readRegisters(0x32, buffer, 2);
 ↓
voltage = (buffer[0] << 8) | buffer[1];
```

---

##  Full Process Example

### Setting BUCK1 Voltage to 3.3V

```cpp
// User code
pmic.writeBuck1(3.3);

// Layer 1: User-facing wrapper
bool NPM1300::writeBuck1(float voltage) {
    return setBuck1Voltage(voltage);
}

// Layer 2: Convert voltage to register value
bool NPM1300::setBuck1Voltage(float voltage) {
    uint8_t regValue = voltageToRegValue(voltage);
    return writeRegister(NPM1300_BUCK1_VOUT_SEL, regValue);
}

// Layer 3: Conversion function
uint8_t NPM1300::voltageToRegValue(float voltage) {
    if (voltage < 1.0) voltage = 1.0;
    if (voltage > 3.3) voltage = 3.3;
    return (uint8_t)((voltage - 1.0) / 0.1);  // e.g., 3.3V → 0x17
}

// Layer 4: I²C Write
bool NPM1300::writeRegister(uint8_t reg, uint8_t value) {
    _i2c->beginTransmission(0x6B);
    _i2c->write(0x20);  // BUCK1_VOUT_SEL
    _i2c->write(0x17);  // Voltage value
    return (_i2c->endTransmission() == 0);
}
```

**Actual I²C Bus Signal:**
```
START → 0x6B (Address) → 0x20 (Register) → 0x17 (Value) → STOP
```

---

##  Reading Battery Voltage Example

```cpp
// User code
uint16_t voltage = pmic.readVBAT();  // Returns 3850 mV

// Layer 1
uint16_t NPM1300::readVBAT() {
    return getVbatVoltage();
}

// Layer 2
uint16_t NPM1300::getVbatVoltage() {
    uint8_t buffer[2];
    if (readRegisters(0x32, buffer, 2)) {
        return (buffer[0] << 8) | buffer[1]; // 0x0F0A = 3850
    }
    return 0;
}

// Layer 3
bool NPM1300::readRegisters(uint8_t reg, uint8_t *buffer, uint8_t len) {
    _i2c->beginTransmission(0x6B);
    _i2c->write(0x32);
    _i2c->endTransmission();
    
    _i2c->requestFrom(0x6B, 2);
    buffer[0] = _i2c->read();
    buffer[1] = _i2c->read();
    return true;
}
```

**I²C Bus Signal:**
```
Write Phase:
  START → 0x6B+W → 0x32 → STOP
Read Phase:
  START → 0x6B+R → [0x0F] → [0x0A] → STOP
```

---

##  Key Concepts Summary

### I²C Address
```cpp
#define NPM1300_I2C_ADDR 0x6B
```
Each I²C device has a unique 7-bit address (`0x00–0x7F`).

---

### Registers
```cpp
#define NPM1300_BUCK1_VOUT_SEL  0x20  // BUCK1 voltage register
#define NPM1300_BAT_SOC         0x42  // Battery state-of-charge register
```
Registers act like internal memory locations controlling different functions.

---

### Data Size
```cpp
uint8_t   reg;     // Register address (1 byte)
uint8_t   value;   // Single data (1 byte)
uint16_t  voltage; // 16-bit data (2 bytes)
```

---

##  Practical Techniques

### 1️ Check Device Connection
```cpp
bool NPM1300::isConnected() {
    _i2c->beginTransmission(NPM1300_I2C_ADDR);
    return (_i2c->endTransmission() == 0);  // 0 = success
}
```

### 2️ Combine Two Bytes
```cpp
uint8_t high = 0x0F;
uint8_t low  = 0x0A;
uint16_t value = (high << 8) | low;  // 0x0F0A = 3850
```

### 3️ Error Handling
```cpp
bool NPM1300::readRegister(uint8_t reg, uint8_t *value) {
    _i2c->beginTransmission(NPM1300_I2C_ADDR);
    _i2c->write(reg);
    
    if (_i2c->endTransmission() != 0) {
        return false;
    }
    
    if (_i2c->requestFrom(NPM1300_I2C_ADDR, 1) != 1) {
        return false;
    }
    
    *value = _i2c->read();
    return true;
}
```

---

##  Complete Call Flow Diagram

```
User calls
    ↓
pmic.readVBAT()
    ↓
High-level wrapper → getVbatVoltage()
    ↓
Low-level I²C → readRegisters(0x32, buffer, 2)
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
Return → 3850 mV
```

---

##  Summary

### The Role of I²C in the Library

| Function | Description |
|-----------|--------------|
| **Bridge** | Connects Arduino and nPM1300 |
| **Protocol** | Defines how they communicate (address → register → value) |
| **Abstraction** | Wraps complex I²C operations into simple functions |

### Three Core API Patterns

| Type | Function |
|------|-----------|
| Write | `beginTransmission()` → `write()` → `endTransmission()` |
| Read | `write(reg)` → `requestFrom()` → `read()` |
| Data | `uint8_t` (1 byte per transfer) |

---

I²C enables structured, byte-level communication between MCU and peripheral ICs while the library encapsulates these low-level operations into clear, reusable APIs!!!
