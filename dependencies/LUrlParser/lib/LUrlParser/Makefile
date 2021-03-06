LURLPARSER_ROOT_DIR:=../..

include $(LURLPARSER_ROOT_DIR)/common.mk

SRC_DIRS:=. $(wildcard */.)
FILTER_OUT:=
INCLUDE_DIRS:=
SYSTEM_INCLUDE_DIRS:=
LIB_DIRS:=

# '-isystem <dir>' supress warnings from included headers in <dir>. These headers are also excluded from dependency generation
CXXFLAGS:=$(LURLPARSER_CXXFLAGS) $(addprefix -I, $(INCLUDE_DIRS)) $(addprefix -isystem , $(SYSTEM_INCLUDE_DIRS))
LDFLAGS:=$(LURLPARSER_SHARED_LDFLAGS) $(addprefix -L, $(LIB_DIRS))
ARFLAGS:=rcs

################################################################################

STATIC_LIB:=$(LURLPARSER_STATIC_LIB)
SHARED_LIB:=$(LURLPARSER_SHARED_LIB)

SRC_DIRS:=$(subst /.,,$(SRC_DIRS))
SRCS:=$(filter-out $(FILTER_OUT), $(wildcard $(addsuffix /*.cpp, $(SRC_DIRS))))
OBJS:=$(addsuffix .o, $(basename $(SRCS)))
DEPS:=$(addsuffix .d, $(basename $(SRCS)))

.PHONY: all static shared clean

all: static shared

static: $(STATIC_LIB)

shared: $(SHARED_LIB)

$(STATIC_LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

$(SHARED_LIB): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(addsuffix /*.d, $(SRC_DIRS)) $(addsuffix /*.o, $(SRC_DIRS)) $(STATIC_LIB) $(SHARED_LIB)
#	rm -f $(DEPS) $(OBJS) $(STATIC_LIB) $(SHARED_LIB)

%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MP -MM -MF $@ -MT '$@ $(addsuffix .o, $(basename $<))' $<

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif
