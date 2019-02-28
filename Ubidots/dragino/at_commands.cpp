#include <Arduino.h>
#include "at_commands.h"
/**********************Setting UART Communication**********************/
#define Baudrate   9600                                                   //Define the baud is 9600bps/s
const byte RxPin = 10;                                                    //Software Serial port for Dragino Shield(Rx)
const byte TxPin = 11;                                                    //Software Serial port for Dragino Shield(Tx)
extern SoftwareSerial BG96UART;
SoftwareSerial BG96UART(RxPin, TxPin);                                    //RX->10  TX->11 (According to first datasheet)

/**********************Storage Variables**********************/
static char msg[256] = {'\0'};                                         //save the mcu return message for AT Command
static char IMEI[16] = {'\0'};                                         //save the IMEI Address
#define OPERATOR_CODE 22201                                            //TIM Network MCC:222 MNC:01 Italy

int status = 0;
/**********************Function Declarations**********************/

/**************************************************************************************
   funciton name  ：BG96SerialRead()
   parameter      ：void
   return value   ：char
   effect         ：Data was stored in msg array
*****************************************************************************************/
char BG96SerialRead(void) {
  int i = 0;
  memset(msg, 0, sizeof(msg));                                    //reference c language lib memset
  BG96UART.flush();                                               //Waits for the transmission of outgoing serial data to complete.
  delay(1500);                                                    //Waits for respones from server
  while (BG96UART.available() && i < sizeof(msg) - 1) {
    msg[i] = BG96UART.read();                                     //read return value and store in this array
    i++;
  }
  msg[i] = '\0';
  Serial.println(msg);
  return msg;
}

/**************************************************************************************
   funciton name  ：BG96ClearBuffer()
   parameter      ：void
   return value   ：void
   effect         ：Clear the buffer
*****************************************************************************************/
void BG96ClearBuffer() {
  BG96UART.flush();
  while (BG96UART.available()) {
    BG96UART.read();                                                    //read return value and store in this array
  }
}

/**************************************************************************************
   funciton name  ：BG96ATCommandOK()
   parameter      ：void
   return value   ：void
   effect         ：Sent the AT command to check the module(the return value has to be "OK")
*****************************************************************************************/
void BG96ATCommandOK(void) {
  while (1) {
    BG96ClearBuffer();
    Serial.println(F("AT"));
    BG96UART.println(F("AT"));                                          //send "AT"command to NB-IoT
    BG96SerialRead();                                                   //read the return value
    if (strstr(msg, "OK") != NULL) {                                    //if return value is "OK" than program delay 1second jump out loop
      delay(1000);
      break;
    }
  }
}

/**************************************************************************************
   funciton name  ：nwscan()
   parameter      ：void
   return value   ：void
   effect         ：Extended Configuration Settings
*****************************************************************************************/
void nwscan() {
  BG96ClearBuffer();
  Serial.println(F("AT+QCFG=\"nwscanseq\",03,1"));   // Configure RAT Searching Sequence
  BG96UART.println(F("AT+QCFG=\"nwscanseq\",03,1"));
  BG96SerialRead();
  BG96ClearBuffer();
  Serial.println(F("AT+QCFG=\"nwscanmode\",0,1"));   //Configure RAT(s) to be Searched
  BG96UART.println(F("AT+QCFG=\"nwscanmode\",0,1"));
  BG96SerialRead();
  BG96ClearBuffer();
  Serial.println(F("AT+QCFG=\"iotopmode\",1,1"));    //Configure Network Category to be Searched under LTE RAT
  BG96UART.println(F("AT+QCFG=\"iotopmode\",1,1"));
  BG96SerialRead();
  BG96ClearBuffer();
  Serial.println(F("AT+QCFG=\"band\",03,A0E189F"));  //Band Configuration
  BG96UART.println(F("AT+QCFG=\"band\",03,A0E189F"));
  BG96SerialRead();
}

/**************************************************************************************
   funciton name  ：BG96ActivateErrorCode()
   parameter      ：void
   return value   ：void
   effect         ：Display the Name of Registered Network
*****************************************************************************************/
void BG96ActivateErrorCode() {
  BG96ClearBuffer();
  Serial.println(F("AT+QSPN"));
  BG96UART.println(F("AT+QSPN"));                                            //send "AT"command to NB-IoT
  BG96SerialRead();
}

