################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../main.c \
../receiver.c \
../transmitter.c \
../wavpcm_io.c 

OBJS += \
./main.o \
./receiver.o \
./transmitter.o \
./wavpcm_io.o 

C_DEPS += \
./main.d \
./receiver.d \
./transmitter.d \
./wavpcm_io.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


