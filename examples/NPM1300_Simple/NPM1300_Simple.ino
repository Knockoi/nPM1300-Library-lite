/*
 * NPM1300 Simple Example (Arduino Style)
 * 
 * Ultra simple example using Arduino-style read/write functions
 */

#include <NPM1300.h>

NPM1300 pmic;

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    
    Serial.println("NPM1300 Simple Test");
    
    // Initialize NPM1300
    if (!pmic.begin()) {
        Serial.println("PMIC init failed!");
        while (1);
    }
    
    Serial.println("PMIC OK!");
    
    // Set BUCK voltages (Arduino style!)
    pmic.writeBuck1(3.3);  // Set BUCK1 to 3.3V
    pmic.writeBuck2(1.8);  // Set BUCK2 to 1.8V
    
    // Set charging parameters
    pmic.writeChargeCurrent(100);    // 100mA charge current
    pmic.writeChargeVoltage(4.2);    // 4.2V charge termination
    pmic.enableCharger();            // Enable charging
    
    Serial.println("Config done!");
    Serial.println("BUCK1: 3.3V");
    Serial.println("BUCK2: 1.8V");
    Serial.println("Charge: 100mA @ 4.2V");
    Serial.println();
}

void loop() {
    // Read values - Arduino style!
    uint16_t vbat = pmic.readVBAT();      // Battery voltage
    uint16_t vbus = pmic.readVBUS();      // USB voltage
    int16_t current = pmic.readCurrent(); // Battery current
    uint8_t soc = pmic.readSOC();         // Battery %
    int8_t temp = pmic.readTemp();        // Temperature
    
    // Print results
    Serial.print("Battery: ");
    Serial.print(vbat);
    Serial.print("mV  ");
    
    Serial.print(soc);
    Serial.print("%  ");
    
    Serial.print(current);
    Serial.print("mA  ");
    
    Serial.print(temp);
    Serial.print("°C  ");
    
    // Charging status
    if (pmic.isCharging()) {
        Serial.print("[Charging]");
    } else if (pmic.isBatteryFull()) {
        Serial.print("[Full]");
    } else {
        Serial.print("[Discharging]");
    }
    
    Serial.println();
    
    delay(1000);
}
