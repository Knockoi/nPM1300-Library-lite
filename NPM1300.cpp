#include "NPM1300.h"

NPM1300::NPM1300(TwoWire &wirePort) {
    _i2c = &wirePort;
}

bool NPM1300::begin() {
    _i2c->begin();
    
    if (!isConnected()) {
        return false;
    }
    
    // 配置 BUCK1 和 BUCK2 為 3.3V
    setBuck1Voltage(3.3);
    setBuck2Voltage(3.3);
    
    // 啟動 BUCK
    enableBuck1();
    enableBuck2();
    
    return true;
}

bool NPM1300::isConnected() {
    _i2c->beginTransmission(NPM1300_I2C_ADDR);
    return (_i2c->endTransmission() == 0);
}

// ===== BUCK Control =====
bool NPM1300::enableBuck1() {
    return writeRegister(NPM1300_BUCK1_ENASET, 0x01);
}

bool NPM1300::enableBuck2() {
    return writeRegister(NPM1300_BUCK2_ENASET, 0x01);
}

bool NPM1300::disableBuck1() {
    return writeRegister(NPM1300_BUCK1_ENACLR, 0x01);
}

bool NPM1300::disableBuck2() {
    return writeRegister(NPM1300_BUCK2_ENACLR, 0x01);
}

bool NPM1300::setBuck1Voltage(float voltage) {
    uint8_t regValue = voltageToRegValue(voltage);
    
    if (!writeRegister(NPM1300_BUCK1_NORMVOUT, regValue)) {
        return false;
    }
    
    // 啟用軟體控制
    uint8_t swCtrl;
    if (readRegister(NPM1300_BUCKSWCTRLSEL, &swCtrl)) {
        swCtrl |= 0x01;
        return writeRegister(NPM1300_BUCKSWCTRLSEL, swCtrl);
    }
    
    return false;
}

bool NPM1300::setBuck2Voltage(float voltage) {
    uint8_t regValue = voltageToRegValue(voltage);
    
    if (!writeRegister(NPM1300_BUCK2_NORMVOUT, regValue)) {
        return false;
    }
    
    // 啟用軟體控制
    uint8_t swCtrl;
    if (readRegister(NPM1300_BUCKSWCTRLSEL, &swCtrl)) {
        swCtrl |= 0x02;
        return writeRegister(NPM1300_BUCKSWCTRLSEL, swCtrl);
    }
    
    return false;
}

// ===== Voltage Reading =====
uint16_t NPM1300::getVbusVoltage() {
    writeRegister(NPM1300_TASK_VBUS_MEAS, 0x01);
    delay(1);
    
    uint8_t msb, lsb;
    if (readRegister(NPM1300_ADC_VBAT3_MSB, &msb) && 
        readRegister(NPM1300_ADC_GP1_LSBS, &lsb)) {
        uint16_t raw = (msb << 2) | ((lsb >> 6) & 0x03);
        return (raw * 7500UL) / 1024;
    }
    return 0;
}

uint16_t NPM1300::getVbatVoltage() {
    writeRegister(NPM1300_TASK_VBAT_MEAS, 0x01);
    delay(1);
    
    uint8_t msb, lsb;
    if (readRegister(NPM1300_ADC_VBAT_MSB, &msb) && 
        readRegister(NPM1300_ADC_GP0_LSBS, &lsb)) {
        uint16_t raw = (msb << 2) | (lsb & 0x03);
        return (raw * 5000UL) / 1024;
    }
    return 0;
}

uint16_t NPM1300::getVsysVoltage() {
    writeRegister(NPM1300_TASK_VSYS_MEAS, 0x01);
    delay(1);
    
    uint8_t msb, lsb;
    if (readRegister(NPM1300_ADC_VSYS_MSB, &msb) && 
        readRegister(NPM1300_ADC_GP0_LSBS, &lsb)) {
        uint16_t raw = (msb << 2) | ((lsb >> 6) & 0x03);
        return (raw * 6375UL) / 1024;
    }
    return 0;
}

// ===== Battery State =====
uint8_t NPM1300::getBatterySOC() {
    uint16_t vbat = getVbatVoltage();
    
    if (vbat >= 4200) return 100;
    if (vbat <= 3000) return 0;
    
    return (uint8_t)((vbat - 3000) * 100 / 1200);
}

bool NPM1300::isCharging() {
    uint8_t status;
    if (readRegister(NPM1300_BCHG_CHARGESTATUS, &status)) {
        return (status & 0x1C) != 0;
    }
    return false;
}

bool NPM1300::isBatteryFull() {
    uint8_t status;
    if (readRegister(NPM1300_BCHG_CHARGESTATUS, &status)) {
        return (status & 0x02) != 0;
    }
    return false;
}

// ===== Charger Control =====
bool NPM1300::enableCharger() {
    return writeRegister(NPM1300_BCHG_ENABLESET, 0x01);
}

bool NPM1300::disableCharger() {
    return writeRegister(NPM1300_BCHG_ENABLECLR, 0x01);
}

