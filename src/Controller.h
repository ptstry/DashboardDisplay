/*
 * Controller.h
 *
 * Created: 02/01/2018 16:50:50
 *  Author: Piotr
 */ 

#include <inttypes.h>

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

void controller_init(void);
volatile uint32_t tc_1_val;
void my_dummy3(volatile uint32_t);
void onoff(void);

//volatile uint8_t offtime2 = 0;


#endif /* CONTROLLER_H_ */