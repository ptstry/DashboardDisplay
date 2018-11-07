/*
 * Controller.c
 *
 * Created: 02/01/2018 16:52:48
 *  Author: Piotr
 */ 

#include "Controller.h"
#include "pinout.h"
#include "Memory.h"
#include "Can.h"
#include <asf.h>

void configure_out(void);
void configure_clocks(void);	
void configure_tc0(void);
void configure_tc0_callback(void);
void configure_tc1(void);
void configure_tc1_callback(void);
void configure_tc2(void);
void configure_tc2_callback(void);

void write_DATA_single(void);
void draw_rpm(int i);
void draw_throttle(int i);
void draw_gear(int i);
void draw_temp(int i);
void draw_black(void);
//void counter(struct tc_module *const module_inst);
//void clearer(struct tc_module *const module_inst);

volatile int whater;
volatile uint32_t mask = 0b11011111010110000011111111111111;
volatile uint16_t line = 0;
//volatile uint32_t *address = 0x41000090;

void configure_out(void) {
	//ioport_set_pin_mode(DCLK_OUT_PIN, DCLK_OUT_MUX);
	ioport_disable_pin(DCLK_OUT_PIN);
	ioport_set_pin_mode(DCLK_OUT_PIN, DCLK_OUT_MUX);
    ioport_disable_pin(PIN_TC0_TIOA0);
	ioport_set_pin_mode(PIN_TC0_TIOA0, DCLK_OUT_MUX);
	ioport_disable_pin(HSYNC_OUT_PIN);
	ioport_set_pin_mode(HSYNC_OUT_PIN, HSYNC_OUT_MUX);
	//gpio_configure_pin(HSYNC_OUT_PIN, PIO_PERIPH_A | PIO_TYPE_PIO_PERIPH_A);
	
	//ioport_disable_pin(HSYNC_OUT_PIN);
	//gpio_set_pin_low(PIO_PA4_IDX);
	//*((__IO uint32_t*) 0x400E0E04) = (uint32_t)(1<<1);
    
    //ioport_disable_pin(PIO_PC9_IDX); // hack on that pin (TIOB7)
    ioport_set_pin_dir(PIO_PC9_IDX, IOPORT_DIR_INPUT);
    gpio_set_pin_low(PIO_PC11_IDX);
    gpio_set_pin_low(PIO_PC9_IDX);
    
	ioport_disable_pin(VSYNC_OUT_PIN);
    ioport_set_pin_mode(VSYNC_OUT_PIN, VSYNC_OUT_MUX);
    //ioport_set_pin_mode(PIO_PC9_IDX, VSYNC_OUT_MUX);
    
    
    /* Temporary output values */
    //volatile uint32_t col = convert_color(0x0000FF);
    ioport_set_port_dir(IOPORT_PIOD, mask, IOPORT_DIR_OUTPUT);
    ioport_set_port_level(IOPORT_PIOD, mask, 0);
    //ioport_set_port_level(IOPORT_PIOD, mask&col, 1);
    //ioport_set_port_level(IOPORT_PIOD, GAR[0], GAR[0]);
    
    ioport_set_pin_dir(DE_PIN, IOPORT_DIR_OUTPUT);
    gpio_set_pin_low(DE_PIN);
    
    ioport_set_pin_dir(DISP_PIN, IOPORT_DIR_OUTPUT);
    gpio_set_pin_high(DISP_PIN);
    
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

void configure_clocks() {
	// Configure the PLLA to enable the TC module.
	//REG_CKGR_PLLAR = CKGR_PLLAR_ONE
	//| CKGR_PLLAR_DIVA_BYPASS
	//| CKGR_PLLAR_MULA(11); // multiplier - 1
	
	REG_PMC_WPMR = PMC_WPMR_WPKEY_PASSWD; // disable PMC write protection
	*((__IO uint32_t*)0x400E0640U+PMC_PCK_6) = (2<<PMC_PCK_PRES_Pos) // PCK_6 - PLLA clock
  | 2; // Prescaler +1 = 2+1. Results in 2MHz output freq, that is later multiplied by PLLAR_MULA
	
	REG_PMC_SCER |= PMC_SCER_PCK6; // enable peripheral clock 6 used by Timer Counters
  
  // SUMMARY: PLLA = 6MHz; divided by 3 with PCK prescaler giving 2MHz; multiplied by PLLAR_MULA+1 giving 24MHz
}

void configure_tc0(){
	sysclk_enable_peripheral_clock(TC0_ID);
	
	// Init TC to waveform mode.
	tc_init(TC0, TC0_CHANNEL,
	 	TC_CMR_TCCLKS_TIMER_CLOCK1 // Waveform Clock Selection
	 	| TC_CMR_WAVE       // Waveform mode is enabled
		| TC_CMR_WAVSEL(2)	// 10b clear on RC compare
	 	//| TC_CMR_ACPA_SET   // RA Compare Effect: set
		//| TC_CMR_ACPC_CLEAR	// RC Compare Effect: clear
		| TC_CMR_BCPB_SET
		| TC_CMR_BCPC_CLEAR
        | TC_CMR_EEVT_XC0
	);
	 
	// Configure waveform TC channels
	tc_write_rc(TC0, TC0_CHANNEL, 4);
	tc_write_rb(TC0, TC0_CHANNEL, 2);
	tc_write_ra(TC0, TC0_CHANNEL, 2);
	//tc_enable_interrupt(TC0, TC0_CHANNEL, TC_IER_CPBS);
	//irq_initialize_vectors();
	//irq_register_handler(TC0_IRQn,1); // tc0 irq, priority = 1
	
	// Enable TC TC_CHANNEL_WAVEFORM.
	//tc_start(TC0, TC0_CHANNEL);
	//(*(__O  uint32_t*)0x4000C040) = TC_CCR_CLKEN;
} 

ISR (TC0_Handler) {
    //gpio_toggle_pin(LED_0_PIN);
}
void configure_tc1() {
	sysclk_enable_peripheral_clock(TC1_ID);
    
	
    
    
	// Init TC to waveform mode.
	tc_init(TC2, TC1_CHANNEL,
	TC_CMR_TCCLKS_TIMER_CLOCK1 // Waveform Clock Selection
	| TC_CMR_WAVE       // Waveform mode is enabled
	| TC_CMR_WAVSEL(2)	// 10 clear on RC
	| TC_CMR_ACPA_SET   // RA Compare Effect: set
	| TC_CMR_ACPC_CLEAR	// RC Compare Effect: clear
    | TC_CMR_EEVT_XC0   // RB interrupt
    //| TC_CMR_BCPB_SET
    //| TC_CMR_BCPC_CLEAR
	);
	
	// Configure waveform TC channels
	tc_write_ra(TC2, TC1_CHANNEL, 20); // val = 3 is rise of second DCLK -- old 2
	tc_write_rb(TC2, TC1_CHANNEL, 118); // 30 DCLK -- old 59
	tc_write_rc(TC2, TC1_CHANNEL, 2048); // HSYNC down -- old 1024
    //tc_enable_interrupt(TC0, TC1_CHANNEL, TC_IER_CPAS);
    
    /* Enable interrupts */
    tc_enable_interrupt(TC2, TC1_CHANNEL, TC_IER_CPBS);
    irq_initialize_vectors();
	irq_register_handler(TC7_IRQn,3); // tc1 irq, priority = 1
    NVIC_DisableIRQ(TC7_IRQn);
}

void TC7_Handler(void) {
    tc_get_status(TC2, TC1_CHANNEL); // must be here to work!
    if (line < 272) {
        
        int i=0;
        
        draw_rpm(i++);
        //gpio_set_pin_high(DE_PIN);
        
        for (i; i<472; i++) {
            // here comes the implementation of line display
            //gpio_set_pin_group_high(PIOD, 0x03FF);
            //gpio_set_pin_group_low(PIOD, 0xC00);
            //ioport_set_port_level(IOPORT_PIOD, mask&0xFFF, 1);
            /*
            nop();
            nop();
            nop();
            
            nop();
            nop();
            nop();
            nop();
            nop();
            nop();
            //*/
            if(line<150)
                draw_rpm(i);
            else if(line<180)
                draw_throttle(i);
            else {
                //if(i<100)
                    //draw_gear(i);
                //else if(i<300)
                    draw_temp(i);
                //else
                //    draw_black();
            }
            
        }
        line++;
        //gpio_toggle_pin(DE_PIN);
        gpio_set_pin_low(DE_PIN);
        /* Extra wait for line = 0 */
        /*
        if(line == 0) {
            int tmot = 500;
            while(tmot--) {nop();}
        }   
        */         
        //tc_disable_interrupt(TC2, TC1_CHANNEL, TC_IDR_CPBS);
    }        
    else {
        //tc_disable_interrupt(TC2, TC1_CHANNEL, TC_IDR_CPBS);
        NVIC_DisableIRQ(TC7_IRQn);
        offtime = 1;
        can_receive();
    }
}
void TC8_Handler(void) {
    tc_get_status(TC2, TC2_CHANNEL); // must be here to work!
    
    offtime = 0;
    
	//gpio_toggle_pin(DE_PIN);
    line = 0;
    //*((__IO uint32_t*)0x40014064) = 1<<3;
    int tmot = 300;
    while(tmot--) {nop();}
    //irq_register_handler(TC7_IRQn,3);
    NVIC_EnableIRQ(TC7_IRQn);
    
    //tc_enable_interrupt(TC2, TC1_CHANNEL, TC_IER_CPBS);
}

void configure_tc2() {
	sysclk_enable_peripheral_clock(TC2_ID);
	
	// Init TC to waveform mode.
	tc_init(TC2, TC2_CHANNEL,
	TC_CMR_TCCLKS_TIMER_CLOCK3 // Waveform Clock Selection
	| TC_CMR_WAVE       // Waveform mode is enabled
	| TC_CMR_WAVSEL(2)	// 10 clear on RC
	| TC_CMR_ACPA_SET   // RA Compare Effect: set
	| TC_CMR_ACPC_CLEAR	// RC Compare Effect: clear
    | TC_CMR_EEVT_XC0   // RB interrupt
    //| TC_CMR_EEVT_TIOB
	);
	
	// Configure waveform TC channels
	tc_write_ra(TC2, TC2_CHANNEL, 192); // val = 1024 is 1 HSYNC, for CLK3 val 48 is half of H -- old 96
	tc_write_rb(TC2, TC2_CHANNEL, 2496); // 13H, enable data -- old 1248
	tc_write_rc(TC2, TC2_CHANNEL, 54912); // 13+272+1H VSYNC down -- old 27456
    
    /* Enable interrupts */
    tc_enable_interrupt(TC2, TC2_CHANNEL, TC_IER_CPBS);
	irq_initialize_vectors();
	irq_register_handler(TC8_IRQn,2); // tc1 irq, priority = 1
	// Enable TC TC_CHANNEL_WAVEFORM.
	//tc_start(TC1, TC1_CHANNEL);
}

void controller_init(void) {
	
    set_GAR();
	configure_out();
	configure_clocks();
	configure_tc0();
	configure_tc1();
	configure_tc2();
	tc_start(TC0, TC0_CHANNEL);
    tc_start(TC2, TC1_CHANNEL);
    int tmr = 30;
    while (tmr--) {nop();}
	tc_start(TC2, TC2_CHANNEL);
}
volatile cnt = 0;

void draw_rpm(int i) {
    int tmr = 6;
    //while(tmr--) {nop();}
    if(i>CAN_data[0]) {
        ioport_set_port_level(IOPORT_PIOD, 0xFFFFFFFF&mask, 0);
    }        
    else if(i < 218) {
        //gpio_set_pin_group_high(PIOD, GAR[0]);
        //gpio_set_pin_group_low(PIOD, ~GAR[0]&mask);
        ioport_set_port_level(IOPORT_PIOD, GAR[0], 1);
        ioport_set_port_level(IOPORT_PIOD, ~GAR[0]&mask, 0);
    }
    else if(i < 326) {
        //gpio_set_pin_group_high(PIOD, GAR[1]);
        //gpio_set_pin_group_low(PIOD, ~GAR[1]&mask);
        ioport_set_port_level(IOPORT_PIOD, GAR[1], 1);
        ioport_set_port_level(IOPORT_PIOD, ~GAR[1]&mask, 0);
    }
    else {
        //gpio_set_pin_group_high(PIOD, GAR[2]);
        //gpio_set_pin_group_low(PIOD, ~GAR[2]&mask);
        ioport_set_port_level(IOPORT_PIOD, GAR[2], 1);
        ioport_set_port_level(IOPORT_PIOD, ~GAR[2]&mask, 0);
    }
}

void draw_throttle(int i) {
    nop();
    nop();
    nop();
    nop();
    nop();
    if(i>CAN_data[3]){
        ioport_set_port_level(IOPORT_PIOD, 0xFFFFFFFF&mask, 0);
    }
    else {
        ioport_set_port_level(IOPORT_PIOD, 0xFFFFFFFF&mask, 1);
    }
}

void draw_gear(int i) {
    if(CAN_data[4]==0){
        ioport_set_port_level(IOPORT_PIOD, GAR[2], 1);
        ioport_set_port_level(IOPORT_PIOD, ~GAR[2]&mask, 0);
    }
    else {
        ioport_set_port_level(IOPORT_PIOD, B&mask, 1);
        ioport_set_port_level(IOPORT_PIOD, ~B&mask, 0);
    }
}

void draw_temp(int i) {
    if(i<100) {
        ioport_set_port_level(IOPORT_PIOD, color_temp, 1);
        ioport_set_port_level(IOPORT_PIOD, ~color_temp&mask, 0); 
    }           
    /* Draw Gear */
    else if(i<400){
        ioport_set_port_level(IOPORT_PIOD, color_gear, 1);
        ioport_set_port_level(IOPORT_PIOD, ~color_gear&mask, 0);
    }
    else {
        /* Draw Black */
        ioport_set_port_level(IOPORT_PIOD, 0xFFFFFFFF&mask, 0);
    }
}

void draw_black() {
    ioport_set_port_level(IOPORT_PIOD, 0xFFFFFFFF&mask, 0);
}