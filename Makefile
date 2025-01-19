# Compiler and linker definitions
CC = gcc
LD = ld

# Qoraal source files
QORAAL_SRCS := \
    src/debug.c \
    src/os_mlock.c \
    src/os_posix.c \
    src/os.c \
    src/qoraal.c \
    src/svc/svc_events.c \
    src/svc/svc_logger.c \
    src/svc/svc_services.c \
    src/svc/svc_shell.c \
    src/svc/svc_tasks.c \
    src/svc/svc_threads.c \
    src/svc/svc_wdt.c \
    src/common/cbuffer.c \
    src/common/dictionary.c \
    src/common/lists.c \
    src/common/mlog.c \
    src/common/rtclib.c \
    src/common/strsub.c \
    src/qshell/posixcmd.c \
    src/qshell/servicescmd.c

# Test source files
TEST_SRCS := \
    test/main.c \
    test/demo.c \
    test/shell.c \
    test/system.c

# Build settings
CFLAGS = -O0  -g
LDFLAGS = -lpthread --static -Xlinker -Map=output.map -T test/posix.ld

TARGET_EXEC ?= qoraal
BUILD_DIR ?= build

# Include directories (explicitly list only relevant directories with headers)
INC_DIRS := include
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Add precompiled defines
DEFINES := -DCFG_OS_POSIX

CPPFLAGS ?= $(INC_FLAGS) $(DEFINES) -MMD -MP

# Object and dependency files for Qoraal
QORAAL_OBJS := $(QORAAL_SRCS:%=$(BUILD_DIR)/%.o)
QORAAL_DEPS := $(QORAAL_OBJS:.o=.d)

# Object and dependency files for Test
TEST_OBJS := $(TEST_SRCS:%=$(BUILD_DIR)/%.o)
TEST_DEPS := $(TEST_OBJS:.o=.d)

# Detect operating system
OS := $(shell uname -s)

# Define commands for mkdir and rm based on OS
ifeq ($(OS), Linux)
  MKDIR = mkdir -p  $1
  RM = rm -rf
else ifeq ($(OS), Darwin)
  MKDIR = mkdir -p  $1
  RM = rm -rf
else
  MKDIR = mkdir "$(subst /,\\,$(1))" >nul 2>&1 || exit 0
  RM = rmdir /s /q "$(subst /,\\,$(1))"
endif

# Trim leading and trailing spaces
trim = $(strip $(1))

# Build target for test program
$(BUILD_DIR)/$(TARGET_EXEC): $(QORAAL_OBJS) $(TEST_OBJS)
	$(call MKDIR,$(BUILD_DIR))
	$(CC) $(QORAAL_OBJS) $(TEST_OBJS) -o $@ $(LDFLAGS)

# C source compilation rule
$(BUILD_DIR)/%.c.o: %.c
	$(call MKDIR,$(call trim,$(dir $@)))
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(call RM,$(BUILD_DIR))

-include $(QORAAL_DEPS)
-include $(TEST_DEPS)
