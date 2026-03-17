## Plan: RS485 DMA Driver for CH32V003

Implement a full RS485 UART driver with DMA TX/RX, hardware-accurate DE pin control via GPIO, and IDLE-based frame detection. SDI debug replaces USART printf. Modbus RTU layer to follow in a future plan.

**Phases (4 phases)**

1. **Phase 1: Board pins header + SDI debug switch**
    - **Objective:** Create `boardPins.h` with all pin definitions. Switch debug output from USART printf to SDI. Remove old USART echo code from main.
    - **Files to Create:** `App/boardPins.h`
    - **Files to Modify:** `CMakeLists.txt`, `User/main.c`
    - **Steps:**
        1. Create `App/boardPins.h` defining all pins: USART1_TX (PD5), USART1_RX (PD6), RS485_DE (PC1), TIM1_CH1_PWM (PD2), ADC humidity (PA0), ADC temperature (PA1)
        2. Add `-DSDI_PRINT=1` to compile definitions in CMakeLists.txt
        3. Remove `USARTx_CFG()` function and USART echo loop from main.c

2. **Phase 2: RS485 driver — TX with DMA + DE control**
    - **Objective:** Implement RS485 transmit: assert DE, DMA transfer, TC interrupt to deassert DE.
    - **Files to Create:** `App/RS485.c`, `App/RS485.h`
    - **Files to Modify:** `User/ch32v00x_it.c`, `User/ch32v00x_it.h`, `CMakeLists.txt`
    - **Steps:**
        1. Create RS485.h with API: `RS485_Init()`, `RS485_Transmit(data, len)` returning status, `RS485_IsTxBusy()`
        2. Create RS485.c: USART1 config (9600, 8N1), DMA1_Ch4 TX, PC1 DE GPIO, NVIC for DMA1_Ch4 and USART1
        3. TX flow: set DE HIGH → start DMA → DMA TC ISR enables USART TC interrupt → USART TC ISR sets DE LOW
        4. Add ISR handlers to ch32v00x_it.c
        5. RS485.c auto-included via glob in CMakeLists.txt

3. **Phase 3: RS485 driver — RX with DMA + IDLE detection**
    - **Objective:** Implement RS485 receive: DMA1_Ch5 buffer, IDLE interrupt for frame-end detection.
    - **Files to Modify:** `App/RS485.c`, `App/RS485.h`, `User/ch32v00x_it.c`
    - **Steps:**
        1. Add to API: `RS485_GetReceivedFrame(buf, len)`, `RS485_IsFrameReady()`
        2. Configure DMA1_Ch5 in normal mode with 256-byte RX buffer, enable USART IDLE interrupt
        3. IDLE ISR: read remaining DMA count → compute frame length → flag frame ready, reset DMA for next
        4. DE pin LOW during RX (receiver enabled)

4. **Phase 4: Integration + smoke test in main**
    - **Objective:** Wire RS485 driver into main.c with basic echo test.
    - **Files to Modify:** `User/main.c`
    - **Steps:**
        1. Call `RS485_Init()` in main
        2. Main loop: check `RS485_IsFrameReady()`, echo back via `RS485_Transmit()`
        3. Verify build passes

**Open Questions (resolved)**
1. Buffer sizes: 256 bytes TX and RX — confirmed sufficient
2. `RS485_Transmit` returns error/busy status — confirmed
3. Default USART IDLE detection acceptable for Modbus frame boundary — confirmed
