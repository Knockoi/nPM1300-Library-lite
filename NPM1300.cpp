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
    // 寫入 1 到 BUCK1ENASET 暫存器來啟動 BUCK1
    return writeRegister(NPM1300_BUCK1_ENASET, 0x01);
}

bool NPM1300::enableBuck2() {
    // 寫入 1 到 BUCK2ENASET 暫存器來啟動 BUCK2
    return writeRegister(NPM1300_BUCK2_ENASET, 0x01);
}

bool NPM1300::disableBuck1() {
    // 寫入 1 到 BUCK1ENACLR 暫存器來關閉 BUCK1
    return writeRegister(NPM1300_BUCK1_ENACLR, 0x01);
}

bool NPM1300::disableBuck2() {
    // 寫入 1 到 BUCK2ENACLR 暫存器來關閉 BUCK2
    return writeRegister(NPM1300_BUCK2_ENACLR, 0x01);
}

bool NPM1300::setBuck1Voltage(float voltage) {
    uint8_t regValue = voltageToRegValue(voltage);
    
    // 寫入 BUCK1 Normal mode 電壓暫存器
    if (!writeRegister(NPM1300_BUCK1_NORMVOUT, regValue)) {
        return false;
    }
    
    // 必須設定 BUCKSWCTRLSEL 讓軟體控制生效
    // Bit 0: BUCK1SWCTRLSEL = 1 (允許軟體覆蓋 VSET pin)
    uint8_t swCtrl;
    if (readRegister(NPM1300_BUCKSWCTRLSEL, &swCtrl)) {
        swCtrl |= 0x01;  // 設定 BUCK1 軟體控制
        return writeRegister(NPM1300_BUCKSWCTRLSEL, swCtrl);
    }
    
    return false;
}

bool NPM1300::setBuck2Voltage(float voltage) {
    uint8_t regValue = voltageToRegValue(voltage);
    
    // 寫入 BUCK2 Normal mode 電壓暫存器
    if (!writeRegister(NPM1300_BUCK2_NORMVOUT, regValue)) {
        return false;
    }
    
    // 必須設定 BUCKSWCTRLSEL 讓軟體控制生效
    // Bit 1: BUCK2SWCTRLSEL = 1 (允許軟體覆蓋 VSET pin)
    uint8_t swCtrl;
    if (readRegister(NPM1300_BUCKSWCTRLSEL, &swCtrl)) {
        swCtrl |= 0x02;  // 設定 BUCK2 軟體控制
        return writeRegister(NPM1300_BUCKSWCTRLSEL, swCtrl);
    }
    
    return false;
}

// Voltage Reading Functions
uint16_t NPM1300::getVbusVoltage() {
    // 觸發 VBUS 測量
    writeRegister(NPM1300_TASK_VBUS_MEAS, 0x01);
    delay(1);  // 等待測量完成（約 250μs）
    
    uint8_t msb, lsb;
    // VBUS 儲存在 VBAT3 暫存器
    if (readRegister(NPM1300_ADC_VBAT3_MSB, &msb) && 
        readRegister(NPM1300_ADC_GP1_LSBS, &lsb)) {
        // VBUS 使用 7.5V 量程，10-bit ADC
        uint16_t raw = (msb << 2) | ((lsb >> 6) & 0x03);
        return (raw * 7500UL) / 1024;  // 轉換為 mV
    }
    return 0;
}

uint16_t NPM1300::getVbatVoltage() {
    // 觸發 VBAT 測量
    writeRegister(NPM1300_TASK_VBAT_MEAS, 0x01);
    delay(1);  // 等待測量完成（約 250μs）
    
    uint8_t msb, lsb;
    if (readRegister(NPM1300_ADC_VBAT_MSB, &msb) && 
        readRegister(NPM1300_ADC_GP0_LSBS, &lsb)) {
        // VBAT 使用 5.0V 量程，10-bit ADC
        uint16_t raw = (msb << 2) | (lsb & 0x03);
        return (raw * 5000UL) / 1024;  // 轉換為 mV
    }
    return 0;
}

