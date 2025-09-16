#include "nPM1300.h"

// Create nPM1300 instance
nPM1300 pmic;

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing nPM1300...");
    
    // Initialize nPM1300
    if (!pmic.begin()) {
        Serial.println("nPM1300 initialization failed!");
        while(1);
    }
    
    Serial.println("nPM1300 initialized successfully!");
    
    // Show initial status
    printStatus();
}

void loop() {
    // Update status every 5 seconds
    printStatus();
    delay(5000);
    
    // Check interrupts
    uint8_t intStatus = pmic.getInterruptStatus();
    if (intStatus != 0) {
        Serial.print("Interrupt status: 0x");
        Serial.println(intStatus, HEX);
        pmic.clearInterrupts();
    }
}

void printStatus() {
    Serial.println("=== nPM1300 Status ===");
    
    // Battery information
    float battVoltage = pmic.getBatteryVoltage();
    uint8_t battPercent = pmic.getBatteryPercent();
    float sysVoltage = pmic.getSystemVoltage();
    
    Serial.print("Battery Voltage: ");
    Serial.print(battVoltage);
    Serial.println("V");
    
    Serial.print("Battery Level: ");
    Serial.print(battPercent);
    Serial.println("%");
    
    Serial.print("System Voltage: ");
    Serial.print(sysVoltage);
    Serial.println("V");
    
    // Charging status
    if (pmic.isCharging()) {
        Serial.println("Charging");
    } else {
        Serial.println("Not Charging");
    }
    
    // Low battery warning
    if (pmic.isBatteryLow()) {
        Serial.println("Low Battery!");
    }
    
    // Power status
    if (pmic.isPowerGood()) {
        Serial.println("Power Good");
    } else {
        Serial.println("Power Fault");
    }
    
    Serial.println("==================");
    Serial.println();
}
