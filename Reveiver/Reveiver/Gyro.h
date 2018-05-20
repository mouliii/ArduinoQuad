// Gyro.h

#ifndef _GYRO_h
#define _GYRO_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class Gyro
{
 public:
	 Gyro();
	 void setup_mpu_6050_registers();
	 void Init();
	 void read_mpu_6050_data();
	 float anglePitch()
	 {
		 return angle_pitch - angle_pitch_offset;
	 }
	 float angleRoll()
	 {
		 return angle_roll - angle_roll_offset;
	 }
private:
	const float gyroWeight = 0.6f;
	const float accWight = 1.0f - gyroWeight;
	float angle_roll_acc = 0;
	float angle_pitch_acc = 0;
	float angle_pitch_offset, angle_roll_offset;
	int gyro_x, gyro_y, gyro_z;
	long acc_x, acc_y, acc_z, acc_total_vector;
	int temperature;
	long gyro_x_cal, gyro_y_cal, gyro_z_cal;
	int lcd_loop_counter;
	float angle_pitch, angle_roll;
	int angle_pitch_buffer, angle_roll_buffer;
};

//////////////////////////////////////
// !!!!! !!!!!!! !!!!! !!!!! !!!!! !!!!!
//  angleRoll() JA anglePitch() ON INVERTOITU
///////////////////////////////////////

#endif

