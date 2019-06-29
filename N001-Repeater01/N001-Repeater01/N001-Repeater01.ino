#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define MY_RADIO_RF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID REPEATER_01_NODE_ID
#define MY_PARENT_NODE_ID GATEWAY_NODE_ID
#define MY_PARENT_NODE_IS_STATIC
#define STATUS_LEDS

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Repeater 01"

AlarmId heartbeatTimer;

void before()
{

}

void setup()
{
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, heartbeat);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, getCodeVersion());
}

void loop()
{
	Alarm.delay(1);
}

void receive(const MyMessage &message)
{

}

void heartbeat()
{
	sendHeartbeat();
}