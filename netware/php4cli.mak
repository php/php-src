# Temporarily here -- later may go into some batch file
# which will set this as an environment variable
PROJECT_ROOT = ../..

# Module details
MODULE_NAME = php
MODULE_DESC = "PHP 4.2.3 - Command Line Interface"
VMAJ = 2
VMIN = 0
VREV = 0

#include the common settings
include $(PROJECT_ROOT)/netware/common.mif

# Extensions of all input and output files
.SUFFIXES:
.SUFFIXES: .nlm .lib .obj .cpp .c .msg .mlc .mdb .xdc .d

# Source files
C_SRC = getopt.c \
        php_cli.c

# Library files
LIBRARY =

# Destination directories and files
OBJ_DIR = $(BUILD)
FINAL_DIR = $(BUILD)
MAP_FILE = $(FINAL_DIR)\$(MODULE_NAME).map
OBJECTS = $(addprefix $(OBJ_DIR)/,$(CPP_SRC:.cpp=.obj) $(C_SRC:.c=.obj))
DEPDS  = $(addprefix $(OBJ_DIR)/,$(CPP_SRC:.cpp=.d) $(C_SRC:.c=.d))

# Binary file
ifndef BINARY
	BINARY=$(FINAL_DIR)\$(MODULE_NAME).nlm
endif


# Compile flags
C_FLAGS += -c -maxerrors 25 -msgstyle gcc
C_FLAGS += -wchar_t on -bool on
C_FLAGS += -processor Pentium
C_FLAGS += -w nounusedarg -msext on
C_FLAGS += -nostdinc
C_FLAGS += -relax_pointers		# To remove type-casting errors
C_FLAGS += -DNETWARE -DTHREAD_SWITCH
C_FLAGS += -DZTS
C_FLAGS += -DNLM_PLATFORM
C_FLAGS += -DN_PLAT_NLM -DNLM=1 -D__NO_MATH_OPS
C_FLAGS += -D__C9X_CMATH_INLINES_DEFINED -DAPACHE_OS_H -DNO_USE_SIGACTION -DMULTITHREAD
C_FLAGS += -DNEW_LIBC
C_FLAGS += -I. -I- -I. -I../../netware -I$(SDK_DIR)/include		# ../../netware added for special SYS/STAT.H
C_FLAGS += -I$(MWCIncludes)
C_FLAGS += -I- -I../../main -I../../Zend -I../../TSRM -I../../ext/standard
C_FLAGS += -I../../ -I../../netware -I$(PROJECT_ROOT)/regex
C_FLAGS += -I$(WINSOCK_DIR)/include/nlm -I$(WINSOCK_DIR)/include

# Extra stuff based on debug / release builds
ifeq '$(BUILD)' 'debug'
	SYM_FILE = $(FINAL_DIR)\$(MODULE_NAME).sym
	C_FLAGS  += -inline smart -sym on -sym codeview4 -opt off -opt intrinsics -sym internal -DDEBUGGING -DDKFBPON
	C_FLAGS += -r -DZEND_DEBUG=1
	C_FLAGS += -exc cw
	LD_FLAGS += -sym on -sym codeview4 -sym internal -osym $(SYM_FILE) 
	export MWLibraryFiles=$(SDK_DIR)/imports/libcpre.o;mwcrtld.lib
else
	C_FLAGS  += -opt speed -inline on -inline smart -inline auto -sym off -DZEND_DEBUG=0
	C_FLAGS += -opt intrinsics
	C_FLAGS += -opt level=4
	LD_FLAGS += -sym off
	export MWLibraryFiles=$(SDK_DIR)/imports/libcpre.o;mwcrtl.lib
endif


# Dependencies
MODULE = LibC   \
         phplib
IMPORT = @$(SDK_DIR)/imports/libc.imp        \
         @$(PROJECT_ROOT)/netware/phplib.imp
EXPORT = 
API    =


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

$(OBJ_DIR)/%.obj: %.c
	@echo Compiling $?...
	@$(CC) $< $(C_FLAGS) -o $@


$(BINARY): $(OBJECTS)
	@echo Import $(IMPORT) > $(basename $@).def
ifdef API
	@echo Import $(API) >> $(basename $@).def
endif
	@echo Module $(MODULE) >> $(basename $@).def
ifdef EXPORT
	@echo Export $(EXPORT) >> $(basename $@).def
endif
	@echo AutoUnload >> $(basename $@).def
ifeq '$(BUILD)' 'debug'
	@echo Debug >> $(basename $@).def
endif
	@echo Flag_On 0x00000008 >> $(basename $@).def
	@echo Start _LibCPrelude >> $(basename $@).def
	@echo Exit _LibCPostlude >> $(basename $@).def

	$(MPKTOOL) $(XDCFLAGS) $(basename $@).xdc
	@echo xdcdata $(basename $@).xdc >> $(basename $@).def

	@echo Linking $@...
	@echo $(LD_FLAGS) -commandfile $(basename $@).def > $(basename $@).link
ifdef LIBRARY
	@echo $(LIBRARY) >> $(basename $@).link
endif
	@echo $(OBJECTS) >> $(basename $@).link

	@$(LINK) @$(basename $@).link


.PHONY: clean
clean: cleanobj cleanbin

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
	-@del "$(FINAL_DIR)\$(MODULE_NAME).nlm"
	@echo Deleting MAP, DEF files, etc....
	-@del "$(FINAL_DIR)\$(MODULE_NAME).map"
	-@del "$(FINAL_DIR)\$(MODULE_NAME).def"
	-@del "$(FINAL_DIR)\$(MODULE_NAME).link"
ifeq '$(BUILD)' 'debug'
	-@del $(FINAL_DIR)\$(MODULE_NAME).sym
endif
