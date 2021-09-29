#include "main.h"


/**@brief Application main function.
 */
int main(void)
{
    ret_code_t err_code;

    bool erase_bonds;

    // Initialize.
    gpio_init();
    uart_init();
    log_init();
    lfclk_config();
    timers_init();
    buttons_leds_init(&erase_bonds);
    ble_stack_init();
    gap_params_init();
    gatt_init();
    services_init();
    advertising_init();

    // Advertising Strength selected 
    err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, m_advertising.adv_handle, +4);
    APP_ERROR_CHECK(err_code);

    conn_params_init();

    saadc_sampling_event_init();
    saadc_init();
    saadc_sampling_event_enable();

    // Start execution.
    printf("\r\nUART started.\r\n");
    NRF_LOG_INFO("Debug logging for UART over RTT started.");
    advertising_start();

    // Enter main loop.
    for (;;)
    {
    }
}


/**
 * @}
 */