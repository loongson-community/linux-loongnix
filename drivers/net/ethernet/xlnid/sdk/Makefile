# SPDX-License-Identifier: GPL-2.0
# Copyright(c) 2008 - 2022 Xel Technology.

# kbuild part of makefile
#
obj-$(CONFIG_XLNID) += xlnid.o

define xlnid-y
	xlnid_main.o
	xlnid_lib.o
	xlnid_ethtool.o
	../common/xlnid_skylake.o
	../common/xlnid_api.o
	../common/xlnid_common.o
	../common/xlnid_sriov.o
	../common/xlnid_param.o
	../common/xlnid_procfs.o
endef
xlnid-y := $(strip ${xlnid-y})

xlnid-${CONFIG_DEBUG_FS} += ../common/xlnid_debugfs.o

xlnid-$(CONFIG_PTP_1588_CLOCK:m=y) += ../common/xlnid_ptp.o

xlnid-${CONFIG_SYSFS} += ../common/xlnid_sysfs.o

xlnid-y += ../common/kcompat.o
