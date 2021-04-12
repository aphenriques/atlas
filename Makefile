ATLAS_ROOT_DIR:=.

include common.mk

INSTALL_TOP:=/usr/local
INSTALL_INC:=$(INSTALL_TOP)/include/$(ATLAS)
INSTALL_LIB:=$(INSTALL_TOP)/lib

.PHONY: all, static, shared, samples, dependencies, dependencies_clean, dependencies_install, dependencies_uninstall, lurlparser_static, lurlparser_clean, lurlparser_install_include, lurlparser_uninstall, exception_install, exception_uninstall, install_static, install_shared, install, uninstall, clean

# Any of the following make rules can be executed with the `-j` option (`make -j`) for parallel compilation 

all:
	cd $(ATLAS_LIB_DIR) && $(MAKE) $@

static:
	cd $(ATLAS_LIB_DIR) && $(MAKE) $@

shared:
	cd $(ATLAS_LIB_DIR) && $(MAKE) $@

samples: static
	cd $(ATLAS_BIN_DIR) && $(MAKE) all

dependencies: lurlparser_static

dependencies_clean: lurlparser_clean

dependencies_install: lurlparser_install_include exception_install

dependencies_uninstall: lurlparser_uninstall exception_uninstall

lurlparser_static:
	cd $(LURLPARSER_ROOT_DIR) && $(MAKE) static

lurlparser_clean:
	cd $(LURLPARSER_ROOT_DIR) && $(MAKE) clean

lurlparser_install_include:
	cd $(LURLPARSER_ROOT_DIR) && $(MAKE) install_static

lurlparser_uninstall:
	cd $(LURLPARSER_ROOT_DIR) && $(MAKE) uninstall

exception_install:
	cd $(ATLAS_DEPENDENCIES_DIR)/exception && $(MAKE) install

exception_uninstall:
	cd $(ATLAS_DEPENDENCIES_DIR)/exception && $(MAKE) uninstall

install_static:
	mkdir -p $(INSTALL_INC) $(INSTALL_LIB)
	install -p -m 0644 $(ATLAS_LIB_DIR)/*.hpp $(INSTALL_INC)
	install -p -m 0644 $(ATLAS_LIB_DIR)/$(ATLAS_STATIC_LIB) $(INSTALL_LIB)

install_shared:
	mkdir -p $(INSTALL_INC) $(INSTALL_LIB)
	install -p -m 0644 $(ATLAS_LIB_DIR)/*.hpp $(INSTALL_INC)
	install -p -m 0644 $(ATLAS_LIB_DIR)/$(ATLAS_SHARED_LIB) $(INSTALL_LIB)

install:
	mkdir -p $(INSTALL_INC) $(INSTALL_LIB)
	install -p -m 0644 $(ATLAS_LIB_DIR)/*.hpp $(INSTALL_INC)
	install -p -m 0644 $(ATLAS_LIB_DIR)/$(ATLAS_STATIC_LIB) $(ATLAS_LIB_DIR)/$(ATLAS_SHARED_LIB) $(INSTALL_LIB)

uninstall:
	$(RM) -R $(INSTALL_INC)
	$(RM) $(INSTALL_LIB)/$(ATLAS_STATIC_LIB) $(INSTALL_LIB)/$(ATLAS_SHARED_LIB)

clean:
	cd $(ATLAS_LIB_DIR) && $(MAKE) $@
	cd $(ATLAS_BIN_DIR) && $(MAKE) $@
