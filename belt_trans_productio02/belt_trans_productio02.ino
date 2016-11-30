// transmitter.pde
//
// Simple example of how to use VirtualWire to transmit messages
// Implements a simplex (one-way) transmitter with an TX-C1 module
//
// See VirtualWire.h for detailed API docs
// Author: Mike McCauley (mikem@airspayce.com)
// Copyright (C) 2008 Mike McCauley
// $Id: transmitter.pde,v 1.3 2009/03/30 00:07:24 mikem Exp $

#include <VirtualWire.h>
#define THERMISTORPIN A0
void setup()
{
    Serial.begin(9600);	  // Debugging only
    Serial.println("setup");
    analogReference(INTERNAL);
  //  vw_set_tx_pin(1);
    // Initialise the IO and ISR
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(2000);	 // Bits per sec
}

void loop()
{
    unsigned short reading = analogRead(THERMISTORPIN);
    Serial.print("reading ");
    Serial.println(reading );
    //char *msg = "00";
    uint8_t msg[3];
    digitalWrite(13, true); // Flash a light to show transmitting
    msg[0] = (uint8_t) (reading & 0x000000FF);
    //msg[1] = (uint8_t) (0x80 | ((reading & 0x0000FF00) >> 8));
    msg[1] = (uint8_t)((reading & 0x0000FF00) >> 8);
    //msg[2] = 31;
    
    //msg[1] = (uint8_t)(msg[1] | 0b00000000);
    

    //Serial.println(msg[0]);
    //Serial.println(msg[1]);
    
    vw_send((uint8_t *)msg, 2);
    
    vw_wait_tx(); // Wait until the whole message is gone
    digitalWrite(13, false);
//    delay(10);
   // Serial.println("hel");
}

