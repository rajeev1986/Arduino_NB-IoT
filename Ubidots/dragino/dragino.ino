/**************************************************************************************
 * Authors        ：Rajeev Piyare
 * Device         ：Dragino Arduino Uno Quectel BG96 NB-IoT Shield 
 * Documents      :(1)->https://www.robotshop.com/media/files/pdf2/nb-iot_shield_-_wiki_for_dragino_project.pdf
                   (2)->http://www.dragino.com/downloads/index.php?dir=NB-IoT/BG96/&file=Quectel_BG96_AT_Commands_Manual_V2.0.pdf
                   (3)->http://www.dragino.com/downloads/index.php?dir=NB-IoT/BG96/&file=Quectel_BG96_TCP%28IP%29_AT_Commands_Manual_V1.0.pdf
                   (4)->http://help.ubidots.com/developer-guides/sendretrieve-data-from-ubidots-over-tcp-or-udp 
 * date           ：26/02/2019
*****************************************************************************************/
#include "at_commands.h"
#include "sensor.h"
/**********************Device Configuration**********************/
const char  PinCode[]         = "0000";                               //SIM Pin Code
/**********************TIM UDP Server Configuration**************/
const char TIM_APNAddress[]    = "nbiot.tids.tim.it";                 //APN address of TIM Network
const char TIM_SERVER[]        = "192.168.200.225";                   //Server IP address of TIM Network
const int  TIM_PORT            = 5681;                                //Server port of TIM Network
const int  DEVICE_PORT         = 5685;                                //Local Device port                   
const int  CONNECT_ID          = 3;                                   //The socket service index for UDP  
/****************************************************************/

/**********************Ubidot Configuration**********************/
const char UBIDOTS_SERVER[] = "169.55.61.243";                        // industrial.api.ubidots.com (converted IP Address)
const int UBIDOTS_PORT      =  9012;                                  // port address as per UBIDOTS API docs
String USER_AGENT           = "DraginoNB/1.1";                        // Examples: Arduino/1.0
String TOKEN                = "BBFF-xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";  // Assign your user Ubidots TOKEN
String DEVICE_NAME          = "draginonb1";                           // Assign the unique device API label
String VARIABLE_LABEL_1     = "temperature:";                         //variable to be monitored
String VARIABLE_LABEL_2     = ":lamp";
String VARIABLE_LABEL_3     = "status:";
String data_upload = "";
const char TCPSERVER[] = "50.23.124.66";                              // translate.ubidots.com (converted IP Address)
/****************************************************************/

long start = 0;
long start2 = 0;
int lamp_status = 0;
int prev_status = 0;
int curr_status = 0;

void setup() {  
  pinMode(LED, OUTPUT);      
  BG96Initialization();                                                                 //Initilaziton for Module
  BG96DefineAPNAddr(TIM_APNAddress);
  BG96OpenUDPSocket(CONNECT_ID, DEVICE_PORT);                                           
}

void loop() {

  String _packet = "";
  int data0;
  data0 = random(10, 40);
  
  TemperatureValue = ReadTemperature(TemperaturePin);

  /*** Retrieve lamp status ***/
  if (millis() - start > 10000)  // 10 sec
  {
    start = millis();

    _packet = USER_AGENT + "|LV|" + TOKEN + "|" + DEVICE_NAME + VARIABLE_LABEL_2 + "|end";
    Serial.println("Retrive:" + _packet);
    BG96SendUDPData(_packet, CONNECT_ID, TCPSERVER, UBIDOTS_PORT);

    delay(300);
    lamp_status = BG96UDPReceive();

    data_upload = VARIABLE_LABEL_3 + String(lamp_status);
    _packet = USER_AGENT + "|POST|" + TOKEN + "|" + DEVICE_NAME + "=>" + data_upload + "|end";
    Serial.println("Lamp status changed...");
    BG96SendUDPData(_packet, CONNECT_ID, UBIDOTS_SERVER, UBIDOTS_PORT);
  }

    /*** Post data to platform ***/
  if (millis() - start2 > 60000)  // 60
  {
    start2 = millis();

  /* Send data to ubidot dashboard */
//  data_upload = VARIABLE_LABEL_1 + String(data0) + "," + VARIABLE_LABEL_3 + String(lamp_status);
  data_upload = VARIABLE_LABEL_1 + String(data0);
  _packet = USER_AGENT + "|POST|" + TOKEN + "|" + DEVICE_NAME + "=>" + data_upload + "|end";
//  Serial.println("post:" + _packet);
  Serial.println("Posting to Ubidots...");
  BG96SendUDPData(_packet, CONNECT_ID, UBIDOTS_SERVER, UBIDOTS_PORT);

  }                   
}
