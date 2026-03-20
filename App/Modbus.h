#ifndef MODBUS_H
#define MODBUS_H

#include "ch32v00x.h"

typedef enum {
    MB_IREG_STUB = 0,
    MB_INPUT_REG_COUNT
} MB_InputReg_t;

typedef enum {
    MB_HREG_STUB = 0,
    MB_HOLDING_REG_COUNT
} MB_HoldingReg_t;

/* Public register arrays — fill from application code */
extern int16_t mb_inputRegs[MB_INPUT_REG_COUNT];
extern int16_t mb_holdingRegs[MB_HOLDING_REG_COUNT];

void    Modbus_Init(void);
void    Modbus_SetAddress(uint8_t addr);
uint8_t Modbus_GetAddress(void);
void    Modbus_ProcessFrame(const uint8_t *frame, uint16_t len);

#endif /* MODBUS_H */
