#
# this file was created by a computer. trust it.
#

# compiler tools
XILINX_VITIS ?= /tools/Xilinx/Vitis/2022.2

HOST_CXX ?= aarch64-linux-gnu-g++
RM = rm -f
RMDIR = rm -rf

VITIS_PLATFORM = kria-1-2-rt-dp-platform
VITIS_PLATFORM_DIR = /home/petr/Documents/xilinx-workspace/vitis-workspace/dp/kria-1-2-rt-dp-platform/export/kria-1-2-rt-dp-platform
VITIS_PLATFORM_PATH = $(VITIS_PLATFORM_DIR)/kria-1-2-rt-dp-platform.xpfm

# host compiler global settings
CXXFLAGS += -std=c++1y -DVITIS_PLATFORM=$(VITIS_PLATFORM) -D__USE_XOPEN2K8 -I/tools/Xilinx/Vitis_HLS/2022.2/include/ -I$(VITIS_PLATFORM_DIR)/sw/kria-1-2-rt-dp-platform/linux_domain/sysroot/cortexa72-cortexa53-xilinx-linux/usr/include/xrt/ -O2 -g -Wall -c -fmessage-length=0 --sysroot=$(VITIS_PLATFORM_DIR)/sw/kria-1-2-rt-dp-platform/linux_domain/sysroot/cortexa72-cortexa53-xilinx-linux
LDFLAGS += -lxilinxopencl -lpthread -lrt -ldl -lcrypt -lstdc++ -L$(VITIS_PLATFORM_DIR)/sw/kria-1-2-rt-dp-platform/linux_domain/sysroot/cortexa72-cortexa53-xilinx-linux/usr/lib/ --sysroot=$(VITIS_PLATFORM_DIR)/sw/kria-1-2-rt-dp-platform/linux_domain/sysroot/cortexa72-cortexa53-xilinx-linux

#
# host files
#

HOST_OBJECTS += src/function/CurVelModel.o
HOST_OBJECTS += src/function/MotorModel.o
HOST_OBJECTS += src/function/invertor.o
HOST_OBJECTS += src/function/regulator.o
HOST_OBJECTS += src/function/spi.o
HOST_OBJECTS += src/function/svmCore.o
HOST_OBJECTS += src/function/transformation.o
HOST_OBJECTS += src/main.o

HOST_EXE = kria-1-2-rt-dp-application

BUILD_SUBDIRS += src/function/
BUILD_SUBDIRS += src/

#
# primary build targets
#

.PHONY: all clean
all:  $(HOST_EXE)

clean:
	-$(RM) $(HOST_EXE) $(HOST_OBJECTS)

.PHONY: $(HOST_OBJECTS) $(HOST_EXE)
.PHONY: incremental
incremental: all


nothing:

#
# host rules
#

src/function/CurVelModel.o: ../src/function/CurVelModel.cpp ../src/header/CurVelModel.h ../src/header/transformation.h
	-@mkdir -p $(@D)
	$(HOST_CXX) $(CXXFLAGS) -o "$@" "$<"

src/function/MotorModel.o: ../src/function/MotorModel.cpp ../src/header/MotorModel.h ../src/header/transformation.h
	-@mkdir -p $(@D)
	$(HOST_CXX) $(CXXFLAGS) -o "$@" "$<"

src/function/invertor.o: ../src/function/invertor.cpp ../src/header/invertor.h ../src/header/svmCore.h
	-@mkdir -p $(@D)
	$(HOST_CXX) $(CXXFLAGS) -o "$@" "$<"

src/function/regulator.o: ../src/function/regulator.cpp ../src/header/regulator.h
	-@mkdir -p $(@D)
	$(HOST_CXX) $(CXXFLAGS) -o "$@" "$<"

src/function/spi.o: ../src/function/spi.cpp ../src/header/spi.h
	-@mkdir -p $(@D)
	$(HOST_CXX) $(CXXFLAGS) -o "$@" "$<"

src/function/svmCore.o: ../src/function/svmCore.cpp ../src/header/svmCore.h
	-@mkdir -p $(@D)
	$(HOST_CXX) $(CXXFLAGS) -o "$@" "$<"

src/function/transformation.o: ../src/function/transformation.cpp ../src/header/transformation.h
	-@mkdir -p $(@D)
	$(HOST_CXX) $(CXXFLAGS) -o "$@" "$<"

src/main.o: ../src/main.cpp ../src/allocator.h ../src/header/MotorModel.h ../src/header/CurVelModel.h ../src/header/regulator.h ../src/header/svmCore.h ../src/header/transformation.h ../src/header/invertor.h ../src/header/spi.h
	-@mkdir -p $(@D)
	$(HOST_CXX) $(CXXFLAGS) -o "$@" "$<"

$(HOST_EXE): $(HOST_OBJECTS)
	$(HOST_CXX) -o "$@" $(+) $(LDFLAGS)

