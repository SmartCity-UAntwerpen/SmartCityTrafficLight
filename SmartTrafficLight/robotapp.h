#ifndef ROBOT_APP
#define ROBOT_APP

#include "project.h"
#include "rs485client.h"
#include "lego-motor.h"
#include "lin-lego-motor-log.h"
#include "lego-sensor.h"
#include "imu.h"
#include "rf-cc1101.h"
#include "pwr-liion1a.h"
#include "lin-delay.h"
#include "TimeSupport.h"
#include <stdlib.h>


#define RS485_LEGO_MOTOR_ADDR 1
#define RS485_LEGO_SENSOR_ADDR 2
#define RS485_RF_ADDR 4
#define RS485_BAT_ADDR 3
#define RS485_IMU_ADDR 5


#define CYCLE_TIME 2000 //Red-green time (ms)
#define SWITCH_TIME 1000 //Time between red-green switch where all lights are red (ms)

#define LIGHT1 0
#define LIGHT2 1
#define GREEN 1
#define RED 0


extern RS485ClientStruct RS485Client;
extern LegoMotorStruct LegoMotor;
extern LegoSensorStruct LegoSensor;
extern RfCC1101Struct RfCC1101;
extern PwrLiIon1AStruct PwrLiIion1A;
extern ImuStruct Imu;

void RobotApp(int argc, char *argv[]);


#endif
