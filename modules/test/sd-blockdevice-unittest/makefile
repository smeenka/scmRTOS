PROJECT=sd-blockdevice-unittest
DESCRIPTION=Unit test sd card block device for fiat fs

DRIVERS=serial ssp
MODULES=newlib scmprintf fatfs sd_blockdevice

LIBS_PREFIX=
LIBS_SUFFIX= 

include ${SCM_RTOS_DIR}/build/scmRTOS.mk
FLAGS += -DBUILD_MMCFS  -DDEBUG_SD_BLOCKDEVICE
