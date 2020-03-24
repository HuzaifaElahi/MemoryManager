################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../cpu.c \
../interpreter.c \
../kernel.c \
../memorymanager.c \
../pcb.c \
../ram.c \
../shell.c \
../shellmemory.c 

OBJS += \
./cpu.o \
./interpreter.o \
./kernel.o \
./memorymanager.o \
./pcb.o \
./ram.o \
./shell.o \
./shellmemory.o 

C_DEPS += \
./cpu.d \
./interpreter.d \
./kernel.d \
./memorymanager.d \
./pcb.d \
./ram.d \
./shell.d \
./shellmemory.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


