
// Interrupt Service Routines Header

/**
 * @author Jihoon Jang
 */

#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include "stm32f4xx_hal.h"
#include "tim.h"
#include "define.h"
#include "app.h"
#include <stdlib.h>
#include <string.h>

extern CLOCK step;
extern CLOCK counter;
extern unsigned long milliseconds;
extern unsigned long msec_count;
extern ENCODER encoder;

#endif		/* _INTERRUPT_H_ */

