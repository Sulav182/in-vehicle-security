// demo: CAN-BUS Shield, receive data with check mode
// send data coming to fast, such as less than 10ms, you can use this way
// loovee, 2014-6-13
#include <SPI.h>
#include <Crypto.h>
#include <Poly1305.h>
#include <string.h>
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

    while (CAN_OK != CAN.begin(CAN_500KBPS)) {             // init can bus : baudrate = 500k
        SERIAL_PORT_MONITOR.println("CAN init fail, retry...");
        delay(100);
    }
    SERIAL_PORT_MONITOR.println("CAN init ok!");
}
Poly1305 poly1305;
// setting value for key and nonce; only the verified sender has the same key and nonce value
uint8_t key[16] = {0x12, 0x97, 0x6a, 0x08, 0xc4, 0x42, 0x6d, 0x0c,
                 0xe8, 0xa8, 0x24, 0x07, 0xc4, 0xf4, 0x82, 0x07};
uint8_t nonce[16]    = {0x80, 0xf8, 0xc2, 0x0a, 0xa7, 0x12, 0x02, 0xd1,
                 0xe2, 0x91, 0x79, 0xcb, 0xcb, 0x55, 0x5a, 0x57};
size_t dataLen = 3;
void loop() {
    unsigned char len = 0;
    unsigned char buf[8];
    unsigned char poly_buf[8];

    if (CAN_MSGAVAIL == CAN.checkReceive()) {         // check if data coming
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

        unsigned long canId = CAN.getCanId();

        SERIAL_PORT_MONITOR.println("-----------------------------");
        SERIAL_PORT_MONITOR.print("Get data from ID: 0x");
        SERIAL_PORT_MONITOR.println(canId, HEX);
        SERIAL_PORT_MONITOR.println(len, HEX);
        //len = ;
        //Comparing the mac address
        poly1305.reset(key);
        // adding the received value, buf, along with data length
        poly1305.update(buf, dataLen);
        // getting the MAC value which will be stored in poly_buf
        // since the MAC values should be same when same key and nonce are used, we can check if it from verified sender
        // if the values are different, unverified sended is trying to send the data
        
        poly1305.finalize(nonce, poly_buf, 4);
        SERIAL_PORT_MONITOR.println("-----------------------------");
        SERIAL_PORT_MONITOR.print("Comparing MAC values");
        SERIAL_PORT_MONITOR.println();
        // comparing the MAC values, first four bytes of poly_buf should match last four bytes of buf
        for (int i = 0; i < 4; i++) { // print the data
            SERIAL_PORT_MONITOR.print(buf[i+4], HEX);
            SERIAL_PORT_MONITOR.print("\t");
            SERIAL_PORT_MONITOR.print(poly_buf[i], HEX);
            SERIAL_PORT_MONITOR.print("\t");
        }
        SERIAL_PORT_MONITOR.println();
        SERIAL_PORT_MONITOR.println("-----------------------------");
        
        //////////////////////////////////////
        
        for (int i = 0; i < len; i++) { // print the data
            SERIAL_PORT_MONITOR.print(buf[i], HEX);
            SERIAL_PORT_MONITOR.print("\t");
        }
        SERIAL_PORT_MONITOR.println();
    }
}

/****************************************************************************************************
    END FILE
*********************************************************************************************************/
