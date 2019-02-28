#ifndef _SENSOR_H_
#define _SENSOR_H_

/**********************Sensor Configuration**********************/
const int TemperaturePin = 0;   //Select ADC Pin A0
float TemperatureValue =  0;
/**********************Sensor Configuration**********************/

float ReadTemperature(int);
void delay_seconds(int sec);
void delay_minutes(int mn);

#endif 
