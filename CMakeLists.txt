cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/toolchain-ch32v003.cmake)

project(ch32v003_firmware C ASM)

# CH32V003-specific compiler flags
set(MCU_FLAGS "-march=rv32ec -mabi=ilp32e -msmall-data-limit=8")
set(CMAKE_C_FLAGS   "${MCU_FLAGS} -Wall -fdata-sections -ffunction-sections -Os")
set(CMAKE_ASM_FLAGS "${MCU_FLAGS}")

# Linker script
set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/Ld/link.ld")
set(CMAKE_EXE_LINKER_FLAGS
    "-T${LINKER_SCRIPT} -Wl,--gc-sections -nostartfiles --specs=nano.specs --specs=nosys.specs"
)

# SDK sources (from WCH CH32V003 EVT package)
file(GLOB SDK_SOURCES
    "Peripheral/src/*.c"
    "Core/*.c"
    "Debug/*.c"
)

# Application sources
file(GLOB APP_SOURCES "User/*.c")

# Startup file
set(STARTUP_SOURCE "Startup/startup_ch32v00x.S")

add_executable(${PROJECT_NAME}.elf
    ${APP_SOURCES}
    ${SDK_SOURCES}
    ${STARTUP_SOURCE}
)

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    Core
    Peripheral/inc
    Debug
    User
)

# Generate .hex and .bin after build
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}.elf>
)