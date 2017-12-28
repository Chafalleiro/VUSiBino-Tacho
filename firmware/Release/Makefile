################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
LIB_AS_ARGS :=
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
HEX_FLASH_FILE_PATH :=
HEX_FLASH_FILE_PATH_AS_ARGS :=
HEX_EEPROM_FILE_PATH :=
HEX_EEPROM_FILE_PATH_AS_ARGS :=
LSS_FILE_PATH :=
LSS_FILE_PATH_AS_ARGS :=
MAP_FILE_PATH :=
MAP_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=C:/WinAVR-20100110/bin/
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS :=  \
../usbdrv


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../Tachometer.c \
../usbdrv/oddebug.c \
../usbdrv/usbdrv.c


PREPROCESSING_SRCS +=  \
../usbdrv/usbdrvasm.S


ASM_SRCS += 


OBJS +=  \
Tachometer.o \
usbdrv/oddebug.o \
usbdrv/usbdrv.o \
usbdrv/usbdrvasm.o


OBJS_AS_ARGS +=  \
Tachometer.o \
usbdrv/oddebug.o \
usbdrv/usbdrv.o \
usbdrv/usbdrvasm.o


C_DEPS +=  \
Tachometer.d \
usbdrv/oddebug.d \
usbdrv/usbdrv.d


C_DEPS_AS_ARGS +=  \
Tachometer.d \
usbdrv/oddebug.d \
usbdrv/usbdrv.d


OUTPUT_FILE_PATH +=Tachometer.elf

OUTPUT_FILE_PATH_AS_ARGS +=Tachometer.elf

HEX_FLASH_FILE_PATH +=Tachometer.hex

HEX_FLASH_FILE_PATH_AS_ARGS +=Tachometer.hex

HEX_EEPROM_FILE_PATH +=Tachometer.eep

HEX_EEPROM_FILE_PATH_AS_ARGS +=Tachometer.eep

LSS_FILE_PATH +=Tachometer.lss

LSS_FILE_PATH_AS_ARGS +=Tachometer.lss

MAP_FILE_PATH =Tachometer.map

MAP_FILE_PATH_AS_ARGS =Tachometer.map

LIB_AS_ARGS +=libTachometer.a

ADDITIONAL_DEPENDENCIES:= $(HEX_FLASH_FILE_PATH) $(LSS_FILE_PATH) $(HEX_EEPROM_FILE_PATH) size

OUTPUT_FILE_DEP:= ./makedep.mk

# AVR/GNU C Compiler







./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler
	$(QUOTE)$(AVR_APP_PATH)avr-gcc.exe$(QUOTE) -funsigned-char -funsigned-bitfields -DF_CPU=16000000UL  -I"..\usbdrv"  -Os -ffunction-sections -fpack-struct -fshort-enums -mshort-calls -Wall -c -gdwarf-2 -std=gnu99   -mmcu=atmega168   -MD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo Finished building: $<

usbdrv/%.o: ../usbdrv/%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler
	$(QUOTE)$(AVR_APP_PATH)avr-gcc.exe$(QUOTE) -funsigned-char -funsigned-bitfields -DF_CPU=16000000UL  -I"..\usbdrv"  -Os -ffunction-sections -fpack-struct -fshort-enums -mshort-calls -Wall -c -gdwarf-2 -std=gnu99   -mmcu=atmega168   -MD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo Finished building: $<



# AVR/GNU Assembler

usbdrv/%.o: ../usbdrv/%.s
	@echo Building file: $<
	@echo Invoking: AVR/GNU Assembler
	$(QUOTE)$(AVR_APP_PATH)avr-gcc.exe$(QUOTE) -Wa,-gdwarf2 -x assembler-with-cpp -c -Wall -gdwarf-2 -std=gnu99    -DF_CPU=16000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums  -mmcu=atmega168   -o"$@" "$<"
	@echo Finished building: $<




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

# AVR/GNU C/C++ Linker
$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU C/C++ Linker
	$(QUOTE)$(AVR_APP_PATH)avr-gcc.exe$(QUOTE) -Wl,--gc-sections --rodata-writable -mrelax  -mmcu=atmega168  -Wl,-Map=$(MAP_FILE_PATH_AS_ARGS) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS)
	@echo Finished building target: $@



$(HEX_FLASH_FILE_PATH): $(OUTPUT_FILE_PATH)
	$(QUOTE)$(AVR_APP_PATH)avr-objcopy.exe$(QUOTE) -O ihex -R .eeprom -R .fuse -R .lock -R .signature  $(QUOTE)$<$(QUOTE) $(QUOTE)$@$(QUOTE)

$(HEX_EEPROM_FILE_PATH): $(OUTPUT_FILE_PATH)
	-$(QUOTE)$(AVR_APP_PATH)avr-objcopy.exe$(QUOTE) -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O ihex $(QUOTE)$<$(QUOTE) $(QUOTE)$@$(QUOTE) || exit 0

$(LSS_FILE_PATH): $(OUTPUT_FILE_PATH)
	$(QUOTE)$(AVR_APP_PATH)avr-objdump.exe$(QUOTE) -h -S $(QUOTE)$<$(QUOTE) > $(QUOTE)$@$(QUOTE)

size: $(OUTPUT_FILE_PATH)
	@$(QUOTE)$(AVR_APP_PATH)avr-size.exe$(QUOTE) -C --mcu=atmega168 $(OUTPUT_FILE_PATH_AS_ARGS)

# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS)$(C_DEPS_AS_ARGS) $(EXECUTABLES) $(LIB_AS_ARGS) $(HEX_FLASH_FILE_PATH_AS_ARGS) $(HEX_EEPROM_FILE_PATH_AS_ARGS) $(LSS_FILE_PATH_AS_ARGS) $(MAP_FILE_PATH_AS_ARGS)