uint16_t NPM1300::getVsysVoltage() {
    // 觸發 VSYS 測量
    writeRegister(NPM1300_TASK_VSYS_MEAS, 0x01);
    delay(1);  // 等待測量完成（約 250μs）
    
    uint8_t msb, lsb;
    if (readRegister(NPM1300_ADC_VSYS_MSB, &msb) && 
        readRegister(NPM1300_ADC_GP0_LSBS, &lsb)) {
        // VSYS 使用 6.375V 量程，10-bit ADC
        uint16_t raw = (msb << 2) | ((lsb >> 6) & 0x03);
        return (raw * 6375UL) / 1024;  // 轉換為 mV
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
    // 注意：NPM1300 沒有內建 SOC 計算
    // 需要配合 nRF Connect SDK 的 fuel gauge 算法
    // 這裡僅提供基於電壓的簡單估算
    uint16_t vbat = getVbatVoltage();
    
    // 簡單的 Li-ion 電壓到 SOC 映射 (3.0V-4.2V)
    if (vbat >= 4200) return 100;
    if (vbat <= 3000) return 0;
    
    return (uint8_t)((vbat - 3000) * 100 / 1200);
}

bool NPM1300::isCharging() {
    uint8_t status;
    if (readRegister(NPM1300_BCHG_CHARGESTATUS, &status)) {
        // Bit 2: TRICKLECHARGE
        // Bit 3: CONSTANTCURRENT
        // Bit 4: CONSTANTVOLTAGE
        return (status & 0x1C) != 0;  // 任一充電模式啟動
    }
    return false;
}

bool NPM1300::isBatteryFull() {
    uint8_t status;
    if (readRegister(NPM1300_BCHG_CHARGESTATUS, &status)) {
        // Bit 1: COMPLETED (Battery Full)
        return (status & 0x02) != 0;
    }
    return false;
}

// Charger Control Functions
bool NPM1300::enableCharger() {
    // Bit 0: ENABLECHARGING - 寫 1 啟動充電
    return writeRegister(NPM1300_BCHG_ENABLESET, 0x01);
}

bool NPM1300::disableCharger() {
    // 寫 1 關閉充電
    return writeRegister(NPM1300_BCHG_ENABLECLR, 0x01);
}

bool NPM1300::setChargeCurrent(uint16_t currentMa) {
    // 根據說明書 6.2.4，充電電流範圍 32-800 mA，步進 2 mA
    // 公式：ISETMSB = ICHG / 2 (整數部分)
    //       ISETLSB = ICHG % 2
    
    if (currentMa < 32) currentMa = 32;
    if (currentMa > 800) currentMa = 800;
    
    // 必須先關閉充電器
    disableCharger();
    delay(10);
    
    // 計算暫存器值
    uint8_t msb = currentMa / 2;
    uint8_t lsb = currentMa % 2;
    
    // 寫入充電電流設定
    if (!writeRegister(NPM1300_BCHG_ISETMSB, msb)) {
        return false;
    }
    if (!writeRegister(NPM1300_BCHG_ISETLSB, lsb)) {
        return false;
    }
    
    // 重新啟動充電器
    return enableCharger();
}

bool NPM1300::setChargeVoltage(float voltage) {
    // 根據說明書 6.2.14.12 BCHGVTERM
    // 範圍：3.50V - 4.45V，步進 50mV
    // 也支援 3.55V, 3.60V, 3.65V
    
    uint8_t regValue = 0;
    
    // 轉換電壓到暫存器值
    if (voltage >= 3.50 && voltage < 3.525) regValue = 0;       // 3.50V
    else if (voltage >= 3.525 && voltage < 3.575) regValue = 1; // 3.55V
    else if (voltage >= 3.575 && voltage < 3.625) regValue = 2; // 3.60V
    else if (voltage >= 3.625 && voltage < 3.75) regValue = 3;  // 3.65V
    else if (voltage >= 3.75 && voltage < 4.025) regValue = 4;  // 4.00V
    else if (voltage >= 4.025 && voltage < 4.075) regValue = 5; // 4.05V
    else if (voltage >= 4.075 && voltage < 4.125) regValue = 6; // 4.10V
    else if (voltage >= 4.125 && voltage < 4.175) regValue = 7; // 4.15V
    else if (voltage >= 4.175 && voltage < 4.225) regValue = 8; // 4.20V
    else if (voltage >= 4.225 && voltage < 4.275) regValue = 9; // 4.25V
    else if (voltage >= 4.275 && voltage < 4.325) regValue = 10;// 4.30V
    else if (voltage >= 4.325 && voltage < 4.375) regValue = 11;// 4.35V
    else if (voltage >= 4.375 && voltage < 4.425) regValue = 12;// 4.40V
    else if (voltage >= 4.425) regValue = 13;                    // 4.45V
    
    return writeRegister(NPM1300_BCHG_VTERM, regValue);
}

// Private I2C Helper Functions - 使用 16-bit 位址
bool NPM1300::writeRegister(uint16_t reg, uint8_t value) {
    _i2c->beginTransmission(NPM1300_I2C_ADDR);
    _i2c->write((uint8_t)(reg >> 8));    // 高位元組 (Base address)
    _i2c->write((uint8_t)(reg & 0xFF));  // 低位元組 (Offset)
    _i2c->write(value);
    return (_i2c->endTransmission() == 0);
}

bool NPM1300::readRegister(uint16_t reg, uint8_t *value) {
    _i2c->beginTransmission(NPM1300_I2C_ADDR);
    _i2c->write((uint8_t)(reg >> 8));    // 高位元組
    _i2c->write((uint8_t)(reg & 0xFF));  // 低位元組
    if (_i2c->endTransmission() != 0) {
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
    _i2c->write((uint8_t)(reg >> 8));    // 高位元組
    _i2c->write((uint8_t)(reg & 0xFF));  // 低位元組
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
    // nPM1300 BUCK voltage: 1.0V to 3.3V with 0.1V steps
    // According to datasheet:
    // 0 = 1.0V, 1 = 1.1V, 2 = 1.2V, ... 23 = 3.3V
    if (voltage < 1.0) voltage = 1.0;
    if (voltage > 3.3) voltage = 3.3;
    
    // Calculate register value
    // (1.0V = 0, 1.1V = 1, ..., 3.3V = 23)
    uint8_t regValue = (uint8_t)((voltage - 1.0) / 0.1 + 0.5);  // +0.5 for rounding
    
    // Clamp to valid range (0-23)
    if (regValue > 23) regValue = 23;
    
    return regValue;
}

float NPM1300::regValueToVoltage(uint8_t regValue) {
    // Convert register value back to voltage
    if (regValue > 23) regValue = 23;
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

// GPIO Functions
bool NPM1300::setGpioMode(uint8_t pin, uint8_t mode) {
    if (pin > 4) return false;  // 只有 GPIO0-4
    if (mode > 9) return false; // Mode 0-9
    
    uint16_t reg = NPM1300_GPIO_MODE0 + pin;
    return writeRegister(reg, mode);
}

bool NPM1300::setGpioOutput(uint8_t pin, bool state) {
    if (pin > 4) return false;
    
    // 設定為輸出模式
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
