#ifndef UART_ESP32_H
#define UART_ESP32_H

#define UART_NUM UART_NUM_2
#define TX_PIN 17
#define RX_PIN 16
#define BUF_SIZE 1024
#define UART_NUM UART_NUM_1

void uart_init(void);
void uart_send_data(const char* data);
void uart_receive_data(void);

#endif // UART_ESP32_H
