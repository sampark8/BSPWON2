#include <stdio.h>
#include "nus_to_central.h"
#include "nordic_common.h"
#include "app_error.h"
#include "bsp_btn_ble.h"
#include "ble_db_discovery.h"
#include "nrf_ble_scan.h"
#include "nrf_ble_gatt.h"
#include "ble_nus_c.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define NUS_SERVICE_UUID_TYPE   BLE_UUID_TYPE_VENDOR_BEGIN              /**< UUID type for the Nordic UART Service (vendor specific). */

#define APP_BLE_CONN_CFG_TAG    1                                       /**< Tag that refers to the BLE stack configuration set with @ref sd_ble_cfg_set. The default tag is @ref BLE_CONN_CFG_TAG_DEFAULT. */
#define APP_BLE_OBSERVER_PRIO   3                                       /**< BLE observer priority of the application. There is no need to modify this value. */

BLE_NUS_C_ARRAY_DEF(m_ble_nus_c, NRF_SDH_BLE_CENTRAL_LINK_COUNT);                                            /**< BLE Nordic UART Service (NUS) client instance. */
BLE_DB_DISCOVERY_DEF(m_db_disc);                                        /**< Database discovery module instance. */
NRF_BLE_GQ_DEF(m_ble_gatt_queue,                                        /**< BLE GATT Queue instance. */
               NRF_SDH_BLE_CENTRAL_LINK_COUNT,
               NRF_BLE_GQ_QUEUE_SIZE);
NRF_BLE_SCAN_DEF(m_scan);                                               /**< Scanning Module instance. */
static nus_to_central_on_data_received_t m_on_data_received = 0;


static ble_uuid_t const m_nus_uuid =
{
    .uuid = BLE_UUID_NUS_SERVICE,
    .type = NUS_SERVICE_UUID_TYPE
};
static void nus_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}
static void db_disc_handler(ble_db_discovery_evt_t * p_evt)
{
    ble_nus_c_on_db_disc_evt(&m_ble_nus_c[p_evt->conn_handle], p_evt);
}

static void db_discovery_init(void)
{
    ble_db_discovery_init_t db_init;

    memset(&db_init, 0, sizeof(ble_db_discovery_init_t));

    db_init.evt_handler  = db_disc_handler;
    db_init.p_gatt_queue = &m_ble_gatt_queue;

    ret_code_t err_code = ble_db_discovery_init(&db_init);
    APP_ERROR_CHECK(err_code);
}

static void scan_start(void)
{
    ret_code_t ret;

    ret = nrf_ble_scan_start(&m_scan);
    APP_ERROR_CHECK(ret);

    ret = bsp_indication_set(BSP_INDICATE_SCANNING);
    APP_ERROR_CHECK(ret);
}
static void scan_evt_handler(scan_evt_t const * p_scan_evt)
{

    ret_code_t err_code;
    unsigned char id_string[50];
    switch(p_scan_evt->scan_evt_id)
    {
         case NRF_BLE_SCAN_EVT_CONNECTING_ERROR:
         {
              err_code = p_scan_evt->params.connecting_err.err_code;
              APP_ERROR_CHECK(err_code);
         } break;

         case NRF_BLE_SCAN_EVT_CONNECTED:
         {
              ble_gap_evt_connected_t const * p_connected =
                               p_scan_evt->params.connected.p_connected;

             NRF_LOG_INFO("Connecting to target %02x%02x%04x%02x%02x%02x",
                      p_connected->peer_addr.addr[0],
                      p_connected->peer_addr.addr[1],
                      p_connected->peer_addr.addr[2],
                      p_connected->peer_addr.addr[3],
                      p_connected->peer_addr.addr[4],
                      p_connected->peer_addr.addr[5]
                      );  
              
              //sprintf(id_string, "%02x%02x%02x%02x%02x%02x",p_connected->peer_addr.addr[0],p_connected->peer_addr.addr[1],p_connected->peer_addr.addr[2],p_connected->peer_addr.addr[3],p_connected->peer_addr.addr[4],p_connected->peer_addr.addr[5]);
              //bytes_to_send = a;
             *bytes_to_send = p_connected->peer_addr.addr[2];
             NRF_LOG_INFO("%p", *bytes_to_send);
             
         } break;

         case NRF_BLE_SCAN_EVT_SCAN_TIMEOUT:
         {
             NRF_LOG_INFO("Scan timed out.");
             scan_start();
         } break;

         default:
             break;
    }
}

