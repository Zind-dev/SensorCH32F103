cmake_minimum_required(VERSION 3.21)

project(firmware)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

enable_language(C ASM)

set(TARGET_NAME ${PROJECT_NAME})
set(TARGET_ELF ${TARGET_NAME}.elf)
file(GLOB APP_SOURCES App/*.c)
add_executable(${TARGET_ELF} src/main.c ${APP_SOURCES})
target_include_directories(${TARGET_ELF} PRIVATE App)

add_subdirectory(wch)

add_custom_command(TARGET ${TARGET_ELF} POST_BUILD
    COMMAND ${CROSS_OBJDUMP} "-CdSx" $<TARGET_FILE:${TARGET_ELF}> > $<TARGET_FILE:${TARGET_ELF}>.lst
    COMMENT "Generating disassembler file")
add_custom_command(TARGET ${TARGET_ELF} POST_BUILD
    COMMAND ${CROSS_OBJCOPY} "-Obinary" $<TARGET_FILE:${TARGET_ELF}> $<TARGET_FILE:${TARGET_ELF}>.bin
    COMMENT "Generating bin file")
add_custom_command(TARGET ${TARGET_ELF} POST_BUILD
    COMMAND ${CROSS_OBJCOPY} "-Oihex" $<TARGET_FILE:${TARGET_ELF}> $<TARGET_FILE:${TARGET_ELF}>.hex
    COMMENT "Generating hex file")

add_custom_target(flash
        COMMAND openocd -f ${CMAKE_CURRENT_SOURCE_DIR}/wch/wch-riscv.cfg
            -c init
            -c halt
            -c "flash erase_sector wch_riscv 0 last"
            -c "program ${TARGET_ELF} verify"
            -c wlink_reset_resume
            -c exit
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${TARGET_ELF}
        COMMENT "Flashing ${TARGET_ELF} via OpenOCD"
        USES_TERMINAL)
