
// Crypto - Version: Latest 
#include <AES.h>
#include <AuthenticatedCipher.h>
#include <BLAKE2b.h>
#include <BLAKE2s.h>
#include <BigNumberUtil.h>
#include <BlockCipher.h>
#include <CTR.h>
#include <ChaCha.h>
#include <ChaChaPoly.h>
#include <Cipher.h>
#include <Crypto.h>
#include <Curve25519.h>
#include <EAX.h>
#include <Ed25519.h>
#include <GCM.h>
#include <GF128.h>
#include <GHASH.h>
#include <Hash.h>
#include <KeccakCore.h>
#include <NoiseSource.h>
#include <OMAC.h>
#include <P521.h>
#include <Poly1305.h>
#include <RNG.h>
#include <SHA256.h>
#include <SHA3.h>
#include <SHA512.h>
#include <SHAKE.h>
#include <XOF.h>
#include <XTS.h>

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
// stmp:data to send; can send only eight bytes of data in can bus 
unsigned char stmp[] = {0, 0, 0, 0, 0, 0, 0, 0};
void loop() {
    // send data:  id = 0x01, standrad frame, data len = 8, stmp: data buf
    CAN.sendMsgBuf(0x01, 0, 8, stmp);
    delay(5000);                       // send data per 5000ms
    SERIAL_PORT_MONITOR.println("CAN BUS sendMsgBuf ok!");
}

// END FILE
