CSRCS += back_scr.c
CSRCS += ctl.c
CSRCS += menu_scr.c
CXXSRCS += meters.cpp
CSRCS += meter_120.c

DEPPATH += --dep-path $(LVGL_DIR)/car/ui
VPATH += $(LVGL_DIR)/car/ui
