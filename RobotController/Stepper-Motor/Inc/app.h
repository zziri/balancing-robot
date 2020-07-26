
// Application Header

/**
 * @author Jihoon Jang
 */

#ifndef _APP_H_
#define _APP_H_

#include "stm32f4xx_hal.h"
#include "define.h"
#include "usart.h"
#include "i2c.h"
#include "math.h"
#include "interrupt.h"
#include <string.h>

// Variables
extern CONTROLLER posture;
extern uint8_t buffer[12];
extern uint8_t pData[1];
extern double accXangle, accYangle; // Angle calculate using the accelerometer
extern double gyroXrate, gyroYrate;
extern unsigned long preTimeMPU;
extern double gotAngle;
extern double angleResult;
extern FLAG flag;
extern PC2MCU pc2mcu;
extern MCU2PC mcu2pc;
extern Motor motor;

// Functions
void GET_ANGLE(void);
void InitControl(void);
void PostureControl(void);
void CentroidControl(void);
void MPU_Stabilize(void);
void SetStepOption(int);
int isFall(void);
int isStand(void);
void SetMotor(void);
void Calibration(void);
void SetDeadZone(void);
void ResetControl(void);
void StartControl(void);
void Monitoring(void);
int isChecksumError(void);
void TuningOrDriving(void);
void Tuning(void);
void Driving(void);
void SlowlyIncreaseDriving(void);

#endif		/* _APP_H_ */

