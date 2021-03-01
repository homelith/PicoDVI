# Replace TMDS with 10 bit UART (same baud rate):
# add_definitions(-DDVI_SERIAL_DEBUG=1)
# add_definitions(-DRUN_FROM_CRYSTAL)

add_executable(spi_show
	main.c
)

target_link_libraries(spi_show
	pico_stdlib
	pico_multicore
	pico_util
	libdvi
)

# create map/bin/hex file etc.
pico_add_extra_outputs(spi_show)

