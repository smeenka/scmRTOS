PROJECT=dm-unittest
DESCRIPTION=Unit test for multitasking safe variant for printf

DRIVERS=serial ssp
MODULES=scmprintf dispatcher mmcfs shell newlib sd_blockdevice fatfs  semifs

LIBS_PREFIX=
LIBS_SUFFIX= 

include ${SCM_RTOS_DIR}/build/scmRTOS.mk
FLAGS+= -DDEBUG_FATFS -DDEBUG_SD_BLOCKDEVICE -DNEWLIB_VERBOSE
