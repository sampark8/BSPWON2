#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_drv_timer.h"
#include "bsp.h"
#include "app_error.h"

const nrfx_timer_t TIMER_LED = NRFX_TIMER_INSTANCE(0);  // Timer 0 enabled


#define PIN1 12
#define PIN2 13
#define PIN3 14
#define PIN4 15
void timer0_handler(nrf_timer_event_t event_type, void* p_context)
{
  static uint32_t i,x, val;
  bool status1, status2, status3, status4;
  
  val = (i++) % (5);
  x = val + 12;
  switch(event_type)
  {
    
    case NRF_TIMER_EVENT_COMPARE0:
    status1 = nrf_gpio_pin_out_read(PIN1);
    status2 = nrf_gpio_pin_out_read(PIN2);
    status3 = nrf_gpio_pin_out_read(PIN3);
    status4 = nrf_gpio_pin_out_read(PIN4);
    nrf_gpio_pin_toggle(x);
    if ((status1 == 1) && (status2 == 1) && (status3 == 1) && (status4 == 1))
    {
      nrf_gpio_pin_clear(PIN1); // Turn off the LED
      nrf_gpio_pin_clear(PIN2); // Turn off the LED
      nrf_gpio_pin_clear(PIN3); // Turn off the LED
      nrf_gpio_pin_clear(PIN4); // Turn off the LED
    }
    
    
    break;
    default:
    // Nothing
    break;
  }
}

void timer_init(void)
{
  uint32_t err_code = NRF_SUCCESS;

  uint32_t time_ms = 500;

  uint32_t time_ticks;

  nrfx_timer_config_t timer_cfg = NRFX_TIMER_DEFAULT_CONFIG; // Configure the timer instance to default settings

  err_code = nrfx_timer_init(&TIMER_LED, &timer_cfg, timer0_handler); // Initialize the timer0 with default settings
  APP_ERROR_CHECK(err_code); // check if any error occured 

  time_ticks = nrfx_timer_ms_to_ticks(&TIMER_LED, time_ms); // convert ms to ticks

  nrfx_timer_extended_compare(&TIMER_LED, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

}
/**
 * @brief Function for main application entry.
 */
int main(void)
{
    bool status1, status2, status3, status4;
    uint32_t x,i;
    nrf_gpio_cfg_output(PIN1); // Initialize the pin
    nrf_gpio_cfg_output(PIN2); // Initialize the pin
    nrf_gpio_cfg_output(PIN3); // Initialize the pin
    nrf_gpio_cfg_output(PIN4); // Initialize the pin
    nrf_gpio_pin_clear(PIN1); // Turn off the LED
    nrf_gpio_pin_clear(PIN2); // Turn off the LED
    nrf_gpio_pin_clear(PIN3); // Turn off the LED
    nrf_gpio_pin_clear(PIN4); // Turn off the LED
    timer_init();
    nrfx_timer_enable(&TIMER_LED);


    while (1)
    {
        __WFI(); // Go into low power mode
        status1 = nrf_gpio_pin_out_read(PIN1);
        status2 = nrf_gpio_pin_out_read(PIN2);
        status3 = nrf_gpio_pin_out_read(PIN3);
        status4 = nrf_gpio_pin_out_read(PIN4);
        printf("LED1: %d, LED2: %d, LED3: %d, LED4: %d, \n",status1, status2, status3, status4);
        printf("%d\n",i++);
    }
}

/** @} */
