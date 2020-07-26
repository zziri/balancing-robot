// Application Source

/**
 * @author Jihoon Jang
 */

#include "stm32f4xx_hal.h"
#include <math.h>
#include "i2c.h"
#include "define.h"
#include "interrupt.h"
#include "tim.h"
#include "usart.h"
#include "app.h"

uint8_t buffer[12];
uint8_t pData[1];
double accXangle, accYangle; // Angle calculate using the accelerometer
double gyroXrate, gyroYrate;
unsigned long preTimeMPU;
// FLAG
FLAG flag = {1, };
// Angle
double gotAngle;
double angleResult;
const double Angle_Scale = 0.001;
// Output Compare Init Structure
TIM_OC_InitTypeDef OC_Config_Struct;
// Control variables
CONTROLLER posture = {0, };
CONTROLLER centroid = {0, };
CONTROLLER position = {0, };
// Encoder
ENCODER encoder = {0, };
// steering
double steer = 0.0;

void GET_ANGLE(void)
{
	unsigned long present;
	double timePass;
	double A;
	const double k = 3.0;
	int16_t accX, accY, accZ;
	int16_t gyroX, gyroY, gyroZ;
	HAL_I2C_IsDeviceReady(&hi2c1, 0xD0, 1, 1000);
	HAL_I2C_Mem_Write(&hi2c1, 0xD0, 0x6B, I2C_MEMADD_SIZE_8BIT, pData, 1, 500);
	// memory read
	HAL_I2C_Mem_Read(&hi2c1,0xD0,0x3B,I2C_MEMADD_SIZE_8BIT, &buffer[0],1,500);
	HAL_I2C_Mem_Read(&hi2c1,0xD0,0x3C,I2C_MEMADD_SIZE_8BIT, &buffer[1],1,500);
	HAL_I2C_Mem_Read(&hi2c1,0xD0,0x3D,I2C_MEMADD_SIZE_8BIT, &buffer[2],1,500);
	HAL_I2C_Mem_Read(&hi2c1,0xD0,0x3E,I2C_MEMADD_SIZE_8BIT, &buffer[3],1,500);
	HAL_I2C_Mem_Read(&hi2c1,0xD0,0x3F,I2C_MEMADD_SIZE_8BIT, &buffer[4],1,500);
	HAL_I2C_Mem_Read(&hi2c1,0xD0,0x40,I2C_MEMADD_SIZE_8BIT, &buffer[5],1,500);
	HAL_I2C_Mem_Read(&hi2c1,0xD0,0x43,I2C_MEMADD_SIZE_8BIT, &buffer[6],1,500);
	HAL_I2C_Mem_Read(&hi2c1,0xD0,0x44,I2C_MEMADD_SIZE_8BIT, &buffer[7],1,500);
	HAL_I2C_Mem_Read(&hi2c1,0xD0,0x45,I2C_MEMADD_SIZE_8BIT, &buffer[8],1,500);
	HAL_I2C_Mem_Read(&hi2c1,0xD0,0x46,I2C_MEMADD_SIZE_8BIT, &buffer[9],1,500);
	HAL_I2C_Mem_Read(&hi2c1,0xD0,0x47,I2C_MEMADD_SIZE_8BIT, &buffer[10],1,500);
	HAL_I2C_Mem_Read(&hi2c1,0xD0,0x48,I2C_MEMADD_SIZE_8BIT, &buffer[11],1,500);
	accX = (int)buffer[0] << 8 | (int)buffer[1];
	accY = (int)buffer[2] << 8 | (int)buffer[3];
	accZ = (int)buffer[4] << 8 | (int)buffer[5];
	gyroX = (int)buffer[6] << 8 | (int)buffer[7];
	gyroY = (int)buffer[8] << 8 | (int)buffer[9];
	gyroZ = (int)buffer[10] << 8 | (int)buffer[11];
	accXangle = (atan2(accY,accZ)+PI)*RAD_TO_DEG;
	accYangle = (atan2(accX,accZ)+PI)*RAD_TO_DEG;
	gyroXrate = (double)gyroX/131.0;
	gyroYrate = -((double)gyroY/131.0);
	present = milliseconds;
	timePass = (present - preTimeMPU)/1000.0;
	preTimeMPU = present;
	A = k/(k+timePass);
	if(flag.set_angle_offset){
		gotAngle = accXangle;
		flag.set_angle_offset = 0;
	}
	else if(!(flag.set_angle_offset)){
		// Complementary Filter
		gotAngle = A*(gotAngle + gyroXrate*timePass) + (1 - A)*accXangle;
	}
	angleResult = gotAngle - 180.0 + MPU_OFFSET;
	if(counter >= 50){
		counter = 0;
		printf("angle : %5.3f\r\n", angleResult);
	}
}

// PWM Pulse Absolute, Limit PWM MAX

int PWM_Filter(double value)
{
	if(value < -PWM_MAX || value > PWM_MAX)
		return PWM_MAX;
	else if(value < 0)
		return (-value);
	else
		return value;
}

