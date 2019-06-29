#define MY_DEBUG 
#define MY_RADIO_RF24
#define MY_RF24_PA_LEVEL RF24_PA_LOW
#define MY_GATEWAY_SERIAL

#define STATUS_LEDS
#define APPLICATION_NAME "Serial Gateway"

#include <MyNodes.h>
#include <SPI.h>
#include <MySensors.h>  

void setup()
{

}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, getCodeVersion());
}

void loop()
{

}