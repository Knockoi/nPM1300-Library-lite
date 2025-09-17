// nPM1300.cpp

#include "nPM1300.h"

#ifdef ARDUINO
#include <Arduino.h>
#else
// millis() placeholder for non-Arduino
unsigned long millis() { /* Implement */ return 0; }
#endif

nPM1300::nPM1300() {
#ifdef USE_ZEPHYR
  i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));  // Adjust for your device tree
#endif
}

int nPM1300::init() {
  // Default configs
  setBuckVoltage(1, 3.3);
  setBuckVoltage(2, 3.3);
  setLdoVoltage(1, 3.3);
  enableLdo(1, true);
  enableLdo(2, false);
  setChargeTermVoltage(4.2);
  setChargeCurrent(100.0);  // 0.2C for 500mAh
  setNtcResistance(NTC_10K);
  configGpio1AsInt();
  configGpio2AsDrvEn();
  configGpio3AsDrvPwm();
  configShphldAsPowerButton();
  disableAutoMeasurements();  // Low power
  return NPM_ERR_SUCCESS;
}

void nPM1300::setBuckVoltage(uint8_t buckNum, float voltage) {
  uint8_t vset = (uint8_t)((voltage - 0.6) / 0.05);  // Example encoding; adjust per datasheet
  uint8_t reg = (buckNum == 1) ? BUCK1_VSET : BUCK2_VSET;
  i2cWrite(reg, vset);
}

void nPM1300::setLdoVoltage(uint8_t ldoNum, float voltage) {
  uint8_t vset = (uint8_t)((voltage - 0.8) / 0.1);  // Example; adjust
  uint8_t reg = (ldoNum == 1) ? LDO1_VSET : LDO2_VSET;
  i2cWrite(reg, vset);
}

void nPM1300::enableLdo(uint8_t ldoNum, bool enable) {
  uint8_t reg = (ldoNum == 1) ? LDO1_EN : LDO2_EN;
  i2cWrite(reg, enable ? 0x01 : 0x00);
}

void nPM1300::setChargeTermVoltage(float voltage) {
  uint8_t vterm = (uint8_t)((voltage - 3.5) / 0.05);  // Example
  i2cWrite(CHG_TERM_VOLT, vterm);
}

void nPM1300::setChargeCurrent(float current_mA) {
  uint8_t iset = (uint8_t)(current_mA / 10.0);  // Example scaling
  i2cWrite(CHG_CURR_LIMIT, iset);
}

void nPM1300::setNtcResistance(NtcResistance res) {
  ntcRes = res;
  i2cWrite(ADC_NTC_RSEL, (uint8_t)res);
}

void nPM1300::setBatteryCapacity(float capacity_mAh) {
  batteryCapacity_mAh = capacity_mAh;
}

void nPM1300::setNtcBeta(uint16_t beta) {
  ntcBeta = beta;
}

void nPM1300::configGpio1AsInt() {
  i2cWrite(GPIO1_CFG, 0x01);  // Example: Interrupt mode
}

void nPM1300::configGpio2AsDrvEn() {
  i2cWrite(GPIO2_CFG, 0x02);  // Output enable
}

void nPM1300::configGpio3AsDrvPwm() {
  i2cWrite(GPIO3_CFG, 0x03);  // PWM output
}

void nPM1300::configShphldAsPowerButton() {
  i2cWrite(SHPHLD_CFG, 0x01);  // Enable short/long press detection
}

float nPM1300::getBatteryVoltage() {
  triggerMeasurement(TASK_VBAT_MEASURE);
  uint16_t adcVal = readAdcResult(ADC_VBAT_RESULT_MSB, ADC_GP0_RESULT_LSBS, 0);
  return calculateVbat(adcVal);
}

float nPM1300::getBatteryCurrent() {
  i2cWrite(ADC_IBAT_MEAS_EN, 0x01);  // Enable IBAT after VBAT
  triggerMeasurement(TASK_IBAT_MEASURE);
  uint16_t adcVal = readAdcResult(ADC_VBAT_RESULT_MSB, ADC_GP0_RESULT_LSBS, 0);  // Adjust for IBAT reg
  uint8_t status;
  i2cRead(ADC_IBAT_MEAS_STATUS, &status);
  bool charging = (status & 0x02) == 0x02;  // Example bit check
  if (status & 0x04) return NPM_ERR_INVALID_MEAS;  // Invalid flag
  return calculateIbat(adcVal, charging);
}

float nPM1300::getBatteryTemperature() {
  triggerMeasurement(TASK_NTC_MEASURE);
  uint16_t adcVal = readAdcResult(ADC_NTC_RESULT_MSB, ADC_GP0_RESULT_LSBS, 0);
  return calculateTbat(adcVal) - 273.15f;  // To Celsius
}

float nPM1300::getDieTemperature() {
  triggerMeasurement(TASK_TEMP_MEASURE);
  uint16_t adcVal = readAdcResult(ADC_TEMP_RESULT_MSB, ADC_GP0_RESULT_LSBS, 0);
  return calculateTdie(adcVal);
}

float nPM1300::getVsysVoltage() {
  triggerMeasurement(TASK_VSYS_MEASURE);
  uint16_t adcVal = readAdcResult(ADC_VSYS_RESULT_MSB, ADC_GP0_RESULT_LSBS, 0);
  return calculateVsys(adcVal);
}

