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
#include <zephyr/sys/slist.h>
#include <math.h>
// #include "mpu9250.h"
#include "lis2dh.h"
#include "uart.h"
#include "speaker.h"
#include "ble_receive_mobile.h"

#define LED0_NODE DT_ALIAS(led1)
#define DATA_OFFSET 20
#define IBEACON_RSSI 0xc8
#define TRAIN_SIZE 60
#define DEVICE_ON 1
#define DEVICE_OFF 0
#define MET_WALK 3
#define MET_RUN 8

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
const struct device *uart0 = DEVICE_DT_GET(DT_NODELABEL(uart0));
sys_slist_t sl_list;

static int train_x[] = {15, 22, 16, 13, 10, 10, 9, 8, 8, 7, 6, 3, 4, 4, 4, 4, 5, 5, 5, 6, 24, 41, 44, 47, 62, 72, 73, 73, 76, 93, 80, 72, 88, 89, 91, 89, 88, 90, 90, 74, 91, 90, 99, 95, 154, 189, 219, 221, 240, 263, 250, 286, 316, 332, 327, 284, 300, 296, 329, 308};
static int train_y[] = {22, 22, 24, 23, 23, 22, 21, 21, 22, 22, 22, 22, 19, 20, 20, 20, 22, 22, 21, 21, 44, 51, 52, 51, 55, 55, 60, 59, 63, 66, 48, 53, 70, 73, 70, 73, 73, 89, 106, 111, 109, 109, 119, 174, 221, 302, 360, 361, 345, 341, 336, 341, 342, 325, 344, 286, 229, 233, 227, 225};
static int train_z[] = {15, 15, 20, 18, 16, 14, 13, 12, 12, 12, 11, 10, 9, 10, 9, 10, 9, 11, 12, 11, 14, 14, 14, 12, 16, 22, 23, 24, 31, 40, 46, 52, 55, 56, 60, 57, 65, 65, 79, 75, 73, 75, 72, 77, 130, 185, 180, 251, 290, 288, 285, 321, 333, 361, 372, 350, 354, 299, 252, 279};
static int train_activity[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
static int energy_calac = 0;
static int dev_state = DEVICE_ON;
static int stand = 0;
static int weight;
static char *actions[4] = {"stand", "walk", "run", "sit"};

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

struct data_ibeacon
{
    uint8_t major_1;
    uint8_t major_2;
    uint8_t minor_1;
    uint8_t minor_2;
    // uint8_t rssi;
};

struct k_msgq ibeacon_msgq;

K_MSGQ_DEFINE(ibeacon_msgq, sizeof(struct data_ibeacon), 10, 4);

extern int tsk_ibeacon(void)
{
    // speaker_activate();
    // k_msleep(1000);
    // speaker_deactivate();
    int err;

    // bt_addr_le_t *addr_le = malloc(sizeof(struct bt_addr_le_t));
    bt_addr_le_t addr_le;

    // Enable Bluetooth subsystem.
    err = bt_enable(NULL);

    // Start non-connectable advertising.
    bt_le_adv_start(BT_LE_ADV_NCONN, ad, ARRAY_SIZE(ad), NULL, 0);

    // speaker_activate();
    // k_msleep(1000);
    // speaker_deactivate();

    // Infinite loop to handle advertising data updates.
    while (1)
    {
        if (dev_state == DEVICE_ON)
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
                // mfg_data[DATA_OFFSET + 4] = data.rssi;

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

            // speaker_activate();

            // Sleep for 500ms before attempting the next operation.
        }
        else
        {
            gpio_pin_set_dt(&led, 1);
        }
        k_msleep(1000);
    }
}

struct node_data
{
    sys_snode_t node;
    int x;
    int y;
    int z;
};

// static double abs(int val)
// {
//     if (val < 0)
//     {
//         return val * -1;
//     }
//     return val;
// }

static double calc_dist(int x1, int x2, int y1, int y2, int z1, int z2)
{
    return sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2) + pow((z1 - z2), 2));
}