void OC_Struct_Init(void)
{
	OC_Config_Struct.OCMode = TIM_OCMODE_PWM1;
	OC_Config_Struct.Pulse = 0;
	OC_Config_Struct.OCPolarity = TIM_OCPOLARITY_HIGH;
	OC_Config_Struct.OCFastMode = TIM_OCFAST_DISABLE;
}

void MotorDrive(void)
{
	// According to Motor Driver
	int	speed1 = (uint32_t)PWM_Filter(posture.control.result + steer);
	int	speed2 = (uint32_t)PWM_Filter(posture.control.result - steer);
	if(posture.control.result < 0){
		// PB0, PA6
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		OC_Config_Struct.Pulse = speed1;
		HAL_TIM_PWM_ConfigChannel(&htim3, &OC_Config_Struct, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
		// PB1, PA7
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
		OC_Config_Struct.Pulse = speed2;
		HAL_TIM_PWM_ConfigChannel(&htim3, &OC_Config_Struct, TIM_CHANNEL_2);
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	}
	else if(posture.control.result > 0){
		// PB0, PA6
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		OC_Config_Struct.Pulse = speed1;
		HAL_TIM_PWM_ConfigChannel(&htim3, &OC_Config_Struct, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
		// PB1, PA7
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
		OC_Config_Struct.Pulse = speed2;
		HAL_TIM_PWM_ConfigChannel(&htim3, &OC_Config_Struct, TIM_CHANNEL_2);
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	}
	else if(posture.control.result == 0){
		OC_Config_Struct.Pulse = 0;
		HAL_TIM_PWM_ConfigChannel(&htim3, &OC_Config_Struct, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
		HAL_TIM_PWM_ConfigChannel(&htim3, &OC_Config_Struct, TIM_CHANNEL_2);
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	}
}

void Motor_Stop(void)
{
	OC_Config_Struct.Pulse = 0;
	HAL_TIM_PWM_ConfigChannel(&htim3, &OC_Config_Struct, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_ConfigChannel(&htim3, &OC_Config_Struct, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
}

int isTilted(void)
{
	return (angleResult < -TILTED || angleResult > TILTED);
}

int isStanded(void)
{
	return (angleResult < STANDED && angleResult > -STANDED);
}

// PID Posture Control
void Posture_Control(void)
{
	static double preinput = 0.0;
	double input = centroid.control.result - angleResult;
	// P Control
	posture.control.P = posture.gain.P * input;
	// I Control
	if(ABSOLUTE(posture.control.I) < SATURATION){
		posture.control.I += posture.gain.I * input;
	}
	// Anti Windup
	else{
		posture.control.I += posture.gain.I * input * SAFTYFACTOR;
	}
	// D Control
	posture.control.D = (posture.gain.D + (100.0 * ABSOLUTE(angleResult))) * (input - preinput);
	// Cal PID
	posture.control.result = posture.control.P + posture.control.I + posture.control.D;
	// preinput renew
	preinput = input;
	if(flag.driving_state)
		return;
	// Set Dead Zone
	if(ABSOLUTE(posture.control.result) < ((double)DEAD_ZONE) && isStanded()){
		posture.control.result = 0.0;
	}
}


// PID Center of Gravity Control
void Centroid_Control(void)
{
	static double preinput = 0.0;
	double input = position.control.result + posture.control.result * Angle_Scale;
	// P Control
	centroid.control.P = centroid.gain.P * input;
	// I Control
	if(ABSOLUTE(centroid.control.I) < ANGLE_OFFSET_MAX){
		centroid.control.I += centroid.gain.I * input;
	}
	// Anti Windup
	else{
		centroid.control.I += centroid.gain.I * input * SAFTYFACTOR;
	}
	// D Control
	centroid.control.D = centroid.gain.D * (input - preinput);
	// Cal PID
	centroid.control.result = centroid.control.P + centroid.control.I + centroid.control.D;
	// preinput renew
	preinput = input;
}



// Position control
void PositionControl(void)
{
	// Encoder Input, Everage
	double input = (encoder.left + encoder.right)/2.0;
	// if Platform in driving state, do not position control
	if(flag.driving_state)
		return;
	// P control
	position.control.P = position.gain.P * input;
	// I control
	if(ABSOLUTE(position.control.I) < SPEED_SAT){
		position.control.I += position.gain.I * input;
	}
	else{
		position.control.I += position.gain.I * input * SAFTYFACTOR;
	}
	position.control.result = position.control.P + position.control.I;
	if(ABSOLUTE(position.control.result) > SPEED_SAT){
		if(position.control.result > 0)
			position.control.result = SPEED_SAT;
		else
			position.control.result = -SPEED_SAT;
	}
	
//	// P Control
//	position.control.P = position.gain.P * input;
//	if(ABSOLUTE(position.control.result) < SPEED_SAT){
//		position.control.result = position.control.P;
//	}
//	else{
//		// P Control value hold
//	}
//	position.control.result = position.control.P;
	// preinput renew
}

void InitControl(void)
{
	CONTROLLER clear = {0, };

	// clear
	posture = clear;
	// Set gain
	posture.gain.P = 250.0;
	posture.gain.I = 2.6;
	posture.gain.D = 4150.0;
	// clear
	centroid = clear;
	// Set gain
	centroid.gain.P = 0.08;
	centroid.gain.I = 0.045;
	centroid.gain.D = 0.0;	
	// clear
	position = clear;
	// Set gain
	position.gain.P = 0.000001;
	position.gain.I = 0.0;
	position.gain.D = 0.0;
	// clear Variables
	encoder.right = encoder.left = 0;
	flag.set_angle_offset = 1;
}

void ReceiveDataFromPC(void)
{
	if(pc2mcu.drive.dir){								/* Driving Mode */
		SetDrive();
		mcu2pc.command |= 0x80;						/* Callback */
	}
	else{																/* Tuning Mode */
		posture.gain.P = pc2mcu.gain.pPosture;
		posture.gain.I = pc2mcu.gain.iPosture;
		posture.gain.D = pc2mcu.gain.dPosture;
		centroid.gain.P = pc2mcu.gain.pCentroid;
		centroid.gain.I = pc2mcu.gain.iCentroid;
		centroid.gain.D = pc2mcu.gain.dCentroid;
		position.gain.P = pc2mcu.gain.pPosition;
		position.gain.I = pc2mcu.gain.iPosition;
		mcu2pc.command = 0x80;									/* Call back */
	}
}

/*
//void ReceiveCommand(char flag)
//{
//	if(flag == 'T'){
//		posture.gain.P = command.gain.pPosture;
//		posture.gain.I = command.gain.iPosture;
//		posture.gain.D = command.gain.dPosture;
//		centroid.gain.P = command.gain.pCentroid;
//		centroid.gain.I = command.gain.iCentroid;
//		centroid.gain.D = command.gain.dCentroid;
//		position.gain.P = command.gain.pSpeed;
//		position.gain.I = command.gain.iSpeed;
//		command.flag = 'C';

//		HAL_UART_Transmit_IT(&huart1, (uint8_t*)(&command), sizeof(command));
//	}
//	else if(flag == 'D'){
//		COMMAND clearCommand = {0, };
//		// Driving
//		SetDrive();
//		// command clear
//		command = clearCommand;
//		// Drive Callback
//		command.flag = 'D', HAL_UART_Transmit_IT(&huart1, (uint8_t*)(&command), sizeof(command));
//	}
//}
*/


void SetDrive(void)
{
	switch(pc2mcu.drive.dir)
	{
		case FORWARD:
			position.control.result = (pc2mcu.drive.speed)/(double)PWM_MAX;
			flag.driving_state = 1;
			mcu2pc.command = 0x02;
		break;
		
		case BACKWARD:
			position.control.result = -(pc2mcu.drive.speed)/(double)PWM_MAX;
			flag.driving_state = 1;
			mcu2pc.command = 0x10;
		break;
		
		case STOP:
			position.control.result = 0.0;
			encoder.left = encoder.right = 0;
			flag.driving_state = 0, flag.steering = 0;
			steer = 0;
			mcu2pc.command = 0x01;
		break;
		
		case LEFT:
			flag.steering = 1, flag.dir = LEFT;
			flag.driving_state = 1;
			mcu2pc.command = 0x04;
		break;
		
		case RIGHT:
			flag.steering = 1, flag.dir = RIGHT;
			flag.driving_state = 1;
			mcu2pc.command = 0x08;
		break;
		default:
			break;
	}
}
/*
//void SetDrive(void)
//{
//	switch(command.drive.dir)
//	{
//		case FORWARD:
//			position.control.result = (command.drive.speed)/(double)PWM_MAX;
//			flag.driving_state = 1;
//			break;
//		case BACKWARD:
//			position.control.result = -(command.drive.speed)/(double)PWM_MAX;
//			flag.driving_state = 1;
//			break;
//		case STOP:
//			position.control.result = 0.0;
//			// Counter Clear
//			encoder.left = encoder.right = 0;
//			// flag clear
//			flag.driving_state = 0, flag.steering = 0;
//			// steer clear
//			steer = 0;
//		break;
//		case LEFT:
//			// flag set
//			flag.steering = 1, flag.dir = LEFT;
//			flag.driving_state = 1;
//		break;
//		case RIGHT:
//			// flag set
//			flag.steering = 1, flag.dir = RIGHT;
//			flag.driving_state = 1;
//		break;
//		default:
//			break;
//	}
//}
*/


void GetEncoder(void)
{
	// Sum
	encoder.left += TIM4->CNT;
	encoder.right += TIM1->CNT;
	// clear counter register
	TIM4->CNT = TIM1->CNT = 0;
}

void SetSteering(void)
{
	if(flag.steering){
		if(flag.dir == LEFT){
			if(ABSOLUTE(steer) < STEER_SAT)
				steer += 1;
		}
		else if(flag.dir == RIGHT){
			if(ABSOLUTE(steer) < STEER_SAT)
				steer -= 1;
		}
	}
}

// Standard Output String
PUTCHAR_PROTOTYPE			// For printf Function
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
//	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);     // Module
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);   // USB
	return ch;
}



