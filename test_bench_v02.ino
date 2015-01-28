//-----------------------------------------------------------------------------------//
// Project Name 	: TEST_BENCH FOR IC
// File Name	        : main_IC.c
// Version              : v02 
// RUN[Y/N]             : No 
// Creation Date 	: 01.01.2014
// Modification Date    : 26.01.2014
// 
// Writer		: Ph. Bovey - BINJ  
//
// Description 		: this program must allow to define if the IC CDP68HC68P1 
//                        is functional or not 
// 
// Remark		: 
// link                 : link for the IC CDP68HC68P1 
//                       http://www.alldatasheet.fr/datasheet-pdf/pdf/87734/INTERSIL/CDP68HC68P1E.html
//                        link for using the ARDUINO Board 
//                       http://www.arduino.cc 
//                        link concerning the SPI protocol
//                       http://www.didel.com/diduino/SerieSPI.pdf

//                       https://github.com/philouxy/arduino_IC68HC68P1.git
//----------------------------------------------------------------------------------//
//--- library used ---//
#include <SPI.h>

//--- definition of constant ---// 
const char ID1_b    = 0x0;                       // corresponding at bit 7         
const char ID0_b    = 0x0;                       // corresponding at bit 6

const char TB_MODE[3] = {0, 1, 2};               // table to select the mode to configure the PORTs (input / output)
                                                 // => 0  
                                                 // => 1 : On ARDUINO allows to configure the DA7 to DA0 in input => On  
                                                 // => 2 : On ARDUINO allows to configure the DA7 to DA0 in ouput => On  

const char TB_MESSAGES[9] = {0b11111111,         // this table allows to see different configurations for I/O going 
                             0b01111111,         // of all ports which are in high level to all ports wich are low level
                             0b00111111,         // D7 to D0 
                             0b00011111,
                             0b00001111,
                             0b00000111,          
                             0b00000011,           
                             0b00000001,
                             0b00000000};        // all ports should be at '0' (state)
                             
const char TB_CONFIG_PORTS[2] = {0xFF,            // the ports D7 to D0 configured in OUTPUT on the IC CDP68HC68P1 |=>  0xFF => D0 = 1 / D1 = 1 / D2 = 1 / D3 = 1 / D4 = 1 / D5 = 1 / D6 = 1 / D7 = 1 
                                 0x00};           //                               in INPUT on the IC CDP68HC68P1  |=>  0x00 => D0 = 0 / D1 = 0 / D2 = 0 / D3 = 0 / D4 = 0 / D5 = 0 / D6 = 0 / D7 = 0 
                                                 
//--- definition of PIN used on the board ---// 
#define   SS         10      // PIN 10 => Chip enabale 
#define   ID1         9      // PIN 9  => ID1 
#define   ID0         8      // PIN 8  => ID0
#define   D0         A5      // PIN 0  => connecting at the port D0-7 of IC CDP68HC68P1
#define   D1         A4      // PIN 1
#define   D2          2      // PIN 2
#define   D3          3      // PIN 3
#define   D4          4      // PIN 4
#define   D5          5      // PIN 5
#define   D6          6      // PIN 6
#define   D7          7      // PIN 7

//--- definition of numeric constantes ---//
#define   NB_BITS       8                    //
#define   SAUT_LIGNE   Serial.println("")    // 

//--- definition of global variable ---//
          
//--- prototype definition ---// 
void Init_Chip_SPI();
void Init_Ports(char mode); 
void Write_on_Chip(char conf, char msg, char statu); 
void Read_on_Chip(char conf, char msg); 
void Read_Arduino_Ports(char *pt_tb);
void Message_Console(char select, char conf, char msg); 
void Compare_S_R(char val_s, char val_r); 
//void 

//--- principal programm ---//
void setup()
{
  //--- initialisation of comunication ---//
  Serial.begin(2400);       // configuration of Setial communication with ARDUINO interface on the PC   
  
  //--- message on the terminal ---// 
  Serial.println("--- TEST BENCH FOR IC CDP68HC68P1  ---"); 
  Serial.println("--- VERSION 2 - 2014/2015  ---");   
  SAUT_LIGNE; 
  
  Init_Ports(TB_MODE[0]);   // configuration of ports using on ARDUINO Board (INPUT / OUTPUT / bidirectionnal) 
  Init_Chip_SPI();          // configuration of SPI communication with IC CDP68HC68P1 
}

