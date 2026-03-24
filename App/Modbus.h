#ifndef MODBUS_H
#define MODBUS_H

#include "ch32v00x.h"

/* Modbus function codes (subset supported by this slave) */
typedef enum {
    MB_FC_READ_HOLDING   = 0x03,
    MB_FC_READ_INPUT     = 0x04,
    MB_FC_WRITE_SINGLE   = 0x06,
    MB_FC_WRITE_MULTIPLE = 0x10
} MB_FuncCode_t;

/* Modbus exception codes */
typedef enum {
    MB_EX_ILLEGAL_FUNC  = 0x01,
    MB_EX_ILLEGAL_ADDR  = 0x02,
    MB_EX_ILLEGAL_VALUE = 0x03
} MB_Exception_t;

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
