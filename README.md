# in-vehicle-security
## Introduction
This lab uses the kali-linux virtual machine (VM) with Arduino IDE installed.
The learning objective of this lab is for students to get familiar with the concept of secure in-vehicle communication. After finishing the lab, students should be able to gain a first-hand experience on the use of cryptographic techniques for securing in-vehicle communication. We will use the [a can bus shield](https://github.com/Seeed-Studio/Seeed_Arduino_CAN),[a crypto](https://www.arduino.cc/reference/en/libraries/crypto/) and [a adafruit library](https://github.com/adafruit/Adafruit_LiquidCrystal) in this lab.

The testbed consists of three Arduino Uno (Rev3) controllers. Each one is equipped with a CAN-BUS Shield (V2), and they are all connected on a serial bus. This setup simulates an in-vehicle network with three Electronic Control Units (ECUs) communicating using the CAN bus protocol. The ECUs are marked Alice, Bob and Charlie. We assume that Alice and Bob are legitimate ECUs and Charlie was recently introduced by the attacker.

![alt Setup](https://raw.githubusercontent.com/Sulav182/in-vehicle-security/main/img/setup.jpg)
## Hardware Setup

### Components:
- 1 Breadboard
- 3 Arduino Uno
- 3 CAN Bus Shield V2
- 6 Jumper wire cables
- 3 Power supply chord

### Setup
Each arduino is fitted with CAN Bus Shield. The CAN protocol consists of two wires, CAN high and CAN low, for bidirectional data transmission. As can be seen in the picture, CAN high port of the shield is connected together via green, blue and orange wires on the breadboard. Similarly, CAN low port is connected by brown, yellow, and red wire on the breadboard. Since the wires pinned along the vertical lines in a breadboard connect the arduinos, the data can be sent and received. The arduinos can be connected to a laptop or directly to a power cable to turn it on.

### Libraries used
- CAN_BUS_SHIELD
- Crypto


## Adding Code on arduino
To write code on arduino, we use Arduino IDE 2.0.1. It can be downloaded from the official arduino site. If running on VirtualBox, make sure to enable usb controller so that the arduinos can be accessed inside the VirtualBox. To add the usb devices, first connect the arduino to a usb port. Then go to ‘Settings’ in VirtualBox and select the USB tab. Then click the icon with plus sign on to add the arduino. If more than one arduino is connected, be sure to add it as well. Once the setting is complete, start the operating system inside VirtualBox. 

### Objective
Alice and Bob are legitimate users whereas Charlie is not. Alice sends the data and Bob receives it. They both share same encryption keys to encrypt/decrypt the data. Charlie tries to send the data to Bob by guessing the key. There are three scenarios: one where no keys are used(section 1), second where we use poly to hash the data and verify authenticity by using MAC(Message Authentication Code) value, and third where we encrypt data with key and iv. The objective is to observe the data received by Bob from Alice and Charlie in all three scenarios.

### Using Arduino IDE in Virtual Box
- Make sure virtual box in installed
- Click on the kali.ova file and wait for it tp open
- Enter username: kali, password: kali when prompted
- Click on Home folder present in desktop
- Go to Documents -> arduino -> arduino-ide_2.0.1_Linux_64bit -> arduino-ide
- On top of the Arduino IDE, there is a dropdown to select arduinos. If your arduino is connected, it will show up there. If multiple arduinos are connected, all will show up in the dropdown. If you do not see anything listed even after connecting an arduino, check the usb setting mentioned above. 
- For this lab we will use three arduinos. We will start from 'Section 1.' First select one arduino from the dropdown, then copy and paste code from file 'plaintext_send_1'. 
- Click the save button (checkmark) on top of the IDE, then click on upload button(right arrow).
- Select another arduino and repeat the process for 'plaintext_send_2'.
- Select the third arduino and repeat the process for 'plaintext_receive_check.
- To check the output of ‘Serial.print()’ statement, make sure third arduino, with plaintext_receive_check is selected, then click on ‘Tools’ and select ‘Serial Monitor’. On the monitor select the baud rate to be 115200.
- Repeat the process for 'Section 2' and Section 3'

### Files used
#### Section 1
- Plaintext_alice : Sends data via can bus without any encryption or authentication with can id 0x01.
- Plaintext_charlie : Sends data via can bus without any encryption or authentication with can id 0x01.
- Plaintext_bob: Receives data from both of the above code, both with can id 0x01. Cannot differentiate between the sender and attacker.

#### Section 2
- Poly_alice : Sends data via can bus with authentication using poly 1305. Only the sender and receiver has the key and nonce.
- Poly_charlie: Sends the data via the can bus with authentication using poly 1305 as well. But it does not have the original key and nonce, so it uses random value for key and nonce and sends the data.
- Poly_bob : Receives the data from both the above code. Generates MAC itself using key and nonce on the received data. Compares it with MAC of received data. It matches in the first case, but not in the second case. This shows that the data from the second file came from the attacker.

#### Section 3
- CahchaPoly_alice : Sends data via can bus with encryption and authentication using chachapoly protocol. Only the sender and  receiver has the key and iv.
- ChachaPoly_charlie: Sends the data via the can bus with authentication using chachapoly as well. But it does not have the original key and iv, so it uses random value for key and nonce and sends the data.
- ChachaPoly_bob : Receives the data from both the above code. Decrypts the data using key and iv. Compares it with MAC of received data. It matches in the first case, but not in the second case. This shows that the data from the second file came from the attacker.

#### Optional
You can store the key and iv inside the arduino to mimic the setup inside a car. We use EEPROM to store the value inside arduino. To do so, do the following steps:
- Uncomment Block C for Alice and Bob in section 3 and upload the code. This saves the key and iv inside the EEPROM of arduino.
- Comment Block A and Block C, and uncomment Block B and Block D for Alice and Bob in section 3 and upload the code again. Now the arduino uses the saved value of key and iv to encrypt/decrypt the data. 