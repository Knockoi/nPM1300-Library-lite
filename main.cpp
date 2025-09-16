#include "nPM1300.h"

#ifndef ARDUINO
// TWI instance for nRF SDK environment
const nrf_drv_twi_t* nPM1300::m_twi = nullptr;
#endif

// Constructor
#ifdef ARDUINO
nPM1300::nPM1300(uint8_t addr) : _i2c_addr(addr), _initialized(false) {
}
#else
nPM1300::nPM1300(const nrf_drv_twi_t* twi_instance, uint8_t addr) : _i2c_addr(addr), _initialized(false) {
    m_twi = twi_instance;
}
#endif

// Initialization
bool nPM1300::begin() {
#ifdef ARDUINO
    Wire.begin();
    delay(100); // Wait for nPM1300 to stabilize
#else
    nrf_delay_ms(100);
#endif
    
    if (!isConnected()) {
        return false;
    }
    
    _initialized = true;
    return configureDefault();
}

// Check device connection
bool nPM1300::isConnected() {
    uint8_t status;
    return readRegister(NPM1300_MAIN_STATUS, &status);
}

// Write to register
bool nPM1300::writeRegister(uint8_t reg, uint8_t data) {
#ifdef ARDUINO
    Wire.beginTransmission(_i2c_addr);
    Wire.write(reg);
    Wire.write(data);
    return (Wire.endTransmission() == 0);
#else
    if (!m_twi) return false;
    
    uint8_t tx_data[2] = {reg, data};
    ret_code_t err_code = nrf_drv_twi_tx(m_twi, _i2c_addr, tx_data, 2, false);
    return (err_code == NRF_SUCCESS);
#endif
}

// Read a single register
bool nPM1300::readRegister(uint8_t reg, uint8_t* data) {
#ifdef ARDUINO
    Wire.beginTransmission(_i2c_addr);
    Wire.write(reg);
    if (Wire.endTransmission() != 0) return false;
    
    if (Wire.requestFrom(_i2c_addr, (uint8_t)1) != 1) return false;
    *data = Wire.read();
    return true;
#else
    if (!m_twi) return false;
    
    ret_code_t err_code = nrf_drv_twi_tx(m_twi, _i2c_addr, &reg, 1, true);
    if (err_code != NRF_SUCCESS) return false;
    
    err_code = nrf_drv_twi_rx(m_twi, _i2c_addr, data, 1);
    return (err_code == NRF_SUCCESS);
#endif
}

// Read multiple registers
bool nPM1300::readMultipleRegisters(uint8_t reg, uint8_t* data, uint8_t length) {
#ifdef ARDUINO
    Wire.beginTransmission(_i2c_addr);
    Wire.write(reg);
    if (Wire.endTransmission() != 0) return false;
    
    if (Wire.requestFrom(_i2c_addr, length) != length) return false;
    for (uint8_t i = 0; i < length; i++) {
        data[i] = Wire.read();
    }
    return true;
#else
    if (!m_twi) return false;
    
    ret_code_t err_code = nrf_drv_twi_tx(m_twi, _i2c_addr, &reg, 1, true);
    if (err_code != NRF_SUCCESS) return false;
    
    err_code = nrf_drv_twi_rx(m_twi, _i2c_addr, data, length);
    return (err_code == NRF_SUCCESS);
#endif
}

// Default configuration
bool nPM1300::configureDefault() {
    // Set Buck1 and Buck2 to 3.3V and enable
    if (!setBuck1Voltage(3.3)) return false;
    if (!setBuck2Voltage(3.3)) return false;
    if (!enableBuck1(true)) return false;
    if (!enableBuck2(true)) return false;
    
    // Set LDO1 to 3.3V and enable, disable LDO2
    if (!setLDO1Voltage(3.3)) return false;
    if (!enableLDO1(true)) return false;
    if (!enableLDO2(false)) return false;
    
    // Configure charger to 4.2V
    if (!setChargeVoltage(4.2)) return false;
    if (!enableCharger(true)) return false;
    
    // Configure GPIOs for DRV2603
    if (!configureDRV2603()) return false;
    
    // Enable SHIP HOLD feature
    if (!setShipHoldMode(SHPHLD_MODE_WAKE_SHORT)) return false;
    
    return true;
}

