#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define MY_RADIO_RF24
#define MY_NODE_ID DEFAULT_FIRMWARE_NODE_ID

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Default MYS Firmware"

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

void heartbeat()
{
	sendHeartbeat();
}