bool NPM1300::setChargeCurrent(uint16_t currentMa) {
    if (currentMa < 32) currentMa = 32;
    if (currentMa > 800) currentMa = 800;
    
    disableCharger();
    delay(10);
    
    uint8_t msb = currentMa / 2;
    uint8_t lsb = currentMa % 2;
    
    if (!writeRegister(NPM1300_BCHG_ISETMSB, msb)) {
        return false;
    }
    if (!writeRegister(NPM1300_BCHG_ISETLSB, lsb)) {
        return false;
    }
    
    return enableCharger();
}

bool NPM1300::setChargeVoltage(float voltage) {
    uint8_t regValue = 0;
    
    if (voltage >= 3.50 && voltage < 3.525) regValue = 0;
    else if (voltage >= 3.525 && voltage < 3.575) regValue = 1;
    else if (voltage >= 3.575 && voltage < 3.625) regValue = 2;
    else if (voltage >= 3.625 && voltage < 3.75) regValue = 3;
    else if (voltage >= 3.75 && voltage < 4.025) regValue = 4;
    else if (voltage >= 4.025 && voltage < 4.075) regValue = 5;
    else if (voltage >= 4.075 && voltage < 4.125) regValue = 6;
    else if (voltage >= 4.125 && voltage < 4.175) regValue = 7;
    else if (voltage >= 4.175 && voltage < 4.225) regValue = 8;
    else if (voltage >= 4.225 && voltage < 4.275) regValue = 9;
    else if (voltage >= 4.275 && voltage < 4.325) regValue = 10;
    else if (voltage >= 4.325 && voltage < 4.375) regValue = 11;
    else if (voltage >= 4.375 && voltage < 4.425) regValue = 12;
    else if (voltage >= 4.425) regValue = 13;
    
    return writeRegister(NPM1300_BCHG_VTERM, regValue);
}

// ===== GPIO Control =====
bool NPM1300::setGpioMode(uint8_t pin, uint8_t mode) {
    if (pin > 4) return false;
    if (mode > 9) return false;
    
    uint16_t reg = NPM1300_GPIO_MODE0 + pin;
    return writeRegister(reg, mode);
}

bool NPM1300::setGpioOutput(uint8_t pin, bool state) {
    if (pin > 4) return false;
    
    uint8_t mode = state ? NPM1300_GPIO_OUT_LOGIC1 : NPM1300_GPIO_OUT_LOGIC0;
    return setGpioMode(pin, mode);
}

bool NPM1300::getGpioInput(uint8_t pin) {
    if (pin > 4) return false;
    
    uint8_t status;
    if (readRegister(NPM1300_GPIO_STATUS, &status)) {
        return (status & (1 << pin)) != 0;
    }
    return false;
}

bool NPM1300::setGpioPullUp(uint8_t pin, bool enable) {
    if (pin > 4) return false;
    
    uint16_t reg = NPM1300_GPIO_PUEN0 + pin;
    return writeRegister(reg, enable ? 1 : 0);
}

bool NPM1300::setGpioPullDown(uint8_t pin, bool enable) {
    if (pin > 4) return false;
    
    uint16_t reg = NPM1300_GPIO_PDEN0 + pin;
    return writeRegister(reg, enable ? 1 : 0);
}

// ===== Simplified API =====
uint16_t NPM1300::readVBUS() {
    return getVbusVoltage();
}

uint16_t NPM1300::readVBAT() {
    return getVbatVoltage();
}

uint16_t NPM1300::readVSYS() {
    return getVsysVoltage();
}

uint8_t NPM1300::readSOC() {
    return getBatterySOC();
}

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

// ===== Private I2C Functions (16-bit addressing) =====
bool NPM1300::writeRegister(uint16_t reg, uint8_t value) {
    _i2c->beginTransmission(NPM1300_I2C_ADDR);
    _i2c->write((uint8_t)(reg >> 8));    // 高位元組
    _i2c->write((uint8_t)(reg & 0xFF));  // 低位元組
    _i2c->write(value);
    return (_i2c->endTransmission() == 0);
}

bool NPM1300::readRegister(uint16_t reg, uint8_t *value) {
    _i2c->beginTransmission(NPM1300_I2C_ADDR);
    _i2c->write((uint8_t)(reg >> 8));
    _i2c->write((uint8_t)(reg & 0xFF));
    
    if (_i2c->endTransmission(false) != 0) {
        return false;
    }
    
    if (_i2c->requestFrom(NPM1300_I2C_ADDR, (uint8_t)1) != 1) {
        return false;
    }
    
    *value = _i2c->read();
    return true;
}

bool NPM1300::readRegisters(uint16_t reg, uint8_t *buffer, uint8_t len) {
    _i2c->beginTransmission(NPM1300_I2C_ADDR);
    _i2c->write((uint8_t)(reg >> 8));
    _i2c->write((uint8_t)(reg & 0xFF));
    
    if (_i2c->endTransmission(false) != 0) {
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

// ===== Conversion Functions =====
uint8_t NPM1300::voltageToRegValue(float voltage) {
    if (voltage < 1.0) voltage = 1.0;
    if (voltage > 3.3) voltage = 3.3;
    
    uint8_t regValue = (uint8_t)((voltage - 1.0) / 0.1 + 0.5);
    if (regValue > 23) regValue = 23;
    
    return regValue;
}
