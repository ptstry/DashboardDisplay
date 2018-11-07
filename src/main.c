/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include <board.h>
//#include <delay.h>
#include <gpio.h>
#include "Can.h"
#include "Controller.h"
#include "Memory.h"
//#include "is42s16100e.h"
//#include "is42S16400J.h"

int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */
    const usart_serial_options_t usart_serial_options = {
        .baudrate   = (115200UL),
        .charlength = US_MR_CHRL_8_BIT,
        .paritytype = US_MR_PAR_NO,
        .stopbits   = US_MR_NBSTOP_1_BIT,
    };
    irq_initialize_vectors();
    cpu_irq_enable();
    
	sysclk_init();
	board_init();
    
    // Configure the PLLA to 48MHz
    REG_CKGR_PLLAR = CKGR_PLLAR_ONE
    | CKGR_PLLAR_DIVA_BYPASS
    | CKGR_PLLAR_MULA(23); // multiplier - 1
    
    //SysTick_Config(SystemCoreClock / (10));
    
    /* Enable UART peripheral clock */
    sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
    stdio_serial_init(CONSOLE_UART, &usart_serial_options);
    
    /* Enable SDRAMC peripheral clock */
    //pmc_enable_periph_clk(ID_SDRAMC);
    
    /* Complete SDRAM configuration */
    //sdramc_init((sdramc_memory_dev_t *)&SDRAM_ISSI_IS42S16400J,
    //        sysclk_get_cpu_hz());
    //sdram_enable_unaligned_support();
    
    /* Init Complete */
    printf("\x0C\n\r-- DashboardDisplay --\n\r");
    
    //sdram_test();
    
	//gpio_configure_pin(PIO_PA1_IDX,PIO_TYPE_PIO_OUTPUT_0);
    //gpio_set_pin_low(PIO_PA1_IDX);
    //sd_mmc_init();
    //memory_copy();
    printf("test\n\r");
    //pmc_enable_upll_clock();
    can_init();
    //can_test(0);
    //offtime = 1;
    CAN_data[0] = 470;
    color_temp = B;
	controller_init();
	
	while(1) {
        //can_receive();
        /*
		int tmer = 1000000000;
		while (tmer--)
		{
    		nop();
		}
		can_test(0);
		
		tmer = 1000000000;
		while (tmer--)
		{
    		nop();
		}
		can_test(1);
        */
	}

	/* Insert application code here, after the board has been initialized. */
}
