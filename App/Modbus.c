#include "Modbus.h"
#include "RS485.h"

static uint8_t mb_address = 1;

/* Register arrays */
int16_t mb_inputRegs[MB_INPUT_REG_COUNT];
int16_t mb_holdingRegs[MB_HOLDING_REG_COUNT];

/* Response buffer — max Modbus RTU frame = 256 */
static uint8_t mb_respBuf[256];

static uint16_t Modbus_CRC16(const uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}

static void Modbus_SendException(uint8_t fc, uint8_t exCode)
{
    mb_respBuf[0] = mb_address;
    mb_respBuf[1] = fc | 0x80;
    mb_respBuf[2] = exCode;
    uint16_t crc = Modbus_CRC16(mb_respBuf, 3);
    mb_respBuf[3] = (uint8_t)(crc & 0xFF);
    mb_respBuf[4] = (uint8_t)((crc >> 8) & 0xFF);
    RS485_Transmit(mb_respBuf, 5);
}

static void Modbus_SendResponse(uint16_t respLen)
{
    uint16_t crc = Modbus_CRC16(mb_respBuf, respLen);
    mb_respBuf[respLen] = (uint8_t)(crc & 0xFF);
    mb_respBuf[respLen + 1] = (uint8_t)((crc >> 8) & 0xFF);
    RS485_Transmit(mb_respBuf, respLen + 2);
}

void Modbus_Init(void)
{
    mb_address = 1;
    for (uint16_t i = 0; i < MB_INPUT_REG_COUNT; i++)
        mb_inputRegs[i] = 0;
    for (uint16_t i = 0; i < MB_HOLDING_REG_COUNT; i++)
        mb_holdingRegs[i] = 0;
}

void Modbus_SetAddress(uint8_t addr)
{
    if (addr >= 1 && addr <= 247)
        mb_address = addr;
}

uint8_t Modbus_GetAddress(void)
{
    return mb_address;
}

void Modbus_ProcessFrame(const uint8_t *frame, uint16_t len)
{
    /* Minimum frame: addr(1) + fc(1) + crc(2) = 4 bytes */
    if (len < 4)
        return;

    /* Verify CRC */
    uint16_t recvCrc = (uint16_t)frame[len - 2] | ((uint16_t)frame[len - 1] << 8);
    uint16_t calcCrc = Modbus_CRC16(frame, len - 2);
    if (recvCrc != calcCrc)
        return;

    /* Check slave address */
    if (frame[0] != mb_address)
        return;

    uint8_t fc = frame[1];

    switch (fc)
    {
    case MB_FC_READ_HOLDING:
    case MB_FC_READ_INPUT:
    {
        if (len < 8)
            return;

        int16_t *regs;
        uint16_t regCount;

        if (fc == MB_FC_READ_HOLDING)
        {
            regs = mb_holdingRegs;
            regCount = MB_HOLDING_REG_COUNT;
        }
        else
        {
            regs = mb_inputRegs;
            regCount = MB_INPUT_REG_COUNT;
        }

        uint16_t startAddr = ((uint16_t)frame[2] << 8) | frame[3];
        uint16_t quantity = ((uint16_t)frame[4] << 8) | frame[5];

        if (quantity < 1 || quantity > 125)
        {
            Modbus_SendException(fc, MB_EX_ILLEGAL_VALUE);
            return;
        }
        if (startAddr + quantity > regCount)
        {
            Modbus_SendException(fc, MB_EX_ILLEGAL_ADDR);
            return;
        }

        mb_respBuf[0] = mb_address;
        mb_respBuf[1] = fc;
        mb_respBuf[2] = (uint8_t)(quantity * 2);
        for (uint16_t i = 0; i < quantity; i++)
        {
            uint16_t val = (uint16_t)regs[startAddr + i];
            mb_respBuf[3 + i * 2] = (uint8_t)(val >> 8);
            mb_respBuf[4 + i * 2] = (uint8_t)(val & 0xFF);
        }
        Modbus_SendResponse(3 + quantity * 2);
        break;
    }

    case MB_FC_WRITE_SINGLE:
    {
        if (len < 8)
            return;

        uint16_t regAddr = ((uint16_t)frame[2] << 8) | frame[3];
        uint16_t regValue = ((uint16_t)frame[4] << 8) | frame[5];

        if (regAddr >= MB_HOLDING_REG_COUNT)
        {
            Modbus_SendException(fc, MB_EX_ILLEGAL_ADDR);
            return;
        }

        mb_holdingRegs[regAddr] = (int16_t)regValue;

        /* Echo request back: copy addr + fc + regAddr(2) + value(2) */
        for (uint16_t i = 0; i < 6; i++)
            mb_respBuf[i] = frame[i];
        Modbus_SendResponse(6);
        break;
    }

    case MB_FC_WRITE_MULTIPLE:
    {
        if (len < 11)
            return;

        uint16_t startAddr = ((uint16_t)frame[2] << 8) | frame[3];
        uint16_t quantity = ((uint16_t)frame[4] << 8) | frame[5];
        uint8_t byteCount = frame[6];

        if (quantity < 1 || quantity > 123 || byteCount != quantity * 2)
        {
            Modbus_SendException(fc, MB_EX_ILLEGAL_VALUE);
            return;
        }
        if (len < (uint16_t)(7 + byteCount + 2))
            return;
        if (startAddr + quantity > MB_HOLDING_REG_COUNT)
        {
            Modbus_SendException(fc, MB_EX_ILLEGAL_ADDR);
            return;
        }

        for (uint16_t i = 0; i < quantity; i++)
            mb_holdingRegs[startAddr + i] = (int16_t)(((uint16_t)frame[7 + i * 2] << 8) | frame[8 + i * 2]);

        /* Response: addr + fc + startAddr(2) + quantity(2) */
        for (uint16_t i = 0; i < 6; i++)
            mb_respBuf[i] = frame[i];
        Modbus_SendResponse(6);
        break;
    }

    default:
        Modbus_SendException(fc, MB_EX_ILLEGAL_FUNC);
        break;
    }
}
