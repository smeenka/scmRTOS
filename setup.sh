#! /bin/bash
# Modify next line and set destination drive to match mbed device

#on macos
#CODESOURCERY_DIR=/Users/anton/Workspace/mbed/adamgreen-gcc4mbed-f5adc35/arm-2011.03/
#CROSS_COMPILER=/Users/anton/arm-cs-tools/
#export MOUNT_DIR=/Volumes/MBED

#UBUNTU
CODESOURCERY_DIR=/mnt/data/tools/gcc/arm-2011.03/
export MOUNT_DIR=/media/MBED

export PLATFORM=lpc17XX
export     CHIP=LPC1766
export    BOARD=mbed


export PATH=$CROSS_COMPILER/bin:$PATH

export SCM_RTOS_DIR=$PWD

#location of target mount dir, when connected to usb  



echo SCM_RTOS_DIR ......: $SCM_RTOS_DIR
echo Code Sourcery dir .: $CODESOURCERY_DIR
echo platform...........: $PLATFORM
echo chip ..............: $CHIP
echo board .............: $BOARD

echo '=========================================='
echo 'Set environment for offline Mbed compiling'
echo '=========================================='
echo '== Note that this script must be sourced ='
echo '=========================================='
