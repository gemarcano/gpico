cmake_minimum_required(VERSION 3.20)

include(pico_sdk_import.cmake)
include(FreeRTOS_Kernel_import.cmake)

project(gpico)

pico_sdk_init()

add_library(gpico INTERFACE)

target_sources(gpico INTERFACE
	src/cdc_device.cpp
	src/FreeRTOS_support.cpp
	src/syscalls.cpp
	src/watchdog.cpp
	src/log.cpp
	#	src/usb_descriptors.cpp
)

#pico_generate_pio_header(gpico
#	${CMAKE_CURRENT_LIST_DIR}/src/controllers.pio
#)

target_link_libraries(gpico INTERFACE
	pico_stdlib
	pico_rand
	FreeRTOS-Kernel-Heap4
	tinyusb_device
	tinyusb_board
	hardware_pio
)

target_compile_options(gpico INTERFACE
	$<$<CXX_COMPILER_ID:MSVC>:/W4>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
	-fstack-usage
	-Wno-psabi
)

target_include_directories(gpico INTERFACE
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include"
)

target_compile_features(gpico INTERFACE
	cxx_std_23
)
