#ifndef NPM1300_H
#define NPM1300_H

#include <Arduino.h>
#include <Wire.h>

// I2C Address
#define NPM1300_I2C_ADDR 0x6B

// Base Addresses (從說明書確認)
#define NPM1300_MAIN_BASE       0x0000
#define NPM1300_VBUSIN_BASE     0x0200
#define NPM1300_BCHARGER_BASE   0x0300
#define NPM1300_BUCK_BASE       0x0400
#define NPM1300_ADC_BASE        0x0500
#define NPM1300_LDSW_BASE       0x0800

// BUCK Register Addresses (BUCK_BASE + offset)
#define NPM1300_BUCK1_ENASET        (NPM1300_BUCK_BASE + 0x00)  // 0x0400
#define NPM1300_BUCK1_ENACLR        (NPM1300_BUCK_BASE + 0x01)  // 0x0401
#define NPM1300_BUCK2_ENASET        (NPM1300_BUCK_BASE + 0x02)  // 0x0402
#define NPM1300_BUCK2_ENACLR        (NPM1300_BUCK_BASE + 0x03)  // 0x0403
#define NPM1300_BUCK1_PWMSET        (NPM1300_BUCK_BASE + 0x04)  // 0x0404
#define NPM1300_BUCK1_PWMCLR        (NPM1300_BUCK_BASE + 0x05)  // 0x0405
#define NPM1300_BUCK2_PWMSET        (NPM1300_BUCK_BASE + 0x06)  // 0x0406
#define NPM1300_BUCK2_PWMCLR        (NPM1300_BUCK_BASE + 0x07)  // 0x0407
#define NPM1300_BUCK1_NORMVOUT      (NPM1300_BUCK_BASE + 0x08)  // 0x0408
#define NPM1300_BUCK1_RETVOUT       (NPM1300_BUCK_BASE + 0x09)  // 0x0409
#define NPM1300_BUCK2_NORMVOUT      (NPM1300_BUCK_BASE + 0x0A)  // 0x040A
#define NPM1300_BUCK2_RETVOUT       (NPM1300_BUCK_BASE + 0x0B)  // 0x040B
#define NPM1300_BUCKENCTRL          (NPM1300_BUCK_BASE + 0x0C)  // 0x040C
#define NPM1300_BUCKVRETCTRL        (NPM1300_BUCK_BASE + 0x0D)  // 0x040D
#define NPM1300_BUCKPWMCTRL         (NPM1300_BUCK_BASE + 0x0E)  // 0x040E
#define NPM1300_BUCKSWCTRLSEL       (NPM1300_BUCK_BASE + 0x0F)  // 0x040F
#define NPM1300_BUCK1_VOUTSTATUS    (NPM1300_BUCK_BASE + 0x10)  // 0x0410
#define NPM1300_BUCK2_VOUTSTATUS    (NPM1300_BUCK_BASE + 0x11)  // 0x0411
#define NPM1300_BUCKCTRL0           (NPM1300_BUCK_BASE + 0x15)  // 0x0415
#define NPM1300_BUCKSTATUS          (NPM1300_BUCK_BASE + 0x34)  // 0x0434

