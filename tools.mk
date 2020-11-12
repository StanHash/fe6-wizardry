
# making sure devkitARM exists and is set up
ifeq ($(strip $(DEVKITARM)),)
  $(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

# including devkitARM tool definitions
include $(DEVKITARM)/base_tools

ifeq ($(OS),Windows_NT)
  MONO :=
  EXE  := .exe
else
  MONO := mono
  EXE  :=
endif

# Making sure we are using python 3
ifeq ($(shell python -c 'import sys; print(int(sys.version_info[0] > 2))'),1)
  export PYTHON3 := python
else
  export PYTHON3 := python3
endif

# EA

EA_DIR := Tools/EventAssembler
EA := $(MONO) $(EA_DIR)/ColorzCore.exe

# EA Tools

LYN := $(EA_DIR)/Tools/lyn$(EXE)

# Other Tools

EA_DEP := Tools/ea-dep$(EXE)
ELF2REF := $(PYTHON3) Tools/Scripts/elf2ref.py
