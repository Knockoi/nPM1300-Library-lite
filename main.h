// nPM1300 Library for Arduino and nRF SDK/Zephyr RTOS
// Author: Grok 4 (xAI) - Generated based on user specifications, updated with corrections
// Version: 1.1
// Date: September 18, 2025
//
// This library provides a flexible interface to control the Nordic nPM1300 PMIC via I2C.
// It supports configuration of Bucks, LDOs, Charger, GPIOs, and System Monitor (ADC) for measurements.
// Fuel gauge is implemented as a simple voltage-based estimation with Coulomb counting for better accuracy (requires periodic updates).
// Optimized for low power: Measurements are triggered on-demand, automatic modes disabled by default.
// Compatible with:
// - Arduino (using Wire.h)
// - nRF SDK / Zephyr RTOS (using Zephyr I2C API; define USE_ZEPHYR to switch)
//
// Usage:
// - Include this in your project.
// - In examples, override defaults via setters.
// - For Zephyr: Provide device tree overlay for I2C and define USE_ZEPHYR.
//
// Default Configurations (overridable):
// - Buck1 and Buck2: 3.3V output
// - LDO1: 3.3V enabled
// - LDO2: Disabled
// - Charger: 4.2V termination, suitable for 500mAh Li-Ion (adjust current limits as needed)
// - GPIO1: INT (interrupt output)
// - GPIO2: DRV2603_EN (output for enable)
// - GPIO3: DRV2603_PWM (output for PWM)
// - SHPHLD_B: Configured as power button (short press wake, long press shutdown)
// - Battery Capacity: 500mAh (for fuel gauge)
// - NTC: 10kOhm (default, configurable)
//
// Fuel Gauge Notes:
// - Simple voltage-based SOC estimation with temperature compensation.
// - Coulomb counting for current integration (update periodically in loop).
// - Battery health (SOH) estimated based on cycle count and capacity fade (simplified model).
// - For better accuracy, consider implementing a lookup table based on battery discharge curve.
//
// Error Reporting: All functions return error codes (0 = success).
//
// Changes in v1.1:
// - Updated register bases and offsets based on Zephyr driver.
// - Updated voltage encoding for Bucks and LDOs to 1.0V base, 0.1V steps.
// - Fixed getVbusVoltage and getBatteryCurrent MSB registers.
// - Implemented multi-byte I2C functions.
// - Added placeholders for charge status and error reading.
// - Kept charger and GPIO values as examples; verify with datasheet for exact values.

#ifndef NPM1300_H
#define NPM1300_H

// Define USE_ZEPHYR for Zephyr RTOS compatibility
// #define USE_ZEPHYR

#ifdef ARDUINO
#include <Wire.h>
#else
// For nRF SDK or Zephyr, include appropriate headers
#ifdef USE_ZEPHYR
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#else
// nRF SDK placeholders (user to implement)
#include <nrf_drv_twi.h>
#endif
#endif

// nPM1300 I2C Address
#define NPM1300_I2C_ADDR 0x6B

// Register Bases (from Zephyr driver and datasheet)
#define ADC_BASE 0x05
#define BUCK_BASE 0x04
#define GPIO_BASE 0x06
#define LDO_BASE 0x08
#define CHARGER_BASE 0x03  // Assumed; verify with datasheet
#define SHPHLD_BASE 0x0B

// Key Registers for ADC (from original, assumed correct)
#define TASK_VBAT_MEASURE 0x00
#define TASK_NTC_MEASURE 0x01
#define TASK_TEMP_MEASURE 0x02
#define TASK_VSYS_MEASURE 0x03
#define TASK_IBAT_MEASURE 0x06
#define TASK_VBUS7_MEASURE 0x07
#define ADC_CONFIG 0x09
#define ADC_NTC_RSEL 0x0A
#define ADC_AUTO_TIM_CONF 0x0B
#define TASK_AUTO_TIM_UPDATE 0x0C
#define ADC_DEL_TIM_CONF 0x0D
#define ADC_IBAT_MEAS_STATUS 0x10
#define ADC_VBAT_RESULT_MSB 0x11
#define ADC_NTC_RESULT_MSB 0x12
#define ADC_TEMP_RESULT_MSB 0x13
#define ADC_VSYS_RESULT_MSB 0x14
#define ADC_GP0_RESULT_LSBS 0x15
#define ADC_IBAT_RESULT_MSB 0x16  // Assumed based on pattern
#define ADC_VBUS_RESULT_MSB 0x17  // Assumed based on pattern
#define ADC_IBAT_MEAS_EN 0x24

// Charger Registers (example offsets; verify with datasheet)
#define CHG_TERM_VOLT 0x20  // Example
#define CHG_CURR_LIMIT 0x21 // Example
#define CHG_STATUS 0x22     // Assumed for status
#define ERR_STATUS 0x23     // Assumed for error status

// Buck Registers (from Zephyr)
#define BUCK_EN_SET 0x00    // Offset for EN_SET (Buck1), Buck2 +0x02
#define BUCK_VOUT_NORM 0x08 // Offset for VOUT_NORMAL (Buck1), Buck2 +0x02