void loop()
{
  //--- variables declaration ---// 
  char i, y;   
  char val_test = 0x00;  
  char byte_conf_IC_PORT = 0x00;   // byte to configure the IC CDP68HC68P1 (PORTS) 
  char byte_conf_IC_DATA = 0x00;   // byte to configure the IC CDP68HC68P1 (DATA)
  char byte_conf_IO = 0x00;        // byte to configure the input/ouput 
  char byte_msg;                   // byte inlcude the message   
  char mask_config_byte = 0xFF;    // byte using like mask when configuration of byte  
  char RS   = 0x01;                // selection of datas or pin direction
                                   // 1 -> pin direction | 0 -> datas
  char _RW  = 0x01;                // writing or reading 
                                   // 1 -> writing | 0 -> reading 
  char DF   = 0x00;                // Data Format  -> all bits will be change of the register selected 
  char CM   = 0x00;                // Compare Function => all matche (see the page 6 of datasheet)
  char tb_ports[NB_BITS];          // table which will be used for the recording or the reading of ports connected at the IC CDP68HC68P1 
  
  //--- Port configuration on the ARDUINO : INPUT ---// 
  Init_Ports(TB_MODE[1]);   // configuration of ports using on ARDUINO Board (INPUT / OUTPUT / bidirectionnal)
  
  //--- configuration first byte to configure the I/O (PORT) ---// 
  byte_conf_IC_PORT = ((ID1_b << 7) | (ID0_b << 6) | (RS << 5) | (_RW << 4) | (DF << 2) | CM);    // conf = 0b00110000 = 0x30
  
  //--- configuration first byte to configure the I/O (DATA) ---//
  mask_config_byte = ~(RS << 5);    
  byte_conf_IC_DATA = byte_conf_IC_PORT & mask_config_byte;                                       // conf = 0b00010000 = 0x10  
  
  //--- configuration I/O on the IC CDP68HC68P1 ---//
  byte_conf_IO = TB_CONFIG_PORTS[0];                                                              // all in OUTPUT => 0xFF 
  
  //--- messages to display on the terminal (CONFIGURATION - PORTS) ---//
  Message_Console(0, byte_conf_IC_PORT, byte_conf_IO); 
  Message_Console(1, byte_conf_IC_DATA, 0); 
  
  //--- transfert of configuration and the selection of port to send at the IC CDP68HC68P1 ---//  
  Write_on_Chip(byte_conf_IC_PORT, byte_conf_IO, 1);                                             
 
  //--- testing to send 4 different message ---// 
  for(i = 0; i < 9; i++)
  {
    Serial.print("Sending of message : ");
    Serial.println(i + 1, DEC);  

    //--- transfert of configuration and the selection of port to send at the IC CDP68HC68P1 ---//
    Write_on_Chip(byte_conf_IC_DATA, TB_MESSAGES[i], 2);
    
    delay(10); 
    
    //--- messages to display on the terminal (CONFIGURATION - MESSAGE) ---//
    Message_Console(2, 0, TB_MESSAGES[i]);
   
    //--- reset of variable ---//
    val_test = 0x00; 
    
    //--- reading on the ARDUINO Port ---//
    Read_Arduino_Ports(tb_ports); 
    Serial.print("=> reading on the port ARUDINO P7 à P0: 0b");
                                    
    for(y = NB_BITS; y > 0; y--)
    {
      Serial.print(tb_ports[y - 1], DEC);
      val_test = val_test | (tb_ports[y - 1] << (y -1)); 
    }
    
    Serial.print("  0x");
    Serial.print(val_test, HEX);  
  
    SAUT_LIGNE; 
    SAUT_LIGNE;  
  
    //Compare_S_R(TB_MESSAGES[i], val_test); 
  
    SAUT_LIGNE; 
  }    
}



