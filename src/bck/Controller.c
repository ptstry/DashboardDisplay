/*
 * Controller.c
 *
 * Created: 02/01/2018 16:52:48
 *  Author: Piotr
 */ 

#include "Controller.h"
#include "pinout.h"
#include <asf.h>

void configure_out(void);
void configure_tc0(void);
void configure_tc0_callback(void);
void configure_tc1(void);
void configure_tc1_callback(void);
void configure_tc2(void);
void configure_tc2_callback(void);
//void enable_LINE(struct tcc_module *const module_inst);
//void disable_LINE(struct tcc_module *const module_inst);
//void toggle_HSYNC(struct tc_module *const module_inst);
//void toggle_DATA(struct tc_module *const module_inst);
//void write_DATA(struct tc_module *const module_inst);
void write_DATA_single(void);
//void counter(struct tc_module *const module_inst);
//void clearer(struct tc_module *const module_inst);

static void tc_waveform_initialize(void);

volatile int whater;
//volatile uint32_t *address = 0x41000090;

void configure_out(void) {
	ioport_set_pin_mode(PIN_TC0_TIOA0, PIN_TC0_TIOA0_MUX);
	ioport_disable_pin(PIN_TC0_TIOA0);
	/*
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);
	
	pin_conf.direction = PORT_PIN_DIR_OUTPUT;
	//pin_conf.input_pull = PORT_PIN_PULL_DOWN;
	port_pin_set_config(DE_PIN, &pin_conf);
	port_pin_set_config(HSYNC_PIN, &pin_conf);
	port_group_set_config(&PORTB, 0x00FFFFFF, &pin_conf);
	port_pin_set_output_level(PIN_PA02, 0);
	port_pin_set_output_level(PIN_PB02, 0);
	port_pin_set_output_level(DE_PIN, 0);
	*/
}

#define TC_WAVEFORM_TIMER_SELECTION TC_CMR_TCCLKS_TIMER_CLOCK1
#define TC_WAVEFORM_DIVISOR     64
#define TC_WAVEFORM_FREQUENCY   500000
#define TC_WAVEFORM_DUTY_CYCLE  30
static void tc_waveform_initialize(){
	uint32_t ra, rc;
	// Configure the PMC to enable the TC module.
	REG_CKGR_PLLAR = CKGR_PLLAR_ONE
		| CKGR_PLLAR_DIVA_BYPASS
		| CKGR_PLLAR_MULA(11);
	
	REG_PMC_WPMR = PMC_WPMR_WPKEY_PASSWD;
	*((__IO uint32_t*)0x400E0640U+PMC_PCK_6) = (2<<PMC_PCK_PRES_Pos) | 2; // 4 stands for MCK
	REG_PMC_SCER |= PMC_SCER_PCK6;
	REG_PMC_PCR = PMC_PCR_CMD | (uint32_t)ID_TC_WAVEFORM;
	REG_PMC_PCR = (1<<8) | PMC_PCR_EN | (1<<29);
	//REG_PMC_PCR = (1<<8) | PMC_PCR_EN | (1<<29) | PMC_PCR_CMD | (uint32_t)ID_TC_WAVEFORM;
	
	
	//PMC_PCK_7
	
	
	//read the config
	//REG_PMC_PCR = ~PMC_PCR_CMD;
	
	//REG_PMC_PCR |= PMC_PCR_DIV_PERIPH_DIV_MCK;
	//REG_PMC_PCR |= (1<<8); // select MAIN_CLK
	//REG_PMC_PCR |= PMC_PCR_EN;
	
	volatile uint32_t *PMC_SCER = (uint32_t*)0x400E0600;
	volatile uint32_t *PMC_PCER0 = (uint32_t*)0x400E0610;
	volatile uint32_t temp = ~PMC_PCR_CMD & (uint32_t)ID_TC_WAVEFORM;
	//*PMC_SCER |= (uint32_t)(1<<14);
	
	//REG_PMC_PCDR0 |= PMC_PCDR0_PID23;
	
	sysclk_enable_peripheral_clock(ID_TC_WAVEFORM);
	REG_PMC_PCER0 = (1<<(uint32_t)ID_TC_WAVEFORM);
	//sysclk_enable_peripheral_clock(PMC_SCER_PCK0);
	//REG_PMC_PCR = 0;
	
	// Init TC to waveform mode.
	tc_init(TC, TC_CHANNEL_WAVEFORM,
	 	TC_WAVEFORM_TIMER_SELECTION // Waveform Clock Selection
	 	| TC_CMR_WAVE       // Waveform mode is enabled
		| TC_CMR_WAVSEL(2)	// 10 clear on 0xFFFF
	 	| TC_CMR_ACPA_SET   // RA Compare Effect: set
		| TC_CMR_ACPC_CLEAR
		| TC_CMR_LDRA_EDGE
	);
	 
	// Configure waveform frequency and duty cycle.
	volatile uint32_t speed = sysclk_get_peripheral_bus_hz(TC);
	rc = (sysclk_get_peripheral_bus_hz(TC) /
	 	TC_WAVEFORM_DIVISOR) /
	 	TC_WAVEFORM_FREQUENCY;
	rc = 2;
	tc_write_rc(TC, TC_CHANNEL_WAVEFORM, rc);
	ra = (100 - TC_WAVEFORM_DUTY_CYCLE) * rc / 100;
	tc_write_ra(TC, TC_CHANNEL_WAVEFORM, 1);
	// Enable TC TC_CHANNEL_WAVEFORM.
	tc_start(TC, TC_CHANNEL_WAVEFORM);
} 

