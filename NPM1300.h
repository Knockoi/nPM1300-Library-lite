#ifndef NPM1300_H
#define NPM1300_H

#include <Arduino.h>
#include <Wire.h>

// I2C Address
#define NPM1300_I2C_ADDR 0x6B

// Base Addresses
#define NPM1300_BUCK_BASE       0x0400
#define NPM1300_BCHARGER_BASE   0x0300
#define NPM1300_ADC_BASE        0x0500
#define NPM1300_GPIO_BASE       0x0600

// BUCK Register Addresses
#define NPM1300_BUCK1_ENASET        (NPM1300_BUCK_BASE + 0x00)
#define NPM1300_BUCK1_ENACLR        (NPM1300_BUCK_BASE + 0x01)
#define NPM1300_BUCK2_ENASET        (NPM1300_BUCK_BASE + 0x02)
#define NPM1300_BUCK2_ENACLR        (NPM1300_BUCK_BASE + 0x03)
#define NPM1300_BUCK1_NORMVOUT      (NPM1300_BUCK_BASE + 0x08)
#define NPM1300_BUCK2_NORMVOUT      (NPM1300_BUCK_BASE + 0x0A)
#define NPM1300_BUCKSWCTRLSEL       (NPM1300_BUCK_BASE + 0x0F)

// CHARGER Register Addresses
#define NPM1300_BCHG_ENABLESET          (NPM1300_BCHARGER_BASE + 0x04)
#define NPM1300_BCHG_ENABLECLR          (NPM1300_BCHARGER_BASE + 0x05)
#define NPM1300_BCHG_ISETMSB            (NPM1300_BCHARGER_BASE + 0x08)
#define NPM1300_BCHG_ISETLSB            (NPM1300_BCHARGER_BASE + 0x09)
#define NPM1300_BCHG_VTERM              (NPM1300_BCHARGER_BASE + 0x0C)
#define NPM1300_BCHG_CHARGESTATUS       (NPM1300_BCHARGER_BASE + 0x34)

// ADC Register Addresses
#define NPM1300_TASK_VBAT_MEAS      (NPM1300_ADC_BASE + 0x00)
#define NPM1300_TASK_VSYS_MEAS      (NPM1300_ADC_BASE + 0x03)
#define NPM1300_TASK_VBUS_MEAS      (NPM1300_ADC_BASE + 0x07)
#define NPM1300_ADC_VBAT_MSB        (NPM1300_ADC_BASE + 0x11)
#define NPM1300_ADC_VSYS_MSB        (NPM1300_ADC_BASE + 0x14)
#define NPM1300_ADC_GP0_LSBS        (NPM1300_ADC_BASE + 0x15)
#define NPM1300_ADC_VBAT3_MSB       (NPM1300_ADC_BASE + 0x19)
#define NPM1300_ADC_GP1_LSBS        (NPM1300_ADC_BASE + 0x1A)

// GPIO Register Addresses
#define NPM1300_GPIO_MODE0          (NPM1300_GPIO_BASE + 0x00)
#define NPM1300_GPIO_PUEN0          (NPM1300_GPIO_BASE + 0x0A)
#define NPM1300_GPIO_PDEN0          (NPM1300_GPIO_BASE + 0x0F)
#define NPM1300_GPIO_STATUS         (NPM1300_GPIO_BASE + 0x1E)

// GPIO Mode Definitions
#define NPM1300_GPIO_INPUT          0
#define NPM1300_GPIO_OUT_LOGIC1     8
#define NPM1300_GPIO_OUT_LOGIC0     9

class NPM1300 {
public:
    NPM1300(TwoWire &wirePort = Wire);
    
    // Initialization
    bool begin();
    bool isConnected();
    
    // BUCK Control
    bool enableBuck1();
    bool enableBuck2();
    bool disableBuck1();
    bool disableBuck2();
    bool setBuck1Voltage(float voltage);
    bool setBuck2Voltage(float voltage);
    
    // Voltage Readings (mV)
    uint16_t getVbusVoltage();
    uint16_t getVbatVoltage();
    uint16_t getVsysVoltage();
    
    // Battery State
    uint8_t getBatterySOC();
    bool isCharging();
    bool isBatteryFull();
    
    // Charger Control
    bool enableCharger();
    bool disableCharger();
    bool setChargeCurrent(uint16_t currentMa);
    bool setChargeVoltage(float voltage);
    
    // GPIO Control
    bool setGpioMode(uint8_t pin, uint8_t mode);
    bool setGpioOutput(uint8_t pin, bool state);
    bool getGpioInput(uint8_t pin);
    bool setGpioPullUp(uint8_t pin, bool enable);
    bool setGpioPullDown(uint8_t pin, bool enable);
    
    // Simplified API
    uint16_t readVBUS();
    uint16_t readVBAT();
    uint16_t readVSYS();
    uint8_t readSOC();
    bool writeBuck1(float voltage);
    bool writeBuck2(float voltage);
    bool writeChargeCurrent(uint16_t mA);
    bool writeChargeVoltage(float voltage);
    
private:
    TwoWire *_i2c;
    
    // I2C Helper Functions - 使用 uint16_t 支援 16-bit 位址
    bool writeRegister(uint16_t reg, uint8_t value);
    bool readRegister(uint16_t reg, uint8_t *value);
    bool readRegisters(uint16_t reg, uint8_t *buffer, uint8_t len);
    
    // Conversion Functions
    uint8_t voltageToRegValue(float voltage);
};

#endif // NPM1300_H
