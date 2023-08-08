TOOLCHAIN ?= $(DEVKITARM)

ifneq (,$(TOOLCHAIN))
  export PATH := $(TOOLCHAIN)/bin:$(PATH)
endif

PREFIX := arm-none-eabi-

CC := $(PREFIX)gcc
AS := $(PREFIX)as
OBJCOPY := $(PREFIX)objcopy

# Making sure we are using python 3
ifeq ($(shell python -c 'import sys; print(int(sys.version_info[0] > 2))'),1)
  export PYTHON3 := python
else
  export PYTHON3 := python3
endif

# EA

EA_DIR   := tools/EventAssembler
EA       := $(EA_DIR)/ColorzCore
EA_DEP   := $(EA_DIR)/ea-dep
LYN      := $(EA_DIR)/Tools/lyn
PNG2DMP  := $(EA_DIR)/Tools/Png2Dmp
COMPRESS := $(EA_DIR)/Tools/compress

# Other Tools

ELF2REF := $(PYTHON3) tools/scripts/elf2ref.py
ELF2SYM := $(PYTHON3) tools/scripts/elf2sym.py