extern uint8_t k_NN_classify(int x, int y, int z)
{
    int neighbor_val[3] = {0xFFFFFF, 0xFFFFFF, 0xFFFFFF};
    int neighbor_key[3] = {0, 0, 0};
    int dist;
    for (int i = 0; i < TRAIN_SIZE; i++)
    {
        dist = calc_dist(x, train_x[i], y, train_y[i], z, train_z[i]);
        for (int j = 0; j < 3; j++)
        {
            if (dist < neighbor_val[j])
            {
                neighbor_val[j] = dist;
                neighbor_key[j] = train_activity[i];
                break;
            }
        }
    }
    int classes[3] = {0, 0, 0};
    for (int j = 0; j < 3; j++)
    {

        classes[neighbor_key[j]]++;
    }
    int max_class = 0;
    if (classes[0] < classes[1])
    {
        max_class = 1;
    }
    if (classes[max_class] < classes[2])
    {
        max_class = 2;
    }
    return max_class;
}

static int decimal_abs(int val)
{
    if (val < 0)
    {
        return val * -1;
    }
    return val;
}

static int format_sensor_val(int real, int decimal)
{
    int val;
    if (decimal > 0)
    {
        val = real * 100 + (decimal / 10000);
    }
    else if (decimal < 0 && real == 0)
    {
        val = (decimal / 10000);
    }
    else if (decimal < 0)
    {
        val = real * 100 + (decimal * -1 / 10000);
    }
    return val;
}

static int format_sensor_val_x(int real, int decimal)
{
    int val;
    real = real - 10;
    if (decimal > 0)
    {
        val = real * 100 + (decimal / 10000);
    }
    else if (decimal < 0 && real == 0)
    {
        val = (decimal / 10000);
    }
    else if (decimal < 0)
    {
        val = real * 100 + (decimal * -1 / 10000);
    }
    if (real < 0)
    {
        val = val * -1;
    }
    return val;
}

static int format_sensor_val_x_sign(int real, int decimal)
{
    int val;
    real = real - 10;
    if (decimal > 0)
    {
        val = real * 100 + (decimal / 10000);
    }
    else if (decimal < 0 && real == 0)
    {
        val = (decimal / 10000);
    }
    else if (decimal < 0)
    {
        val = real * 100 + (decimal * -1 / 10000);
    }
    return val;
}

