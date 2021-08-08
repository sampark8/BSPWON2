#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_drv_timer.h"
#include "bsp.h"
#include "app_error.h"

const nrf_drv_timer_t TIMER_LED = NRFX_TIMER_INSTANCE(0);  // Timer 0 enabled

void timer0_handler(nrf_timer_event_t event_type, void* p_context)
{
  static uint32_t i,val;
  bool status1, status2, status3, status4;
    val = (i++) % (5); // Due to the timer aspect, we give one extra timer slot for turning off all of the LED's. Thus 4 Led + 1 off = 5
    uint32_t led_to_invert = (val);
    switch (event_type)
    {
        case NRF_TIMER_EVENT_COMPARE0:
            status1 = bsp_board_led_state_get(BSP_BOARD_LED_0); // state of LED 1
            status2 = bsp_board_led_state_get(BSP_BOARD_LED_1); // state of LED 2
            status3 = bsp_board_led_state_get(BSP_BOARD_LED_2); // state of LED 3
            status4 = bsp_board_led_state_get(BSP_BOARD_LED_3); // state of LED 4
            bsp_board_led_on(val);
            if ((status1 == 1) && (status2 == 1) && (status3 == 1) && (status4 == 1))
            {
            bsp_board_leds_off();
            }
            break;
        default:
            //Do nothing.
            break;
    }
}

void timer_init(void)
{
  uint32_t err_code = NRF_SUCCESS;

  uint32_t time_ms = 500;

  uint32_t time_ticks;

  nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG; // Configure the timer instance to default settings

  err_code = nrf_drv_timer_init(&TIMER_LED, &timer_cfg, timer0_handler); // Initialize the timer0 with default settings
  APP_ERROR_CHECK(err_code); // check if any error occured 

  time_ticks = nrf_drv_timer_ms_to_ticks(&TIMER_LED, time_ms); // convert ms to ticks

  nrf_drv_timer_extended_compare(&TIMER_LED, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

}
/**
 * @brief Function for main application entry.
 */
int main(void)
{   
    bool status1, status2, status3, status4;
    uint32_t x,i;
    bsp_board_init(BSP_INIT_LEDS);
    timer_init();
    nrf_drv_timer_enable(&TIMER_LED);
    

    while (1)
    {
        __WFI(); // Go into low power mode
         status1 = bsp_board_led_state_get(BSP_BOARD_LED_0);
         status2 = bsp_board_led_state_get(BSP_BOARD_LED_1);
         status3 = bsp_board_led_state_get(BSP_BOARD_LED_2);
         status4 = bsp_board_led_state_get(BSP_BOARD_LED_3);
         printf("LED1: %d, LED2: %d, LED3: %d, LED4: %d, \n",status1, status2, status3, status4);
         x = status1 + status2 + status3 + status4;
         printf("status total:%d\n",x);
         printf("%d\n",i++);
        
    }
}

/** @} */
