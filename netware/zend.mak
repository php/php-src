# Temporarily here -- later may go into some batch file
# which will set this as an environment variable
PROJECT_ROOT = ..

# Module details
MODULE_NAME = ZEND

#MODULE_ROOT = $(PROJECT_ROOT)\\$(MODULE_NAME)

#include the common settings
include $(PROJECT_ROOT)/netware/common.mif

# Extensions of all input and output files
.SUFFIXES:
.SUFFIXES: .nlm .lib .obj .cpp .c .msg .mlc .mdb .xdc .d

# Source files
##CC_SRC = zend_ini_scanner_cc.cc \
##         zend_language_scanner_cc.cc

##CPP_SRC = zend_ini_scanner.cpp \
##          zend_language_scanner.cpp

C_SRC = zend.c \
        zend_alloc.c \
        zend_API.c \
        zend_builtin_functions.c \
        zend_compile.c \
        zend_constants.c \
        zend_dynamic_array.c \
        zend_execute.c \
        zend_execute_API.c \
        zend_extensions.c \
        zend_hash.c \
        zend_highlight.c \
        zend_indent.c \
        zend_ini.c \
        zend_list.c \
        zend_llist.c \
        zend_opcode.c \
        zend_operators.c \
        zend_ptr_stack.c \
        zend_qsort.c \
        zend_sprintf.c \
        zend_stack.c \
        zend_static_allocator.c \
        zend_variables.c \
        zend_ini_parser.c \
        zend_ini_scanner.c \
        zend_language_parser.c \
        zend_language_scanner.c


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

#C_FLAGS = -c -maxerrors 25 -processor Pentium -align packed
#C_FLAGS += -msgstyle gcc -ext obj -ARM on
#C_FLAGS += -cpp_exceptions on -wchar_t off -bool on -w on -ansi off -nostdinc -nosyspath
C_FLAGS  = -c -maxerrors 25 -msgstyle std
C_FLAGS += -wchar_t on -bool on
C_FLAGS += -processor Pentium -align 1
#C_FLAGS += -r
C_FLAGS += -nostdinc
#C_FLAGS += -nosyspath -ext obj
C_FLAGS += -D__C9X_CMATH_INLINES_DEFINED
C_FLAGS += -DNETWARE -DZTS
C_FLAGS += -DCLIB_STAT_PATCH
C_FLAGS += -DTHREAD_SWITCH
#C_FLAGS += -I. -I- -I../netware -I$(SDK_DIR)/sdk	# ../netware added for special SYS/STAT.H : Venkat(6/2/02)
C_FLAGS += -I. -I- -I../netware -I$(SDK_DIR)/include	# ../netware added for special SYS/STAT.H : Venkat(6/2/02)
C_FLAGS += -I$(MWCIncludes)

#C_FLAGS  += -lang c++ -prefix NLM_PREFIX.H
#C_FLAGS  += -ext obj -msext on -r -RTTI on

# Link flags
LD_FLAGS  = -type library
LD_FLAGS += -o $(BINARY)
#LDFLAGS += -zerobss -nofail -msgstyle gcc -nostderr


# Extra stuff based on debug / release builds
ifeq '$(BUILD)' 'debug'
	C_FLAGS  += -DZEND_DEBUG
	C_FLAGS  += -inline smart -sym on -sym codeview4 -sym internal -opt off -opt intrinsics
	LD_FLAGS += -sym codeview4 -sym internal
	export MWLibraryFiles=$(SDK_DIR)/imports/libcpre.o;mwcrtld.lib
else
	C_FLAGS  += -opt speed -inline on -inline auto -sym off
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


##zend_ini_parser.c zend_ini_parser.h : zend_ini_parser.y
##	@bison --output=$@ -v -d -p ini_ zend_ini_parser.y

##zend_ini_scanner.c : zend_ini_scanner.l
##	@flex -i -Pini_ -o$@ zend_ini_scanner.l

##zend_language_parser.c zend_language_parser.h : zend_language_parser.y
##	@bison --output=$@ -v -d -p zend zend_language_parser.y

##zend_language_scanner.c : zend_language_scanner.l
##	@flex -i -Pzend -o$@ zend_language_scanner.l


#zend_ini_scanner.cpp : zend_ini_scanner.l
#	@flex -+ -B -i -Sflex.skl -Pini_ -o$@ zend_ini_scanner.l

#zend_language_scanner.cpp : zend_language_scanner.l
#	@flex -+ -B -i -Sflex.skl -Pzend -o$@ zend_language_scanner.l


#$(OBJ_DIR)/%.d: %.cpp
#	@echo Building Dependencies for $(<F)
#	@$(CC) -M $< $(C_FLAGS) -o $@

$(OBJ_DIR)/%.d: %.c
	@echo Building Dependencies for $(<F)
	@$(CC) -M $< $(C_FLAGS) -o $@

#$(OBJ_DIR)/%.d: %.cc
#	@echo Building Dependencies for $(<F)
#	@$(CC) -M $< $(C_FLAGS) -o $@

#$(OBJ_DIR)/%.obj: %.cpp
#	@echo Compiling $?...
#	@$(CC) $< $(C_FLAGS) -o $@
	
$(OBJ_DIR)/%.obj: %.c
	@echo Compiling $?...
	@$(CC) $< $(C_FLAGS) -o $@

#$(OBJ_DIR)/%.obj: %.cc
#	@echo Compiling $?...
#	@$(CC) $< $(C_FLAGS) -o $@


$(BINARY): $(DEPDS) $(OBJECTS)
	@echo Linking $@...
	@$(LINK) $(LD_FLAGS) $(OBJECTS)


.PHONY: clean
clean: cleansrc cleand cleanobj cleanbin

.PHONY: cleansrc
cleansrc:
	@echo Deleting all generated source files...
	-@del "zend_ini_parser.c"
	-@del "zend_ini_parser.h"
	-@del "zend_ini_scanner.c"
	-@del "zend_language_parser.c"
	-@del "zend_language_parser.h"
	-@del "zend_language_scanner.c"
	-@del "zend_ini_parser.output"
	-@del "zend_language_parser.output"

#	-@del "zend_ini_scanner.cpp"
#	-@del "zend_language_scanner.cpp"


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
#	@echo Deleting MAP, DEF files, etc....
#	-@del "$(FINAL_DIR)\$(MODULE_NAME).map"
#	-@del "$(FINAL_DIR)\$(MODULE_NAME).def"
#	-@del "$(FINAL_DIR)\$(MODULE_NAME).link"
#ifeq '$(BUILD)' 'debug'
#	-@del $(FINAL_DIR)\$(MODULE_NAME).sym
#endif
