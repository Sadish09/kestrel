PROJECT := kestrel_bootloader

BUILD_DIR := build
SRC_DIR := src
STARTUP_DIR := startup
INC_DIR := include
LD_SCRIPT := ld/stm32f103c8_bootloader.ld

CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE := arm-none-eabi-size

C_SOURCES := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(STARTUP_DIR)/*.c)
OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
DEPS := $(OBJECTS:.o=.d)

MCU_FLAGS := -mcpu=cortex-m3 -mthumb
CFLAGS := $(MCU_FLAGS) -std=c11 -Wall -Wextra -Werror -Os -ffunction-sections -fdata-sections
CFLAGS += -I$(INC_DIR) -MMD -MP
LDFLAGS := $(MCU_FLAGS) -T$(LD_SCRIPT) -nostartfiles -Wl,--gc-sections -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map

ELF := $(BUILD_DIR)/$(PROJECT).elf
BIN := $(BUILD_DIR)/$(PROJECT).bin
HEX := $(BUILD_DIR)/$(PROJECT).hex

.PHONY: all clean size layout

all: $(ELF) $(BIN) $(HEX) size

$(ELF): $(OBJECTS) $(LD_SCRIPT)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

size: $(ELF)
	$(SIZE) $<

layout:
	@echo "Bootloader flash: 0x08000000 - 0x08003BFF"
	@echo "Metadata page:    0x08003C00 - 0x08003FFF"
	@echo "Application:      0x08004000 - 0x0800FFFF"

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
