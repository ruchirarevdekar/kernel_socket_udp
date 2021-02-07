# kernel_socket_udp

# Kernel version: v3.*
The code is tested on kernel version 3.10. It might require some changes for recent kernel versions.

# Files Usage:

1. Create Kernel object (.ko) files required for adding modules to the kernel.
   
   $make
   
   This will generate kserver.ko and kclient.ko alongwith many other files in the directory.

2. Insert server module to the kernel:

   #insmod kserver.ko 

3. In another terminal use following command to insert client module.
   
   #insmod kclient.ko

4. Connection establishment and data transfer can be seen by:

   #dmesg
   
   

