#include <zephyr/types.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/logging/log.h>
#include "ble_receive.h"

void start_scan(void);

static struct bt_conn *default_conn;

int temperature;
int humidity;
int pressure;
int tvoc;
int manufact_data = 2753 << 16 | 32998;
int8_t rssi_data = 50;

LOG_MODULE_REGISTER(ble_central);

// Callback function for handling discovered Bluetooth devices during a scan.
static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
                         struct net_buf_simple *ad)
{
    char addr_str[BT_ADDR_LE_STR_LEN];
    int err;

    // If a default connection already exists, return without doing anything.
    if (default_conn)
    {
        return;
    }

    // Convert the Bluetooth address fromat from binary to string.
    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));

    // Filter out week signals.
    // if (rssi < -50)
    // {
    // 	return;
    // }

    // printk("Device found: %s (RSSI %d)\n", addr_str, rssi);

    // Extract specific bytes from the advertisement data which represent sensor data.
    uint8_t bit1 = ad->data[ad->len - 5];
    uint8_t bit2 = ad->data[ad->len - 4];
    uint8_t bit3 = ad->data[ad->len - 3];
    uint8_t bit4 = ad->data[ad->len - 2];

    // Combine three bytes into a single integer value representing the sensor data.
    manufact_data = bit1 << 24 | bit2 << 16 | bit3 << 8 | bit4;
    rssi_data = rssi;

    // If bt_le_scan_stop() is called on shell, stop scanning
    if (bt_le_scan_stop())
    {
        return;
    }

    // Restart the scanning process.
    start_scan();
}

// Callback function for handling discovered Bluetooth devices with filter.
static void device_found_filter(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
                                struct net_buf_simple *ad)
{
    char addr_str[BT_ADDR_LE_STR_LEN];
    int err;

    // If a default connection already exists, return without doing anything.
    if (default_conn)
    {
        return;
    }

    // Convert the Bluetooth address fromat from binary to string.
    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));

    // Extract specific bytes from the advertisement data which represent sensor data.
    uint8_t bit1 = ad->data[ad->len - 5];
    uint8_t bit2 = ad->data[ad->len - 4];
    uint8_t bit3 = ad->data[ad->len - 3];
    uint8_t bit4 = ad->data[ad->len - 2];
    rssi_data = rssi;

    // Combine three bytes into a single integer value representing the sensor data.
    manufact_data = bit1 << 24 | bit2 << 16 | bit3 << 8 | bit4;

    // printk("Device found: %s (RSSI %d)\n", addr_str, rssi);

    // If bt_le_scan_stop() is called on shell, stop scanning
    if (bt_le_scan_stop())
    {
        return;
    }

    // Restart the scanning process.
    start_scan_filter();
}

// Starts a BLE scan in passive mode.
void start_scan(void)
{
    int err;

    // Attempts to start a BLE scan in passive mode, with the `device_found` callback function to handle found devices.
    err = bt_le_scan_start(BT_LE_SCAN_PASSIVE, device_found);

    if (err)
    {
        printk("Scanning failed to start (err %d)\n", err);
        return;
    }

    k_msleep(50);
}

// Starts a BLE scan with filter accept list.
void start_scan_filter(void)
{
    int err;
    // Attempts to start a BLE scan with filter accept list.
    err = bt_le_scan_start(BT_LE_SCAN_PARAM(BT_LE_SCAN_TYPE_PASSIVE,
                                            BT_LE_SCAN_OPT_FILTER_ACCEPT_LIST,
                                            BT_GAP_SCAN_FAST_INTERVAL,
                                            BT_GAP_SCAN_FAST_WINDOW),
                           device_found_filter);

    if (err)
    {
        printk("Scanning failed to start (err %d)\n", err);
        return;
    }

    k_msleep(100);
}

// Initializes BLE scanning with a specific address filter.
void scan_init(char *filter)
{
    int err;
    bt_addr_le_t addr;
    char *type = "random"; // Type of BLE address

    // Clears any existing address filters from the BLE accept list.
    bt_le_filter_accept_list_clear();

    // Converts the string representation of the BLE address to `bt_addr_le_t` format.
    err = bt_addr_le_from_str(filter, type, &addr);
    if (err < 0)
    {
        LOG_ERR("Filter address invalid, %d", err);
    }

    // Adds the converted address to the BLE filter accept list.
    err = bt_le_filter_accept_list_add(&addr);

    // Starts scanning with the filter applied. The function `start_scan_filter` is assumed to encapsulate starting a scan with the configured filter.
    start_scan_filter();

    if (err)
    {
        printk("Scanning failed to start (err %d)\n", err);
        return;
    }
}

extern int get_manufact_data(void)
{
    return manufact_data;
}

extern int8_t get_rssi_data(void)
{
    return rssi_data;
}