#include "RS485.h"
#include "boardPins.h"

static uint8_t  rs485_txBuf[RS485_TX_BUF_SIZE];
static volatile uint8_t rs485_txBusy = 0;

void RS485_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef  NVIC_InitStructure = {0};

    /* Enable clocks: GPIOC (DE pin), GPIOD (TX/RX), USART1, DMA1 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* DE pin: push-pull output, default LOW (RX mode) */
    GPIO_InitStructure.GPIO_Pin = BOARD_RS485_DE_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(BOARD_RS485_DE_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(BOARD_RS485_DE_PORT, BOARD_RS485_DE_PIN);

    /* USART1 TX pin (PD5): alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = BOARD_USART1_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(BOARD_USART1_TX_PORT, &GPIO_InitStructure);

    /* USART1 RX pin (PD6): floating input */
    GPIO_InitStructure.GPIO_Pin = BOARD_USART1_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(BOARD_USART1_RX_PORT, &GPIO_InitStructure);

    /* USART1: 9600 baud, 8N1, TX+RX */
    USART_InitStructure.USART_BaudRate = RS485_BAUDRATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStructure);

    /* Enable USART1 DMA TX request */
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

    /* NVIC: DMA1_Channel4 (USART1 TX DMA) */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* NVIC: USART1 (for TC interrupt) */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART1, ENABLE);
}

RS485_Status_t RS485_Transmit(const uint8_t *data, uint16_t len)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    if (rs485_txBusy)
        return RS485_BUSY;

    if (data == NULL || len == 0 || len > RS485_TX_BUF_SIZE)
        return RS485_ERR;

    rs485_txBusy = 1;

    /* Copy data to internal buffer */
    for (uint16_t i = 0; i < len; i++)
        rs485_txBuf[i] = data[i];

    /* Assert DE HIGH — enable TX on transceiver */
    GPIO_SetBits(BOARD_RS485_DE_PORT, BOARD_RS485_DE_PIN);

    /* Clear any pending TC flag before starting */
    USART_ClearFlag(USART1, USART_FLAG_TC);

    /* Configure DMA1_Channel4 for USART1 TX */
    DMA_DeInit(DMA1_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DATAR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rs485_txBuf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = len;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);

    /* Enable DMA1_Channel4 Transfer Complete + Error interrupts */
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TE, ENABLE);

    /* Start DMA transfer */
    DMA_Cmd(DMA1_Channel4, ENABLE);

    return RS485_OK;
}

uint8_t RS485_IsTxBusy(void)
{
    return rs485_txBusy;
}

/* Called from DMA1_Channel4_IRQHandler in ch32v00x_it.c */
void RS485_DMA_TX_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC4) != RESET)
    {
        DMA_ClearITPendingBit(DMA1_IT_TC4);
        DMA_Cmd(DMA1_Channel4, DISABLE);

        /* All bytes transferred to USART data register.
         * Now enable USART TC interrupt to wait for the shift register
         * to finish transmitting the last byte. */
        USART_ITConfig(USART1, USART_IT_TC, ENABLE);
    }

    if (DMA_GetITStatus(DMA1_IT_TE4) != RESET)
    {
        DMA_ClearITPendingBit(DMA1_IT_TE4);
        DMA_Cmd(DMA1_Channel4, DISABLE);

        /* DMA error — abort: deassert DE and release bus */
        GPIO_ResetBits(BOARD_RS485_DE_PORT, BOARD_RS485_DE_PIN);
        rs485_txBusy = 0;
    }
}

/* Called from USART1_IRQHandler in ch32v00x_it.c */
void RS485_USART_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
    {
        /* Transmission physically complete — last bit shifted out */
        USART_ITConfig(USART1, USART_IT_TC, DISABLE);
        USART_ClearITPendingBit(USART1, USART_IT_TC);

        /* Deassert DE — switch to RX mode */
        GPIO_ResetBits(BOARD_RS485_DE_PORT, BOARD_RS485_DE_PIN);

        rs485_txBusy = 0;
    }
}
