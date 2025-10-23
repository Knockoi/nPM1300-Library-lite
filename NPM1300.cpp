#include "NPM1300.h"

NPM1300::NPM1300(TwoWire &wirePort) {
    _i2c = &wirePort;
}

bool NPM1300::begin() {
    _i2c->begin();
    
    if (!isConnected()) {
        return false;
    }
    
    // Configure BUCK1 and BUCK2 to 3.3V
    setBuck1Voltage(3.3);
    setBuck2Voltage(3.3);
    
    // Enable BUCK1 and BUCK2
    enableBuck1();
    enableBuck2();
    
    return true;
}

bool NPM1300::isConnected() {
    _i2c->beginTransmission(NPM1300_I2C_ADDR);
    return (_i2c->endTransmission() == 0);
}

// BUCK Control Functions
bool NPM1300::enableBuck1() {
    return writeRegister(NPM1300_BUCK1_EN, 0x01);
}

bool NPM1300::enableBuck2() {
    return writeRegister(NPM1300_BUCK2_EN, 0x01);
}

bool NPM1300::disableBuck1() {
    return writeRegister(NPM1300_BUCK1_EN, 0x00);
}

bool NPM1300::disableBuck2() {
    return writeRegister(NPM1300_BUCK2_EN, 0x00);
}

bool NPM1300::setBuck1Voltage(float voltage) {
    uint8_t regValue = voltageToRegValue(voltage);
    return writeRegister(NPM1300_BUCK1_VOUT_SEL, regValue);
}

bool NPM1300::setBuck2Voltage(float voltage) {
    uint8_t regValue = voltageToRegValue(voltage);
    return writeRegister(NPM1300_BUCK2_VOUT_SEL, regValue);
}

// Voltage Reading Functions
uint16_t NPM1300::getVbusVoltage() {
    uint8_t buffer[2];
    if (readRegisters(NPM1300_VBUS_VOLTAGE, buffer, 2)) {
        return (buffer[0] << 8) | buffer[1];
    }
    return 0;
}

uint16_t NPM1300::getVbatVoltage() {
    uint8_t buffer[2];
    if (readRegisters(NPM1300_VBAT_VOLTAGE, buffer, 2)) {
        return (buffer[0] << 8) | buffer[1];
    }
    return 0;
}

uint16_t NPM1300::getVsysVoltage() {
    uint8_t buffer[2];
    if (readRegisters(NPM1300_VSYS_VOLTAGE, buffer, 2)) {
        return (buffer[0] << 8) | buffer[1];
    }
    return 0;
}

// Current Reading Function
int16_t NPM1300::getBatCurrent() {
    uint8_t buffer[2];
    if (readRegisters(NPM1300_IBAT_CURRENT, buffer, 2)) {
        return (int16_t)((buffer[0] << 8) | buffer[1]);
    }
    return 0;
}

// Temperature Functions
int8_t NPM1300::getBatTemperature() {
    uint8_t value;
    if (readRegister(NPM1300_BAT_TEMP, &value)) {
        return (int8_t)value;
    }
    return 0;
}

int8_t NPM1300::getDieTemperature() {
    uint8_t value;
    if (readRegister(NPM1300_DIE_TEMP, &value)) {
        return (int8_t)value;
    }
    return 0;
}

// Battery State Functions
uint8_t NPM1300::getBatterySOC() {
    uint8_t value;
    if (readRegister(NPM1300_BAT_SOC, &value)) {
        return value;
    }
    return 0;
}

bool NPM1300::isCharging() {
    uint8_t status;
    if (readRegister(NPM1300_BATCHG_STATUS, &status)) {
        return (status & 0x01) != 0;
    }
    return false;
}

bool NPM1300::isBatteryFull() {
    uint8_t status;
    if (readRegister(NPM1300_BATCHG_STATUS, &status)) {
        return (status & 0x02) != 0;
    }
    return false;
}

// Charger Control Functions
bool NPM1300::enableCharger() {
    return writeRegister(NPM1300_CHARGER_EN, 0x01);
}

bool NPM1300::disableCharger() {
    return writeRegister(NPM1300_CHARGER_EN, 0x00);
}

bool NPM1300::setChargeCurrent(uint16_t currentMa) {
    // Convert mA to register value (example: 32mA per step)
    uint8_t regValue = currentMa / 32;
    if (regValue > 0x1F) regValue = 0x1F; // Max 5 bits
    return writeRegister(NPM1300_CHARGER_ISET, regValue);
}

bool NPM1300::setChargeVoltage(float voltage) {
    uint8_t regValue = voltageToRegValue(voltage);
    return writeRegister(NPM1300_CHARGER_VTERM, regValue);
}

// Private I2C Helper Functions
bool NPM1300::writeRegister(uint8_t reg, uint8_t value) {
    _i2c->beginTransmission(NPM1300_I2C_ADDR);
    _i2c->write(reg);
    _i2c->write(value);
    return (_i2c->endTransmission() == 0);
}

bool NPM1300::readRegister(uint8_t reg, uint8_t *value) {
    _i2c->beginTransmission(NPM1300_I2C_ADDR);
    _i2c->write(reg);
    if (_i2c->endTransmission() != 0) {
        return false;
    }
    
    if (_i2c->requestFrom(NPM1300_I2C_ADDR, (uint8_t)1) != 1) {
        return false;
    }
    
    *value = _i2c->read();
    return true;
}

bool NPM1300::readRegisters(uint8_t reg, uint8_t *buffer, uint8_t len) {
    _i2c->beginTransmission(NPM1300_I2C_ADDR);
    _i2c->write(reg);
    if (_i2c->endTransmission() != 0) {
        return false;
    }
    
    if (_i2c->requestFrom(NPM1300_I2C_ADDR, len) != len) {
        return false;
    }
    
    for (uint8_t i = 0; i < len; i++) {
        buffer[i] = _i2c->read();
    }
    return true;
}

// Voltage Conversion Functions
uint8_t NPM1300::voltageToRegValue(float voltage) {
    // nPM1300 BUCK voltage range: 1.0V to 3.3V
    // Example conversion: 0.1V steps
    if (voltage < 1.0) voltage = 1.0;
    if (voltage > 3.3) voltage = 3.3;
    
    return (uint8_t)((voltage - 1.0) / 0.1);
}

float NPM1300::regValueToVoltage(uint8_t regValue) {
    return 1.0 + (regValue * 0.1);
}

// Simplified Arduino-style Read Functions
uint16_t NPM1300::readVBUS() {
    return getVbusVoltage();
}

uint16_t NPM1300::readVBAT() {
    return getVbatVoltage();
}

uint16_t NPM1300::readVSYS() {
    return getVsysVoltage();
}

int16_t NPM1300::readCurrent() {
    return getBatCurrent();
}

uint8_t NPM1300::readSOC() {
    return getBatterySOC();
}

int8_t NPM1300::readTemp() {
    return getBatTemperature();
}

// Simplified Arduino-style Write Functions
bool NPM1300::writeBuck1(float voltage) {
    return setBuck1Voltage(voltage);
}

bool NPM1300::writeBuck2(float voltage) {
    return setBuck2Voltage(voltage);
}

bool NPM1300::writeChargeCurrent(uint16_t mA) {
    return setChargeCurrent(mA);
}

bool NPM1300::writeChargeVoltage(float voltage) {
    return setChargeVoltage(voltage);
}
