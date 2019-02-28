#ifndef _AT_COMMANDS_H_
#define _AT_COMMANDS_H_

#include <SoftwareSerial.h>
extern SoftwareSerial BG96UART;
#define LED 13

void BG96Initialization();                                                        //Initialization Function
char BG96SerialRead(void);                                                        //Read data from BG96
void BG96ClearBuffer(void);                                                       //Clear UART buffer 
void BG96ATCommandOK(void);                                                       //Test to device send AT Command 
void BG96ATEchoModeOff(void);                                                     //Echo mode off function
void BG96SIMPinCode(char*);                                                       //Unlock to SIM pin
void BG96RequestIMSI(void);                                                       //Get IMSI information from device
void BG96GetIMEI(void);                                                           //Get IMEI information from device
void BG96NetworkAttach(void);                                                     //Connect to network
void BG96DefineAPNAddr(char *apn);                                              //Set the APN address(Which provide by Network Operator)
void BG96OperatorSelection(void);                                                 //Manual Operator selection
void BG96GetSignalQualityReport(void);                                            //Get RSSI and BER informations
void BG96SetDeviceFunctionality(void);                                            //Set the device functionality
void BG96NetworkRegStatus(void);                                                  //Get network registration information                                   
void BG96OpenUDPSocket(int connectID, int local_port);                            //Create a UDP socket
String ConvertFloat2String(float);       
void BG96SendUDPData(String data, int connectID, char *remoteip, int remote_port); //Send data using UDP protocol
void BG96GetPDPAddress(void);                                                     //Get PDP address from device
int BG96UDPReceive(void);
void BG96UDPClose(void);
void BG96QuerySocket(void);
/**********************************************************************/

#endif 
