CC      := gcc
CFLAGS  := -Wall -Wextra -Werror
BUILD   := build
SRC_DIR := src

SRCS    := $(wildcard $(SRC_DIR)/*.c)
OBJS    := $(patsubst $(SRC_DIR)/%.c,$(BUILD)/%.o,$(SRCS))

TARGET  := $(BUILD)/lz77archive

.PHONY: all clean rebuild
.INTERMEDIATE: $(OBJS)

all: $(TARGET)

$(TARGET): $(BUILD) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(BUILD)/%.o: $(SRC_DIR)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)

rebuild: clean all