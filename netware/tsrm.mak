# Temporarily here -- later may go into some batch file
# which will set this as an environment variable
PROJECT_ROOT = ..

# Module details
MODULE_NAME = TSRM

#MODULE_ROOT = $(PROJECT_ROOT)\\$(MODULE_NAME)

#include the common settings
include $(PROJECT_ROOT)/netware/common.mif

# Extensions of all input and output files
.SUFFIXES:
.SUFFIXES: .nlm .lib .obj .cpp .c .msg .mlc .mdb .xdc .d

# Source files
CPP_SRC = 
C_SRC = TSRM.c \
        tsrm_strtok_r.c \
        tsrm_virtual_cwd.c \
        tsrm_nw.c

# Destination directories and files
OBJ_DIR = $(BUILD)
FINAL_DIR = $(BUILD)
OBJECTS = $(addprefix $(OBJ_DIR)/,$(CPP_SRC:.cpp=.obj) $(C_SRC:.c=.obj))
DEPDS  = $(addprefix $(OBJ_DIR)/,$(CPP_SRC:.cpp=.d) $(C_SRC:.c=.d))

# Binary file
ifndef BINARY
	BINARY=$(FINAL_DIR)\$(MODULE_NAME).lib
endif

# Compile flags
#C_FLAGS   = -c -maxerrors 25 -processor Pentium -align packed
#C_FLAGS  += -w on -cpp_exceptions on -wchar_t off -bool on
#C_FLAGS  += -msgstyle gcc -ext obj -ARM on -msext off -ansi off -nostdinc
C_FLAGS  = -c -maxerrors 25 -msgstyle gcc
C_FLAGS += -wchar_t on -bool on
C_FLAGS += -processor Pentium -align 1
#C_FLAGS += -r
C_FLAGS += -nostdinc
#C_FLAGS +=  -ext obj
#C_FLAGS  += -DNLM_PLATFORM -D__GNUC__ -DTSRM_EXPORTS -D_LIB
C_FLAGS  += -DZTS -DNETWARE -DHAVE_DIRENT_H
C_FLAGS  += -DNEW_LIBC
#C_FLAGS  += -DUSE_PIPE_OPEN
C_FLAGS  += -DUSE_MKFIFO
C_FLAGS  += -DCLIB_STAT_PATCH -DUSE_MPK
#C_FLAGS  += -I. -I- -I../netware -I$(SDK_DIR)/sdk	# ../netware added for special SYS/STAT.H : Venkat(6/2/02)
C_FLAGS  += -I. -I- -I../netware -I$(SDK_DIR)/include	# ../netware added for special SYS/STAT.H : Venkat(6/2/02)
C_FLAGS  += -I$(MPK_DIR)/include1
C_FLAGS  += -I$(MWCIncludes)

# Link flags
LD_FLAGS  = -type library
LD_FLAGS += -o $(BINARY)

# Extra stuff based on debug / release builds
ifeq '$(BUILD)' 'debug'
	C_FLAGS  += -inline smart -sym on -sym codeview4 -sym internal -opt off -opt intrinsics -DTSRM_DEBUG=1
	LD_FLAGS += -sym codeview4 -sym internal
	export MWLibraryFiles=$(SDK_DIR)/imports/libcpre.o;mwcrtld.lib
else
	C_FLAGS  += -opt speed -inline smart -inline auto -sym off -DTSRM_DEBUG=0
	LD_FLAGS += -sym off
	export MWLibraryFiles=$(SDK_DIR)/imports/libcpre.o;mwcrtl.lib
endif

# Virtual paths
vpath %.cpp .
vpath %.c .
vpath %.obj $(OBJ_DIR)


all: prebuild project

.PHONY: all

prebuild:
	@if not exist $(OBJ_DIR) md $(OBJ_DIR)

project: $(BINARY) $(MESSAGE)
	@echo Build complete.

$(OBJ_DIR)/%.d: %.c
	@echo Building Dependencies for $(<F)
	@$(CC) -M $< $(C_FLAGS) -o $@

$(OBJ_DIR)/%.d: %.cpp
	@echo Building Dependencies for $(<F)
	@$(CC) -M $< $(C_FLAGS) -o $@

$(OBJ_DIR)/%.obj: %.cpp
	@echo Compiling $?...
	@$(CC) $< $(C_FLAGS) -o $@
	
$(OBJ_DIR)/%.obj: %.c
	@echo Compiling $?...
	@$(CC) $< $(C_FLAGS) -o $@

$(BINARY): $(DEPDS) $(OBJECTS)
	@echo Linking $@...
	@$(LINK) $(LD_FLAGS) $(OBJECTS)


.PHONY: clean
clean: cleand cleanobj cleanbin

.PHONY: cleand
cleand:
	@echo Deleting all dependency files...
	-@del "$(OBJ_DIR)\*.d"

.PHONY: cleanobj
cleanobj:
	@echo Deleting all object files...
	-@del "$(OBJ_DIR)\*.obj"

.PHONY: cleanbin
cleanbin:
	@echo Deleting binary files...
	-@del "$(FINAL_DIR)\$(MODULE_NAME).lib"
