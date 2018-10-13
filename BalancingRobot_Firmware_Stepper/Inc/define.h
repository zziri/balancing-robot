
// Define Header

/**
 * @author Jihoon Jang
 */

#ifndef _DEFINE_H_
#define _DEFINE_H_


// Define Constant
#define MAX_SPEED 400.0													/* MAX Stepper Frequency (MAX_SPEED * TIM3_Frequency) */
#define MARGIN 5
#define PI 3.14159265
#define RAD_TO_DEG 57.2957786										/* Radian To Degree */
#define MPU_OFFSET (-0.298)												/* MPU Sensor Offset */
#define DEAD_ZONE (20.0)						/* Motor Dead Zone */
#define FALL_ANGLE (25.0)
#define STAND_ANGLE (2.0)
#define CENTROID_INPUT_SCALE 0.001
#define MAX_ANGLE_OFFSET 15.0
#define DEFAULT_STEER_SPEED 40.0
#define MAX_DRIVING_SPEED (100.0 * CENTROID_INPUT_SCALE)
/* Set Stepper Driver Mode */
#define FULL_STEP 1
#define HALF_STEP 2
#define QUATER_STEP 3
#define EIGHT_STEP 4
#define SIXTEEN_STEP 5
/* Driving Or Tuning Flag */
#define FORWARD 1
#define LEFT 2
#define RIGHT 3
#define BACKWARD 4
#define STOP 5
#define TUNING 6
/* MCU to PC command */
#define MONITORING_MODE 0x00
#define READY_MESSAGE_MODE 0x40
#define CALL_BACK 0x80
// Define Macro
#define ABSOLUTE(a) ((a>=0)? (a):(-a))					/* Cal Absolute value */
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)								/* for printf */
#endif /* __GNUC__ */

typedef unsigned char Checksum;			/* Checksum */
typedef unsigned char Byte;

// Structure
typedef struct FLAG									/* Flag Type */
{
	char set_angle_offset;
	char signal_in_deadzone;
	char robot_is_down;
	char restart_control;
	char robot_is_ready;
	char tuning_complete;
	char steering;
	char driving_forward, driving_backward;
}FLAG;
typedef struct CLOCK								/* Clock counter type for stepper */
{
	long L, R;
}CLOCK;

typedef struct GAIN									/* PID Gain value type */
{
	double p, i, d;
}GAIN;

typedef struct CONTROL							/* pid control value (signal) type */
{
	double p, i, d, result;
}CONTROL;

typedef struct CONTROLLER						/* pid controller type */
{
	GAIN gain;
	CONTROL control;
}CONTROLLER;

typedef struct Gain									/* Gains from gain tuner(Server) */
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

typedef struct Drive								/* driving command */
{
	char dir;
	short speed;
}Drive;

typedef struct PC2MCU								/* Message Frome Server(PC) */
{
	Checksum checksum;
	Drive drive;
	Gain gain;
}PC2MCU;

typedef struct ENCODER
{
	short left;
	short right;
}ENCODER;

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

typedef struct Motor
{
	double steer;
	double output_left, output_right;
	double speed_set_point;
}Motor;


#endif			/* _DEFINE_H_ */

