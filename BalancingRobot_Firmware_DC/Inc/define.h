// Define Header

/**
 * @author Jihoon Jang
 */

#ifndef _DEFINE_H
#define _DEFINE_H

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
	
#define ABSOLUTE(a) ((a>=0)? (a):(-a))
#define SAFTYFACTOR 0.01

// Driving Command Direction
#define FORWARD 1
#define LEFT 2
#define RIGHT 3
#define BACKWARD 4
#define STOP 5
// Robot State
#define TILTED 25
#define STANDED 2
	
/* User Define Type */
typedef unsigned char Byte;

// Struct
typedef struct ENCODER
{
	short left;
	short right;
}ENCODER;

typedef struct FLAG
{
	char set_angle_offset;
	char gain_tunned;
	char start_control;
	char driving_state;
	char steering;
	char dir;
	char ready_message;
	char receive_message;
}FLAG;
	
// struct
typedef struct GAIN
{
	double P, I, D;
}GAIN;

typedef struct CONTROL
{
	double P, I, D, result;
}CONTROL;

typedef struct CONTOLLER
{
	GAIN gain;
	CONTROL control;
}CONTROLLER;

// Communication
typedef struct Drive
{
	char dir;
	short speed;
}Drive;

typedef struct Gain
{
	double pPosture;
	double iPosture;
	double dPosture;
	double pCentroid;
	double iCentroid;
	double dCentroid;
	double pPosition;
	double iPosition;
}Gain;

typedef struct MONITORING
{
	ENCODER encoder;
	double angle;
}MONITORING;

typedef struct MCU2PC
{
	MONITORING monitoring;
	Byte command;
}MCU2PC;

typedef struct PC2MCU
{
	Gain gain;
	Drive drive;
}PC2MCU;

// For Calculate Angle
	
#define PI 3.14159265
#define RAD_TO_DEG 57.2957786

// PWM Pulse Limit

#define PWM_MAX 1000
#define DEAD_ZONE ((double)PWM_MAX*0.15)
	
// Anti WindUp
	
#define SATURATION ((double)PWM_MAX*1.2)
#define ANGLE_OFFSET_MAX (15.0)
#define SPEED_SAT (0.15)
#define STEER_SAT (300)

// Sensor Offset
#define MPU_OFFSET (8.4)

#endif
