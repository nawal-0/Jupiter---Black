#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <version.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/usb/usb_device.h>
#include <ctype.h>
#include <stddef.h>
#include <zephyr/toolchain.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/__assert.h>
#include <string.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/sys/byteorder.h>

#include "time.h"
#include "hci.h"

#ifdef CONFIG_ARCH_POSIX
#include <unistd.h>
#else
#include <zephyr/posix/unistd.h>
#endif

LOG_MODULE_REGISTER(main);
/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000
#define STACKSIZE 1024
#define PRIORITY 7

// Register shell command
// SHELL_STATIC_SUBCMD_SET_CREATE(led_handler,
//                                SHELL_CMD_ARG(s, NULL, "Set led", cmd_set_led, 2, 0),
//                                SHELL_CMD_ARG(t, NULL, "Toggle led", cmd_toggle_led, 2, 0),
//                                SHELL_SUBCMD_SET_END /* Array terminated. */
// );
// SHELL_CMD_REGISTER(led, &led_handler, "LED handler", NULL);
// SHELL_CMD_REGISTER(time, NULL, "timer display", &cmd_time);
SHELL_CMD_REGISTER(mobile_addr, NULL, "register address", &cmd_reg_mobile);
// SHELL_CMD_REGISTER(add_node, NULL, "add node", &add_node);
// SHELL_CMD_REGISTER(remove_node, NULL, "remove node", &remove_node);
// SHELL_CMD_REGISTER(view_node, NULL, "view node", &view_node);

// Register thread
// K_THREAD_DEFINE(led_id, STACKSIZE, thread_led, NULL, NULL, NULL,
//                 PRIORITY, 0, 0);
// K_THREAD_DEFINE(send_id, STACKSIZE, send_data, NULL, NULL, NULL,
//                 PRIORITY, 0, 0);
// K_THREAD_DEFINE(receive_id, STACKSIZE, receive_data, NULL, NULL, NULL,
//                 PRIORITY, 0, 0);
K_THREAD_DEFINE(receive_id, STACKSIZE, tsk_scan, NULL, NULL, NULL,
                PRIORITY, 0, 0);
// K_THREAD_DEFINE(addr_id, STACKSIZE, reg_addr, NULL, NULL, NULL,
//                 PRIORITY, 0, 0);