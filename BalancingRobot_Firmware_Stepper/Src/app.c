
// Application Source

/**
 * @author Jihoon Jang
 */

#include "app.h"

CONTROLLER posture = {0, };
CONTROLLER centroid = {0, };
FLAG flag = {0, };
uint8_t buffer[12];
uint8_t pData[1];
double accXangle, accYangle; // Angle calculate using the accelerometer
double gyroXrate, gyroYrate;
unsigned long preTimeMPU;
double gotAngle;
double angleResult;
Motor motor;
PC2MCU pc2mcu;
MCU2PC mcu2pc;

// Standard Output String
PUTCHAR_PROTOTYPE			// For printf Function
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
//	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);     // Module
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);   // USB
	return ch;
}

void GET_ANGLE(void)
{
	unsigned long present;
	double timePass;
	double A;
	const double k = 3.0;
	int16_t accY;
	int16_t accZ;
	int16_t gyroX;

	HAL_I2C_IsDeviceReady(&hi2c1, 0xD0, 1, 1000);
	HAL_I2C_Mem_Write(&hi2c1, 0xD0, 0x6B, I2C_MEMADD_SIZE_8BIT, pData, 1, 500);
	// memory read
	HAL_I2C_Mem_Read(&hi2c1,0xD0,0x3D,I2C_MEMADD_SIZE_8BIT, &buffer[2],1,500);
	HAL_I2C_Mem_Read(&hi2c1,0xD0,0x3E,I2C_MEMADD_SIZE_8BIT, &buffer[3],1,500);
	HAL_I2C_Mem_Read(&hi2c1,0xD0,0x3F,I2C_MEMADD_SIZE_8BIT, &buffer[4],1,500);
	HAL_I2C_Mem_Read(&hi2c1,0xD0,0x40,I2C_MEMADD_SIZE_8BIT, &buffer[5],1,500);
	HAL_I2C_Mem_Read(&hi2c1,0xD0,0x43,I2C_MEMADD_SIZE_8BIT, &buffer[6],1,500);
	HAL_I2C_Mem_Read(&hi2c1,0xD0,0x44,I2C_MEMADD_SIZE_8BIT, &buffer[7],1,500);

	accY = (int)buffer[2] << 8 | (int)buffer[3];
	accZ = (int)buffer[4] << 8 | (int)buffer[5];
	gyroX = (int)buffer[6] << 8 | (int)buffer[7];
	accXangle = (atan2(accY,accZ)+PI)*RAD_TO_DEG;
	gyroXrate = (double)gyroX/131.0;
	
	present = milliseconds;
	timePass = (present - preTimeMPU)/1000.0;
	preTimeMPU = present;
	A = k/(k+timePass);
	if(flag.set_angle_offset){
		gotAngle = accXangle;
		flag.set_angle_offset = 0;
	}
	else if(!(flag.set_angle_offset)){
		gotAngle = A*(gotAngle + gyroXrate*timePass) + (1 - A)*accXangle;							/* Complementary Filter */
	}
	angleResult = gotAngle - 180.0 + MPU_OFFSET;
	
//	if(msec_count >= 50){																			/* every 50 msec */
//		msec_count = 0;																					/* reset counter variable */
//		printf("angle : %5.3f || encoder : %d %d\r\n", angleResult, encoder.left, encoder.right);								/* Print Angle to PC */
//	}
}

void InitControl(void)
{
	CONTROLLER clear = {0, };
	
	posture = clear;
	posture.gain.p = 30.0;					/* Set Gain */
	posture.gain.i = 0.9;
	posture.gain.d = 51.0;
	
	centroid = clear;
	centroid.gain.p = 10.5;
	centroid.gain.i = 0.095;
	centroid.gain.d = 4.0;
	
	flag.set_angle_offset = 1;
}

void MPU_Stabilize(void)
{
	int i;
	flag.set_angle_offset = 1;																	/* set angle offset flag set */
	for(i=0; i<1000; i++){
		GET_ANGLE();
		if(msec_count >= 50){																			/* every 50 msec */
			msec_count = 0;																					/* reset counter variable */
			printf("angle : %5.3f\r\n", angleResult);								/* Print Angle to PC */
		}
	}
	printf("Complete stabilize\r\n");														/* print message to PC */
}

void SetStepOption(int option)
{
	switch(option)
	{
		case FULL_STEP:
			
		break;
		case HALF_STEP:
			HAL_GPIO_WritePin(GPIOB, M0_Pin, GPIO_PIN_SET);
		break;
		case QUATER_STEP:
			HAL_GPIO_WritePin(GPIOB, M1_Pin, GPIO_PIN_SET);
		break;
		case EIGHT_STEP:
			HAL_GPIO_WritePin(GPIOB, M1_Pin | M1_Pin, GPIO_PIN_SET);
		break;
		case SIXTEEN_STEP:
			HAL_GPIO_WritePin(GPIOB, M2_Pin, GPIO_PIN_SET);
		break;
		default:
		break;
	}
}

