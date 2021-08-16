################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../emac_dma_interrupt_headers.c \
../emac_dma_main_functions.c \
../xemacps_dma_main.c \
../xemacps_utils.c 

OBJS += \
./emac_dma_interrupt_headers.o \
./emac_dma_main_functions.o \
./xemacps_dma_main.o \
./xemacps_utils.o 

C_DEPS += \
./emac_dma_interrupt_headers.d \
./emac_dma_main_functions.d \
./xemacps_dma_main.d \
./xemacps_utils.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -I../../standalone_bsp_0/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


