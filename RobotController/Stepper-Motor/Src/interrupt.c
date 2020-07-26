
// Interrupt Service Routines

/**
 * @author Jihoon Jang
 */

#include "interrupt.h"

CLOCK step = {0, };																					/* Control Output Signal */
CLOCK counter = {0, };																			/* TIM3 counter */

unsigned long milliseconds;
unsigned long msec_count;
ENCODER encoder;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3){																/* Timer3 is Driver of stepper */
		if(flag.signal_in_deadzone)															/* if the signal in dead zone Or robot is down, do not switching */
			return;
		counter.L++, counter.R++;																/* counter increase ( clock counting ) */
		if(counter.L > step.L){																	/* Generating step pulse */
			HAL_GPIO_TogglePin(GPIOC, L_Motor_Step_Pin);					/* toggle for switching */
			counter.L = 0;																				/* counter reset */
			
			(motor.output_left >= 0) ? (encoder.left++) : (encoder.left--);						// cal encoder
		}
		if(counter.R > step.R){																	/* Generating step pulse */
			HAL_GPIO_TogglePin(GPIOC, R_Motor_Step_Pin);					/* toggle for switching */
			counter.R = 0;																				/* counter reset */
			
			(motor.output_right >= 0) ? (encoder.right++) : (encoder.right--);				// cal encoder
		}
	}
	else if(htim->Instance == TIM2){													/* Timer2 is msec Sampling */
		milliseconds++, msec_count++;														/* counter increase */
		if(flag.robot_is_down)																	/* if robot is down, stop control */
			return;
		PostureControl();																				/* Posture PID */
		SetDeadZone();																					/* Set DeadZone */
		SlowlyIncreaseDriving();																/* Driving Slowly */
		CentroidControl();																			/* Centroid PID */
		Calibration();																					/* calibration signal for stepper motor */
		SetMotor();																							/* Set Direction, Deadzone, Frequency */
	}
	else if(htim->Instance == TIM4){
		Monitoring();																						/* 150msec monitoring */
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2){														/* Receive From USB(PC) */
		
	}
	else if(huart->Instance == USART1){												/* Receive From Wifi Module */
		if(isChecksumError())																		/* Communication Error */
			return;
		else{																										/* Communication success without error */
			TuningOrDriving();
		}
		memset(&pc2mcu, 0, sizeof(pc2mcu));											/* Clear */
		HAL_UART_Receive_IT(&huart1, (uint8_t*)(&pc2mcu), sizeof(pc2mcu));
	}
}