/**************************************************************************************
   funciton name  ：BG96RequestIMSI()
   parameter      ：void
   return value   ：void
   effect         ：This function requests the International Mobile Subscriber Identity (IMSI)
*****************************************************************************************/
void BG96RequestIMSI() {
  while (1) {
    BG96ClearBuffer();
    Serial.println(F("AT+QNWINFO"));
    BG96UART.println(F("AT+QNWINFO"));                                            //send "AT"command to NB-IoT
    BG96SerialRead();
    if (strstr(msg, "+QNWINFO: No Service") != NULL) {
      delay(1000);
      break;
    }
    if (strstr(msg, "+QNWINFO: \"CAT-NB1\",\"22201\",\"LTE BAND 20\",6290") != NULL) {
      delay(1000);
      break;
    }
  }
}

/**************************************************************************************
   funciton name  ：BG96ATEchoModeOff()
   parameter      ：void
   return value   ：void
   effect         ：Echo mode is off
*****************************************************************************************/
void BG96ATEchoModeOff(void) {
  BG96ClearBuffer();
  Serial.println(F("ATE0"));
  BG96UART.println(F("ATE0"));
  BG96SerialRead();
}

/**************************************************************************************
   funciton name  ：BG96SIMPinCode()
   parameter      ：void
   return value   ：void
   effect         ：This function is used to enter a password or query whether or not
                    the module requires a password which is necessary before it can be
                    operated
*****************************************************************************************/
void BG96SIMPinCode(char *PinCode) {
  while (1) {
    BG96ClearBuffer();
    Serial.println(F("AT+CPIN?"));
    BG96UART.println(F("AT+CPIN?"));
    BG96SerialRead();
    if (strstr(msg, "+CPIN: READY") != NULL) {              //Request the PIN situation, If pin is already activated break to loop, else enter the PIN
      break;
    }
    else {
      BG96ClearBuffer();
      char cmd[128] = {'\0'};
      snprintf(cmd, sizeof(cmd), "AT+CPIN=%s", PinCode);    //Create a UDP socket
      Serial.println(cmd);
      BG96UART.println(cmd);
      BG96SerialRead();//send to udp server
    }
  }
}

/**************************************************************************************
   funciton name  ：BG96GetIMEI
   parameter      ：void
   return value   ：void
   effect         ：This function returns the International Mobile Equipment Identity (IMEI).
*****************************************************************************************/
void BG96GetIMEI(void) {
  while (1) {
    BG96ClearBuffer();                                                 //flush bc96_serial
    Serial.println(F("AT+GSN"));
    BG96UART.println(F("AT+GSN"));                                     //send AT+GSN command to get IMEI
    BG96SerialRead();                                                  //read the return value
    break;                                                             //jump out of loop
  }
}

/**************************************************************************************
   funciton name  ：BG96NetworkAttach()
   parameter      ：void
   return value   ：void
   effect         ：This function is used to attach the MT to, or detach the MT from the
                    Packet Domain service.
*****************************************************************************************/
void BG96NetworkAttach(void) {
  while (1) {
    BG96ClearBuffer();                                                //clear the buffer
    Serial.println(F("AT+CGATT=1"));
    BG96UART.println(F("AT+CGATT=1"));                                //send "AT"command to NB-IoT
    delay(500);
    BG96ClearBuffer();                                                //clear the send buffer
    Serial.println(F("AT+CGATT?"));
    BG96UART.println(F("AT+CGATT?"));                                 //"AT+CGATT"command
    BG96SerialRead();                                                 //return the value
    if (strstr(msg, "+CGATT: 1") != NULL) {                           //return value 1：active，0：disavtive
      break;                                                         //active break while
    }
    if (strstr(msg, "+CGATT: 0") != NULL) {
      continue;
    }
  }
}

/**************************************************************************************
   funciton name  ： BG96DefineAPNAddr()
   parameter      ：void
   return value   ：void
   effect         ：This function specifies PDP context parameters for a specific context
*****************************************************************************************/
void BG96DefineAPNAddr(char *apn) {
  BG96ClearBuffer();
  snprintf(msg, sizeof(msg), "AT+CGDCONT=1,\"IP\",\"%s\"", apn); //IP -> IPv4
  Serial.println(msg);
  BG96UART.println(msg);                 //Setting the APN address which provides by network operator
  delay(1000);
  BG96ClearBuffer();
  BG96SerialRead();
}