/*
void configure_tc0(void) {
	struct tc_config config_tc_0;
	
	tc_get_config_defaults(&config_tc_0);
	
	// [change default config]
	config_tc_0.clock_source = GCLK_GENERATOR_2;
	config_tc_0.clock_prescaler = TC_CLOCK_PRESCALER_DIV1;
	config_tc_0.counter_size = TC_COUNTER_SIZE_8BIT;
	config_tc_0.wave_generation = TC_WAVE_GENERATION_NORMAL_PWM;
	//config_tc_0.waveform_invert_output = TC_WAVEFORM_INVERT_OUTPUT_CHANNEL_0;
	config_tc_0.counter_8_bit.period = 1;
	config_tc_0.counter_8_bit.compare_capture_channel[0] = 1;
	//config_tc.counter_8_bit.compare_capture_channel[1] = 2;
	// [change default config]
	
	// [setup PWM]
	config_tc_0.pwm_channel[0].enabled = true;
	config_tc_0.pwm_channel[0].pin_out = DCLK_OUT_PIN;
	config_tc_0.pwm_channel[0].pin_mux = DCLK_OUT_MUX;
	// [setup PWM]
	
	tc_init(&tc_instance_0, PWM_MODULE, &config_tc_0);
	//tc_enable(&tc_instance_0);
}

void configure_tc1(void) {
	struct tc_config config_tc_1;
	tc_get_config_defaults(&config_tc_1);
	
	// [change default config]
	config_tc_1.clock_source = GCLK_GENERATOR_2;
	config_tc_1.clock_prescaler = TC_CLOCK_PRESCALER_DIV4; // 512DCLK width will be 256 (8bit)
	config_tc_1.counter_size = TC_COUNTER_SIZE_8BIT;
	config_tc_1.wave_generation = TC_WAVE_GENERATION_NORMAL_PWM;
	config_tc_1.waveform_invert_output = TC_WAVEFORM_INVERT_OUTPUT_CHANNEL_0;
	config_tc_1.counter_8_bit.value = 1;
	config_tc_1.counter_8_bit.period = 255;
	config_tc_1.counter_8_bit.compare_capture_channel[0] = 1; // Thw = 2 DCLK
	config_tc_1.counter_8_bit.compare_capture_channel[1] = 10; // Thbp = 30 DCLK
	
	// [setup PWM]
	config_tc_1.pwm_channel[0].enabled = true;
	config_tc_1.pwm_channel[0].pin_out = HSYNC_OUT_PIN;
	config_tc_1.pwm_channel[0].pin_mux = HSYNC_OUT_MUX;
	// [setup PWM]
	
	tc_init(&tc_instance_1, PWM_MODULE_1, &config_tc_1);
	//tc_enable(&tc_instance_1);
}

void configure_tc2(void) {
	
	struct tcc_config config_tc_2;
	tcc_get_config_defaults(&config_tc_2, TCC0);
	
	config_tc_2.counter.clock_source = GCLK_GENERATOR_2;
	config_tc_2.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV1024;
	config_tc_2.counter.period = 285; // 286 * 2
	config_tc_2.wave.wave_generation = TCC_WAVE_WAVEGEN_NPWM_Val;
	config_tc_2.compare.match[0] = 285;
	config_tc_2.compare.match[1] = 274; // start passing lines
	config_tc_2.compare.match[2] = 1;
	config_tc_2.wave_ext.invert[0] = false;
	config_tc_2.counter.count = 285;
	config_tc_2.double_buffering_enabled = false;
	config_tc_2.counter.direction = TCC_COUNT_DIRECTION_DOWN;
	
	// [setup PWM]
	config_tc_2.pins.enable_wave_out_pin[0] = true;
	config_tc_2.pins.wave_out_pin[0] = VSYNC_OUT_PIN;
	config_tc_2.pins.wave_out_pin_mux[0] = VSYNC_OUT_MUX;
	// [setup PWM]
	
	tcc_init(&tc_instance_2, PWM_MODULE_2, &config_tc_2);
	
	*//*
	struct tc_config config_tc_2;
	tc_get_config_defaults(&config_tc_2);
	
	// [change default config]
	config_tc_2.clock_source = GCLK_GENERATOR_1;
	config_tc_2.clock_prescaler = TC_CLOCK_PRESCALER_DIV1024; // 512DCLK width will be 256 (8bit)
	config_tc_2.counter_size = TC_COUNTER_SIZE_8BIT;
	config_tc_2.wave_generation = TC_WAVE_GENERATION_NORMAL_PWM;
	config_tc_2.waveform_invert_output = TC_WAVEFORM_INVERT_OUTPUT_CHANNEL_1;
	config_tc_2.counter_8_bit.value = 1; // fix for the initial period
	config_tc_2.counter_8_bit.period = 255;
	//config_tc_2.counter_8_bit.compare_capture_channel[0] = 4; // VSYNC ON
	config_tc_2.counter_8_bit.compare_capture_channel[1] = 1; // start passing lines
	
	// [setup PWM]
	config_tc_2.pwm_channel[0].enabled = true;
	config_tc_2.pwm_channel[0].pin_out = VSYNC_OUT_PIN;
	config_tc_2.pwm_channel[0].pin_mux = VSYNC_OUT_MUX;
	// [setup PWM]
	
	tc_init(&tc_instance_2, PWM_MODULE_2, &config_tc_2);
	*//*
}

void configure_tc0_callback() {
	tc_register_callback(&tc_instance_0, counter, TC_CALLBACK_CC_CHANNEL1);
	tc_enable_callback(&tc_instance_0, TC_CALLBACK_CC_CHANNEL1);
	tc_register_callback(&tc_instance_0, clearer, TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tc_instance_0, TC_CALLBACK_OVERFLOW);
}

void configure_tc1_callback(void) {
	tc_register_callback(&tc_instance_1, toggle_HSYNC, TC_CALLBACK_CC_CHANNEL0);
	tc_register_callback(&tc_instance_1, toggle_DATA, TC_CALLBACK_CC_CHANNEL1);
	tc_register_callback(&tc_instance_1, toggle_HSYNC, TC_CALLBACK_OVERFLOW);
	
	tc_register_callback(&tc_instance_0, write_DATA, TC_CALLBACK_CC_CHANNEL0);
	
	//tc_enable_callback(&tc_instance_1, TC_CALLBACK_CC_CHANNEL1);
}
void configure_tc2_callback() {
	tcc_register_callback(&tc_instance_2, enable_LINE, TCC_CALLBACK_CHANNEL_1);
	tcc_register_callback(&tc_instance_2, disable_LINE, TCC_CALLBACK_CHANNEL_2);
	tcc_enable_callback(&tc_instance_2, TCC_CALLBACK_CHANNEL_1);
	tcc_enable_callback(&tc_instance_2, TCC_CALLBACK_CHANNEL_2);
}

void enable_LINE(struct tcc_module *const module_inst) {
	// TODO: enable toggle_DATA interrupt
	tc_enable_callback(&tc_instance_1, TC_CALLBACK_CC_CHANNEL1);
}
void disable_LINE(struct tcc_module *const module_inst) {
	tc_disable_callback(&tc_instance_1, TC_CALLBACK_CC_CHANNEL1);
}
void toggle_DATA(struct tc_module *const module_inst) {
	//tc_1_val = tc_get_count_value(&tc_instance_1);
	// [enable data write interrupts on tc_instance_0]
	//tc_enable_callback(&tc_instance_0, TC_CALLBACK_CC_CHANNEL0);
	write_DATA_single();
}
void toggle_HSYNC(struct tc_module *const module_inst) {
	//tc_1_val = tc_get_count_value(&tc_instance_1);
	port_pin_toggle_output_level(HSYNC_PIN);
	
}
void write_DATA(struct tc_module *const module_inst) {
	static uint32_t data;
	static uint16_t cnt;
	//tc_1_val = tc_get_count_value(&tc_instance_1);
	
	if(cnt==480) {
		port_group_set_output_level(&PORTA, ~0, 0); // all pins in PORTA set to 0
		tc_disable_callback(&tc_instance_0, TC_CALLBACK_CC_CHANNEL0);
		cnt = 0;
		//data = 0;
		data = data << 1;
		return;
	}
	
	data += 4;
	uint32_t msk = data | port_group_get_output_level(&PORTA, ~0);
	port_group_set_output_level(&PORTA, msk, ~0);
	
	cnt++;
}
//OPTIMIZE_HIGH
RAMFUNC
void my_dummy3(volatile const uint32_t msk) {
	port_pin_set_output_level(DE_PIN, true);
	asm volatile ("mov r0, %[value]" : : [value] "r" (msk));
	__asm (
		//"LDR r0, =0x01\n"
		"LDR r1, =0x41000090 \n"
		"LDR r2, =0x00\n"
		"LDR r3, =1\n" // store 1
		"LDR r4, =127\n" // loop max
		"loop: STR r0, [r1]\n"
		"add r0, r3\n" // add 1
		//"sub r4, r3\n" // cnt--
		"cmp r4, r0\n"
		"bge loop"
	);
	*//*
	__asm (
		"STR r0, [r1]\n"
		"STR r2, [r1]\n"
		"STR r0, [r1]\n"
		"STR r2, [r1]"
	);
	*//*
}

//OPTIMIZE_HIGH
void write_DATA_single() {
	uint32_t data = 0;
	__IO uint32_t msk = 0;
	
	uint32_t *clear_add = 0x41000094;
	//port_pin_set_output_level(DE_PIN, true); // Data Enable Pin
	for(int i=0; i<1; i++) {
		msk += 1; // ~edit fixed data value
		//msk = data;// | port_group_get_output_level(&PORTB, ~0);
		//port_group_set_output_level(&PORTB, msk, ~0);
		// *address = msk;
		//PORTB.OUTCLR.reg = ~msk;
		//PORTB.OUTSET.reg = msk;
		
		//delay_cycles(1);
		//for(int j=0; j<4; j++) {data++;}
		my_dummy3(msk);
		// *address = 0;
		//delay_cycles(1);
		//my_delay();
		//delay_us(0.8);
		//port_group_set_output_level(&PORTB, ~0, 0); // all pins in PORTA set to 0
		 // 2 cycles out of 24MHz = 12MHz
	}
	//port_group_set_output_level(&PORTB, ~0, 0); // all pins in PORTA set to 0
	//PORTB.OUTCLR.reg = ~0;

	// *clear_add = ~0;
	port_pin_set_output_level(DE_PIN, false);
	//delay_cycles(1);
	// *address = 0;
	//delay_cycles(5);
}
void counter(struct tc_module *const module_inst) {
	tc_1_val = tc_get_count_value(&tc_instance_0);
	port_pin_set_output_level(PIN_PA02, 1);
}
void clearer(struct tc_module *const module_inst) {
	port_pin_set_output_level(PIN_PA02, 0);
}
*/

void controller_init(void) {
	/*
	configure_out();
	configure_tc0();
	configure_tc1();
	configure_tc1_callback();
	configure_tc2();
	configure_tc2_callback();
	
	tc_enable2(&tc_instance_0, &tc_instance_1);
	tcc_enable(&tc_instance_2);
	*/
	configure_out();
	tc_waveform_initialize();
	
	
}
volatile cnt = 0;
void onoff() {
	//ioport_set_pin_level(PIO_PD0_IDX, false);
	//ioport_set_pin_level(PIO_PD0_IDX, true);
	gpio_set_pin_low(PIO_PA0_IDX);
	//delay_cycles(1);
	cnt++;
	gpio_set_pin_high(PIO_PA0_IDX);
	cnt++;
	//delay_cycles(1);
}