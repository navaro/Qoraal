# Compiler and linker definitions
CC = gcc

# Source files for the core module
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
    src/common/strsub.c

# Include directories (explicitly list only relevant directories with headers)
INC_DIRS := include
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Add precompiled defines
DEFINES := -DCFG_OS_POSIX

CPPFLAGS ?= $(INC_FLAGS) $(DEFINES) -MMD -MP
CFLAGS = -Os

# Object and dependency files for the core module
QORAAL_OBJS := $(QORAAL_SRCS:%=$(BUILD_DIR)/%.o)
QORAAL_DEPS := $(QORAAL_OBJS:.o=.d)

# C source compilation rule for core module
$(BUILD_DIR)/%.c.o: %.c
	$(call MKDIR,$(call trim,$(dir $@)))
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

-include $(QORAAL_DEPS)
