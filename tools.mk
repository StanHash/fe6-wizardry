
# making sure devkitARM exists and is set up
ifeq ($(strip $(DEVKITARM)),)
  $(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

# including devkitARM tool definitions
include $(DEVKITARM)/base_tools

# Making sure we are using python 3
ifeq ($(shell python -c 'import sys; print(int(sys.version_info[0] > 2))'),1)
  export PYTHON3 := python
else
  export PYTHON3 := python3
endif

# EA

EA_DIR := Tools/EventAssembler
EA := $(EA_DIR)/ColorzCore

# EA Tools

LYN := $(EA_DIR)/Tools/lyn

# Other Tools

EA_DEP := $(EA_DIR)/ea-dep
ELF2REF := $(PYTHON3) Tools/Scripts/elf2ref.py
ELF2SYM := $(PYTHON3) Tools/Scripts/elf2sym.py
