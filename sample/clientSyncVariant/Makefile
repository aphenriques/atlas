ATLAS_ROOT_DIR:=../..

include $(ATLAS_ROOT_DIR)/common.mk

TARGET:=clientSyncVariant
SRC_DIRS:=. $(wildcard */.)
FILTER_OUT:=
INCLUDE_DIRS:=$(ATLAS_STATIC_LIB_INCLUDE_DIR)
SYSTEM_INCLUDE_DIRS:=$(ATLAS_SYSTEM_INCLUDE_DIRS)
LIB_DIRS:=$(ATLAS_LIB_DIRS)
LDLIBS:=$(ATLAS_STATIC_LIB_LDLIB) $(ATLAS_LDLIBS)

# '-isystem <dir>' supress warnings from included headers in <dir>. These headers are also excluded from dependency generation
CXXFLAGS:=$(ATLAS_CXXFLAGS) $(addprefix -I, $(INCLUDE_DIRS)) $(addprefix -isystem , $(SYSTEM_INCLUDE_DIRS))
LDFLAGS:=$(ATLAS_EXECUTABLE_LDFLAGS) $(addprefix -L, $(LIB_DIRS))

################################################################################

SRC_DIRS:=$(subst /.,,$(SRC_DIRS))
SRCS:=$(filter-out $(FILTER_OUT), $(wildcard $(addsuffix /*.cpp, $(SRC_DIRS))))
OBJS:=$(addsuffix .o, $(basename $(SRCS)))
DEPS:=$(addsuffix .d, $(basename $(SRCS)))

.PHONY: all run clean

all:
	cd $(ATLAS_ROOT_DIR)/$(ATLAS_LIB_DIR) && $(MAKE) static
	$(MAKE) $(TARGET)

run: all
	./$(TARGET)

$(TARGET): $(OBJS) $(ATLAS_ROOT_DIR)/$(ATLAS_LIB_DIR)/$(ATLAS_STATIC_LIB)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(addsuffix /*.d, $(SRC_DIRS)) $(addsuffix /*.o, $(SRC_DIRS)) $(TARGET)
#	rm -f $(DEPS) $(OBJS) $(TARGET)

%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MP -MM -MF $@ -MT '$@ $(addsuffix .o, $(basename $<))' $<

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

