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
    oled.display();    
}


void loop() {
    unsigned char len = 0;
    unsigned char buf[8];
    
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
        
        oled.print("Data:"); // text to display
        oled.print("\t");
        oled.display(); 

        for (int i = 0; i < len; i++) { // print the data
            SERIAL_PORT_MONITOR.print(buf[i], HEX);
            SERIAL_PORT_MONITOR.print("\t");
            oled.write(buf[i]); // text to display
            oled.display(); 
        }
        SERIAL_PORT_MONITOR.println();
    }
}

/****************************************************************************************************
    END FILE
*********************************************************************************************************/
