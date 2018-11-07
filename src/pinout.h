/*
 * pinout.h
 *
 * Created: 02/01/2018 17:13:50
 *  Author: Piotr
 */ 


#ifndef PINOUT_H_
#define PINOUT_H_

//#define DCLK_OUT_PIN EXT1_PWM_0_PIN
//#define DCLK_OUT_MUX EXT1_PWM_0_MUX
//#define HSYNC_OUT_PIN EXT3_PWM_0_PIN
//#define HSYNC_OUT_MUX EXT3_PWM_0_MUX
//#define VSYNC_OUT_PIN PIN_PB30E_TCC0_WO0
//#define VSYNC_OUT_MUX MUX_PB30E_TCC0_WO0
//#define VSYNC_OUT_PIN PIN_PA21E_TC3_WO1
//#define VSYNC_OUT_MUX MUX_PA21E_TC3_WO1

#define PWM_MODULE   EXT1_PWM_MODULE
#define PWM_MODULE_1 EXT3_PWM_MODULE
#define PWM_MODULE_2 TCC0
//#define PWM_MODULE_2 TC3

#define DE_PIN (PIO_PD21_IDX) // Data Enable Pin
#define DISP_PIN (PIO_PA5_IDX) // Display Enable Pin
#define HSYNC_PIN PIN_PB02 // HSYNC Pin

/* Configure TC0 channel 0 as waveform output. */
//#define TC0             TC0 //TIOA0
#define TC0_PERIPHERAL  0

#define TC0_CHANNEL 0 // ID % 3
#define TC0_ID      ID_TC0  //TIOA0
//#define DCLK_OUT_PIN    PIN_TC0_TIOA0 // PA00 - can't be used, because of BA1
#define DCLK_OUT_PIN    (PIO_PA1_IDX)
#define DCLK_OUT_MUX	PIN_TC0_TIOA0_MUX


/* Configure TC0 channel 1 as waveform output. */
//#define TC1             TC1 //TIOA1
#define TC1_PERIPHERAL  1

//#define TC1_CHANNEL 1
//#define TC1_ID      ID_TC1  //TIOA1
//#define HSYNC_OUT_PIN   (PIO_PA15_IDX)
#define TC1_CHANNEL 1
#define TC1_ID      ID_TC7  //TIOA1
#define HSYNC_OUT_PIN   (PIO_PC8_IDX)
#define HSYNC_OUT_MUX	PIN_TC0_TIOA0_MUX // standard MUX B

/* Configure TC0 channel 2 as waveform output. */
//#define TC2             TC6 //TIOA6
#define TC2_PERIPHERAL  2

#define TC2_CHANNEL		2 //TIOA8
#define TC2_ID			ID_TC8
//#define VSYNC_OUT_PIN   (PIO_PC5_IDX)
#define VSYNC_OUT_PIN   (PIO_PC11_IDX)
#define VSYNC_OUT_MUX	(IOPORT_MODE_MUX_B)

#endif /* PINOUT_H_ */