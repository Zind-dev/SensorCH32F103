/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/12/25
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "debug.h"
#include "RS485.h"

static uint8_t rxBuf[RS485_RX_BUF_SIZE];

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#else
    USART_Printf_Init(115200);
#endif
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

    RS485_Init();
    printf("RS485 initialized, 9600 8N1\r\n");

    while(1)
    {
        if (RS485_IsFrameReady())
        {
            uint16_t len = RS485_GetReceivedFrame(rxBuf, sizeof(rxBuf));
            if (len > 0)
            {
                /* Echo received frame back over RS485 */
                while (RS485_Transmit(rxBuf, len) == RS485_BUSY)
                {
                }
            }
        }
    }
}