// CHARGER Register Addresses (BCHARGER_BASE + offset)
#define NPM1300_TASK_RELEASEERR         (NPM1300_BCHARGER_BASE + 0x00)  // 0x0300
#define NPM1300_TASK_CLEARCHGERR        (NPM1300_BCHARGER_BASE + 0x01)  // 0x0301
#define NPM1300_TASK_CLEARSAFETYTIMER   (NPM1300_BCHARGER_BASE + 0x02)  // 0x0302
#define NPM1300_BCHG_ENABLESET          (NPM1300_BCHARGER_BASE + 0x04)  // 0x0304
#define NPM1300_BCHG_ENABLECLR          (NPM1300_BCHARGER_BASE + 0x05)  // 0x0305
#define NPM1300_BCHG_DISABLESET         (NPM1300_BCHARGER_BASE + 0x06)  // 0x0306
#define NPM1300_BCHG_DISABLECLR         (NPM1300_BCHARGER_BASE + 0x07)  // 0x0307
#define NPM1300_BCHG_ISETMSB            (NPM1300_BCHARGER_BASE + 0x08)  // 0x0308
#define NPM1300_BCHG_ISETLSB            (NPM1300_BCHARGER_BASE + 0x09)  // 0x0309
#define NPM1300_BCHG_IDISCHARGEMSB      (NPM1300_BCHARGER_BASE + 0x0A)  // 0x030A
#define NPM1300_BCHG_IDISCHARGELSB      (NPM1300_BCHARGER_BASE + 0x0B)  // 0x030B
#define NPM1300_BCHG_VTERM              (NPM1300_BCHARGER_BASE + 0x0C)  // 0x030C
#define NPM1300_BCHG_VTERMR             (NPM1300_BCHARGER_BASE + 0x0D)  // 0x030D
#define NPM1300_BCHG_VTRICKLESEL        (NPM1300_BCHARGER_BASE + 0x0E)  // 0x030E
#define NPM1300_BCHG_ITERMSEL           (NPM1300_BCHARGER_BASE + 0x0F)  // 0x030F
#define NPM1300_NTC_COLD                (NPM1300_BCHARGER_BASE + 0x10)  // 0x0310
#define NPM1300_NTC_COLDLSB             (NPM1300_BCHARGER_BASE + 0x11)  // 0x0311
#define NPM1300_NTC_COOL                (NPM1300_BCHARGER_BASE + 0x12)  // 0x0312
#define NPM1300_NTC_COOLLSB             (NPM1300_BCHARGER_BASE + 0x13)  // 0x0313
#define NPM1300_NTC_WARM                (NPM1300_BCHARGER_BASE + 0x14)  // 0x0314
#define NPM1300_NTC_WARMLSB             (NPM1300_BCHARGER_BASE + 0x15)  // 0x0315
#define NPM1300_NTC_HOT                 (NPM1300_BCHARGER_BASE + 0x16)  // 0x0316
#define NPM1300_NTC_HOTLSB              (NPM1300_BCHARGER_BASE + 0x17)  // 0x0317
#define NPM1300_DIETEMP_STOP            (NPM1300_BCHARGER_BASE + 0x18)  // 0x0318
#define NPM1300_DIETEMP_STOPLSB         (NPM1300_BCHARGER_BASE + 0x19)  // 0x0319
#define NPM1300_DIETEMP_RESUME          (NPM1300_BCHARGER_BASE + 0x1A)  // 0x031A
#define NPM1300_DIETEMP_RESUMELSB       (NPM1300_BCHARGER_BASE + 0x1B)  // 0x031B
#define NPM1300_BCHG_ILIMSTATUS         (NPM1300_BCHARGER_BASE + 0x2D)  // 0x032D
#define NPM1300_NTC_STATUS              (NPM1300_BCHARGER_BASE + 0x32)  // 0x0332
#define NPM1300_DIETEMP_STATUS          (NPM1300_BCHARGER_BASE + 0x33)  // 0x0333
#define NPM1300_BCHG_CHARGESTATUS       (NPM1300_BCHARGER_BASE + 0x34)  // 0x0334
#define NPM1300_BCHG_ERRREASON          (NPM1300_BCHARGER_BASE + 0x36)  // 0x0336
#define NPM1300_BCHG_ERRSENSOR          (NPM1300_BCHARGER_BASE + 0x37)  // 0x0337
#define NPM1300_BCHG_CONFIG             (NPM1300_BCHARGER_BASE + 0x3C)  // 0x033C

// ADC/Monitor Register Addresses (ADC_BASE + offset)
#define NPM1300_TASK_VBAT_MEAS      (NPM1300_ADC_BASE + 0x00)  // 0x0500
#define NPM1300_TASK_NTC_MEAS       (NPM1300_ADC_BASE + 0x01)  // 0x0501
#define NPM1300_TASK_TEMP_MEAS      (NPM1300_ADC_BASE + 0x02)  // 0x0502
#define NPM1300_TASK_VSYS_MEAS      (NPM1300_ADC_BASE + 0x03)  // 0x0503
#define NPM1300_TASK_VBUS_MEAS      (NPM1300_ADC_BASE + 0x07)  // 0x0507
#define NPM1300_ADC_VBAT_MSB        (NPM1300_ADC_BASE + 0x11)  // 0x0511
#define NPM1300_ADC_NTC_MSB         (NPM1300_ADC_BASE + 0x12)  // 0x0512
#define NPM1300_ADC_TEMP_MSB        (NPM1300_ADC_BASE + 0x13)  // 0x0513
#define NPM1300_ADC_VSYS_MSB        (NPM1300_ADC_BASE + 0x14)  // 0x0514
#define NPM1300_ADC_GP0_LSBS        (NPM1300_ADC_BASE + 0x15)  // 0x0515
#define NPM1300_ADC_VBAT2_MSB       (NPM1300_ADC_BASE + 0x18)  // 0x0518 (IBAT)
#define NPM1300_ADC_VBAT3_MSB       (NPM1300_ADC_BASE + 0x19)  // 0x0519 (VBUS)
#define NPM1300_ADC_GP1_LSBS        (NPM1300_ADC_BASE + 0x1A)  // 0x051A
#define NPM1300_ADC_IBAT_MEASEN     (NPM1300_ADC_BASE + 0x24)  // 0x0524

