ARMGNU ?= aarch64-linux-gnu

CFLAGS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only -MP -MD -g


SRC_DIR = src
BUILD_DIR = build
SUB_SRC_DIRS = . mm user




all: kernel8.img

qemu: kernel8.img
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -serial null -serial stdio -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb




clean: 
	rm -rf $(BUILD_DIR) *.img

C_FILES=$(foreach D, $(SUB_SRC_DIRS), $(wildcard $(SRC_DIR)/$(D)/*.c))
ASB_FILES=$(foreach D, $(SUB_SRC_DIRS), $(wildcard $(SRC_DIR)/$(D)/*.S))
OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%_c.o,$(C_FILES))
OBJECTS+=$(patsubst $(SRC_DIR)/%.S,$(BUILD_DIR)/%_s.o,$(ASB_FILES))
DEPFILES=$(patsubst %.o,%.d,$(OBJECTS))

kernel8.img: $(SRC_DIR)/linker.ld $(OBJECTS)
	$(ARMGNU)-ld -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf $(OBJECTS)
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(ARMGNU)-gcc $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	@mkdir -p $(dir $@)
	$(ARMGNU)-gcc $(CFLAGS) -c $< -o $@

-include $(DEPFILES)
