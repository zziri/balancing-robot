// Application header

#ifndef _APP_H
#define _APP_H

#include "stm32f4xx_hal.h"
#include "tim.h"
#include "define.h"

extern uint8_t buffer[12];
extern uint8_t pData[1];
extern double accXangle, accYangle; // Angle calculate using the accelerometer
extern double gyroXrate, gyroYrate;
extern unsigned long preTimeMPU;
extern FLAG flag;
extern double gotAngle;
extern double angleResult;
extern TIM_OC_InitTypeDef OC_Config_Struct;
extern CONTROLLER posture;
extern CONTROLLER centroid;
extern CONTROLLER position;
extern const double Angle_Scale;
extern ENCODER encoder;

// Cal Angle
void GET_ANGLE(void);
int PWM_Filter(double value);
// Output Compare structure Init
void OC_Struct_Init(void);
// Motor Drive Function
void MotorDrive(void);
// Stop Platform
void Motor_Stop(void);
int isTilted(void);
int isStanded(void);
// Control Robot
void Posture_Control(void);
void Centroid_Control(void);
void PositionControl(void);
void InitControl(void);
void ReceiveDataFromPC(void);
void SetDrive(void);
// Get Robot State
int isTilted(void);
int isStanded(void);
// Steering
void SetSteering(void);

void GetEncoder(void);



#endif
