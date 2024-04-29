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
#include "mpu9250.h"
#include "lis2dh.h"
#include "uart.h"

#define LED0_NODE DT_ALIAS(led1)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
const struct device *uart0 = DEVICE_DT_GET(DT_NODELABEL(uart0));

// struct data_ibeacon
// {
//     uint8_t device_id;
//     char
// };

// struct k_msgq ibeacon_msgq;

// K_MSGQ_DEFINE(ibeacon_msgq, sizeof(struct data_ibeacon), 10, 4);

// extern int tsk_ibeacon(void)
// {
//     int err;

//     // Infinite loop to handle advertising data updates.
//     while (1)
//     {
//         struct data_ibeacon data;

//         // Attempt to fetch new data from the message queue without waiting.
//         if (k_msgq_get(&ibeacon_msgq, &data, K_NO_WAIT) == 0)
//         {

//             // Update the data field to send sensor value and device id.
//             mfg_data[DATA_OFFSET] = data.major_1;
//             mfg_data[DATA_OFFSET + 1] = data.major_2;
//             mfg_data[DATA_OFFSET + 2] = data.minor_1;
//             mfg_data[DATA_OFFSET + 3] = data.minor_2;
//             mfg_data[DATA_OFFSET + 4] = data.rssi;

//             if (err)
//             {
//                 return 0;
//             }

//             // Update the advertising data with the new payload.
//             bt_le_adv_update_data(ad, ARRAY_SIZE(ad), NULL, 0);

//             // Check for errors in updating advertising data and handle them.
//             if (err)
//             {
//                 printk("Advertising failed to start (err %d)\n", err);
//                 return 0;
//             }
//         }

//         // Sleep for 500ms before attempting the next operation.
//         k_msleep(500);
//     }
// }

// Task for fetching sensor data and preparing it for BLE advertising.
extern int tsk_sensor(void)
{
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

    // const struct device *const mpu9250 = DEVICE_DT_GET_ONE(invensense_mpu9250);

    // if (!device_is_ready(mpu9250))
    // {
    //     printf("Device %s is not ready\n", mpu9250->name);
    //     return 0;
    // }
    int dev_state_fsm = 0; // State variable for FSM handling sensor reading.
    double *xyz = malloc(3 * sizeof(double));
    double *motion = malloc(6 * sizeof(double));

    // Infinite loop for continuous sensor data reading and processing.
    while (1)
    {
        // FSM for reading from different sensors sequentially.
        switch (dev_state_fsm)
        {
        case 0: // Fetch data from the CCS811 air quality sensor.
            fetch_and_display(lis3dh, xyz);
            //printk("%f, %f, %f", xyz[0], xyz[1], xyz[2]);
            printk("%d, %d, %d", (int)(xyz[0]), (int)(xyz[1]), (int)(xyz[2]));
            gpio_pin_toggle_dt(&led);
            // send_str(uart0, "hello");
            break;
            // case 1: // Fetch temperature data from the HTS221 sensor.
            //     process_mpu9250(mpu9250, motion);
            //     printk("  accel %f %f %f m/s/s, gyro  %f %f %f rad/s\n",
            //            motion[0], motion[1], motion[2], motion[3], motion[4], motion[5]);
            //     break;
        }

        dev_state_fsm++;
        dev_state_fsm = dev_state_fsm % 2;

        k_msleep(1000);
    }
    return 0;
}