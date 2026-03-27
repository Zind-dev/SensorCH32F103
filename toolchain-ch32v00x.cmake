set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR RISC-V)

if(DEFINED ENV{CH32_TOOLCHAIN_DIR})
    set(CROSSTOOL_PATH "$ENV{CH32_TOOLCHAIN_DIR}/bin" CACHE PATH "Cross toolchain root directory")
else()
    message(FATAL_ERROR "CH32_TOOLCHAIN_DIR environment variable is not set. "
        "Set it to the root of your RISC-V GCC toolchain (e.g. C:/tools/riscv-embedded-gcc-12)")
endif()

set(CROSS_PREFIX "riscv-none-embed-")

find_program(CROSS_CC "${CROSS_PREFIX}gcc" PATHS "${CROSSTOOL_PATH}" NO_DEFAULT_PATH REQUIRED)
find_program(CROSS_CXX "${CROSS_PREFIX}g++" PATHS "${CROSSTOOL_PATH}" NO_DEFAULT_PATH REQUIRED)
find_program(CROSS_OBJDUMP "${CROSS_PREFIX}objdump" PATHS "${CROSSTOOL_PATH}" NO_DEFAULT_PATH REQUIRED)
find_program(CROSS_OBJCOPY "${CROSS_PREFIX}objcopy" PATHS "${CROSSTOOL_PATH}" NO_DEFAULT_PATH REQUIRED)

set(CMAKE_C_COMPILER ${CROSS_CC})
set(CMAKE_CXX_COMPILER ${CROSS_CXX})

set(CPU_FLAGS "-march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized")

set(COMMON_FLAGS "-std=gnu99 -MMD")

set(FLAGS_RELEASE "-Os")
set(FLAGS_DEBUG "-Og -g3")

set(CMAKE_C_FLAGS "${CPU_FLAGS} ${OPT_FLAGS} ${COMMON_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE ${FLAGS_RELEASE})
set(CMAKE_C_FLAGS_DEBUG ${FLAGS_DEBUG})
set(CMAKE_CXX_FLAGS "${CPU_FLAGS} ${OPT_FLAGS} ${COMMON_FLAGS} -fno-rtti -fno-exceptions")
set(CMAKE_CXX_FLAGS_RELEASE ${FLAGS_RELEASE})
set(CMAKE_CXX_FLAGS_DEBUG ${FLAGS_DEBUG})
SET(CMAKE_ASM_FLAGS "${CFLAGS} ${CPU_FLAGS} -x assembler-with-cpp")

set(LD_FLAGS "${CPU_FLAGS} -Xlinker -g -lprintf -nostartfiles -Wl,--gc-sections -Wl,--print-memory-usage")
set(CMAKE_EXE_LINKER_FLAGS "${LD_FLAGS} --specs=nano.specs --specs=nosys.specs" CACHE INTERNAL " ${OPT_FLAGS}")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