int isFall(void)
{
	return ((ABSOLUTE(angleResult)) > FALL_ANGLE);
}

int isStand(void)
{
	return ((ABSOLUTE(angleResult)) < STAND_ANGLE);
}

// PID Posture Control
void PostureControl(void)
{
	static double preinput = 0.0;
	double input = centroid.control.result - angleResult;
	
	if(flag.restart_control)																																		/* if restart control */
		preinput = 0.0, flag.restart_control = 0;																									/* clear preinput */
	
	posture.control.p = posture.gain.p * input;																									/* Proportional control */
	posture.control.i += posture.gain.i * input;																								/* Integral control */
	if(posture.control.i > MAX_SPEED - MARGIN){																													/* Limit max output */
		if(posture.control.i > 0)	posture.control.i = MAX_SPEED - MARGIN;
		else	posture.control.i = -MAX_SPEED - MARGIN;
	}
	posture.control.d = posture.gain.d * (input - preinput);																		/* Differential control */
	posture.control.result = posture.control.p + posture.control.i + posture.control.d;					/* Sum PID Controller Result signal */
	
	if(posture.control.result > MAX_SPEED - MARGIN){																						/* Limit max output */
		if(posture.control.result > 0)	posture.control.i = MAX_SPEED - MARGIN;
		else	posture.control.result = -MAX_SPEED - MARGIN;
	}
	
	preinput = input;																																						/* renew preinput */
}

void SetDeadZone(void)
{
		if(flag.steering){
			if(ABSOLUTE(posture.control.result) < DEAD_ZONE)																					/* Set Dead Zone */
				posture.control.result = 0.0;
			flag.signal_in_deadzone = 0;
			motor.output_left = posture.control.result + motor.steer;
			motor.output_right = posture.control.result - motor.steer;
		}
		else{
			if(ABSOLUTE(posture.control.result) < DEAD_ZONE)																					/* Set Dead Zone */
				posture.control.result = 0.0, flag.signal_in_deadzone = 1;
			else
				flag.signal_in_deadzone = 0;																														/* Reset Dead Zone */
			motor.output_left = motor.output_right = posture.control.result;
		}
}

void SlowlyIncreaseDriving(void)
{
	double decel = MAX_DRIVING_SPEED / 4000.0;
	double accel = MAX_DRIVING_SPEED / 50.0;
	if(flag.driving_forward){																																			/* Forward driving */
		if(motor.speed_set_point < MAX_DRIVING_SPEED){
			motor.speed_set_point += accel;																														/* Increase Slowly */
		}
		else if(motor.speed_set_point >= MAX_DRIVING_SPEED){																				/* Limit */
			motor.speed_set_point = MAX_DRIVING_SPEED;
		}
	}
	else if(flag.driving_backward){																																/* Backward Driving */
		if(motor.speed_set_point > -MAX_DRIVING_SPEED){
			motor.speed_set_point -= accel;																														/* Increase Slowly */
		}
		else if(motor.speed_set_point <= -MAX_DRIVING_SPEED){																				/* Limit */
			motor.speed_set_point = -MAX_DRIVING_SPEED;
		}
	}
	else if(!flag.driving_backward && !flag.driving_forward){																			/* Stop slowly */
		if(motor.speed_set_point > (decel * 1.5)){
			motor.speed_set_point -= decel;
		}
		else if(motor.speed_set_point < -(decel * 1.5)){
			motor.speed_set_point += decel;
		}
		else
			motor.speed_set_point = 0;
	}
}

void CentroidControl(void)
{
	static double preinput = 0.0;
	double input = posture.control.result * CENTROID_INPUT_SCALE - motor.speed_set_point;
	if(flag.restart_control)
		preinput = 0.0;
	centroid.control.p = centroid.gain.p * input;
	centroid.control.i += centroid.gain.i * input;
	
	if(ABSOLUTE(centroid.control.i) > MAX_ANGLE_OFFSET){																				/* Max Set point 15.0 degree */
		if(centroid.control.i > 0)	centroid.control.i = MAX_ANGLE_OFFSET;
		else	centroid.control.i = -MAX_ANGLE_OFFSET;
	}
	
	centroid.control.d = centroid.gain.d * (input - preinput);
	centroid.control.result = centroid.control.p + centroid.control.i + centroid.control.d;
	
	if(ABSOLUTE(centroid.control.result) > 15.0){																								/* Max Set point 15.0 degree */
		if(centroid.control.result > 0)	centroid.control.result = MAX_ANGLE_OFFSET;
		else	centroid.control.result = -MAX_ANGLE_OFFSET;
	}
	
	preinput = input;
}

