// Interrupt Service Routines Source

/**
 * @author Jihoon Jang
 */

#include "stm32f4xx_hal.h"
#include "interrupt.h"

unsigned long milliseconds;
unsigned long counter;
MCU2PC mcu2pc = {0, };
PC2MCU pc2mcu = {0, };

// Timer
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)  // Timer Overflow Interrupt Service Routine
{
	TIM_TypeDef* instance = htim->Instance;
	if(instance == TIM2){							/* TIM2 : 1msec */
		milliseconds++;
		counter++;
		if(flag.start_control){
			Posture_Control();
			Centroid_Control();
			PositionControl();
			SetSteering();
			MotorDrive();
		}
	}
	else if(instance == TIM5){				/* TIM5 : 10msec */
		mcu2pc.monitoring.angle = angleResult;
		mcu2pc.monitoring.encoder = encoder;
		if(flag.ready_message){
			mcu2pc.command = 0x40;
			flag.ready_message = 0;
		}
		else{
			if(flag.receive_message){
				flag.receive_message = 0;
			}
			else
				mcu2pc.command = 0x00;
		}
		HAL_UART_Transmit_IT(&huart1, (uint8_t*)(&mcu2pc), sizeof(mcu2pc));
	}
}
// Transmit Complete Interrupt
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	// module
	if(huart->Instance == USART1){
//		HAL_UART_Receive_IT(huart, (uint8_t*)(&command), sizeof(command));
	}
	// USB
	else if(huart->Instance == USART2){
		
	}
}

// Receive Complete Interrupt
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// module
	if(huart->Instance == USART1){
		/* Communication Code */
		/* Drive */
		ReceiveDataFromPC();
		HAL_UART_Receive_IT(&huart1, (uint8_t*)(&pc2mcu), sizeof(pc2mcu));
//		printf("Receive Data From PC\r\n");
//		printf("Command : %x\r\n", mcu2pc.command);
		flag.receive_message = 1;
	}
	// USB
	else if(huart->Instance == USART2){
		
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)			// Input Capture Interrupt Service Routine
{
	if(htim->Instance == TIM1){
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1){
			printf("TIM1 : Channel 1\n");
		}
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2){
			printf("TIM1 : Channel 2 \n");
		}
	}
	else if(htim->Instance == TIM4){
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1){
			printf("TIM4 : Channel 1 \n");
		}
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2){
			printf("TIM4 : Channel 2 \n");
		}
	}
}

