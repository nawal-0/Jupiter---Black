#include <zephyr/drivers/sensor/ccs811.h>
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/types.h>
#include <stddef.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <stdint.h>
#include "ble_receive.h"
#include "ble_send.h"

#ifndef IBEACON_RSSI
#define IBEACON_RSSI 0xc8
#endif

#define DATA_OFFSET 20

// #define STATIC_ADDR [6] { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };

bt_addr_le_t static_addr;

// Data struct for ibeacon message queue.
struct data_ibeacon
{
    uint8_t major_1;
    uint8_t major_2;
    uint8_t minor_1;
    uint8_t minor_2;
    uint8_t rssi;
};

int major_list[8] = {2753, 32975, 26679, 41747, 30679, 6195, 30525, 57395};
struct k_msgq ibeacon_msgq;

K_MSGQ_DEFINE(ibeacon_msgq, sizeof(struct data_ibeacon), 10, 4);

/*
 * Set iBeacon demo advertisement data. These values are for
 * demonstration only and must be changed for production environments!
 *
 * UUID:  18aa1326-016b-4bec-ad96-bcb96d166e97
 * Major: ff
 * Minor: ff
 * RSSI:  -56 dBm
 */
static uint8_t mfg_data[] = {
    0x4c, 0x00,                         /* Apple */
    0x02, 0x15,                         /* iBeacon */
    0x18, 0xaa, 0x13, 0x26,             /* UUID[15..12] */
    0x01, 0x6b,                         /* UUID[11..10] */
    0x4b, 0xec,                         /* UUID[9..8] */
    0xad, 0x96,                         /* UUID[7..6] */
    0xbc, 0xb9, 0x6d, 0x16, 0x6e, 0x97, /* UUID[5..0] */
    0xff,
    0xff, 0xff, 0xff, IBEACON_RSSI};

// Declare the advertising packet.
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, mfg_data, sizeof(mfg_data))};

// Main task for BLE advertising of iBeacon data.
extern int tsk_ibeacon(void)
{
    int err;

    // bt_addr_le_t *addr_le = malloc(sizeof(struct bt_addr_le_t));
    bt_addr_le_t addr_le;

    // Enable Bluetooth subsystem.
    err = bt_enable(NULL);

    // Start non-connectable advertising.
    bt_le_adv_start(BT_LE_ADV_NCONN, ad, ARRAY_SIZE(ad), NULL, 0);

    // Infinite loop to handle advertising data updates.
    while (1)
    {
        struct data_ibeacon data;

        // Attempt to fetch new data from the message queue without waiting.
        if (k_msgq_get(&ibeacon_msgq, &data, K_NO_WAIT) == 0)
        {

            // Update the data field to send sensor value and device id.
            mfg_data[DATA_OFFSET] = data.major_1;
            mfg_data[DATA_OFFSET + 1] = data.major_2;
            mfg_data[DATA_OFFSET + 2] = data.minor_1;
            mfg_data[DATA_OFFSET + 3] = data.minor_2;
            mfg_data[DATA_OFFSET + 4] = data.rssi;

            if (err)
            {
                return 0;
            }

            // Update the advertising data with the new payload.
            bt_le_adv_update_data(ad, ARRAY_SIZE(ad), NULL, 0);

            // Check for errors in updating advertising data and handle them.
            if (err)
            {
                printk("Advertising failed to start (err %d)\n", err);
                return 0;
            }
        }

        // Sleep for 500ms before attempting the next operation.
        k_msleep(50);
    }
}

static void scan_init_mobile()
{
    int err;
    bt_addr_le_t addr;
    char *type = "random"; // Type of BLE address

    // Clears any existing address filters from the BLE accept list.
    bt_le_filter_accept_list_clear();

    // Converts the string representation of the BLE address to `bt_addr_le_t` format.
    // Adds the converted address to the BLE filter accept list.
    bt_addr_le_from_str("F5:75:FE:85:34:67", type, &addr);
    bt_le_filter_accept_list_add(&addr);
    bt_addr_le_from_str("E5:73:87:06:1E:86", type, &addr);
    bt_le_filter_accept_list_add(&addr);
    bt_addr_le_from_str("CA:99:9E:FD:98:B1", type, &addr);
    bt_le_filter_accept_list_add(&addr);
    bt_addr_le_from_str("CB:1B:89:82:FF:FE", type, &addr);
    bt_le_filter_accept_list_add(&addr);
    bt_addr_le_from_str("D4:D2:A0:A4:5C:AC", type, &addr);
    bt_le_filter_accept_list_add(&addr);
    bt_addr_le_from_str("C1:13:27:E9:B7:7C", type, &addr);
    bt_le_filter_accept_list_add(&addr);
    bt_addr_le_from_str("F1:04:48:06:39:A0", type, &addr);
    bt_le_filter_accept_list_add(&addr);
    bt_addr_le_from_str("CA:0C:E0:DB:CE:60", type, &addr);
    bt_le_filter_accept_list_add(&addr);
    start_scan_filter();
}

extern int tsk_scan(void)
{
    int err;
    // err = bt_enable(NULL);

    // Initialize the Bluetooth subsystem with no parameters.
    // err = bt_enable(NULL);
    // if (err) // Check if there was an error initializing Bluetooth.
    // {
    //     printk("Bluetooth init failed (err %d)\n", err);
    //     return 0;
    // }

    struct data_ibeacon data;
    int index = 0;

    // scan_init_mobile();
    //  start_scan_filter();
    // start_scan();

    // Continuously process messages from the BLE message queue.
    while (1)
    {
        start_scan();

        // Get majior & minor number received
        int val = get_manufact_data();
        data.major_1 = val >> 24;
        data.major_2 = (val & 0xff0000) >> 16;
        data.minor_1 = (val & 0xff00) >> 8;
        data.minor_2 = val & 0xff;
        data.rssi = get_rssi_data();

        bt_le_scan_stop();
        //  index++;
        //  index = index % 8;
        //  printk("index %d", index);

        // if (val >> 16 == major_list[index])
        // if (val >> 16 == major_list[index])
        // {
        //     index++;
        //     index = index % 8;
        // printk
        // Attempt to send the data to the iBeacon message queue for advertising.
        while (k_msgq_put(&ibeacon_msgq, &data, K_NO_WAIT) != 0)
        {
            // If the queue is full, wait for 500ms before trying again.
            k_sleep(K_MSEC(500));
        }
        //}

        k_sleep(K_MSEC(50));
    }
}