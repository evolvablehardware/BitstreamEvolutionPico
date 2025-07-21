# BitstreamEvolutionPico
Repository for the tools necessary for interfacing with the pico-ice boards for the Evolvable Hardware project

```
#TODO, Make this into a makefile/cmake? instead of using a clunky/high trust install script
git clone https://github.com/evolvablehardware/BitstreamEvolutionPico.git
cd BitstreamEvolutionPico && git submodule update --init 

cd lib/pico-ice-sdk && git submodule update --init
cd lib/pico-sdk && git submodule update --init
cd ../../../..

cd tools 
export PICOTOOL_FETCH_FROM_GIT_PATH=$(pwd)

cd ../

cd firmwareMicropython/pico-ice-micropython && git submodule update --init

cd lib/pico-ice-mpy-module 
rmdir pico-ice-sdk 
ln -s ../../../../lib/pico-ice-sdk/ pico-ice-sdk && cd ../..
make -C lib/micropython/mpy-cross -j$(nproc)
make -C lib/micropython/ports/rp2 submodules
cd boards/PICO_ICE
mkdir build
cd ../../../../
```
