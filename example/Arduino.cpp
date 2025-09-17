#include <Wire.h>
#include "nPM1300.h"

nPM1300 pmic;

void setup() {
  Serial.begin(115200);
  Wire.begin();  // I2C setup

  if (pmic.init() != NPM_ERR_SUCCESS) {
    Serial.println("nPM1300 init failed!");
    while (1);
  }

  // Optional overrides (uncomment to change defaults)
  // pmic.setBuckVoltage(1, 3.0);  // Change Buck1 to 3.0V
  // pmic.enableLdo(2, true);      // Enable LDO2
  // pmic.setLdoVoltage(2, 1.8);   // Set LDO2 to 1.8V
  // pmic.setChargeCurrent(200.0); // Change charge current to 200mA
  // pmic.setNtcBeta(3380);        // Custom NTC beta

  Serial.println("nPM1300 initialized with defaults.");
}

void loop() {
  // Read and print measurements
  float vbat = pmic.getBatteryVoltage();
  float ibat = pmic.getBatteryCurrent();
  float tbat = pmic.getBatteryTemperature();
  float tdie = pmic.getDieTemperature();
  float vsys = pmic.getVsysVoltage();
  float vbus = pmic.getVbusVoltage();

  // Update fuel gauge (call periodically)
  pmic.updateFuelGauge();

  float soc = pmic.getStateOfCharge();
  float soh = pmic.getStateOfHealth();
  int chargeStatus = pmic.getChargeStatus();
  float chargeV = pmic.getChargeVoltage();
  float chargeI = pmic.getChargeCurrent();

  Serial.print("VBAT: "); Serial.print(vbat); Serial.println(" V");
  Serial.print("IBAT: "); Serial.print(ibat); Serial.println(" mA");
  Serial.print("TBAT: "); Serial.print(tbat); Serial.println(" °C");
  Serial.print("TDIE: "); Serial.print(tdie); Serial.println(" °C");
  Serial.print("VSYS: "); Serial.print(vsys); Serial.println(" V");
  Serial.print("VBUS: "); Serial.print(vbus); Serial.println(" V");
  Serial.print("SOC: "); Serial.print(soc); Serial.println(" %");
  Serial.print("SOH: "); Serial.print(soh); Serial.println(" %");
  Serial.print("Charge Status: "); Serial.println(chargeStatus == 1 ? "Charging" : chargeStatus == 2 ? "Full" : "Not Charging");
  Serial.print("Charge Voltage: "); Serial.print(chargeV); Serial.println(" V");
  Serial.print("Charge Current: "); Serial.print(chargeI); Serial.println(" mA");
  Serial.print("Last Error: "); Serial.println(pmic.getLastError());

  delay(5000);  // Read every 5 seconds for low power
}
