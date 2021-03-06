#############################################################################
#
# Makefile for the win
#
#############################################################################

prefix := /usr/local

uniq = $(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))

# Detect the Raspberry Pi from cpuinfo
# Count the matches for BCM2708 or BCM2709 in cpuinfo
RPI=$(shell cat /proc/cpuinfo | grep Hardware | grep -c BCM2708)
ifneq "${RPI}" "1"
RPI=$(shell cat /proc/cpuinfo | grep Hardware | grep -c BCM2709)
endif

# All parameters goes here
LIBOUT   = libudpsocket.a
SRCDIR   = src
OBJDIR	 = obj
BINDIR   = bin

# Test
TESTBIN  = udpsockettest
TESTDIR  = test

ifeq ($(shell which clang++-3.5 | grep -c "clang++-3.5"), 1)
USE_GCC = 0
else
USE_GCC = 1
endif

ifeq "${USE_GCC}" "1"
CXX      = g++
else
CXX      = clang++-3.5
endif

CCFLAGS  = --std=c++11 -pthread -I$(SRCDIR) -I libs
LDFLAGS  = -Wall

# Turn debug on
#CCFLAGS += -g3
#LDFLAGS += -g3

ifeq "$(RPI)" "1"
# The recommended compiler flags for the Raspberry Pi
CCFLAGS+= -D__RPI__ -Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s 
endif
CCFLAGS += -Iinclude

all: $(BINDIR)/${LIBOUT}

INCLUDES    := $(shell find $(SRCDIR) -name '*.h')
SOURCES := $(shell find $(SRCDIR) -name '*.cpp')
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
DEPS    := $(OBJECTS:%.cpp=%.d)
OBJDIRS := $(call uniq,$(dir $(OBJECTS)))

NODEPS:=clean $(DDPCLIENT_LIB)
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
-include $(OBJECTS:%.o=%.d)
endif

$(BINDIR)/${LIBOUT}: $(OBJECTS)
	@mkdir -p $(BINDIR)
	@echo "[AR ] $@"
	@$(AR) -cvq $@ ${OBJECTS} 

$(OBJDIR)/%.d: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIRS)
	@echo "[Dep] $< --> $@"
	@$(CXX) $(CCFLAGS) -MM -MT '$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$<)' $< -MF $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(OBJDIR)/%.d 
	@mkdir -p $(OBJDIRS)
	@echo "[Bld] $< --> $@"
	@$(CXX) $(CCFLAGS) -c $< -o $@

test: $(BINDIR)/$(TESTBIN)

$(BINDIR)/$(TESTBIN): $(TESTDIR)/main.cpp $(BINDIR)/${LIBOUT}
	@mkdir -p $(BINDIR)
	@echo "[Bld] $< --> $@"
	@$(CXX) $(CCFLAGS) -L$(BINDIR) $< -ludpsocket -o $@

clean:
	rm -rf $(OBJDIRS)
	rm -rf $(BINDIR)
	
install: all
	test -d $(prefix) || mkdir $(prefix)
	test -d $(prefix)/bin || mkdir $(prefix)/bin
	for prog in $(BINDIR)/$(PROGRAMS); do \
	  install -m 0755 $$prog $(prefix)/bin; \
	done

.PHONY: install
