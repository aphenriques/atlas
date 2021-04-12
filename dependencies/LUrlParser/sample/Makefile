LURLPARSER_ROOT_DIR:=..

include $(LURLPARSER_ROOT_DIR)/common.mk

TARGET:=example
SRC_DIRS:=. $(wildcard */.)
FILTER_OUT:=
INCLUDE_DIRS:=$(LURLPARSER_STATIC_LIB_INCLUDE_DIR)/LUrlParser
SYSTEM_INCLUDE_DIRS:=
LIB_DIRS:=
LDLIBS:=$(LURLPARSER_STATIC_LIB_LDLIB)

# '-isystem <dir>' supress warnings from included headers in <dir>. These headers are also excluded from dependency generation
CXXFLAGS:=$(LURLPARSER_CXXFLAGS) $(addprefix -I, $(INCLUDE_DIRS)) $(addprefix -isystem , $(SYSTEM_INCLUDE_DIRS))
LDFLAGS:=$(LURLPARSER_EXECUTABLE_LDFLAGS) $(addprefix -L, $(LIB_DIRS))

################################################################################

SRC_DIRS:=$(subst /.,,$(SRC_DIRS))
SRCS:=$(filter-out $(FILTER_OUT), $(wildcard $(addsuffix /*.cpp, $(SRC_DIRS))))
OBJS:=$(addsuffix .o, $(basename $(SRCS)))
DEPS:=$(addsuffix .d, $(basename $(SRCS)))

.PHONY: all run clean

all:
	cd $(LURLPARSER_ROOT_DIR)/$(LURLPARSER_LIB_DIR) && $(MAKE) static
	$(MAKE) $(TARGET)

run: all
	./$(TARGET)

$(TARGET): $(OBJS) $(LURLPARSER_ROOT_DIR)/$(LURLPARSER_LIB_DIR)/$(LURLPARSER_STATIC_LIB)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(addsuffix /*.d, $(SRC_DIRS)) $(addsuffix /*.o, $(SRC_DIRS)) $(TARGET)
#	rm -f $(DEPS) $(OBJS) $(TARGET)

%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MP -MM -MF $@ -MT '$@ $(addsuffix .o, $(basename $<))' $<

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif


