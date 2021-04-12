ATLAS:=atlas

ATLAS_LIB_ROOT_DIR:=lib
ATLAS_LIB_DIR:=$(ATLAS_LIB_ROOT_DIR)/$(ATLAS)
ATLAS_BIN_DIR:=sample
ATLAS_DEPENDENCIES_DIR:=dependencies
ATLAS_STATIC_LIB:=lib$(ATLAS).a

# ATLAS_ROOT_DIR is defined later. That's why = is used instead of :=
LURLPARSER_ROOT_DIR=$(ATLAS_ROOT_DIR)/$(ATLAS_DEPENDENCIES_DIR)/LUrlParser

include $(LURLPARSER_ROOT_DIR)/common.mk

ATLAS_EXCEPTION_INCLUDE_DIR=$(ATLAS_ROOT_DIR)/$(ATLAS_DEPENDENCIES_DIR)/exception/include
ATLAS_STATIC_LIB_INCLUDE_DIR=$(ATLAS_ROOT_DIR)/$(ATLAS_LIB_ROOT_DIR)
ATLAS_STATIC_LIB_LDLIB=$(ATLAS_ROOT_DIR)/$(ATLAS_LIB_DIR)/$(ATLAS_STATIC_LIB)

# extra flags may be added here or in make invocation. E.g: make EXTRA_CXXFLAGS=-g
EXTRA_CXXFLAGS:=
EXTRA_LDFLAGS:=

ifeq ($(SANITIZED_THREAD), y)
ifneq ($(OPTIMIZED), y)
SANITIZE_FLAGS:=-fsanitize=thread
else
$(error Cannot have SANITIZED_THREAD=y and OPTIMIZED=y)
endif
else ifneq ($(or $(SANITIZED_THREAD), n), n)
$(error Invalid parameter value)
endif

ifneq ($(shell uname -s), Darwin)
PTHREAD_FLAG:=-pthread
endif

ATLAS_SHARED_LIB:=lib$(ATLAS).$(SHARED_LIB_EXTENSION)

ATLAS_CXXFLAGS:=$(EXTRA_CXXFLAGS) -std=c++17 -Werror -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic $(OPTIMIZATION_FLAGS) $(SANITIZE_FLAGS) $(FPIC_FLAG) $(PTHREAD_FLAG)

ATLAS_COMMON_LDFLAGS:=$(EXTRA_LDFLAGS) $(OPTIMIZATION_FLAGS) $(SANITIZE_FLAGS) $(PTHREAD_FLAG)
ATLAS_SHARED_LDFLAGS:=$(ATLAS_COMMON_LDFLAGS) -shared
ATLAS_EXECUTABLE_LDFLAGS:=$(ATLAS_COMMON_LDFLAGS)

ATLAS_SYSTEM_INCLUDE_DIRS:=$(ATLAS_EXCEPTION_INCLUDE_DIR) $(LURLPARSER_STATIC_LIB_INCLUDE_DIR) /usr/local/opt/openssl/include /usr/local/include
ATLAS_LIB_DIRS:=/usr/local/opt/openssl/lib /usr/local/lib
ATLAS_LDLIBS:=$(LURLPARSER_STATIC_LIB_LDLIB) -lboost_system -lboost_date_time -lssl -lcrypto
