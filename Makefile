TARGET_EXEC := psh
BUILD_DIR := ./build
SRC_DIRS := ./src
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_DIRS += readline/build/include ncurses-snapshots/build/include
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS := $(INC_FLAGS) -MMD -MP -Wall -Werror -g -Wextra -pedantic -std=c99

# musl specs
CFLAGS += -nostdinc -isystem /usr/lib/musl/include -isystem include
LDFLAGS += -rpath-link /lib/ld-musl-x86_64.so.1 -nostdlib
LDFLAGS += -L/usr/lib/musl/lib -L .

LDFLAGS := -Lreadline/build/lib -Lncurses-snapshots/build/lib
LDFLAGS := -lreadline -lncursesw -lc

START_OBJS = /usr/lib/musl/lib/Scrt1.o /usr/lib/musl/lib/crti.o
END_OBJS = /usr/lib/musl/lib/crtn.o

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(LD) $(OBJS) -o $@ $(LDFLAGS) $(START_OBJS) $(END_OBJS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

-include $(DEPS)