float nPM1300::getVbusVoltage() {
  triggerMeasurement(TASK_VBUS7_MEASURE);
  uint16_t adcVal = readAdcResult(ADC_VBAT_RESULT_MSB, ADC_GP0_RESULT_LSBS, 0);  // VBUS in VBAT3?
  return calculateVbus(adcVal);
}

float nPM1300::getStateOfCharge() {
  float vbat = getBatteryVoltage();
  float tbat = getBatteryTemperature();
  // Simple voltage-based SOC with temp compensation
  float soc = (vbat - 3.0) / (4.2 - 3.0) * 100.0;
  soc -= (tbat > 25.0) ? (tbat - 25.0) * 0.1 : 0;  // Basic compensation
  soc = max(0.0, min(100.0, soc));
  return soc + (integratedCharge_mAh / batteryCapacity_mAh * 100.0);  // Adjust with Coulomb
}

float nPM1300::getStateOfHealth() {
  // Simplified: 100% - (cycleCount * 0.05)% fade per cycle
  return 100.0 - (cycleCount * 0.05);
}

void nPM1300::updateFuelGauge() {
  float ibat = getBatteryCurrent();
  unsigned long now = millis();
  float deltaTime_h = (now - lastUpdateTime) / 3600000.0f;  // Hours
  integratedCharge_mAh += ibat * deltaTime_h;
  if (ibat > 0) cycleCount++;  // Increment on charge
  lastUpdateTime = now;
}

int nPM1300::getChargeStatus() {
  // Read charger status register (example)
  uint8_t status = 0;
  // i2cRead(CHG_STATUS, &status);  // Placeholder
  return status;  // Map to 0/1/2/-1
}

float nPM1300::getChargeVoltage() {
  return getVbat();  // Approximation
}

float nPM1300::getChargeCurrent() {
  float ibat = getBatteryCurrent();
  return (ibat > 0) ? ibat : 0;
}

int nPM1300::getLastError() {
  // Read error register (placeholder)
  return 0;
}

void nPM1300::triggerMeasurement(uint8_t taskReg) {
  i2cWrite(taskReg, 0x01);  // Trigger
  delay(1);  // Wait for conversion (250us typ, but safe)
}

uint16_t nPM1300::readAdcResult(uint8_t msbReg, uint8_t lsbReg, uint8_t lsbShift) {
  uint8_t msb, lsbs;
  i2cRead(msbReg, &msb);
  i2cRead(lsbReg, &lsbs);
  return (msb << 2) | ((lsbs >> lsbShift) & 0x03);  // 10-bit
}

float nPM1300::calculateVbat(uint16_t adcVal) {
  return (adcVal / 1023.0f) * VFS_VBAT;
}

float nPM1300::calculateIbat(uint16_t adcVal, bool charging) {
  float fs = charging ? 1.25 : 0.836;  // From datasheet
  // Read ISET registers for full scale (placeholder)
  return (adcVal / 1023.0f) * fs * 1000.0;  // mA
}

float nPM1300::calculateTbat(uint16_t adcVal) {
  float tbat_adc = adcVal / 1023.0f;
  return 1.0f / (1.0f / T0_KELVIN + (1.0f / ntcBeta) * log(1.0f / tbat_adc - 1.0f));
}

float nPM1300::calculateTdie(uint16_t adcVal) {
  return 394.67f - 0.7926f * (adcVal / 1023.0f * VFS_TEMP);  // From equation
}

float nPM1300::calculateVsys(uint16_t adcVal) {
  return (adcVal / 1023.0f) * VFS_VSYS;
}

float nPM1300::calculateVbus(uint16_t adcVal) {
  return (adcVal / 1023.0f) * VFS_VBUS;
}

void nPM1300::disableAutoMeasurements() {
  i2cWrite(ADC_CONFIG, 0x00);  // Disable auto
  i2cWrite(TASK_AUTO_TIM_UPDATE, 0x00);
}

// I2C Implementations
int nPM1300::i2cWrite(uint8_t reg, uint8_t val) {
#ifdef ARDUINO
  Wire.beginTransmission(NPM1300_I2C_ADDR);
  Wire.write(reg);
  Wire.write(val);
  return Wire.endTransmission() ? NPM_ERR_I2C_FAIL : NPM_ERR_SUCCESS;
#elif defined(USE_ZEPHYR)
  uint8_t buf[2] = {reg, val};
  return i2c_write(i2c_dev, buf, 2, NPM1300_I2C_ADDR);
#else
  // nRF SDK TWI implementation (user to fill)
  return NPM_ERR_SUCCESS;
#endif
}

int nPM1300::i2cRead(uint8_t reg, uint8_t* val) {
#ifdef ARDUINO
  Wire.beginTransmission(NPM1300_I2C_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(NPM1300_I2C_ADDR, 1);
  if (Wire.available()) {
    *val = Wire.read();
    return NPM_ERR_SUCCESS;
  }
  return NPM_ERR_I2C_FAIL;
#elif defined(USE_ZEPHYR)
  return i2c_write_read(i2c_dev, NPM1300_I2C_ADDR, &reg, 1, val, 1);
#else
  // nRF SDK
  return NPM_ERR_SUCCESS;
#endif
}

// Multi-byte versions similar (implement as needed)


// For Zephyr Optimization: Use low-power modes, disable unused peripherals in device tree.
// Example Zephyr app: Integrate in prj.conf with CONFIG_I2C=y, and call init() in main.
