#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>

#include <stdio.h>
#include <string.h>

#define MSG_SIZE 32

K_MSGQ_DEFINE(uart_msgq, MSG_SIZE, 10, 4);

LOG_MODULE_REGISTER(uart);

// config for uart
struct uart_config uart_cfg = {
    .baudrate = 115200,
    .parity = UART_CFG_PARITY_NONE,
    .stop_bits = UART_CFG_STOP_BITS_1,
    .flow_ctrl = UART_CFG_FLOW_CTRL_NONE,
    .data_bits = UART_CFG_DATA_BITS_8,
};

/*
 * It iterates over each character in the string and sends it using uart_poll_out.
 * This function is used to send data from the device to another UART receiver.
 * - `uart`: Pointer to the UART device structure.
 * - `str`: The string to be sent over UART.
 */
void send_str(const struct device *uart, char *str)
{
    int msg_len = strlen(str);

    for (int i = 0; i < msg_len; i++)
    {
        uart_poll_out(uart, str[i]);
    }
}

/*
 * It reads characters into the provided buffer until no more characters can be read.
 * This function is used to receive data from another UART sender.
 * - `uart`: Pointer to the UART device structure.
 * - `str`: Buffer where the received string will be stored.
 */
void recv_str(const struct device *uart, char *str)
{
    char *head = str;
    char c;

    while (!uart_poll_in(uart, &c))
    {
        *head++ = c;
    }
    *head = '\0';
    if (str[0] != NULL)
    {
        if (str[2] == '0')
        {
            printk("GPB button state: not pressed\n");
        }
        else if (str[2] == '1')
        {
            printk("GPB button  state: pressed\n");
        }
        else
        {
            LOG_INF("Command sent successfully\n");
        }
    }
}