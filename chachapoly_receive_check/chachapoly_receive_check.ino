// Adafruit SSD1306 - Version: Latest 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

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
#include <Crypto.h>
#include <ChaChaPoly.h>
#include <string.h>
#if defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif

#define MAX_PLAINTEXT_LEN 8
ChaChaPoly chachapoly;
// uint8_t key[32] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
//           0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
//           0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
//           0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f};
// uint8_t iv[16]={0x07, 0x00, 0x00, 0x00, 0x40, 0x41, 0x42, 0x43,
//       0x44, 0x45, 0x46, 0x47};
uint8_t key[32];
uint8_t iv[12];
void setup() {
    SERIAL_PORT_MONITOR.begin(115200);

    while (CAN_OK != CAN.begin(CAN_500KBPS)) {             // init can bus : baudrate = 500k
        SERIAL_PORT_MONITOR.println("CAN init fail, retry...");
        delay(100);
    }
    SERIAL_PORT_MONITOR.println("CAN init ok!");
    //oled display setup
    // initialize OLED display with address 0x3C for 128x64
    if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.println(F("SSD1306 allocation failed"));
      while (true);
    }
  
    delay(2000);         // wait for initializing
    oled.clearDisplay(); // clear display
  
    oled.setTextSize(1);          // text size
    oled.setTextColor(WHITE);     // text color
    oled.setCursor(0, 0);        // position to display
    oled.println("Display ok!"); // text to display
    oled.display();               // show on OLED
    // storing the key in eeprom
    // for(int i=0; i<32; i++){
    //   EEPROM.write(i, key[i]);
    // }
    // // storing the iv in eeprom
    // for(int i=32; i<48; i++){
    //   EEPROM.write(i, iv[i-32]);
    // }
    for(int i=0; i<32; i++){
      key[i] = EEPROM.read(i);
      SERIAL_PORT_MONITOR.print(key[i], HEX);
    }
    SERIAL_PORT_MONITOR.println();
    for(int i=0; i<12; i++){
      iv[i] = EEPROM.read(i+32);
      SERIAL_PORT_MONITOR.print(iv[i], HEX);
    }
}


uint8_t authdata[1] = {0x01};
uint8_t after_tag[2];
void loop() {
    unsigned char len = 0;
    unsigned char buf[8];
    unsigned char poly_buf[8];
    
    //setting the display configurations
    delay(2000);         // wait for initializing
    oled.clearDisplay(); // clear display
  
    oled.setTextSize(1);          // text size
    oled.setTextColor(WHITE);     // text color
    oled.setCursor(0, 0);   
    
    if (CAN_MSGAVAIL == CAN.checkReceive()) {         // check if data coming
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

        unsigned long canId = CAN.getCanId();
        
        SERIAL_PORT_MONITOR.println("-----------------------------");
        SERIAL_PORT_MONITOR.print("Get data from ID: 0x");
        SERIAL_PORT_MONITOR.println(canId, HEX);
        //printing the can id in the display
        oled.print("CanId:"); // text to display
        oled.print("\t");
        oled.print(canId, HEX);
        oled.println();
        oled.display(); 
        SERIAL_PORT_MONITOR.println(len, HEX);
        //len = ;
        long int t1 = millis();
        // Setting the key and iv value
        chachapoly.clear();
        chachapoly.setKey(key, 32);
        chachapoly.setIV(iv, 12);
        
        
        byte decrypt_buffer[MAX_PLAINTEXT_LEN];
        memset(decrypt_buffer, 0xba, MAX_PLAINTEXT_LEN);
        chachapoly.addAuthData(authdata, 1);
        // decrypting the received value buf; decrypting the first 6 bytes 
        chachapoly.decrypt(decrypt_buffer, buf, 6);
        long int t2 = millis();
        Serial.print("Time taken by the task: "); Serial.print(t2-t1); Serial.println(" milliseconds");

        // checking the tag value
        after_tag[0] = buf[6];
        after_tag[1] = buf[7];
        
        if (!chachapoly.checkTag(after_tag, sizeof(after_tag))) {
          Serial.print("tag did not check ... ");
          oled.print("Tag did not match"); // text to display
          oled.display(); 
          return false;
        };
        Serial.println();
        oled.print("Data:"); // text to display
        oled.print("\t");
        oled.display(); 
        for(int i=0;i<6;i+=1){
          Serial.write(decrypt_buffer[i]);
          Serial.print(decrypt_buffer[i],HEX);
          oled.write(decrypt_buffer[i]); // text to display
          oled.display(); 
        };
        
        SERIAL_PORT_MONITOR.println("-----------------------------");
        SERIAL_PORT_MONITOR.print("Comparing MAC values");
        SERIAL_PORT_MONITOR.println();
        //checking the tag values
        for (int i = 0; i < 2; i++) { // print the data
            SERIAL_PORT_MONITOR.print(buf[i+6], HEX);
            SERIAL_PORT_MONITOR.print("\t");
            SERIAL_PORT_MONITOR.print(after_tag[i], HEX);
            SERIAL_PORT_MONITOR.print("\t");
        }
        SERIAL_PORT_MONITOR.println();
        SERIAL_PORT_MONITOR.println("-----------------------------");
        
        //////////////////////////////////////
        oled.println();
        oled.print("Enc:"); // text to display
        oled.print("\t");
        oled.display();
        for (int i = 0; i < len; i++) { // print the data
            SERIAL_PORT_MONITOR.print(buf[i], HEX);
            SERIAL_PORT_MONITOR.print("\t");
            oled.print(buf[i], HEX); // text to display
            oled.display(); 
        }
        SERIAL_PORT_MONITOR.println();
    }
}

/****************************************************************************************************
    END FILE
*********************************************************************************************************/