// Task for fetching sensor data and preparing it for BLE advertising.
extern int tsk_sensor(void)
{
    // speaker_activate();
    // k_msleep(1000);
    // speaker_deactivate();
    const struct device *const lis3dh = DEVICE_DT_GET_ANY(st_lis2dh);

    if (lis3dh == NULL)
    {
        printf("No device found\n");
        return 0;
    }
    if (!device_is_ready(lis3dh))
    {
        printf("Device %s is not ready\n", lis3dh->name);
        return 0;
    }
    int *xyz = malloc(6 * sizeof(double));
    // int *motion = malloc(12 * sizeof(double));
    int count = 0;
    // sys_slist_init(&sl_list);
    struct data_ibeacon data;
    int x;
    int x_pol;
    int y;
    int z;
    int x_total = 0;
    int x_pol_total = 0;
    int y_total = 0;
    int z_total = 0;
    int x_avg = 0;
    int x_pol_avg = 0;
    int y_avg = 0;
    int z_avg = 0;
    int prev_states[3] = {0, 0, 0};
    int prev_x_values[3] = {0, 0, 0};

    // Infinite loop for continuous sensor data reading and processing.
    while (1)
    {
        if (dev_state == DEVICE_ON)
        {
            // FSM for reading from different sensors sequentially.
            // Fetch data from the CCS811 air quality sensor.
            fetch_and_display(lis3dh, xyz);
            // printk("%f, %f, %f", xyz[0], xyz[1], xyz[2]);
            // printk("lis2dh: %d.%d, %d.%d, %d.%d\n", xyz[0], decimal_abs(xyz[1]), xyz[2], decimal_abs(xyz[3]), xyz[4], decimal_abs(xyz[5]));

            x = format_sensor_val_x(xyz[0], xyz[1]);
            x_pol = format_sensor_val_x_sign(xyz[0], xyz[1]);
            y = format_sensor_val(xyz[2], xyz[3]);
            z = format_sensor_val(xyz[4], xyz[5]);
            // printk("x: %d, y: %d, z: %d\n", x, y, z);
            // printk("%d, %d, %d\n", x, y, z);
            if (count < 10)
            {
                x_total += x;
                x_pol_total += x_pol;
                y_total += y;
                z_total += z;
                count += 1;
            }
            else
            {
                gpio_pin_toggle_dt(&led);
                count = 1;
                x_total += x;
                x_pol_total += x_pol;
                y_total += y;
                z_total += z;
                x_avg = x_total / 10;
                x_pol_avg = x_pol_total / 10;
                y_avg = y_total / 10;
                z_avg = z_total / 10;
                int current = k_NN_classify(x_avg, y_avg, z_avg);
                if (prev_states[2] == 0 && current == 0 && (prev_x_values[1] < -20 && prev_x_values[0] > 20))
                {
                    stand = 3;
                }
                else if (prev_states[2] == 0 && current == 0 && (prev_x_values[1] > 20 && prev_x_values[0] < -20))
                {
                    stand = 0;
                }
                else if (prev_states[2] != 0 && current != 0 && prev_states[1] != 0 && prev_x_values[0] != 0)
                {
                    stand = 0;
                }
                prev_states[2] = prev_states[1];
                prev_x_values[2] = prev_x_values[1];
                prev_states[1] = prev_states[0];
                prev_x_values[1] = prev_x_values[0];
                prev_states[0] = current;
                prev_x_values[0] = x_pol_avg;
                data.major_1 = 0;
                if (current == 0)
                {
                    data.major_2 = current + stand;
                }
                else
                {
                    data.major_2 = current;
                }

                if (energy_calac)
                {
                    int energy = 0;
                    if (current == 1)
                    {
                        energy = (weight * MET_WALK * 3.5) / 200;
                    }
                    else if (current == 2)
                    {
                        energy = (weight * MET_RUN * 3.5) / 200;
                    }
                    // printk("energy: %d\n", energy);
                    data.minor_1 = energy & 0xFFFF >> 8;
                    data.minor_2 = energy & 0xFF;
                }
                else
                {
                    data.minor_1 = 0;
                    data.minor_2 = 0;
                }

                printk("accel: %d, %d, %d\n", x_avg, y_avg, z_avg);
                printk("state %s\n", actions[data.major_2]);
                printk("x pol: %d\n", x_pol_avg);
                while (k_msgq_put(&ibeacon_msgq, &data, K_NO_WAIT) != 0)
                {
                    // If the queue is full, wait for 500ms before trying again.
                    k_sleep(K_MSEC(500));
                }
                x_total = 0;
                x_pol_total = 0;
                y_total = 0;
                z_total = 0;
            }
        }
        else
        {
            gpio_pin_set_dt(&led, 1);
        }
        k_msleep(100);
    }
    return 0;
}

extern int tsk_scan(void)
{
    int err;
    // err = bt_enable(NULL);

    struct data_ibeacon data;
    int index = 0;

    // speaker_init();
    // speaker_activate();
    // // speaker_deactivate();
    // //   // speaker_activate();
    // k_msleep(1000);
    // speaker_deactivate();

    // Continuously process messages from the BLE message queue.
    while (1)
    {
        // speaker_deactivate();
        start_scan();

        // Get majior & minor number received
        int val = get_manufact_data();
        data.major_1 = val >> 24;
        data.major_2 = (val & 0xff0000) >> 16;
        data.minor_1 = (val & 0xff00) >> 8;
        data.minor_2 = val & 0xff;
        // data.rssi = get_rssi_data();

        bt_le_scan_stop();
        if (data.major_2 == 1 && dev_state == DEVICE_ON)
        {
            dev_state = DEVICE_OFF;

            // //   // speaker_activate();
            // k_msleep(1000);
            // speaker_deactivate();
            // speaker_activate();
            // k_msleep(500);
            // speaker_deactivate();
        }
        else if (data.major_2 == 2 && dev_state == DEVICE_OFF)
        {
            dev_state = DEVICE_ON;
            // gpio_pin_set_dt(&led, 0);
            // speaker_activate();
            // k_msleep(1000);
            // speaker_deactivate();
        }
        else if (data.major_2 == 3)
        {
            energy_calac = 1;
            weight = data.minor_2;
        }
        else if (data.major_2 == 4)
        {
            energy_calac = 0;
        }
        else if (data.major_2 == 5)
        {
            speaker_init();
            speaker_activate();
            k_msleep(1000);
            speaker_deactivate();
        }
        k_msleep(500);
    }
}