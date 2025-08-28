# BitstreamEvolutionPico
Repository for the tools necessary for interfacing with the pico-ice boards for the Evolvable Hardware project

## Requirements before running make
apt install libusb-1.0-0
apt install gcc-arm-none-eabi


```
#TODO, Make this into a makefile/cmake? instead of using a clunky/high trust install script
git clone https://github.com/evolvablehardware/BitstreamEvolutionPico.git
cd BitstreamEvolutionPico && git submodule update --init 

cd lib/pico-ice-sdk && git submodule update --init
cd lib/pico-sdk && git submodule update --init
cd ../../../..

cd tools/picotool 
export PICOTOOL_FETCH_FROM_GIT_PATH=$(pwd)
mkdir build 
cd build 
cmake -DPICO_SDK_PATH=../../../lib/pico-ice-sdk/lib/pico-sdk ..
make -j$(nproc)

cd ../../..
```