// LDO Registers (from Zephyr)
#define LDO_EN_SET 0x00     // Offset for EN_SET (LDO1), LDO2 +0x02
#define LDO_VOUT_SEL 0x0C   // Offset for VOUTSEL (LDO1), LDO2 +0x01

// GPIO Registers (from Zephyr)
#define GPIO_MODE 0x00      // Offset + pin for mode

// SHPHLD_B Config (Power Button)
#define SHPHLD_CFG 0x00     // Example offset; verify

// Constants from Datasheet
#define VFS_VBAT 5.0f
#define VFS_VBUS 7.5f
#define VFS_VSYS 6.375f
#define VFS_TEMP 1.5f
#define T0_KELVIN 298.15f
#define NTC_BETA_DEFAULT 3950  // Common for 10k NTC; configurable

// Error Codes
#define NPM_ERR_SUCCESS 0
#define NPM_ERR_I2C_FAIL -1
#define NPM_ERR_INVALID_MEAS -2

// NTC Resistance Select
enum NtcResistance {
  NTC_HI_Z = 0,
  NTC_10K = 1,
  NTC_47K = 2,
  NTC_100K = 3
};

// Measurement Modes
enum MeasMode {
  SINGLE_SHOT,
  AUTO,
  TIMED
};

class nPM1300 {
public:
  // Constructor
  nPM1300();

  // Initialization with defaults (call in setup())
  int init();

  // Setters for flexible configuration (override defaults)
  void setBuckVoltage(uint8_t buckNum, float voltage);  // buckNum 1 or 2, voltage in V (e.g., 3.3)
  void setLdoVoltage(uint8_t ldoNum, float voltage);    // ldoNum 1 or 2
  void enableLdo(uint8_t ldoNum, bool enable);
  void setChargeTermVoltage(float voltage);             // e.g., 4.2V
  void setChargeCurrent(float current_mA);              // e.g., 100mA for 500mAh battery (0.2C)
  void setNtcResistance(NtcResistance res);
  void setBatteryCapacity(float capacity_mAh);          // For fuel gauge
  void setNtcBeta(uint16_t beta);                      // NTC beta parameter

  // GPIO Config (value is example; verify datasheet for exact mode codes)
  void configGpio1AsInt();
  void configGpio2AsDrvEn();
  void configGpio3AsDrvPwm();

  // Power Button Config (value is example; verify)
  void configShphldAsPowerButton();  // Short press wake, long press shutdown

  // Measurements (low power: single-shot by default)
  float getBatteryVoltage();         // VBAT in V
  float getBatteryCurrent();         // IBAT in mA (positive charging, negative discharging)
  float getBatteryTemperature();     // TBAT in C
  float getDieTemperature();         // TDIE in C
  float getVsysVoltage();            // VSYS in V
  float getVbusVoltage();            // VBUS in V

  // Fuel Gauge
  float getStateOfCharge();          // SOC % (0-100)
  float getStateOfHealth();          // SOH % (estimated)
  void updateFuelGauge();            // Call periodically for Coulomb counting
  int getChargeStatus();             // 0: Not charging, 1: Charging, 2: Full, -1: Error
  float getChargeVoltage();          // Current charge voltage
  float getChargeCurrent();          // Current charge current

  // Error Reporting
  int getLastError();                // Read error register or status

private:
  // I2C Abstraction
  int i2cWrite(uint8_t reg, uint8_t val);
  int i2cRead(uint8_t reg, uint8_t* val);
  int i2cWriteMulti(uint8_t reg, uint8_t* data, size_t len);
  int i2cReadMulti(uint8_t reg, uint8_t* data, size_t len);

#ifdef USE_ZEPHYR
  const struct device* i2c_dev;
#endif

  // Internal States for Fuel Gauge
  float batteryCapacity_mAh = 500.0f;
  float integratedCharge_mAh = 0.0f;  // For Coulomb counting
  uint16_t cycleCount = 0;            // For SOH estimation
  float lastIbat = 0.0f;
  unsigned long lastUpdateTime = 0;
  NtcResistance ntcRes = NTC_10K;
  uint16_t ntcBeta = NTC_BETA_DEFAULT;

  // Helper Functions
  void triggerMeasurement(uint8_t taskOffset); // Offset relative to ADC_BASE
  uint16_t readAdcResult(uint8_t msbReg, uint8_t lsbReg, uint8_t lsbShift);
  float calculateVbat(uint16_t adcVal);
  float calculateIbat(uint16_t adcVal, bool charging);
  float calculateTbat(uint16_t adcVal);
  float calculateTdie(uint16_t adcVal);
  float calculateVsys(uint16_t adcVal);
  float calculateVbus(uint16_t adcVal);

  // Low Power Optimizations
  void disableAutoMeasurements();  // Disable auto for low power
};

// Example Usage (in Arduino sketch or main.c)
// void setup() {
//   nPM1300 pmic;
//   pmic.init();
//   // Override if needed
//   // pmic.setBuckVoltage(1, 3.0);
//   // pmic.enableLdo(2, true);
//   // pmic.setLdoVoltage(2, 1.8);
// }

#endif // NPM1300_H
