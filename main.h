#ifndef NPM1300_H
#define NPM1300_H

#ifdef ARDUINO
#include <Wire.h>
#include <Arduino.h>
#else
// nRF SDK includes
#include "nrf_drv_twi.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#endif

// nPM1300 I2C address
#define NPM1300_I2C_ADDR 0x6B

// Main register addresses
#define NPM1300_MAIN_STATUS     0x00
#define NPM1300_INT_STATUS0     0x01
#define NPM1300_INT_STATUS1     0x02
#define NPM1300_INT_MASK0       0x03
#define NPM1300_INT_MASK1       0x04

// Buck converter registers
#define NPM1300_BUCK1_CTRL      0x14
#define NPM1300_BUCK1_VOUT      0x15
#define NPM1300_BUCK2_CTRL      0x16
#define NPM1300_BUCK2_VOUT      0x17

// LDO registers
#define NPM1300_LDO1_CTRL       0x18
#define NPM1300_LDO1_VOUT       0x19
#define NPM1300_LDO2_CTRL       0x1A
#define NPM1300_LDO2_VOUT       0x1B

// Charger registers
#define NPM1300_CHG_CTRL        0x30
#define NPM1300_CHG_VTERM       0x31
#define NPM1300_CHG_ITERM       0x32
#define NPM1300_CHG_STATUS      0x33

// GPIO registers
#define NPM1300_GPIO1_CTRL      0x50
#define NPM1300_GPIO2_CTRL      0x51
#define NPM1300_GPIO3_CTRL      0x52

// Battery monitoring registers
#define NPM1300_VBAT_HIGH       0x60
#define NPM1300_VBAT_LOW        0x61
#define NPM1300_VSYS_HIGH       0x62
#define NPM1300_VSYS_LOW        0x63

// SHIP HOLD register
#define NPM1300_SHPHLD_CTRL     0x70

// Voltage settings (register values for 3.3V)
#define VOLTAGE_3V3_BUCK        0x32  // Buck output 3.3V
#define VOLTAGE_3V3_LDO         0x28  // LDO output 3.3V
#define VOLTAGE_4V2_CHG         0x2A  // Charge voltage 4.2V

class nPM1300 {
private:
#ifdef ARDUINO
    // Use Wire library in Arduino environment
#else
    // TWI instance for nRF SDK environment
    static const nrf_drv_twi_t* m_twi;
#endif
    
    uint8_t _i2c_addr;
    bool _initialized;
    
    // Private functions
    bool writeRegister(uint8_t reg, uint8_t data);
    bool readRegister(uint8_t reg, uint8_t* data);
    bool readMultipleRegisters(uint8_t reg, uint8_t* data, uint8_t length);
    
public:
    // Constructor
#ifdef ARDUINO
    nPM1300(uint8_t addr = NPM1300_I2C_ADDR);
#else
    nPM1300(const nrf_drv_twi_t* twi_instance, uint8_t addr = NPM1300_I2C_ADDR);
#endif
    
    // Initialization and basic functions
    bool begin();
    bool isConnected();
    void reset();
    
    // Buck converter control
    bool setBuck1Voltage(float voltage);
    bool setBuck2Voltage(float voltage);
    bool enableBuck1(bool enable);
    bool enableBuck2(bool enable);
    
    // LDO control
    bool setLDO1Voltage(float voltage);
    bool setLDO2Voltage(float voltage);
    bool enableLDO1(bool enable);
    bool enableLDO2(bool enable);
    
    // Charger control
    bool setChargeVoltage(float voltage);
    bool setChargeCurrent(uint16_t current_ma);
    bool enableCharger(bool enable);
    
    // GPIO control
    bool setGPIO1Mode(uint8_t mode);  // 0=Input, 1=Output, 2=Interrupt
    bool setGPIO2Mode(uint8_t mode);
    bool setGPIO3Mode(uint8_t mode);
    bool writeGPIO(uint8_t pin, bool state);
    bool readGPIO(uint8_t pin);
    
    // SHIP HOLD control (power management)
    bool enableShipHold(bool enable);
    bool setShipHoldMode(uint8_t mode); // 0=Disable, 1=Wake on short press, 2=Power off on long press
    
    // Battery monitoring
    float getBatteryVoltage();
    float getSystemVoltage();
    uint8_t getBatteryPercent();
    bool isCharging();
    bool isBatteryLow();
    
    // Interrupt handling
    bool clearInterrupts();
    uint8_t getInterruptStatus();
    bool setInterruptMask(uint8_t mask);
    
    // Status queries
    uint8_t getMainStatus();
    bool isPowerGood();
    
    // Default configuration functions (quick setup)
    bool configureDefault();
    bool configureDRV2603(); // Configure GPIO for DRV2603
};

// Common mode definitions
#define GPIO_MODE_INPUT     0
#define GPIO_MODE_OUTPUT    1
#define GPIO_MODE_INTERRUPT 2

#define SHPHLD_MODE_DISABLE     0
#define SHPHLD_MODE_WAKE_SHORT  1
#define SHPHLD_MODE_SLEEP_LONG  2

#endif // NPM1300_H
