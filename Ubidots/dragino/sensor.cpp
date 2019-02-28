#include <Arduino.h>

/**************************************************************************************
 * parameter      ：NONE
 * return value   ：NONE 
 * Device         ：LM35 Temperature Sensor
 * Using Methode  : ADC(10 bit)
 * Documents      :(1)->http://www.redrok.com/TemperatureSensor_LM35_10mVperC.pdf
 * date           ：07/01/2019
*****************************************************************************************/
int value = 0;
float Temp = 0;

float ReadTemperature(int AnalogPin) {
  value = analogRead(AnalogPin);
  Temp = 0.4887 * value;
  return Temp;
}

void delay_seconds(int sec) {
  int j = 0;
  for(j = 0 ; j <= sec ; j++) {
    delay(1000);     
  }
}

void delay_minutes(int mn) {
  int j = 0;
  for(j = 0 ; j <= mn ; j++) {
    delay(60000);     
  }
}