static void scan_init(void)
{
    ret_code_t          err_code;
    nrf_ble_scan_init_t init_scan;

    memset(&init_scan, 0, sizeof(init_scan));

    init_scan.connect_if_match = true;
    init_scan.conn_cfg_tag     = APP_BLE_CONN_CFG_TAG;

    err_code = nrf_ble_scan_init(&m_scan, &init_scan, scan_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_scan_filter_set(&m_scan, SCAN_UUID_FILTER, &m_nus_uuid);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_scan_filters_enable(&m_scan, NRF_BLE_SCAN_UUID_FILTER, false);
    APP_ERROR_CHECK(err_code);
}

static void ble_nus_c_evt_handler(ble_nus_c_t * p_ble_nus_c, ble_nus_c_evt_t const * p_ble_nus_evt)
{
    ret_code_t err_code;

    switch (p_ble_nus_evt->evt_type)
    {
        case BLE_NUS_C_EVT_DISCOVERY_COMPLETE:
            NRF_LOG_INFO("Discovery complete.");
            err_code = ble_nus_c_handles_assign(p_ble_nus_c, p_ble_nus_evt->conn_handle, &p_ble_nus_evt->handles);
            APP_ERROR_CHECK(err_code);

            err_code = ble_nus_c_tx_notif_enable(p_ble_nus_c);
            APP_ERROR_CHECK(err_code);
            NRF_LOG_INFO("Connected to device with Nordic UART Service.");
            break;

        case BLE_NUS_C_EVT_NUS_TX_EVT:
            if(m_on_data_received)
            {
                m_on_data_received(p_ble_nus_evt->p_data, p_ble_nus_evt->data_len);
            }
            break;
        case BLE_NUS_C_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected.");
            scan_start();
            break;
    }
}

static void nus_c_init(void)
{
    ret_code_t       err_code;
    ble_nus_c_init_t init;

    init.evt_handler   = ble_nus_c_evt_handler;
    init.error_handler = nus_error_handler;
    init.p_gatt_queue  = &m_ble_gatt_queue;
    for(int c = 0; c < NRF_SDH_BLE_CENTRAL_LINK_COUNT; c++)
    {
        err_code = ble_nus_c_init(&m_ble_nus_c[c], &init);
        APP_ERROR_CHECK(err_code);
    }
}
uint32_t nus_to_central_send_data(const uint8_t *data_array, uint16_t length)
{
    for(int c = 0; c < NRF_SDH_BLE_CENTRAL_LINK_COUNT; c++)
    {
      return ble_nus_c_string_send(&m_ble_nus_c[c], (uint8_t *)data_array, length);
    }
}

void nus_to_central_ble_evt_handler(ble_evt_t const * p_ble_evt)
{
    ret_code_t            err_code;
    ble_gap_evt_t const * p_gap_evt = &p_ble_evt->evt.gap_evt;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            if(p_gap_evt->params.connected.role == BLE_GAP_ROLE_CENTRAL){
               err_code = ble_nus_c_handles_assign(&m_ble_nus_c[p_ble_evt->evt.gap_evt.conn_handle], p_ble_evt->evt.gap_evt.conn_handle, NULL);
               APP_ERROR_CHECK(err_code);

               err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
               APP_ERROR_CHECK(err_code);
              
                // start discovery of services. The NUS Client waits for a discovery result
               err_code = ble_db_discovery_start(&m_db_disc, p_ble_evt->evt.gap_evt.conn_handle);
               APP_ERROR_CHECK(err_code);
               if ((ble_conn_state_central_conn_count() < NRF_SDH_BLE_CENTRAL_LINK_COUNT))
               {
                    // Resume scanning.
                   scan_start();
               }
            }
            break;
       
    }
}
void nus_to_central_init(nus_to_central_on_data_received_t on_data_received)
{
    m_on_data_received = on_data_received;
    db_discovery_init();
    nus_c_init();
    scan_init();
    scan_start();
}