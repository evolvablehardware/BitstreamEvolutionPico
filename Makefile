.PHONY: all init update clean pico-ice-sdk picotool pico-ice-micropython

all: pico-ice-sdk picotool pico-ice-micropython

update:
	@echo "Updating submodules..."
	git submodule update --init

.ONESHELL:
pico-ice-sdk: update
	@echo "Initializing pico-ice-sdk..."
	cd lib/pico-ice-sdk
	git submodule update --init
	cd lib/pico-sdk
	git submodule update --init
	cd ../../../..

.ONESHELL:
picotool: update
	# cd tools/picotool 
	# export PICOTOOL_FETCH_FROM_GIT_PATH=$(pwd)
	# mkdir build 
	# cd build 
	# cmake -DPICO_SDK_PATH=../../../lib/pico-ice-sdk/lib/pico-sdk ..
	# make -j$(nproc)
	# cd ../../..
	cd tools
	export PICOTOOL_FETCH_FROM_GIT_PATH=$(pwd)
	cd ../

.ONESHELL:
pico-ice-micropython: update
	cd firmwareMicropython/pico-ice-micropython
	git submodule update --init
	cd lib/pico-ice-mpy-module 
	rmdir pico-ice-sdk 
	ln -s ../../../../lib/pico-ice-sdk/ pico-ice-sdk && cd ../..
	make -C lib/micropython/mpy-cross -j$(nproc)
	make -C lib/micropython/ports/rp2 submodules
	cd boards/PICO_ICE
	mkdir build
# TODO: build for which ever board we end up going with
	cd ../../../../

# SUBMODULES := tools/picotool lib/pico-ice-sdk firmwareMicropython/pico-ice-micropython
# clean:
# 	@echo "Cleaning up uninitialized submodules..."
# 	git submodule deinit --all --force
# 	git rm --cached $(SUBMODULES)
# 	# rm -rf .git/modules/$(SUBMODULES)
# 	rm -rf $(SUBMODULES) # Remove the actual directories
# 	@echo "Submodules removed."
