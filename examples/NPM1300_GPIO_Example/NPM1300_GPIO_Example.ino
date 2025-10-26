#include <NPM1300.h>

NPM1300 pmic;

// GPIO definition
#define LED_PIN 0 // GPIO0 connected to LED
#define BUTTON_PIN 1 // GPIO1 connected to button
#define IRQ_PIN 2 // GPIO2 as interrupt output

void setup() {
Serial.begin(115200);

while (!Serial) delay(10);

Serial.println("NPM1300 GPIO Test");

// Initialize NPM1300

if (!pmic.begin()) {

Serial.println("PMIC init failed!");

while (1);

}

Serial.println("PMIC OK!");

// === GPIO configuration ===

// GPIO0: output mode (control LED)

pmic.setGpioMode(LED_PIN, NPM1300_GPIO_OUT_LOGIC0); // Initially LOW
Serial.println("GPIO0: Output (LED)");

// GPIO1: Input mode (read button)
pmic.setGpioMode(BUTTON_PIN, NPM1300_GPIO_INPUT);
pmic.setGpioPullUp(BUTTON_PIN, true); // Enable pull-up resistor
Serial.println("GPIO1: Input with Pull-up (Button)");

// GPIO2: Interrupt output (can be connected to MCU interrupt pin)
pmic.setGpioMode(IRQ_PIN, NPM1300_GPIO_OUT_IRQ);
Serial.println("GPIO2: Interrupt Output");

Serial.println("\nSetup complete!");
Serial.println("Press button to toggle LED");
}

void loop() {
// Read button status
bool buttonPressed = !pmic.getGpioInput(BUTTON_PIN); // Low = Pressed

if (buttonPressed) {
Serial.println("Button Pressed!");

// Toggle LED status
static bool ledState = false;
ledState = !ledState;

pmic.setGpioOutput(LED_PIN, ledState);
Serial.print("LED: ");
Serial.println(ledState ? "ON" : "OFF");

// Anti-bounce delay
delay(200);
}

// Also display battery information
static unsigned long lastPrint = 0;
if (millis() - lastPrint > 2000) {
lastPrint = millis();

Serial.println("\n--- Status ---");
Serial.print("VBAT: ");
Serial.print(pmic.readVBAT());
Serial.println("mV");

Serial.print("Charging: ");
Serial.println(pmic.isCharging() ? "Yes" : "No");
Serial.println("--------------\n");
}

delay(50);
}