/**************************************************************************************
   funciton name  ：BG96OperatorSelection()
   parameter      ：void
   return value   ：void
   effect         ：The command returns the current operators and their status, and allows setting
                    automatic or manual network selection.
*****************************************************************************************/
void BG96OperatorSelection(void) {
  while (1) {
    BG96ClearBuffer();
    Serial.println(F("AT+COPS?"));
    BG96UART.println(F("AT+COPS?"));
    BG96SerialRead();

    if (strstr(msg, "+COPS: 1") != NULL) {            //return value 1：active，0：deactivate
      BG96ClearBuffer();
      Serial.println(F("AT+COPS=1,2,OPERATOR_CODE,9"));
      BG96UART.println(F("AT+COPS=1,2,OPERATOR_CODE,9"));
      BG96SerialRead();
      break;
    }
    if (strstr(msg, "+COPS: 0") != NULL) {          //If chosen 1 TIM network breaks the loop
      BG96ClearBuffer();
      Serial.println(F("AT+COPS=1,2,OPERATOR_CODE,9"));
      BG96UART.println(F("AT+COPS=1,2,OPERATOR_CODE,9"));  //Manuel Operator selection (for TIM Network, MNN->222 & MNC->01)
      BG96SerialRead();
      continue;
    }
  }
}
/**************************************************************************************
   funciton name  ：BG96GetSignalQualityReport()
   parameter      ：void
   return value   ：void
   effect         ：The function indicates the received signal strength <rssi> and the channel bit error rate <ber>.
*****************************************************************************************/
void BG96GetSignalQualityReport(void) {
  while (1) {
    BG96ClearBuffer();
    Serial.println(F("AT+CSQ"));
    BG96UART.println(F("AT+CSQ"));
    BG96SerialRead();
    if (strstr(msg, "+CSQ: 99,99") != NULL) {           //99: Not known or not detectable
      continue;                                         //Wait until detectable for those signals
    }
    if (strstr(msg, "+CSQ:") != NULL) {
      break;
    }
  }
}

/**************************************************************************************
   funciton name  ：BG96SetDeviceFunctionality()
   parameter      ：void
   return value   ：void
   effect         ：This function controls the functionality level of device
*****************************************************************************************/
void BG96SetDeviceFunctionality(void) {
  while (1) {
    BG96ClearBuffer();
    Serial.println(F("AT+CFUN?"));
    BG96UART.println(F("AT+CFUN?"));
    BG96SerialRead();
    if (strstr(msg, "+CFUN: 0") != NULL) {   //if return value is "0",  ME is in minimum functionality
      continue;
    }
    if (strstr(msg, "+CFUN: 1") != NULL) {   //if return value is "1", ME is in full functionality
      break;
    }
  }
}

/**************************************************************************************
   funciton name  ：BG96NetworkRegStatus()
   parameter      ：void
   return value   ：void
   effect         ：This function queries the network registration status
*****************************************************************************************/
void BG96NetworkRegStatus(void) {
  while (1) {
    BG96ClearBuffer();
    Serial.println(F("AT+CEREG=2"));
    BG96UART.println(F("AT+CEREG=2"));
    delay(500);
    BG96UART.println(F("AT+CEREG?"));
    BG96SerialRead();
    if (strstr(msg, "+CEREG: 1,1") != NULL) {     //res +CEREG:n,stat (stat=2 indicate not registered)
      break;
    }
    if (strstr(msg, "+CEREG: 2,1") != NULL) {
      break;
    }
  }
}

/**************************************************************************************
   funciton name  ：BG96OpenUDPSocket()
   parameter      ：int int
   return value   ：void
   effect         ：Open a UDP Socket. You need to define ConnectID which is has to be same when you send data(BG96SendUDPData())
*****************************************************************************************/
void BG96OpenUDPSocket(int connectID, int local_port) {
  BG96ClearBuffer();
  char cmd[128] = {'\0'};
  snprintf(cmd, sizeof(cmd), "AT+QIOPEN=1,%d,\"UDP SERVICE\",\"127.0.0.1\",0,%d,0", connectID, local_port);    //Create a UDP socket
  Serial.println(cmd);
  BG96UART.println(cmd);
  BG96SerialRead();
  while (strstr(msg, "OK") == NULL) {                                                                    //If there is already a socket
    snprintf(cmd, sizeof(cmd), "AT+QICLOSE=%d", connectID);                                              //Try to close old socket
    Serial.println(cmd);
    BG96UART.println(cmd);
    BG96SerialRead();
    snprintf(cmd, sizeof(cmd), "AT+QIOPEN=1,%d,\"UDP SERVICE\",\"127.0.0.1\",0,%d,0", connectID, local_port);      //Create a UDP socket
    Serial.println(cmd);
    BG96UART.println(cmd);
    BG96SerialRead();
  }
}

