
CACHE_DIR := .cache_dir
$(shell mkdir -p $(CACHE_DIR) > /dev/null)

# ====================
# = TOOL DEFINITIONS =
# ====================

include tools.mk

# ===========================
# = DECOMP FILE DEFINITIONS =
# ===========================

FE6_DIR := Tools/fe6

FE6_GBA := $(FE6_DIR)/fe6.gba
FE6_ELF := $(FE6_DIR)/fe6.elf

FE6_REFERENCE := $(CACHE_DIR)/fe6-reference.s
FE6_SYM := $(CACHE_DIR)/fe6.sym

# ===============
# = MAIN TARGET =
# ===============

HACK_GBA := hack.gba
MAIN_EVENT := Main.event

hack: $(HACK_GBA)
base: $(FE6_GBA)

.PHONY: hack base

MAIN_DEPENDS := $(shell $(EA_DEP) $(MAIN_EVENT) -I $(EA_DIR) --add-missings)

$(HACK_GBA): $(FE6_GBA) $(FE6_SYM) $(MAIN_EVENT) $(MAIN_DEPENDS)
	cp -f $(FE6_GBA) $(HACK_GBA)
	$(EA) A FE6 -input:$(MAIN_EVENT) -output:$(HACK_GBA) --nocash-sym || rm -f $(HACK_GBA)
	cat $(FE6_SYM) >> $(HACK_GBA:.gba=.sym)

# =================
# = DECOMP TARGET =
# =================

$(FE6_REFERENCE): $(FE6_ELF)
	$(ELF2REF) $(FE6_ELF) > $(FE6_REFERENCE)

$(FE6_SYM): $(FE6_ELF)
	$(ELF2SYM) $(FE6_ELF) > $(FE6_SYM)

$(FE6_ELF): FORCE
	@$(MAKE) -s -C $(FE6_DIR)

$(FE6_GBA): $(FE6_ELF)
	@touch $(FE6_GBA)

FORCE:
.PHONY: FORCE

# ===================
# = COMPONENT RULES =
# ===================

include wizardry.mk
