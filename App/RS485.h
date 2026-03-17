#ifndef RS485_H
#define RS485_H

#include "ch32v00x.h"

#define RS485_TX_BUF_SIZE  256
#define RS485_RX_BUF_SIZE  256
#define RS485_BAUDRATE     9600

typedef enum {
    RS485_OK   = 0,
    RS485_BUSY = 1,
    RS485_ERR  = 2
} RS485_Status_t;

void          RS485_Init(void);
RS485_Status_t RS485_Transmit(const uint8_t *data, uint16_t len);
uint8_t       RS485_IsTxBusy(void);
uint8_t       RS485_IsFrameReady(void);
uint16_t      RS485_GetReceivedFrame(uint8_t *buf, uint16_t maxLen);

/* Called from ISR context — declared here so ch32v00x_it.c can call them */
void RS485_DMA_TX_IRQHandler(void);
void RS485_USART_IRQHandler(void);

#endif /* RS485_H */
