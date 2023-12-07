################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/API/Src/API_timer.c \
../Drivers/API/Src/API_uart.c \
../Drivers/API/Src/inputs.c \
../Drivers/API/Src/inputs_stm32.c \
../Drivers/API/Src/oee_applicationProtocol.c \
../Drivers/API/Src/oee_eventController.c \
../Drivers/API/Src/oee_linkProtocol.c \
../Drivers/API/Src/oee_stateController.c \
../Drivers/API/Src/ringBuffer.c 

OBJS += \
./Drivers/API/Src/API_timer.o \
./Drivers/API/Src/API_uart.o \
./Drivers/API/Src/inputs.o \
./Drivers/API/Src/inputs_stm32.o \
./Drivers/API/Src/oee_applicationProtocol.o \
./Drivers/API/Src/oee_eventController.o \
./Drivers/API/Src/oee_linkProtocol.o \
./Drivers/API/Src/oee_stateController.o \
./Drivers/API/Src/ringBuffer.o 

C_DEPS += \
./Drivers/API/Src/API_timer.d \
./Drivers/API/Src/API_uart.d \
./Drivers/API/Src/inputs.d \
./Drivers/API/Src/inputs_stm32.d \
./Drivers/API/Src/oee_applicationProtocol.d \
./Drivers/API/Src/oee_eventController.d \
./Drivers/API/Src/oee_linkProtocol.d \
./Drivers/API/Src/oee_stateController.d \
./Drivers/API/Src/ringBuffer.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/API/Src/%.o Drivers/API/Src/%.su Drivers/API/Src/%.cyclo: ../Drivers/API/Src/%.c Drivers/API/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"G:/Mi unidad/STM32 Proyects/PdM/OEE_RTU/Drivers/API" -I"G:/Mi unidad/STM32 Proyects/PdM/OEE_RTU/Drivers/API/Inc" -I"G:/Mi unidad/STM32 Proyects/PdM/OEE_RTU/Drivers/API/Src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-API-2f-Src

clean-Drivers-2f-API-2f-Src:
	-$(RM) ./Drivers/API/Src/API_timer.cyclo ./Drivers/API/Src/API_timer.d ./Drivers/API/Src/API_timer.o ./Drivers/API/Src/API_timer.su ./Drivers/API/Src/API_uart.cyclo ./Drivers/API/Src/API_uart.d ./Drivers/API/Src/API_uart.o ./Drivers/API/Src/API_uart.su ./Drivers/API/Src/inputs.cyclo ./Drivers/API/Src/inputs.d ./Drivers/API/Src/inputs.o ./Drivers/API/Src/inputs.su ./Drivers/API/Src/inputs_stm32.cyclo ./Drivers/API/Src/inputs_stm32.d ./Drivers/API/Src/inputs_stm32.o ./Drivers/API/Src/inputs_stm32.su ./Drivers/API/Src/oee_applicationProtocol.cyclo ./Drivers/API/Src/oee_applicationProtocol.d ./Drivers/API/Src/oee_applicationProtocol.o ./Drivers/API/Src/oee_applicationProtocol.su ./Drivers/API/Src/oee_eventController.cyclo ./Drivers/API/Src/oee_eventController.d ./Drivers/API/Src/oee_eventController.o ./Drivers/API/Src/oee_eventController.su ./Drivers/API/Src/oee_linkProtocol.cyclo ./Drivers/API/Src/oee_linkProtocol.d ./Drivers/API/Src/oee_linkProtocol.o ./Drivers/API/Src/oee_linkProtocol.su ./Drivers/API/Src/oee_stateController.cyclo ./Drivers/API/Src/oee_stateController.d ./Drivers/API/Src/oee_stateController.o ./Drivers/API/Src/oee_stateController.su ./Drivers/API/Src/ringBuffer.cyclo ./Drivers/API/Src/ringBuffer.d ./Drivers/API/Src/ringBuffer.o ./Drivers/API/Src/ringBuffer.su

.PHONY: clean-Drivers-2f-API-2f-Src

