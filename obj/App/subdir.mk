################################################################################
# MRS Version: 2.4.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/Measure.c \
../App/PWM.c 

C_DEPS += \
./App/Measure.d \
./App/PWM.d 

OBJS += \
./App/Measure.o \
./App/PWM.o 

DIR_OBJS += \
./App/*.o \

DIR_DEPS += \
./App/*.d \

DIR_EXPANDS += \
./App/*.234r.expand \


# Each subdirectory must supply rules for building sources it contributes
App/%.o: ../App/%.c
	@	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"c:/Users/Zaslon/Desktop/test/testEmbedWch/Debug" -I"c:/Users/Zaslon/Desktop/test/testEmbedWch/Core" -I"c:/Users/Zaslon/Desktop/test/testEmbedWch/User" -I"c:/Users/Zaslon/Desktop/test/testEmbedWch/Peripheral/inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