//----------------------------------------------------------------------------------//
//--- function name     : Init_Ports
//--- entrée - sortie 	: mode / - / 
//--- Creation Date     : 01.01.2015
//--- Modification Date : 26.01.2015
//--- Version           : V01
//--- description 	: ports configuration on the developping board  
//--- remarque 		: 
//----------------------------------------------------------------------------------//
void Init_Ports(char mode)
{
  switch (mode)
  {
    //--- ports declaration to active the IC CDP68HC68P1 ---// 
    case 0: 
      pinMode(SS, OUTPUT);          // connected physically at the Chip enable of IC CDP68HC68P1
      pinMode(ID1, OUTPUT);          // connected physically at the ID of IC CDP68HC68P1
      pinMode(ID0, OUTPUT);          // connected physically at the ID of IC CDP68HC68P1  
    break; 
    
    //--- ports declaration using to control value on the ports of the IC CDP68HC68P1 ---//
    case 1:
      pinMode(D0, INPUT);
      pinMode(D1, INPUT);
      pinMode(D2, INPUT);
      pinMode(D3, INPUT);
      pinMode(D4, INPUT);
      pinMode(D5, INPUT);
      pinMode(D6, INPUT);
      pinMode(D7, INPUT);
      
      /*pinMode(D0, INPUT_PULLUP);
      pinMode(D1, INPUT_PULLUP);
      pinMode(D2, INPUT_PULLUP);
      pinMode(D3, INPUT_PULLUP);
      pinMode(D4, INPUT_PULLUP);
      pinMode(D5, INPUT_PULLUP);
      pinMode(D6, INPUT_PULLUP);
      pinMode(D7, INPUT_PULLUP);*/
      
    break; 
    
    //--- ports declarattion using to wriiting on the ports of the IC CDP68HC68P1 ---//
    case 2:
      Serial.println("=> port configured in OUTPUT"); 
      pinMode(D0, OUTPUT);
      pinMode(D1, OUTPUT);
      pinMode(D2, OUTPUT);
      pinMode(D3, OUTPUT);
      pinMode(D4, OUTPUT);
      pinMode(D5, OUTPUT);
      pinMode(D6, OUTPUT);
      pinMode(D7, OUTPUT);
    break; 
    
    default: 
    break; 
  }
}


//----------------------------------------------------------------------------------//
//--- function name     : Init_Chip
//--- entrée - sortie 	: - / - 
//--- Creation Date     : 10.12.2014
//--- Modification Date : 24.01.2015
//--- Version           : V01
//--- description 	: initialisation bus SPI with the Chip IC CDP68HC68
//--- remark	        : http://arduino.cc/en/Reference/SPI
//----------------------------------------------------------------------------------//
void Init_Chip_SPI()
{
  //--- activation of adress on the IC CDP68HC68P1 ---//
  digitalWrite(ID1, LOW);
  digitalWrite(ID0, LOW);
  
  //--- activation SPI Bus ---//
  SPI.begin();     // initialisation of SPI BUS (enable) 
                   // using PIN 11 MOSI 
                   // using PIN 12 MISO
                   // using PIN 13 SCK 

  //--- configuration of direction sending (MSB --> LSB) ---//
  SPI.setBitOrder(MSBFIRST); 

  //--- configuration of side (rising or ebbing) on SCK signal ---//
  SPI.setDataMode(SPI_MODE1);      //  rising edge - CPOL 
                                   // activation of start SCK - CPHA 
  
  //--- configuration of frequency (SCK) ---//
  SPI.setClockDivider(SPI_CLOCK_DIV16);          // division by 8 the clock => 16M / 8M = 2MHz
                                                // see the page 3 of datasheet F_SCK < 2.1MHz
}



//----------------------------------------------------------------------------------//
//--- function name     : Write_on_Chip
//--- entrée - sortie 	: conf, msg / - 
//--- Creation Date     : 27.12.2014 
//--- Modification Date : 26.01.2014
//--- Version           : V01
//--- description 	: writting a message (2 bytes) on the SPI Bus  
//--- remarque 		:  
//----------------------------------------------------------------------------------//
void Write_on_Chip(char conf, char msg, char statu)
{
  //--- information message (just for the debbug) ---// 
  if(statu == 1)
  {
    Serial.println("=> WRITING CONFG ON ON THE IC !!! ");
    SAUT_LIGNE;
  }
  else if(statu == 2)  
    Serial.println("=> WRITING DATA ON THE IC !!! ");
  else 
  {
    Serial.println("=> ERROR !!! ");
    SAUT_LIGNE;
  }
  
  //--- activation of chip enable (low state) ---// 
  digitalWrite(SS, LOW); 

  //--- send the message on the SPI Bus ---//
  SPI.transfer(conf); 
  SPI.transfer(msg); 
  
  //--- disable of chip enable (high state) ---// 
  digitalWrite(SS, HIGH); 
}



