/*
 * NPM1300 Test Example
 * 
 * This example demonstrates how to use the NPM1300 library
 * to read battery voltage, current, SOC and other information
 */

#include <NPM1300.h>

NPM1300 pmic;

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    
    Serial.println("NPM1300 Test");
    Serial.println("====================");
    
    // Initialize NPM1300
    if (!pmic.begin()) {
        Serial.println("Failed to initialize NPM1300!");
        Serial.println("Check I2C connection");
        while (1) delay(100);
    }
    
    Serial.println("NPM1300 initialized successfully!");
    Serial.println("BUCK1 and BUCK2 set to 3.3V");
    Serial.println();
}

void loop() {
    Serial.println("==== NPM1300 Status ====");
    
    // Read Voltages
    uint16_t vbus = pmic.getVbusVoltage();
    uint16_t vbat = pmic.getVbatVoltage();
    uint16_t vsys = pmic.getVsysVoltage();
    
    Serial.print("VBUS: ");
    Serial.print(vbus);
    Serial.println(" mV");
    
    Serial.print("VBAT: ");
    Serial.print(vbat);
    Serial.println(" mV");
    
    Serial.print("VSYS: ");
    Serial.print(vsys);
    Serial.println(" mV");
    
    // Read Current
    int16_t ibat = pmic.getBatCurrent();
    Serial.print("Battery Current: ");
    Serial.print(ibat);
    Serial.println(" mA");
    
    // Read Temperatures
    int8_t batTemp = pmic.getBatTemperature();
    int8_t dieTemp = pmic.getDieTemperature();
    
    Serial.print("Battery Temperature: ");
    Serial.print(batTemp);
    Serial.println(" °C");
    
    Serial.print("Die Temperature: ");
    Serial.print(dieTemp);
    Serial.println(" °C");
    
    // Read Battery State
    uint8_t soc = pmic.getBatterySOC();
    Serial.print("Battery SOC: ");
    Serial.print(soc);
    Serial.println(" %");
    
    // Charging Status
    if (pmic.isCharging()) {
        Serial.println("Status: Charging");
    } else if (pmic.isBatteryFull()) {
        Serial.println("Status: Battery Full");
    } else {
        Serial.println("Status: Discharging");
    }
    
    Serial.println();
    delay(2000);  // Update every 2 seconds
}
