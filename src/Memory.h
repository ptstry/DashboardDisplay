/*
 * Memory.h
 *
 * Created: 27/06/2018 12:27:37
 *  Author: Piotr
 */ 


#ifndef MEMORY_H_
#define MEMORY_H_

volatile uint32_t GAR[3]; // green, amber, red colour palette
volatile uint32_t B; // blue
volatile uint16_t CAN_data[8]; // RPM, ECT (temp), BAT, TPS (throttle position), gear
volatile uint32_t color_gear, color_temp;
volatile uint8_t offtime;

void sdram_test(void);
void memory_copy(void);
void set_GAR(void);
uint32_t convert_color(uint32_t color);



#endif /* MEMORY_H_ */