void Calibration(void)
{
	if(motor.output_left > 0)
		motor.output_left = 405 - (1/(motor.output_left + 9)) * 5500;
	else if(motor.output_left < 0)
		motor.output_left = - 405 - (1/(motor.output_left - 9)) * 5500;
	
	if(motor.output_right > 0)
		motor.output_right = 405 - (1/(motor.output_right + 9)) * 5500;
	else if(motor.output_right < 0)
		motor.output_right = - 405 - (1/(motor.output_right - 9)) * 5500;
}

void SetMotor(void)
{
	if(motor.output_left > 0)
		HAL_GPIO_WritePin(L_Motor_Dir_GPIO_Port, L_Motor_Dir_Pin, GPIO_PIN_SET);
	else if(motor.output_left < 0)
		HAL_GPIO_WritePin(L_Motor_Dir_GPIO_Port, L_Motor_Dir_Pin, GPIO_PIN_RESET);
	if(motor.output_right > 0)
		HAL_GPIO_WritePin(R_Motor_Dir_GPIO_Port, R_Motor_Dir_Pin, GPIO_PIN_RESET);
	else if(motor.output_right < 0)
		HAL_GPIO_WritePin(R_Motor_Dir_GPIO_Port, R_Motor_Dir_Pin, GPIO_PIN_SET);
	
	step.L = (long)(MAX_SPEED - ABSOLUTE(motor.output_left));															/* Set Frequency */
	step.R = (long)(MAX_SPEED - ABSOLUTE(motor.output_right));
}



void ResetControl(void)
{
	HAL_TIM_Base_Stop_IT(&htim3);																																/* Motor Drive Stop */
	InitControl();																																							/* initialize variable */
	flag.robot_is_down = 1;																																			/* Assume robot is down */
	HAL_Delay(500);																																							/* Delay 500msec */
}

void StartControl(void)
{
	HAL_TIM_Base_Start_IT(&htim3);																															/* Motor Drive Start */
	flag.robot_is_down = 0;																																			/* robot is stand */
	flag.restart_control = 1;
}

int isChecksumError(void)
{
	char* ptr = (char*)(&pc2mcu);
	int temp = 0;
	Checksum check = 0;
	int i;
	for(i=1; i<sizeof(pc2mcu); i++){
		temp += ptr[i];																																						/* Sum every byte */
		temp &= 0x000000FF;																																				/* Bit Masking */
	}
	check = (char)temp;
	return (check != pc2mcu.checksum);																													/* if checksum error return true */
}

void Monitoring(void)
{
	memset(&mcu2pc, 0, sizeof(mcu2pc));																													/* Clear */
	if(flag.robot_is_ready){																																		/* Transmit ready message to server(PC) */
		flag.robot_is_ready = 0;																																	/* flag reset */
		mcu2pc.command = READY_MESSAGE_MODE;																											/* transmit ready message mode command */
	}
	else if(flag.tuning_complete){
		flag.tuning_complete = 0;
		mcu2pc.command = CALL_BACK;
	}
	else{																																												/* Normal State */
		mcu2pc.command = MONITORING_MODE;																													/* Monitoring mode command */
		mcu2pc.monitoring.angle = angleResult;																										/* Set Data */
		mcu2pc.monitoring.encoder = encoder;																											// add encoder
	}
	HAL_UART_Transmit_IT(&huart1, (uint8_t*)(&mcu2pc), sizeof(mcu2pc));													/* Transmit to Server(PC) */
}

void TuningOrDriving(void)
{
	if(pc2mcu.drive.dir == TUNING){																															/* Tuning Mode */
		Tuning();
	}
	else{																																												/* Driving Mode */
		Driving();
	}
}

void Driving(void)
{
	char dir = pc2mcu.drive.dir;
	switch(dir)
	{
		case FORWARD:
			flag.driving_forward = 1;
			flag.driving_backward = 0;
//			motor.speed_set_point = (double)pc2mcu.drive.speed * (double)CENTROID_INPUT_SCALE;
		break;
		case BACKWARD:
			flag.driving_backward = 1;
			flag.driving_forward = 0;
//			motor.speed_set_point = -(double)pc2mcu.drive.speed * (double)CENTROID_INPUT_SCALE;
		break;
		case STOP:
			motor.steer = 0;
			flag.steering = 0;
			flag.driving_forward = flag.driving_backward = 0;
		break;
		case LEFT:
			motor.steer = DEFAULT_STEER_SPEED;
			flag.steering = 1;
		break;
		case RIGHT:
			motor.steer = -DEFAULT_STEER_SPEED;
			flag.steering = 1;
		break;
		default:
		break;
	}
}

void Tuning(void)
{
	posture.gain.p = pc2mcu.gain.pPosture;
	posture.gain.i = pc2mcu.gain.iPosture;
	posture.gain.d = pc2mcu.gain.dPosture;
	
	centroid.gain.p = pc2mcu.gain.pCentroid;
	centroid.gain.i = pc2mcu.gain.iCentroid;
	centroid.gain.d = pc2mcu.gain.dCentroid;
	flag.tuning_complete = 1;
}