// Buck1 voltage setting
bool nPM1300::setBuck1Voltage(float voltage) {
    uint8_t reg_val;
    if (voltage == 3.3) {
        reg_val = VOLTAGE_3V3_BUCK;
    } else {
        // Simplified calculation, can be extended for more voltages
        reg_val = (uint8_t)((voltage - 1.8) * 50);
    }
    return writeRegister(NPM1300_BUCK1_VOUT, reg_val);
}

// Buck2 voltage setting
bool nPM1300::setBuck2Voltage(float voltage) {
    uint8_t reg_val;
    if (voltage == 3.3) {
        reg_val = VOLTAGE_3V3_BUCK;
    } else {
        reg_val = (uint8_t)((voltage - 1.8) * 50);
    }
    return writeRegister(NPM1300_BUCK2_VOUT, reg_val);
}

// Enable or disable Buck1
bool nPM1300::enableBuck1(bool enable) {
    uint8_t ctrl_val = enable ? 0x01 : 0x00;
    return writeRegister(NPM1300_BUCK1_CTRL, ctrl_val);
}

// Enable or disable Buck2
bool nPM1300::enableBuck2(bool enable) {
    uint8_t ctrl_val = enable ? 0x01 : 0x00;
    return writeRegister(NPM1300_BUCK2_CTRL, ctrl_val);
}

// LDO1 voltage setting
bool nPM1300::setLDO1Voltage(float voltage) {
    uint8_t reg_val;
    if (voltage == 3.3) {
        reg_val = VOLTAGE_3V3_LDO;
    } else {
        reg_val = (uint8_t)((voltage - 1.8) * 40);
    }
    return writeRegister(NPM1300_LDO1_VOUT, reg_val);
}

// Enable or disable LDO1
bool nPM1300::enableLDO1(bool enable) {
    uint8_t ctrl_val = enable ? 0x01 : 0x00;
    return writeRegister(NPM1300_LDO1_CTRL, ctrl_val);
}

// Enable or disable LDO2
bool nPM1300::enableLDO2(bool enable) {
    uint8_t ctrl_val = enable ? 0x01 : 0x00;
    return writeRegister(NPM1300_LDO2_CTRL, ctrl_val);
}

// Charger voltage setting
bool nPM1300::setChargeVoltage(float voltage) {
    uint8_t reg_val;
    if (voltage == 4.2) {
        reg_val = VOLTAGE_4V2_CHG;
    } else {
        reg_val = (uint8_t)((voltage - 3.5) * 40);
    }
    return writeRegister(NPM1300_CHG_VTERM, reg_val);
}

// Enable or disable charger
bool nPM1300::enableCharger(bool enable) {
    uint8_t ctrl_val = enable ? 0x01 : 0x00;
    return writeRegister(NPM1300_CHG_CTRL, ctrl_val);
}

// GPIO mode configuration
bool nPM1300::setGPIO1Mode(uint8_t mode) {
    uint8_t ctrl_val = 0x00;
    switch(mode) {
        case GPIO_MODE_INPUT:
            ctrl_val = 0x00;
            break;
        case GPIO_MODE_OUTPUT:
            ctrl_val = 0x01;
            break;
        case GPIO_MODE_INTERRUPT:
            ctrl_val = 0x02;
            break;
    }
    return writeRegister(NPM1300_GPIO1_CTRL, ctrl_val);
}

bool nPM1300::setGPIO2Mode(uint8_t mode) {
    uint8_t ctrl_val = 0x00;
    switch(mode) {
        case GPIO_MODE_OUTPUT:
            ctrl_val = 0x01; // Used for DRV2603_EN
            break;
        default:
            ctrl_val = 0x01;
    }
    return writeRegister(NPM1300_GPIO2_CTRL, ctrl_val);
}

