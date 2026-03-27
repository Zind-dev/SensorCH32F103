

#include "debug.h"
#include "../App/RS485.h"
#include "../App/Modbus.h"

static uint8_t rxBuf[RS485_RX_BUF_SIZE];

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#endif
    RS485_Init();
    Modbus_Init();
    printf("TestSomeText");
    fflush(stdout);
    rxBuf[0] = 1;
    rxBuf[1] = 2;
    RS485_Transmit(rxBuf, 2);
    while (1)
    {
        if (RS485_IsFrameReady())
        {
            uint16_t len = RS485_GetReceivedFrame(rxBuf, sizeof(rxBuf));
            while (RS485_Transmit(rxBuf, len) == RS485_BUSY)
            {
            }
            if (len > 0)
            {
                // Modbus_ProcessFrame(rxBuf, len);
            }
        }
    }
}


void DMA1_Channel4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      DMA1_Channel4_IRQHandler
 *
 * @brief   DMA1 Channel 4 interrupt (USART1 TX DMA complete).
 *
 * @return  none
 */
void DMA1_Channel4_IRQHandler(void)
{
    RS485_DMA_TX_IRQHandler();
}

/*********************************************************************
 * @fn      USART1_IRQHandler
 *
 * @brief   USART1 interrupt (TC for RS485 DE control).
 *
 * @return  none
 */
void USART1_IRQHandler(void)
{
    RS485_USART_IRQHandler();
}