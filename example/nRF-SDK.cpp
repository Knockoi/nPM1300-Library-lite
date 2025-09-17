#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>
#include "nPM1300.h"

LOG_MODULE_REGISTER(main);

nPM1300 pmic;

void main(void) {
  if (pmic.init() != NPM_ERR_SUCCESS) {
    LOG_ERR("nPM1300 init failed!");
    return;
  }

  // Optional overrides
  // pmic.setBuckVoltage(1, 3.0);
  // pmic.enableLdo(2, true);
  // pmic.setLdoVoltage(2, 1.8);

  LOG_INF("nPM1300 initialized.");

  while (1) {
    float vbat = pmic.getBatteryVoltage();
    float ibat = pmic.getBatteryCurrent();
    float tbat = pmic.getBatteryTemperature();
    float tdie = pmic.getDieTemperature();
    float vsys = pmic.getVsysVoltage();
    float vbus = pmic.getVbusVoltage();

    pmic.updateFuelGauge();

    float soc = pmic.getStateOfCharge();
    float soh = pmic.getStateOfHealth();
    int chargeStatus = pmic.getChargeStatus();
    float chargeV = pmic.getChargeVoltage();
    float chargeI = pmic.getChargeCurrent();

    LOG_INF("VBAT: %.2f V, IBAT: %.2f mA, TBAT: %.2f °C, TDIE: %.2f °C", vbat, ibat, tbat, tdie);
    LOG_INF("VSYS: %.2f V, VBUS: %.2f V, SOC: %.2f %%, SOH: %.2f %%", vsys, vbus, soc, soh);
    LOG_INF("Charge Status: %d, Charge V: %.2f V, Charge I: %.2f mA", chargeStatus, chargeV, chargeI);
    LOG_INF("Last Error: %d", pmic.getLastError());

    k_msleep(5000);  // Low power sleep
  }
}
