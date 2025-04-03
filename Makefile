TARGET_EXEC := psh
BUILD_DIR := ./build
SRC_DIRS := ./src
SRCS := $(shell find $(SRC_DIRS) -name '*.c' -or -name '*.s')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_DIRS += readline/build/include ncurses-snapshots/build/include
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS += $(INC_FLAGS) -MMD -MP

LDFLAGS += -Lreadline/build/lib -Lncurses-snapshots/build/lib
LDFLAGS += -lreadline -lncursesw -lc

# musl specs
START_OBJS = /usr/lib/musl/lib/Scrt1.o /usr/lib/musl/lib/crti.o
END_OBJS = /usr/lib/musl/lib/crtn.o

CFLAGS += -nostdinc -isystem /usr/lib/musl/include -isystem include
LDFLAGS += -dynamic-linker=/lib/ld-musl-x86_64.so.1 -rpath-link /lib/ld-musl-x86_64.so.1 -nostdlib -s
LDFLAGS += -L/usr/lib/musl/lib -L .
LDFLAGS += $(START_OBJS) $(END_OBJS)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(LD) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

-include $(DEPS)
