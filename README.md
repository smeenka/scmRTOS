scmRTOS
=======

Port of scmRTOS to CortexM3 processor, with development of device drivers based on CMCISv2 

Goals:
* develop an easy to use open source environment for CortexM3 processors (LPC1768)
* make environment should be simple and modular
* develop device drivers for the multithreaded environment scmRTOS 
* develop a wireless communication stack based on ANT devices
* develop tooling (stack usage, debugging ) 


Currently working:
* uart and spi device driver
* threadsafe printf implementation
* block driver for SD card

* A working example of a HTTP server based on an example from CodeRed for CMCIS2

Next goals:
* develop the low level device driver for the lwpip stack + unit test (comparable to the Ethernet class of mbed)
* web server test based on single threaded lwip stack
* implement os dependent part of lwip, to make lwip multithreaded for scmRTOS (+ unit test)

Installation:
* Tested on Ubuntu 10.04 and MAC-OS 10.6.8
* Toolchain: code sourcery. 
* Clone project https://github.com/adamgreen/gcc4mbed.git into your workspace
* change dir to adamgreen-gcc4mbed-xxxxx
* run shell script linux_install or mac_install 
* 
* in this project:
* edit setup.sh and let variable CODESOURCERY_DIR point to adamgreen-gcc4mbed-xxxxx/arm-2011.03/
* edit setup.sh and let variable MOUNT_DIR point to the moint point for the mbed 

Usage:
* for each session source setup.sh:  '. ./setup.sh'
* In a sample dir run target         'make clean deploy'
 