/**************************************************************************************
   funciton name  ：BG96QuerySocket()
   parameter      ：void
   return value   ：void
   effect         ：
*****************************************************************************************/
void BG96QuerySocket(void) {
  BG96ClearBuffer();
  Serial.println("AT+QISTATE=0,1");
  BG96UART.println(F("AT+QISTATE=0,1"));
  BG96SerialRead();
}

/**************************************************************************************
   funciton name  ：BG96SendUDPData()
   parameter      ：int, int
   return value   ：void
   effect         ：Send Data to UDP server. You need to define IP address of server and Port
*****************************************************************************************/
String ConvertFloat2String(float data) {
  static char buf[256];
  String tostring;
  dtostrf(data, 1, 2, buf);
  tostring = String(buf);
  return tostring;
}

void BG96SendUDPData(String data, int connectID, char *remoteip, int remote_port) {
  String buf;
  buf = data;
  snprintf(msg, sizeof(msg), "AT+QISEND=%d,%d,\"%s\",%d", connectID, buf.length(), remoteip, remote_port);    //After send this command it will open a text window
  Serial.println(msg);
  BG96UART.println(msg);
  while (BG96SerialRead() == '>') {                                                               //Wait until this response appears
    Serial.print(buf);                                                                           //You can write your data
  }
  Serial.print(buf);
  BG96UART.println(buf);
  BG96SerialRead();
  delay(2000);
}

/**************************************************************************************
   funciton name  ：BG96UDP_receive()
   parameter      ：void
   return value   ：void
   effect         ：
*****************************************************************************************/
int BG96UDPReceive(void) {
//  Serial.println("Receiving the UDP Msg...");
  BG96ClearBuffer();
  //  Serial.println(F("AT+QIRD=3"));
  BG96UART.println(F("AT+QIRD=3"));
  delay(300);
  BG96SerialRead();
  
  if (strstr(msg, "OK|1")){
    digitalWrite(LED, HIGH);
    status = 1;
    } 
  else if (strstr(msg, "OK|0")){
    digitalWrite(LED, LOW);
    status = 0;
  }
//  Serial.println("Status is");
//  Serial.println(status);
  
  return status;
}

/**************************************************************************************
   funciton name  ：BG96UDP_close()
   parameter      ：void
   return value   ：void
   effect         ：
*****************************************************************************************/
void BG96UDPClose(void) {
  Serial.println("Close the UDP Port");
  BG96ClearBuffer();
  Serial.println(F("AT+QICLOSE=3"));
  BG96UART.println(F("AT+QICLOSE=3"));
  BG96SerialRead();
  delay(500);
}

/**************************************************************************************
   funciton name  ：BG96GetPDPAddress()
   parameter      ：void
   return value   ：void
   effect         ：send "AT+CGPADDR"command, show IP address of device
*****************************************************************************************/
void BG96GetPDPAddress(void) {
  BG96ClearBuffer();
  Serial.println(F("AT+CGPADDR"));                                        //Just to see in terminal
  BG96UART.println(F("AT+CGPADDR"));                                      //Request to IP address
  BG96SerialRead();                                                       //See return values in the terminal
  delay(500);
}
/**************************************************************************************
   funciton name  ：BGInitialization()
   parameter      ：void
   return value   ：void
   effect         ：Initialization the NB-IoT module
*****************************************************************************************/
void BG96Initialization() {
  BG96UART.begin(Baudrate);                                     //config module of bc96 baud of uart is 9600bps/s too.
  Serial.begin(Baudrate);                                       //Use Serial Port for debug(config the baud of uart is 9600bps/s)
  BG96ATCommandOK();
  nwscan();
  BG96ATEchoModeOff();
  BG96RequestIMSI();
  BG96GetIMEI();
  BG96ActivateErrorCode();
  BG96OperatorSelection();
  BG96SetDeviceFunctionality();
  BG96NetworkAttach();
  BG96GetSignalQualityReport();
  BG96NetworkRegStatus();
  BG96GetPDPAddress();
}
