// Crypto - Version: Latest 


#include <Crypto.h>
#include <ChaChaPoly.h>
#include <EEPROM.h>
// demo: CAN-BUS Shield, send data
// loovee@seeed.cc


#include <SPI.h>

#define CAN_2515
// #define CAN_2518FD

// Set SPI CS Pin according to your hardware

#if defined(SEEED_WIO_TERMINAL) && defined(CAN_2518FD)
// For Wio Terminal w/ MCP2518FD RPi Hat：
// Channel 0 SPI_CS Pin: BCM 8
// Channel 1 SPI_CS Pin: BCM 7
// Interupt Pin: BCM25
const int SPI_CS_PIN  = BCM8;
const int CAN_INT_PIN = BCM25;
#else

// For Arduino MCP2515 Hat:
// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;
const int CAN_INT_PIN = 2;
#endif


#ifdef CAN_2518FD
#include "mcp2518fd_can.h"
mcp2518fd CAN(SPI_CS_PIN); // Set CS pin
#endif

#ifdef CAN_2515
#include "mcp2515_can.h"
mcp2515_can CAN(SPI_CS_PIN); // Set CS pin
#endif
// Block A -----------------------------------------------------
uint8_t key[32] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
          0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
          0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
          0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f};
// iv to use for encryption
uint8_t iv[16]={0x07, 0x00, 0x00, 0x00, 0x40, 0x41, 0x42, 0x43,
      0x44, 0x45, 0x46, 0x47};
//  --------------------------------------------------------------

// Block B ---------------------------------------------------------
// uint8_t key[32];
// uint8_t iv[16];
//  -----------------------------------------------------------------
void setup() {
    SERIAL_PORT_MONITOR.begin(115200);
    while(!Serial){};

    while (CAN_OK != CAN.begin(CAN_500KBPS)) {             // init can bus : baudrate = 500k
        SERIAL_PORT_MONITOR.println("CAN init fail, retry...");
        delay(100);
    }
    SERIAL_PORT_MONITOR.println("CAN init ok!");
    // Block C  -----------------------------------------------------------------
    // storing the key in eeprom
    // for(int i=0; i<32; i++){
    //   EEPROM.write(i, key[i]);
    // }
    // // storing the iv in eeprom
    // for(int i=32; i<48; i++){
    //   EEPROM.write(i, iv[i-32]);
    // }
    //  ------------------------------------------------------------------------------

    // Block D  ----------------------------------------------------------------------
    // reading the key and iv value from eeprom
    // for(int i=0; i<32; i++){
    //   key[i] = EEPROM.read(i);
    //   SERIAL_PORT_MONITOR.print(key[i], HEX);
    // }
    // SERIAL_PORT_MONITOR.println();
    // for(int i=0; i<12; i++){
    //   iv[i] = EEPROM.read(i+32);
    //   SERIAL_PORT_MONITOR.print(iv[i], HEX);
    // }
    // -------------------------------------------------------------------------------------
}
#define MAX_PLAINTEXT_LEN 8
unsigned char stmp[] = {0, 0, 0, 0x01, 0, 0, 0, 0};
// key to use for encryption

ChaChaPoly chachapoly;
// buffer stores encrypted data
byte buffer[MAX_PLAINTEXT_LEN];

// setting authdata
uint8_t authdata[1] = {0x01};
// setting tag; used as MAC address in this case
uint8_t tag[2];
// data to be encrypted
uint8_t plaintext[MAX_PLAINTEXT_LEN] = {0x68, 0x65, 0x6c, 0x6c,0x6f,0x21,0,0};

void loop() {
    // send data:  id = 0x00, standrad frame, data len = 8, stmp: data buf
  // prepopulationg value of buffer to 0xba
  memset(buffer, 0xba, sizeof(buffer));
  //clearing the chachapoly object
  chachapoly.clear();  
  chachapoly.setKey(key, 32);
  chachapoly.setIV(iv, 12);
  // adding auth data of size 1 byte; it can be the value of can id  
  chachapoly.addAuthData(authdata, 1);
  //encrypting 6 bytes of data; adding tag inside last two bytes
  chachapoly.encrypt(buffer, plaintext, 6);
  size_t i;
  // computing the tag value/MAC address in this case
  chachapoly.computeTag(tag, sizeof(tag));
  for(i=0;i<2;i+=1){
    Serial.print(tag[i], HEX);
    // storing tag value in last two bytes
    buffer[6+i] = tag[i];
  };
  Serial.println();
  Serial.println("Done with tag");
  // printing out all the encrypted value with tag
  for(i=0;i<8;i+=1){
    Serial.print(buffer[i], HEX);
  }
  Serial.println();
  Serial.println("Done with buffer");
  
    
  Serial.println();
  // sending message using sendMsgBuf method of CAN
  // manually changing the value of tag
  // buffer[6] = 00000001;
  // buffer[7] = 00000000;
  CAN.sendMsgBuf(0x01, 0, 8, buffer);
  delay(20000);                       // send data per 5000ms
  SERIAL_PORT_MONITOR.println("CAN BUS sendMsgBuf ok!");
}

// END FILE
