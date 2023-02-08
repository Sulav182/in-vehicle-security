# in-vehicle-security
Introduction
This lab uses the kali-linux virtual machine (VM) with Ardiono IDE installed.
The learning objective of this lab is for students to get familiar with the concept of secure in-vehicle communication. After finishing the lab, students should be able to gain a first-hand experience on the use of cryptographic techniques for securing in-vehicle communication. We will use the … library (link) in this lab.
The testbed consists of three Arduino Uno (Rev3) controllers. Each one is equipped with a CAN-BUS Shield (V2), and they are all connected on a serial bus. This setup simulates an in-vehicle network with three Electronic Control Units (ECUs) communicating using the CAN bus protocol. The ECUs are marked Alice, Bob and Charlie. Their’s CAN IDs are “…” for Alice, “…” for Bob, and “…” for Charlie. We assume that Alice and Bob are legitimate ECUs and Charlie was recently introduced by the attacker.

Hardware Setup

Components:
1 Breadboard
3 Arduino Uno
3 CAN Bus Shield V2
6 Jumper wire cables
3 Power supply chord

Setup
Each arduino is fitted with CAN Bus Shield. The CAN protocol consists of two wires, CAN high and CAN low, for bidirectional data transmission. As can be seen in the picture, CAN high port of the shield is connected together via green, blue and orange wires on the breadboard. Similarly, CAN low port is connected by brown, yellow, and red wire on the breadboard. The arduinos can be connected to a laptop or directly to a power cable to turn it on.

Adding Code on arduino
To write code on arduino, we use Arduino IDE 2.0.1. It can be downloaded from the official arduino site. If running on VirtualBox, make sure to enable usb controller so that the arduinos can be accessed inside the VirtualBox. To add the usb devices, first connect the arduino to a usb port. Then go to ‘Settings’ in VirtualBox and select the USB tab. Then clicking the icon with plus sign on to add the arduino. If more than one arduino is connected, be sure to add it as well. Once the setting is complete, start the operating system inside VirtualBox. Then open Arduino IDE. On top of the arduino IDE select the arduino where you want to add the code. After selecting the arduino copy and paste the code in the code editor. Save the code by clicking the checkmark button on the top-left of the IDE. After saving, click on the upload button next to it to upload the code to arduino. To see the output of ‘Serial.print()’ statement, click on ‘Tools’ and select ‘Serial Monitor’. On the monitor select the baud rate to be 115200.

Libraries used
CAN_BUS_SHIELD
Crypto

Files used
Plaintext_send_1 : Sends data via can bus without any encryption or authentication with can id 0x01.
Plaintext_send_2 : Sends data via can bus without any encryption or authentication with can id 0x01.
Plaintext_receive_check: Receives data from both of the above code, both with can id 0x01. Cannot differentiate between the sender and attacker

Poly_send_1 : Sends data via can bus with authentication using poly 1305. Only the sender and receiver has the key and nonce.
Poly_send_2: Sends the data via the can bus with authentication using poly 1305 as well. But it does not have the original key and nonce, so it uses random value for key and nonce and sends the data.
Poly_receive_check : Receives the data from both the above code. Generates MAC itself using key and nonce on the received data. Compares it with MAC of received data. It matches in the first case, but not in the second case. This shows that the data from the second file came from the attacker.

CahchaPoly_send_1 : Sends data via can bus with encryption and authentication using chachapoly protocol. Only the sender and receiver has the key and iv.
ChachaPoly_send_2: Sends the data via the can bus with authentication using chachapoly as well. But it does not have the original key and iv, so it uses random value for key and nonce and sends the data.
ChachaPoly_receive_check : Receives the data from both the above code. Decrypts the data using key and iv. Compares it with MAC of received data. It matches in the first case, but not in the second case. This shows that the data from the second file came from the attacker.

