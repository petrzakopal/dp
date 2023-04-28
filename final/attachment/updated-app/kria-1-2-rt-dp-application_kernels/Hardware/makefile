#
# this file was created by a computer. trust it.
#

# compiler tools
XILINX_VITIS ?= /tools/Xilinx/Vitis/2022.2
XILINX_VIVADO ?= /tools/Xilinx/Vivado/2022.2
XILINX_HLS ?= $(XILINX_VITIS)/Vivado_HLS

VPP ?= ${XILINX_VITIS}/bin/v++
RM = rm -f
RMDIR = rm -rf

VITIS_PLATFORM = kria-1-2-rt-dp-platform
VITIS_PLATFORM_DIR = /home/petr/Documents/xilinx-workspace/vitis-workspace/dp/kria-1-2-rt-dp-platform/export/kria-1-2-rt-dp-platform
VITIS_PLATFORM_PATH = $(VITIS_PLATFORM_DIR)/kria-1-2-rt-dp-platform.xpfm

TARGET = hw
VPP_OPTS = --target $(TARGET)

#
# Hw kernel files
#


BUILD_SUBDIRS += build
HW_KERNEL_OBJS += build/krnl_calculateCurVelModel.xo
SRC_KERNEL_OBJS += build/krnl_calculateCurVelModel.xo
BUILD_SUBDIRS += build
HW_KERNEL_OBJS += build/krnl_calculateInvMot.xo
SRC_KERNEL_OBJS += build/krnl_calculateInvMot.xo

ALL_MESSAGE_FILES = $(subst .xo,.o,.mdb,$(SRC_KERNEL_OBJS))

#
# primary build targets
#

.PHONY: all clean
all: $(HW_KERNEL_OBJS)


clean:
	-$(RM) $(SRC_KERNEL_OBJS) $(ALL_MESSAGE_FILES) 
	-$(RMDIR) $(BUILD_SUBDIRS)
	-$(RMDIR) .Xil

.PHONY: incremental
incremental: all


nothing:

#
# Hw Kernels compile;
#

build/krnl_calculateCurVelModel.xo: ../src/krnl_main.cpp ../src/function/regulator.cpp ../src/function/svmCore.cpp krnl_calculateCurVelModel-compile.cfg
	-@mkdir -p $(@D)
	-@$(RM) $@
	$(VPP) $(VPP_OPTS) --compile -I"$(<D)" --config krnl_calculateCurVelModel-compile.cfg -o"$@" "$<"

build/krnl_calculateInvMot.xo: ../src/krnl_inv_asm.cpp krnl_calculateInvMot-compile.cfg
	-@mkdir -p $(@D)
	-@$(RM) $@
	$(VPP) $(VPP_OPTS) --compile -I"$(<D)" --config krnl_calculateInvMot-compile.cfg -o"$@" "$<"

