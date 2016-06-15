// rf95_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf95_server
// Tested with Anarduino MiniWirelessLoRa, Rocket Scream Mini Ultra Pro with the RFM95W 

#include <SPI.h>
#include <RH_RF95.h>

////  TP-IoT
const int device_address = 3;  //  My own address.
const int gateway_address = 1;  //  Gateway address.

// Singleton instance of the radio driver
RH_RF95 rf95;
//RH_RF95 driver(5, 2); // Rocket Scream Mini Ultra Pro with the RFM95W

// Need this on Arduino Zero with SerialUSB port (eg RocketScream Mini Ultra Pro)
//#define Serial SerialUSB

extern int testRF95; ////

void setup() 
{
testRF95 = 1; ////
  // Rocket Scream Mini Ultra Pro with the RFM95W only:
  // Ensure serial flash is not interfering with radio communication on SPI bus
//  pinMode(4, OUTPUT);
//  digitalWrite(4, HIGH);  
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init())
    Serial.println("init failed");    
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
//  driver.setTxPower(23, false);
  // If you are using Modtronix inAir4 or inAir9,or any other module which uses the
  // transmitter RFO pins and not the PA_BOOST pins
  // then you can configure the power transmitter power for -1 to 14 dBm and with useRFO true. 
  // Failure to do that will result in extremely low transmit powers.
//  driver.setTxPower(14, true);

  ////  TP-IoT
  rf95.setThisAddress(device_address);
  rf95.setHeaderFrom(device_address);
  rf95.setHeaderTo(gateway_address);
  rf95.setHeaderId(0);  //  TP-IoT: Byte 3 of header is the message counter.
}

void loop()
{
  // Send a message to rf95_server
  // dst, src, packnum, length, data, retry
  //uint8_t data[] = "1|2|3|4";
  uint8_t data[] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x00 };
  Serial.print("Sending to rf95_server: ");
  Serial.print(sizeof(data));
  Serial.print(" / ");
  Serial.println((char *) data);
  rf95.setHeaderFlags(sizeof(data), 0xff);  //  TP-IoT: Byte 4 of header is the message length. 0xff will clear all bits.
  rf95.send(data, sizeof(data));  //  Last byte is 0, don't send it.
  
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf95.waitAvailableTimeout(3000))
  { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {
      Serial.print("got reply: ");
      Serial.println((char*)buf);
//      Serial.print("RSSI: ");
//      Serial.println(rf95.lastRssi(), DEC);    
    }
    else
    {
      Serial.println("recv failed");
    }
  }
  else
  {
    Serial.println("No reply");
  }
  delay(400);
}


