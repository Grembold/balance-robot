################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../fifo.c \
../main.c \
../timer.c \
../uart.c 

OBJS += \
./fifo.o \
./main.o \
./timer.o \
./uart.o 

C_DEPS += \
./fifo.d \
./main.d \
./timer.d \
./uart.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -O3 -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -mmcu=atmega168 -DF_CPU=20000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


