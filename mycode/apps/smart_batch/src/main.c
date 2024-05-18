#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/types.h>
#include <stddef.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include "motion_detect.h"

#ifndef IBEACON_RSSI
#define IBEACON_RSSI 0xc8
#endif

#define PRIORITY 7
#define STACKSIZE 1024

// Register sensor sampling and ibeacon task
K_THREAD_DEFINE(ibeacon_id, STACKSIZE, tsk_ibeacon, NULL, NULL, NULL,
                PRIORITY, 0, 0);
K_THREAD_DEFINE(scan_id, STACKSIZE, tsk_scan, NULL, NULL, NULL,
                PRIORITY, 0, 0);
K_THREAD_DEFINE(sensor_id, STACKSIZE, tsk_sensor, NULL, NULL, NULL,
                PRIORITY, 0, 0);

int main(void)
{
    // speaker_activate();
    // k_msleep(1000);
    // speaker_deactivate();
    // return 0;
}