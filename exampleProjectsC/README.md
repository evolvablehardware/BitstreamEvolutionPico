# Instructions
In order to run these examples the following workflow is current in place:


1. go into the demo folder you would like to compile and run
e.g.
`cd  usb_uart`
2. create a build directory inside the demo folder
`mkdir build`
3. enter the build directory
`cd build`
4. run cmake with the -D flag with "PICO_BOARD=pico2_ice" and one level up (..)
(warning, Ubuntu currently does not display the underscores in vs code terminal correctly)
`cmake -D PICO_BOARD=pico2_ice ..`
5. make the project using all available processors
`make -j $(nproc)`
6. Unplug and plug the pico(2)-ice board with the reset button depressed
7. Open the file manager to the new USB folder
8. Open a second file manager to the build directory
9. Drag the create `.u2f` file from the build directory into the USB folder
e.g. 
`rp2_usb_uart.uf2`
 