//----------------------------------------------------------------------------------//
//--- function name     : Read_on_Chip
//--- entrée - sortie 	: conf, msg / - 
//--- Creation Date     : 27.12.2014
//--- Modification Date : 
//--- Version           : V01
//--- description 	: read the information come from the IC CDP68HC68P1 
//--- remarque 		: 
//----------------------------------------------------------------------------------//
void Read_on_Chip(char conf, char msg)
{
  SPI.transfer(conf); 
  SPI.transfer(msg); 
}


//----------------------------------------------------------------------------------//
//--- function name     : Read_Arduino_Ports
//--- entrée - sortie 	: - / - / char *pt_tb
//--- Creation Date     : 31.12.2014
//--- Modification Date : 20.01.2015
//--- Version           : V01
//--- description 	: Send a message 
//--- remarque 		: 
//----------------------------------------------------------------------------------//
void Read_Arduino_Ports(char *pt_tb)
{
  //--- variable declaration ---// 
  //char i; 
  
  //Serial.print("Verification of table initialisation : ob"); 
  
  //--- initialisation of table ---//
  /*for(i = 0; i < NB_BITS; i++)
    pt_tb[i] = 0;*/
  
  //{
    
    //Serial.print(pt_tb[i], BIN);  
  //}
  //Serial.println(""); 
    
  //--- reading of port ---//
  pt_tb[0] = digitalRead(D0);
  pt_tb[1] = digitalRead(D1);
  pt_tb[2] = digitalRead(D2);
  pt_tb[3] = digitalRead(D3);
  pt_tb[4] = digitalRead(D4);
  pt_tb[5] = digitalRead(D5);
  pt_tb[6] = digitalRead(D6);
  pt_tb[7] = digitalRead(D7); 
}



//----------------------------------------------------------------------------------//
//--- function name     : Message_Console
//--- entrée - sortie 	: select, conf, msg / - / 
//--- Creation Date     : 08.01.2015
//--- Modification Date : 26.01.2015
//--- Version           : V01
//--- description 	: function container the message to display on the monitor PC  
//--- remarque 		: 
//----------------------------------------------------------------------------------//
void Message_Console(char select, char conf, char msg)
{
  switch(select)
  {
    case 0:
       Serial.println("--- Configuration I/O for IC CDP68HC68P1 in OUTPUT ---");
       Serial.print("=> Configuration Byte for PORTS : 0x");  
       Serial.println(conf, HEX);
       Serial.print("=> Configuration port I/O : 0x"); 
       Serial.println(msg, HEX);
    break; 
    
    case 1:
      Serial.print("=> Configuration Byte for DATAS : 0x");
      Serial.println(conf, HEX);
      SAUT_LIGNE;   
    break; 
   
    case 2: 
      Serial.print("=> Values to send on the port I/O of IC (D7 to D0): 0x"); 
      Serial.println(msg, HEX);
    break;   
  }
}


//----------------------------------------------------------------------------------//
//--- function name     : Compare_S_R
//--- entrée - sortie 	: - / - / 
//--- Creation Date     : 08.01.2015
//--- Modification Date : xx.01.2015
//--- Version           : V01
//--- description 	: compare if two values are identical or not 
//--- remarque 		: 
//----------------------------------------------------------------------------------//
void Compare_S_R(char val_s, char val_r)
{
  Serial.print("=> sent value : 0b"); 
  Serial.println(val_s, BIN); 
  Serial.print("=> receveid value : 0b"); 
  Serial.println(val_r, BIN); 
  
  if(val_s == val_r)
    Serial.println("=> After comparison : the values are identical"); 
  else 
    Serial.println("=> After comparison : the values are not identical"); 
  
  SAUT_LIGNE; 
}
