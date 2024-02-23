ARMGNU ?= aarch64-none-linux-gnu


BUILD_DIR = build
CFLAGS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only -MP -MD


CFILES = $(wildcard *.c)
ASB_FILES = $(wildcard *.S)

OBJECTS = $(patsubst %.c, $(BUILD_DIR)/%_c.o, $(CFILES))
OBJECTS += $(patsubst %.S, $(BUILD_DIR)/%_s.o, $(ASB_FILES))

DEPFILES = $(patsubst %.o, %.d, $(OBJECTS))

all: $(BUILD_DIR)/user_program.img

$(BUILD_DIR)/user_program.img: $(BUILD_DIR)/user_program.elf
	$(ARMGNU)-objcopy $(BUILD_DIR)/user_program.elf -O binary user_program.img

$(BUILD_DIR)/user_program.elf: $(OBJECTS)
	$(ARMGNU)-ld -T user_program.ld -o $(BUILD_DIR)/user_program.elf $(OBJECTS)

$(BUILD_DIR)/%_c.o: %.c
	@mkdir -p $(dir $@)
	$(ARMGNU)-gcc $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%_s.o: %.S
	@mkdir -p $(dir $@)
	$(ARMGNU)-gcc $(CFLAGS) -c $< -o $@


clean:
	rm -rf $(BUILD_DIR) *.img