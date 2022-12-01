#
# Makefile
#
BIN = demo
DESTDIR = /usr
PREFIX = /local

CC=gcc
CXX=g++
CFLAGS = -Wall -Wshadow -Wundef -Wmaybe-uninitialized
CFLAGS += -g3 -I./

# directory for local libs
LDFLAGS = -L$(DESTDIR)$(PREFIX)/lib
LIBS += -lstdc++ -lm -lpthread

VPATH =

$(info LDFLAGS ="$(LDFLAGS)")

LVGL_DIR = lvgl

include $(LVGL_DIR)/car/car.mk
include $(LVGL_DIR)/lvgl.mk
include $(LVGL_DIR)/lv_drivers/lv_drivers.mk
include $(LVGL_DIR)/lv_examples/lv_examples.mk

# folder for object files
OBJDIRC = ./objc
OBJDIRCXX = ./objcxx

CSRCS += $(wildcard *.c)
CXXSRCS += $(wildcard *.cpp)
COBJS := $(patsubst %.c,$(OBJDIRC)/%.o,$(CSRCS))
CXXOBJS := $(patsubst %.cpp,$(OBJDIRCXX)/%.o,$(CXXSRCS))

SRCS = $(CSRCS) $(CXXSRCS)
OBJS = $(COBJS) $(CXXOBJS)

#.PHONY: clean

all: default

$(OBJDIRC)/%.o: %.c 
	@mkdir -p $(OBJDIRC)
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC $<"

$(OBJDIRCXX)/%.o: %.cpp
	@mkdir -p $(OBJDIRCXX)
	@$(CXX)  $(CFLAGS) -c $< -o $@
	@echo "CXX $<"

default: $(OBJS)
	$(CC) -o $(BIN) $(OBJS) $(LDFLAGS) $(LIBS)

#	nothing to do but will print info
nothing:
	$(info OBJS ="$(OBJS)")
	$(info SRCS ="$(SRCS)")
	$(info DONE)


clean:
	rm -f $(OBJS)

