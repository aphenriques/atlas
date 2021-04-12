LURLPARSER_ROOT_DIR:=.

include common.mk

INSTALL_TOP:=/usr/local
INSTALL_INC:=$(INSTALL_TOP)/include/$(LURLPARSER)
INSTALL_LIB:=$(INSTALL_TOP)/lib

.PHONY: all, static, static_release, shared, shared_release, sample, install_include, install_static, install, uninstall, clean

# Any of the following make rules can be executed with the `-j` option (`make -j`) for parallel compilation 

all:
	cd $(LURLPARSER_LIB_DIR) && $(MAKE) $@

static:
	cd $(LURLPARSER_LIB_DIR) && $(MAKE) $@

static_release: clean
	cd $(LURLPARSER_LIB_DIR) && $(MAKE) static OPTIMIZED=y WITH_FPIC=n

shared:
	cd $(LURLPARSER_LIB_DIR) && $(MAKE) $@

shared_release:
	cd $(LURLPARSER_LIB_DIR) && $(MAKE) shared OPTIMIZED=y

sample: static
	cd $(LURLPARSER_BIN_DIR) && $(MAKE) all

install_include:
	mkdir -p $(INSTALL_INC) $(INSTALL_LIB)
	install -p -m 0644 $(LURLPARSER_LIB_DIR)/*.hpp $(INSTALL_INC)

install_static:
	mkdir -p $(INSTALL_INC) $(INSTALL_LIB)
	install -p -m 0644 $(LURLPARSER_LIB_DIR)/*.hpp $(INSTALL_INC)
	install -p -m 0644 $(LURLPARSER_LIB_DIR)/$(LURLPARSER_STATIC_LIB) $(INSTALL_LIB)

install:
	mkdir -p $(INSTALL_INC) $(INSTALL_LIB)
	install -p -m 0644 $(LURLPARSER_LIB_DIR)/*.hpp $(INSTALL_INC)
	install -p -m 0644 $(LURLPARSER_LIB_DIR)/$(LURLPARSER_STATIC_LIB) $(LURLPARSER_LIB_DIR)/$(LURLPARSER_SHARED_LIB) $(INSTALL_LIB)

uninstall:
	$(RM) -R $(INSTALL_INC)
	$(RM) $(INSTALL_LIB)/$(LURLPARSER_STATIC_LIB) $(INSTALL_LIB)/$(LURLPARSER_SHARED_LIB)

clean:
	cd $(LURLPARSER_LIB_DIR) && $(MAKE) $@
	cd $(LURLPARSER_BIN_DIR) && $(MAKE) $@