// GPIO Register Addresses (GPIO_BASE + offset)
#define NPM1300_GPIO_BASE           0x0600
#define NPM1300_GPIO_MODE0          (NPM1300_GPIO_BASE + 0x00)  // 0x0600
#define NPM1300_GPIO_MODE1          (NPM1300_GPIO_BASE + 0x01)  // 0x0601
#define NPM1300_GPIO_MODE2          (NPM1300_GPIO_BASE + 0x02)  // 0x0602
#define NPM1300_GPIO_MODE3          (NPM1300_GPIO_BASE + 0x03)  // 0x0603
#define NPM1300_GPIO_MODE4          (NPM1300_GPIO_BASE + 0x04)  // 0x0604
#define NPM1300_GPIO_DRIVE0         (NPM1300_GPIO_BASE + 0x05)  // 0x0605
#define NPM1300_GPIO_PUEN0          (NPM1300_GPIO_BASE + 0x0A)  // 0x060A
#define NPM1300_GPIO_PDEN0          (NPM1300_GPIO_BASE + 0x0F)  // 0x060F
#define NPM1300_GPIO_OPENDRAIN0     (NPM1300_GPIO_BASE + 0x14)  // 0x0614
#define NPM1300_GPIO_DEBOUNCE0      (NPM1300_GPIO_BASE + 0x19)  // 0x0619
#define NPM1300_GPIO_STATUS         (NPM1300_GPIO_BASE + 0x1E)  // 0x061E

// GPIO Mode Values
#define NPM1300_GPIO_INPUT          0   // GPI Input
#define NPM1300_GPIO_LOGIC1         1   // GPI Logic 1
#define NPM1300_GPIO_LOGIC0         2   // GPI Logic 0
#define NPM1300_GPIO_EVENT_RISE     3   // GPI Rising Edge Event
#define NPM1300_GPIO_EVENT_FALL     4   // GPI Falling Edge Event
#define NPM1300_GPIO_OUT_IRQ        5   // GPO Interrupt
#define NPM1300_GPIO_OUT_RESET      6   // GPO Reset
#define NPM1300_GPIO_OUT_PLW        7   // GPO Power Loss Warning
#define NPM1300_GPIO_OUT_LOGIC1     8   // GPO Logic 1
#define NPM1300_GPIO_OUT_LOGIC0     9   // GPO Logic 0

#define NPM1300_VBUS_VOLTAGE        0x30
#define NPM1300_VBAT_VOLTAGE        0x32
#define NPM1300_VSYS_VOLTAGE        0x34
#define NPM1300_IBAT_CURRENT        0x36
#define NPM1300_BAT_TEMP            0x38
#define NPM1300_DIE_TEMP            0x3A

#define NPM1300_BATCHG_STATUS       0x40
#define NPM1300_BAT_SOC             0x42

#define NPM1300_CHARGER_EN          0x50
#define NPM1300_CHARGER_ISET        0x51
#define NPM1300_CHARGER_VTERM       0x52

class NPM1300 {
public:
    NPM1300(TwoWire &wirePort = Wire);
    
    // Initialization
    bool begin();
    bool isConnected();
    
    // BUCK Configuration
    bool enableBuck1();
    bool enableBuck2();
    bool disableBuck1();
    bool disableBuck2();
    bool setBuck1Voltage(float voltage);
    bool setBuck2Voltage(float voltage);
    
    // Voltage Readings (in mV)
    uint16_t getVbusVoltage();
    uint16_t getVbatVoltage();
    uint16_t getVsysVoltage();
    
    // Current Reading (in mA)
    int16_t getBatCurrent();
    
    // Temperature (in °C)
    int8_t getBatTemperature();
    int8_t getDieTemperature();
    
    // Battery State
    uint8_t getBatterySOC();  // State of Charge (%)
    bool isCharging();
    bool isBatteryFull();
    
    // Simplified Read Functions (Arduino style)
    uint16_t readVBUS();      // Read VBUS voltage in mV
    uint16_t readVBAT();      // Read battery voltage in mV
    uint16_t readVSYS();      // Read system voltage in mV
    int16_t readCurrent();    // Read battery current in mA
    uint8_t readSOC();        // Read battery percentage (0-100%)
    int8_t readTemp();        // Read battery temperature in °C
    
    // Simplified Write Functions (Arduino style)
    bool writeBuck1(float voltage);      // Set BUCK1 voltage (1.0-3.3V)
    bool writeBuck2(float voltage);      // Set BUCK2 voltage (1.0-3.3V)
    bool writeChargeCurrent(uint16_t mA); // Set charge current in mA
    bool writeChargeVoltage(float voltage); // Set charge termination voltage
    
    // Charger Control
    bool enableCharger();
    bool disableCharger();
    bool setChargeCurrent(uint16_t currentMa);
    bool setChargeVoltage(float voltage);
    
private:
    TwoWire *_i2c;
    
    // I2C Helper Functions
    bool writeRegister(uint8_t reg, uint8_t value);
    bool readRegister(uint8_t reg, uint8_t *value);
    bool readRegisters(uint8_t reg, uint8_t *buffer, uint8_t len);
    
    // Conversion Functions
    uint8_t voltageToRegValue(float voltage);
    float regValueToVoltage(uint8_t regValue);
};

#endif // NPM1300_H
