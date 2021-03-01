#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "hardware/sync.h"
#include "hardware/gpio.h"
#include "hardware/vreg.h"

#include "dvi.h"
#include "dvi_serialiser.h"
#include "common_dvi_pin_configs.h"

#include "testcard_1280x60_rgb565.h"

// DVDD 1.2V (1.1V seems ok too)
#define FRAME_WIDTH 80
#define FRAME_HEIGHT 60
#define VREG_VSEL VREG_VOLTAGE_1_20
#define DVI_TIMING dvi_timing_640x480p_60hz

struct dvi_inst dvi0;

void core1_main() {
	dvi_register_irqs_this_core(&dvi0, DMA_IRQ_0);
	while (queue_is_empty(&dvi0.q_colour_valid))
		__wfe();
	dvi_start(&dvi0);
	dvi_scanbuf_x8scale_main_16bpp(&dvi0);
}

int main() {
	// system global setting
	vreg_set_voltage(VREG_VSEL);
	sleep_ms(10);
	set_sys_clock_khz(DVI_TIMING.bit_clk_khz, true);

	// set up default uart (pin0 : TX, pin1 : RX) with baudrate 115200
	setup_default_uart();

	// init bitbang DVI library
	dvi0.timing = &DVI_TIMING;
	dvi0.ser_cfg = pico_sock_cfg;
	dvi_init(&dvi0, next_striped_spin_lock_num(), next_striped_spin_lock_num());

	// Core 1 will wait until it sees the first colour buffer, then start up the
	// DVI signalling.
	multicore_launch_core1(core1_main);

	// Pass out pointers into our preprepared image, discard the pointers when
	// returned to us. Use frame_ctr to scroll the image
	uint x_scroll = 0;
	while (true) {
		for (uint y = 0; y < FRAME_HEIGHT; ++y) {
			uint idx = (y * 1280) + (x_scroll % 1280);
			idx = (idx >= 76800) ? idx - 76800 : idx;
			const uint16_t *scanline = &((const uint16_t*)testcard_1280x60)[idx];
			queue_add_blocking_u32(&dvi0.q_colour_valid, &scanline);
			while (queue_try_remove_u32(&dvi0.q_colour_free, &scanline));
		}
		x_scroll += 2;
	}
}
