#include <NPM1300.h>

NPM1300 pmic;

void setup() {
    Serial.begin(115200);
    delay(2000);  // 等待序列埠
    
    Serial.println("\n=== NPM1300 Test ===");
    
    // 初始化 NPM1300
    if (!pmic.begin()) {
        Serial.println("ERROR: NPM1300 not found!");
        Serial.println("Check I2C connections:");
        Serial.println("  SDA -> GPIO21 (ESP32)");
        Serial.println("  SCL -> GPIO22 (ESP32)");
        Serial.println("  VDD -> 3.3V");
        Serial.println("  GND -> GND");
        while (1) {
            delay(1000);
        }
    }
    
    Serial.println("NPM1300 OK!");
    Serial.println();
    
    // 設定 BUCK 電壓
    Serial.println("Setting BUCK voltages...");
    pmic.writeBuck1(3.3);
    pmic.writeBuck2(3.3);
    Serial.println("  BUCK1: 3.3V");
    Serial.println("  BUCK2: 3.3V");
    Serial.println();
    
    // 設定充電參數
    Serial.println("Configuring charger...");
    pmic.writeChargeCurrent(100);   // 100mA
    pmic.writeChargeVoltage(4.2);   // 4.2V
    pmic.enableCharger();
    Serial.println("  Current: 100mA");
    Serial.println("  Voltage: 4.2V");
    Serial.println();
    
    Serial.println("Setup complete!");
    Serial.println("====================\n");
}

void loop() {
    // 讀取電壓
    uint16_t vbat = pmic.readVBAT();
    uint16_t vbus = pmic.readVBUS();
    uint16_t vsys = pmic.readVSYS();
    
    // 讀取電量
    uint8_t soc = pmic.readSOC();
    
    // 讀取充電狀態
    bool charging = pmic.isCharging();
    bool full = pmic.isBatteryFull();
    
    // 顯示資訊
    Serial.println("--- Status ---");
    Serial.print("VBAT: ");
    Serial.print(vbat);
    Serial.println(" mV");
    
    Serial.print("VBUS: ");
    Serial.print(vbus);
    Serial.println(" mV");
    
    Serial.print("VSYS: ");
    Serial.print(vsys);
    Serial.println(" mV");
    
    Serial.print("SOC:  ");
    Serial.print(soc);
    Serial.println(" %");
    
    Serial.print("Charging: ");
    if (charging) {
        Serial.println("YES");
    } else if (full) {
        Serial.println("FULL");
    } else {
        Serial.println("NO");
    }
    
    Serial.println("--------------\n");
    
    delay(2000);  // 每 2 秒更新一次
}
