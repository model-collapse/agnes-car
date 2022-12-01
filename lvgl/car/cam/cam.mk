CXXSRCS += cam.cpp
CXXSRCS += filter.cpp
CXXSRCS += overlap.cpp
CXXSRCS += models.cpp
CXXSRCS += model.cpp
CXXSRCS += track.cpp

DEPPATH += --dep-path $(LVGL_DIR)/car/cam
VPATH += $(LVGL_DIR)/car/cam
CFLAGS += "-I/usr/include/opencv4"
LDFLAGS += -L/lib/arm-linux-gnueabihf

LIBS += -lopencv_core -lopencv_calib3d -lopencv_imgproc -lopencv_ml -lopencv_hdf -lopencv_hfs -lopencv_highgui -lopencv_video -lopencv_videoio -lopencv_imgcodecs