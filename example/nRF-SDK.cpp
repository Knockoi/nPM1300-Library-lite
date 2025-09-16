#include "nPM1300.h"
#include "nrf_drv_twi.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "app_timer.h"

// TWI instance
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(0);
static nPM1300* pmic = nullptr;

// Timer
APP_TIMER_DEF(status_timer);

static void twi_init(void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_config = {
       .scl                = 27,  // Adjust according to your hardware
       .sda                = 26,  // Adjust according to your hardware
       .frequency          = NRF_DRV_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
}

static void status_timer_handler(void * p_context)
{
    if (pmic == nullptr) return;
    
    // Read battery information
    float batt_voltage = pmic->getBatteryVoltage();
    uint8_t batt_percent = pmic->getBatteryPercent();
    float sys_voltage = pmic->getSystemVoltage();
    
    NRF_LOG_INFO("=== nPM1300 Status ===");
    NRF_LOG_INFO("Battery Voltage: " NRF_LOG_FLOAT_MARKER "V", NRF_LOG_FLOAT(batt_voltage));
    NRF_LOG_INFO("Battery Level: %d%%", batt_percent);
    NRF_LOG_INFO("System Voltage: " NRF_LOG_FLOAT_MARKER "V", NRF_LOG_FLOAT(sys_voltage));
    
    if (pmic->isCharging()) {
        NRF_LOG_INFO("Charging");
    } else {
        NRF_LOG_INFO("Not Charging");
    }
    
    if (pmic->isBatteryLow()) {
        NRF_LOG_WARNING("Low Battery!");
    }
    
    if (!pmic->isPowerGood()) {
        NRF_LOG_ERROR("Power Fault!");
    }
    
    // Check interrupts
    uint8_t int_status = pmic->getInterruptStatus();
    if (int_status != 0) {
        NRF_LOG_INFO("Interrupt Status: 0x%02X", int_status);
        pmic->clearInterrupts();
    }
}

static void timers_init(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
    
    // Create a 5-second repeated timer
    err_code = app_timer_create(&status_timer,
                                APP_TIMER_MODE_REPEATED,
                                status_timer_handler);
    APP_ERROR_CHECK(err_code);
}

static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

int main(void)
{
    // Initialize logging
    log_init();
    
    // Initialize timers
    timers_init();
    
    // Initialize TWI
    twi_init();
    
    NRF_LOG_INFO("nPM1300 nRF SDK Example Started");
    
    // Create nPM1300 instance
    pmic = new nPM1300(&m_twi);
    
    // Initialize nPM1300
    if (!pmic->begin()) {
        NRF_LOG_ERROR("nPM1300 Initialization Failed!");
        while(1);
    }
    
    NRF_LOG_INFO("nPM1300 Initialization Successful!");
    
    // Start status monitoring timer
    ret_code_t err_code = app_timer_start(status_timer, APP_TIMER_TICKS(5000), NULL);
    APP_ERROR_CHECK(err_code);
    
    // Main loop
    while (true)
    {
        if (NRF_LOG_PROCESS() == false)
        {
            // Enter low-power mode
            __WFE();
        }
    }
}
