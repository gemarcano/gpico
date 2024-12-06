# gpico, an opinionated collection of utilities for Raspberry Pi Pico development.

## Using the library

Copy the `gpico_import.cmake` file into your project, include it, and then
define `GPICO_PATH` when configuring CMAKE to the location of this repository.

This uses [FreeRTOS-Kernel](https://github.com/FreeRTOS/FreeRTOS-Kernel.git)
and the [pico-sdk](https://github.com/raspberrypi/pico-sdk.git).

The tinyusb library in the pico-sdk (lib/tinyusb) needs this patch as well:
 - [tinyusb #2474](https://github.com/hathach/tinyusb/pull/2474)

After applying the patches, the build process is similar to a normal pico-sdk
application

```
mkdir build
cd build
cmake ../ -DPICO_SDK_PATH=[path-to-pico-sdk] \
  -DPICO_BOARD=pico_w -DCMAKE_BUILD_TYPE=[Debug|Release|RelWithDebInfo] \
  -DFREERTOS_KERNEL_PATH=[path-to-FreeRTOS-Kernel] \
  -DGPICO_PATH=[path-to-gpico] \
  -GNinja ninja
```
