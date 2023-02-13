// Crypto - Version: Latest 


#include <Poly1305.h>


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

void setup() {
    SERIAL_PORT_MONITOR.begin(115200);
    while(!Serial){};

    while (CAN_OK != CAN.begin(CAN_500KBPS)) {             // init can bus : baudrate = 500k
        SERIAL_PORT_MONITOR.println("CAN init fail, retry...");
        delay(100);
    }
    SERIAL_PORT_MONITOR.println("CAN init ok!");
}
// data to send through can bus
unsigned char stmp[] = {0, 0, 0, 0x01, 0, 0, 0, 0};
// key value used by poly1305 object to generate MAC along with nonce
// key and nonce are shared with receiver, so it can verify the sender
uint8_t key[16] = {0x12, 0x97, 0x6a, 0x08, 0xc4, 0x42, 0x6d, 0x0c,
                 0xe8, 0xa8, 0x24, 0x07, 0xc4, 0xf4, 0x82, 0x07};
uint8_t nonce[16] = {0x80, 0xf8, 0xc2, 0x0a, 0xa7, 0x12, 0x02, 0xd1,
                 0xe2, 0x91, 0x79, 0xcb, 0xcb, 0x55, 0x5a, 0x57};
Poly1305 poly1305;
// variable to hold the MAC
byte buffer[16];
size_t dataLen = 3; 
void loop() {
    
    // intializing the poly1305 object with key
    poly1305.reset(key);
    // adding the data along with its length in bytes
    poly1305.update(stmp, dataLen);
    // finalizing the MAC with nonce; it is stored in buffer and its length is 4 bytes
    poly1305.finalize(nonce, buffer, 4);
    
    size_t i;
    // printing the value in buffer
    for(i=0;i<8;i+=1){
      Serial.print(buffer[i], HEX);
    }
    
    Serial.println();
    // adding the MAC as last four bytes of data, stmp
    for(i=0;i<4;i+=1){
      stmp[i+4] = buffer[i];
      //Serial.print(stmp[i+8], HEX);
      
    }
    // printing all the values of stmp; first four bytes is data, last four bytes is MAC
    for(i=0;i<8;i+=1){
      Serial.print(stmp[i], HEX);
    }
    Serial.println();
    // send data:  id = 0x01, standrad frame, data len = 8, stmp: data buf
    CAN.sendMsgBuf(0x01, 0, 8, stmp);
    delay(20000);                       // send data per 5000ms
    SERIAL_PORT_MONITOR.println("CAN BUS sendMsgBuf ok!");
}

// END FILE
