/*
 * NPM1300 Lite Arduino Library
 * Simple library for nPM1300 PMIC with BUCK converters
 */

#ifndef NPM1300_H
#define NPM1300_H

#include <Arduino.h>
#include <Wire.h>

// I2C Address
#define NPM1300_I2C_ADDR 0x6B

// Register Addresses
#define NPM1300_BUCK1_VOUT_SEL      0x20
#define NPM1300_BUCK2_VOUT_SEL      0x21
#define NPM1300_BUCK1_EN            0x22
#define NPM1300_BUCK2_EN            0x23

#define NPM1300_VBUS_VOLTAGE        0x30
#define NPM1300_VBAT_VOLTAGE        0x32
#define NPM1300_VSYS_VOLTAGE        0x34
#define NPM1300_IBAT_CURRENT        0x36
#define NPM1300_BAT_TEMP            0x38
#define NPM1300_DIE_TEMP            0x3A

#define NPM1300_BATCHG_STATUS       0x40
#define NPM1300_BAT_SOC             0x42

#define NPM1300_CHARGER_EN          0x50
#define NPM1300_CHARGER_ISET        0x51
#define NPM1300_CHARGER_VTERM       0x52

class NPM1300 {
public:
    NPM1300(TwoWire &wirePort = Wire);
    
    // Initialization
    bool begin();
    bool isConnected();
    
    // BUCK Configuration
    bool enableBuck1();
    bool enableBuck2();
    bool disableBuck1();
    bool disableBuck2();
    bool setBuck1Voltage(float voltage);
    bool setBuck2Voltage(float voltage);
    
    // Voltage Readings (in mV)
    uint16_t getVbusVoltage();
    uint16_t getVbatVoltage();
    uint16_t getVsysVoltage();
    
    // Current Reading (in mA)
    int16_t getBatCurrent();
    
    // Temperature (in °C)
    int8_t getBatTemperature();
    int8_t getDieTemperature();
    
    // Battery State
    uint8_t getBatterySOC();  // State of Charge (%)
    bool isCharging();
    bool isBatteryFull();
    
    // Simplified Read Functions (Arduino style)
    uint16_t readVBUS();      // Read VBUS voltage in mV
    uint16_t readVBAT();      // Read battery voltage in mV
    uint16_t readVSYS();      // Read system voltage in mV
    int16_t readCurrent();    // Read battery current in mA
    uint8_t readSOC();        // Read battery percentage (0-100%)
    int8_t readTemp();        // Read battery temperature in °C
    
    // Simplified Write Functions (Arduino style)
    bool writeBuck1(float voltage);      // Set BUCK1 voltage (1.0-3.3V)
    bool writeBuck2(float voltage);      // Set BUCK2 voltage (1.0-3.3V)
    bool writeChargeCurrent(uint16_t mA); // Set charge current in mA
    bool writeChargeVoltage(float voltage); // Set charge termination voltage
    
    // Charger Control
    bool enableCharger();
    bool disableCharger();
    bool setChargeCurrent(uint16_t currentMa);
    bool setChargeVoltage(float voltage);
    
private:
    TwoWire *_i2c;
    
    // I2C Helper Functions
    bool writeRegister(uint8_t reg, uint8_t value);
    bool readRegister(uint8_t reg, uint8_t *value);
    bool readRegisters(uint8_t reg, uint8_t *buffer, uint8_t len);
    
    // Conversion Functions
    uint8_t voltageToRegValue(float voltage);
    float regValueToVoltage(uint8_t regValue);
};

#endif // NPM1300_H
