/* 
 * send_str(const struct device *uart, char *str):
 *    It iterates over each character in the string and sends it using uart_poll_out.
 *    This function is used to send data from the device to another UART receiver.
 *    - `uart`: Pointer to the UART device structure.
 *    - `str`: The string to be sent over UART.
 * 
 * recv_str(const struct device *uart, char *str):
 *    It reads characters into the provided buffer until no more characters can be read.
 *    This function is used to receive data from another UART sender.
 *    - `uart`: Pointer to the UART device structure.
 *    - `str`: Buffer where the received string will be stored.
 */

void send_str(const struct device *uart, char *str);
void recv_str(const struct device *uart, char *str);