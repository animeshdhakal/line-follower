all: clean config compile flash

compile:
	cmake --build build

config:
	cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-arm-none-eabi.cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug

flash:
	openocd -f interface/stlink.cfg -c "set CPUTAPID 0x2ba01477" -f target/stm32f1x.cfg -c "program build/line-follower.elf verify reset exit"

clean:
	rm -rf build
