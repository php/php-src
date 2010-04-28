# Temporarily here -- later may go into some batch file
# which will set this as an environment variable
PROJECT_ROOT = ..\..

# Module details
MODULE_NAME = phppgsql
MODULE_DESC = "PHP 4.3 - PostgreSQL Extension"
VMAJ = 3
VMIN = 0
VREV = 0

#include the common settings
include $(PROJECT_ROOT)/netware/common.mif

# Build type defaults to 'release'
ifndef BUILD
BUILD = release
endif

# Extensions of all input and output files
.SUFFIXES:
.SUFFIXES: .nlm .lib .obj .cpp .c .msg .mlc .mdb .xdc .d

# Source files
C_SRC = pgsql.c \
        start.c

CPP_SRC_NODIR = $(notdir $(CPP_SRC))
C_SRC_NODIR = $(notdir $(C_SRC))
SRC_DIR = $(dir $(CPP_SRC) $(C_SRC))

# Library files
LIBRARY = 

# Destination directories and files
OBJ_DIR = $(BUILD)
FINAL_DIR = $(BUILD)
OBJECTS  = $(addprefix $(OBJ_DIR)/,$(CPP_SRC_NODIR:.c=.obj) $(C_SRC_NODIR:.c=.obj))
DEPDS  = $(addprefix $(OBJ_DIR)/,$(CPP_SRC_NODIR:.c=.d) $(C_SRC_NODIR:.c=.d))

# Binary file
ifndef BINARY
	BINARY=$(FINAL_DIR)\$(MODULE_NAME).nlm
endif

# Compile flags
C_FLAGS += -c -maxerrors 25 -msgstyle gcc -wchar_t on -bool on -processor Pentium
C_FLAGS += -nostdinc -nosyspath  
C_FLAGS += -relax_pointers		# To remove type-casting errors
C_FLAGS += -DNETWARE -DZTS -DNEW_LIBC -DUSE_OLD_FUNCTIONS
C_FLAGS += -D__BIT_TYPES_DEFINED__ -DCOMPILE_DL_PGSQL=1
C_FLAGS += -I. -I- -I$(PROJECT_ROOT) -I$(PROJECT_ROOT)/main
C_FLAGS += -I$(PROJECT_ROOT)/ext/standard -I$(PROJECT_ROOT)/netware
C_FLAGS += -I$(PROJECT_ROOT)/zend -I$(PROJECT_ROOT)/tsrm
C_FLAGS += -I$(SDK_DIR)/include -I$(MWCIncludes)
C_FLAGS += -I$(WINSOCK_DIR)/include/nlm -I$(WINSOCK_DIR)/include


# Extra stuff based on debug / release builds
ifeq '$(BUILD)' 'debug'
	SYM_FILE = $(FINAL_DIR)\$(MODULE_NAME).sym
	C_FLAGS  += -inline smart -sym on -sym codeview4 -opt off -opt intrinsics -DDEBUGGING -DDKFBPON
	C_FLAGS += -exc cw -DZEND_DEBUG=1
	LD_FLAGS += -sym on -sym codeview4 -osym $(SYM_FILE)
	export MWLibraryFiles=$(SDK_DIR)/imports/libcpre.o;mwcrtld.lib
else
	C_FLAGS  += -opt speed -inline on -inline smart -inline auto -sym off -opt intrinsics
	C_FLAGS += -opt level=4 -DZEND_DEBUG=0
	LD_FLAGS += -sym off
	export MWLibraryFiles=$(SDK_DIR)/imports/libcpre.o;mwcrtl.lib
endif


# Dependencies
MODULE = LibC   \
         phplib \
         libpq
IMPORT = @$(SDK_DIR)/imports/libc.imp        \
         @$(SDK_DIR)/imports/ws2nlm.imp      \
         @$(MPK_DIR)/import/mpkOrg.imp       \
         @$(PROJECT_ROOT)/netware/phplib.imp \
         @$(PROJECT_ROOT)/netware/libpq.imp
EXPORT = ($(MODULE_NAME)) get_module
API = OutputToScreen

# Virtual paths
vpath %.cpp .
vpath %.c . ..\..\netware
vpath %.obj $(OBJ_DIR)


all: prebuild project

.PHONY: all

prebuild:
	@if not exist $(OBJ_DIR) md $(OBJ_DIR)

project: $(BINARY)
	@echo Build complete.

$(OBJ_DIR)/%.d: %.cpp
	@echo Building Dependencies for $(<F)
	@$(CC) -M $< $(C_FLAGS) -o $@

$(OBJ_DIR)/%.d: %.c
	@echo Building Dependencies for $(<F)
	@$(CC) -M $< $(C_FLAGS) -o $@

$(OBJ_DIR)/%.obj: %.cpp
	@echo Compiling $?...
	@$(CC) $< $(C_FLAGS) -o $@
	
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

	@echo $(LIBRARY) $(OBJECTS) >> $(basename $@).link

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
