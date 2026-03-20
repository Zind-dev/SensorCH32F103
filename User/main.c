

#include "debug.h"
#include "RS485.h"
#include "Modbus.h"

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
