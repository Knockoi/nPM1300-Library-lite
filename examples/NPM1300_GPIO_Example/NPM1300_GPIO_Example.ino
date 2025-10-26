#include <NPM1300.h>

NPM1300 pmic;

// GPIO 定義
#define LED_PIN     0  // GPIO0 連接 LED
#define BUTTON_PIN  1  // GPIO1 連接按鈕
#define IRQ_PIN     2  // GPIO2 作為中斷輸出

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    
    Serial.println("NPM1300 GPIO Test");
    
    // 初始化 NPM1300
    if (!pmic.begin()) {
        Serial.println("PMIC init failed!");
        while (1);
    }
    
    Serial.println("PMIC OK!");
    
    // === GPIO 配置 ===
    
    // GPIO0: 輸出模式 (控制 LED)
    pmic.setGpioMode(LED_PIN, NPM1300_GPIO_OUT_LOGIC0);  // 初始為 LOW
    Serial.println("GPIO0: Output (LED)");
    
    // GPIO1: 輸入模式 (讀取按鈕)
    pmic.setGpioMode(BUTTON_PIN, NPM1300_GPIO_INPUT);
    pmic.setGpioPullUp(BUTTON_PIN, true);  // 啟用上拉電阻
    Serial.println("GPIO1: Input with Pull-up (Button)");
    
    // GPIO2: 中斷輸出 (可連接到 MCU 中斷腳)
    pmic.setGpioMode(IRQ_PIN, NPM1300_GPIO_OUT_IRQ);
    Serial.println("GPIO2: Interrupt Output");
    
    Serial.println("\nSetup complete!");
    Serial.println("Press button to toggle LED");
}

void loop() {
    // 讀取按鈕狀態
    bool buttonPressed = !pmic.getGpioInput(BUTTON_PIN);  // 低電位 = 按下
    
    if (buttonPressed) {
        Serial.println("Button Pressed!");
        
        // 切換 LED 狀態
        static bool ledState = false;
        ledState = !ledState;
        
        pmic.setGpioOutput(LED_PIN, ledState);
        Serial.print("LED: ");
        Serial.println(ledState ? "ON" : "OFF");
        
        // 防彈跳延遲
        delay(200);
    }
    
    // 也顯示電池資訊
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 2000) {
        lastPrint = millis();
        
        Serial.println("\n--- Status ---");
        Serial.print("VBAT: ");
        Serial.print(pmic.readVBAT());
        Serial.println(" mV");
        
        Serial.print("Charging: ");
        Serial.println(pmic.isCharging() ? "Yes" : "No");
        Serial.println("--------------\n");
    }
    
    delay(50);
}