bool nPM1300::setGPIO3Mode(uint8_t mode) {
    uint8_t ctrl_val = 0x03; // PWM mode for DRV2603_PWM
    return writeRegister(NPM1300_GPIO3_CTRL, ctrl_val);
}

// Write to GPIO
bool nPM1300::writeGPIO(uint8_t pin, bool state) {
    uint8_t reg = NPM1300_GPIO1_CTRL + pin - 1;
    uint8_t current_val;
    if (!readRegister(reg, &current_val)) return false;
    
    if (state) {
        current_val |= 0x80; // Set high bit
    } else {
        current_val &= 0x7F; // Clear high bit
    }
    return writeRegister(reg, current_val);
}

// Configure GPIOs for DRV2603
bool nPM1300::configureDRV2603() {
    // Set GPIO1 as interrupt input
    if (!setGPIO1Mode(GPIO_MODE_INTERRUPT)) return false;
    
    // Set GPIO2 as output (DRV2603_EN)
    if (!setGPIO2Mode(GPIO_MODE_OUTPUT)) return false;
    
    // Set GPIO3 as PWM output (DRV2603_PWM)
    if (!setGPIO3Mode(GPIO_MODE_OUTPUT)) return false;
    
    return true;
}

// Set SHIP HOLD mode
bool nPM1300::setShipHoldMode(uint8_t mode) {
    uint8_t ctrl_val = 0x00;
    switch(mode) {
        case SHPHLD_MODE_DISABLE:
            ctrl_val = 0x00;
            break;
        case SHPHLD_MODE_WAKE_SHORT:
            ctrl_val = 0x01; // Wake on short press
            break;
        case SHPHLD_MODE_SLEEP_LONG:
            ctrl_val = 0x02; // Power off on long press
            break;
    }
    return writeRegister(NPM1300_SHPHLD_CTRL, ctrl_val);
}

// Get battery voltage
float nPM1300::getBatteryVoltage() {
    uint8_t high, low;
    if (!readRegister(NPM1300_VBAT_HIGH, &high)) return 0.0;
    if (!readRegister(NPM1300_VBAT_LOW, &low)) return 0.0;
    
    uint16_t raw_value = (high << 8) | low;
    return (raw_value * 0.001); // Convert to voltage
}

// Get system voltage
float nPM1300::getSystemVoltage() {
    uint8_t high, low;
    if (!readRegister(NPM1300_VSYS_HIGH, &high)) return 0.0;
    if (!readRegister(NPM1300_VSYS_LOW, &low)) return 0.0;
    
    uint16_t raw_value = (high << 8) | low;
    return (raw_value * 0.001);
}

// Get battery percentage
uint8_t nPM1300::getBatteryPercent() {
    float voltage = getBatteryVoltage();
    
    // Simple battery percentage calculation (Li-ion 3.0V–4.2V)
    if (voltage >= 4.2) return 100;
    if (voltage <= 3.0) return 0;
    
    return (uint8_t)((voltage - 3.0) / 1.2 * 100);
}

// Check if charging
bool nPM1300::isCharging() {
    uint8_t status;
    if (!readRegister(NPM1300_CHG_STATUS, &status)) return false;
    return (status & 0x01) != 0;
}

// Check if battery is low
bool nPM1300::isBatteryLow() {
    return getBatteryVoltage() < 3.3;
}

// Clear interrupts
bool nPM1300::clearInterrupts() {
    return writeRegister(NPM1300_INT_STATUS0, 0xFF) && 
           writeRegister(NPM1300_INT_STATUS1, 0xFF);
}

// Get interrupt status
uint8_t nPM1300::getInterruptStatus() {
    uint8_t status;
    readRegister(NPM1300_INT_STATUS0, &status);
    return status;
}

// Get main status register
uint8_t nPM1300::getMainStatus() {
    uint8_t status;
    readRegister(NPM1300_MAIN_STATUS, &status);
    return status;
}

// Check power good status
bool nPM1300::isPowerGood() {
    uint8_t status = getMainStatus();
    return (status & 0x80) != 0; // Assume bit 7 indicates power good
}
