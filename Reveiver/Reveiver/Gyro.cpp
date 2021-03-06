#include "Gyro.h"
#include <Wire.h>

Gyro::Gyro()
{
}

void Gyro::setup_mpu_6050_registers()
{
	//Activate the MPU-6050
	Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
	Wire.write(0x6B);                                                    //Send the requested starting register
	Wire.write(0x00);                                                    //Set the requested starting register
	Wire.endTransmission();                                              //End the transmission
																		 //Configure the accelerometer (+/-8g)
	Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
	Wire.write(0x1C);                                                    //Send the requested starting register
	Wire.write(0x10);                                                    //Set the requested starting register
	Wire.endTransmission();                                              //End the transmission
																		 //Configure the gyro (500dps full scale)
	Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
	Wire.write(0x1B);                                                    //Send the requested starting register
	Wire.write(0x08);                                                    //Set the requested starting register
	Wire.endTransmission();                                              //End the transmission
}

void Gyro::Init()
{
	for (int cal_int = 0; cal_int < 2000; cal_int++) {                 //Run this code 2000 times
		if (cal_int % 125 == 0)
		{  
			Serial.print(".");												//Print a dot on the LCD every 125 readings
		}
		read_mpu_6050_data();                                              //Read the raw acc and gyro data from the MPU-6050
		gyro_x_cal += gyro_x;                                              //Add the gyro x-axis offset to the gyro_x_cal variable
		gyro_y_cal += gyro_y;                                              //Add the gyro y-axis offset to the gyro_y_cal variable
		gyro_z_cal += gyro_z;                                              //Add the gyro z-axis offset to the gyro_z_cal variable
		angle_pitch_acc += angle_pitch_acc;        //<----------                         //Accelerometer calibration value for pitch
		angle_roll_acc += angle_roll_acc;         //<----------                         //Accelerometer calibration value for roll
		delay(4);                                                          //Delay 4ms to simulate the 250Hz program loop
	}
	Serial.println();
	gyro_x_cal /= 2000;                                                  //Divide the gyro_x_cal variable by 2000 to get the avarage offset
	gyro_y_cal /= 2000;                                                  //Divide the gyro_y_cal variable by 2000 to get the avarage offset
	gyro_z_cal /= 2000;													//Divide the gyro_z_cal variable by 2000 to get the avarage offset

	//TODO calibrate here
	angle_pitch_acc /= 2000;
	angle_roll_acc /= 2000;

	angle_pitch_acc -= angle_pitch_offset;
	angle_roll_acc -= angle_roll_offset;

	angle_pitch = angle_pitch_acc;										//Set the gyro pitch angle equal to the accelerometer pitch angle
	angle_roll = angle_roll_acc;										//Set the gyro roll angle equal to the accelerometer roll angle
	
}

void Gyro::read_mpu_6050_data()
{
	Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
	Wire.write(0x3B);                                                    //Send the requested starting register
	Wire.endTransmission();                                              //End the transmission
	Wire.requestFrom(0x68, 14);                                //Request 14 bytes from the MPU-6050
	while (Wire.available() < 14);                             //Wait until all the bytes are received
	acc_x = Wire.read() << 8 | Wire.read();                              //Add the low and high byte to the acc_x variable
	acc_y = Wire.read() << 8 | Wire.read();                              //Add the low and high byte to the acc_y variable
	acc_z = Wire.read() << 8 | Wire.read();                              //Add the low and high byte to the acc_z variable
	temperature = Wire.read() << 8 | Wire.read();                        //Add the low and high byte to the temperature variable
	gyro_x = Wire.read() << 8 | Wire.read();                             //Add the low and high byte to the gyro_x variable
	gyro_y = Wire.read() << 8 | Wire.read();                             //Add the low and high byte to the gyro_y variable
	gyro_z = Wire.read() << 8 | Wire.read();                             //Add the low and high byte to the gyro_z variable

	gyro_x -= gyro_x_cal;                                                //Subtract the offset calibration value from the raw gyro_x value
	gyro_y -= gyro_y_cal;                                                //Subtract the offset calibration value from the raw gyro_y value
	gyro_z -= gyro_z_cal;                                                //Subtract the offset calibration value from the raw gyro_z value

																		 //Gyro angle calculations
																		 //0.0000611 = 1 / (250Hz / 65.5)
	angle_pitch += gyro_x * 0.0000611;                                   //Calculate the traveled pitch angle and add this to the angle_pitch variable
	angle_roll += gyro_y * 0.0000611;                                    //Calculate the traveled roll angle and add this to the angle_roll variable
	angle_z += gyro_z * 0.0000611;

																		 //0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians
	angle_pitch += angle_roll * sin(gyro_z * 0.000001066);               //If the IMU has yawed transfer the roll angle to the pitch angel
	angle_roll -= angle_pitch * sin(gyro_z * 0.000001066);               //If the IMU has yawed transfer the pitch angle to the roll angel
																		 //Accelerometer angle calculations
	acc_total_vector = sqrt((acc_x * acc_x) + (acc_y * acc_y) + (acc_z * acc_z)); //Calculate the total accelerometer vector
																				  //57.296 = 1 / (3.142 / 180) The Arduino asin function is in radians
	angle_pitch_acc = asin((float)acc_y / acc_total_vector) * 57.296;    //Calculate the pitch angle
	angle_roll_acc = asin((float)acc_x / acc_total_vector) * -57.296;    //Calculate the roll angle

	angle_pitch = angle_pitch * 0.98f + angle_pitch_acc * 0.02f;			//Correct the drift of the gyro pitch angle with the accelerometer pitch angle
	angle_roll = angle_roll * 0.98f + angle_roll_acc * 0.02f;				 //Correct the drift of the gyro roll angle with the accelerometer roll angle
	angle_z = angle_z * 0.98f + gyro_z * 0.02f;
	
	angle_pitch = gyroWeight * angle_pitch + accWight * angle_pitch_acc;
	angle_roll = gyroWeight * angle_roll + accWight * angle_roll_acc;
}