#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define LIGHT_NODE
#define NODE_HAS_RELAY

#define MY_RADIO_RF24
#define MY_NODE_ID GATE_LIGHT_NODE_ID
#define MY_PARENT_NODE_ID REPEATER_04_NODE_ID
#define MY_PARENT_NODE_IS_STATIC

#define STATUS_LEDS
#define APPLICATION_NAME "Gate Light"

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

byte lightRelayStatusRequestCount;

boolean lightRelayStatusReceived;
boolean sendLightRelayStatusRequest;

AlarmId requestTimer;
AlarmId heartbeatTimer;

AlarmId onTimer;
AlarmId offTimer;

MyMessage lightRelayMessage(LIGHT_RELAY_ID, V_STATUS);
MyMessage staircaseLightRelayMessage(LIGHT_RELAY_ID, V_STATUS);
MyMessage thingspeakMessage(WIFI_NODEMCU_ID, V_CUSTOM);

void before()
{
	pinMode(LIGHT_RELAY_PIN, OUTPUT);
}

void setup()
{
	digitalWrite(LIGHT_RELAY_PIN, LOW);
	lightRelayStatusReceived = false;
	sendLightRelayStatusRequest = true;
	staircaseLightRelayMessage.setDestination(STAIRCASE_LIGHT_NODE_ID);
	staircaseLightRelayMessage.setType(V_STATUS);
	staircaseLightRelayMessage.setSensor(LIGHT_RELAY_ID);
	thingspeakMessage.setDestination(THINGSPEAK_NODE_ID);
	thingspeakMessage.setType(V_CUSTOM);
	thingspeakMessage.setSensor(WIFI_NODEMCU_ID);
	lightRelayStatusRequestCount = 0;
	setSyncInterval(ONE_HOUR * 3);
	setSyncProvider(getTimeFromGateway);
	onTimer = Alarm.alarmRepeat(HRS19, MIN00, SEC00, turnOnLights);
	offTimer = Alarm.alarmRepeat(HRS23, MIN45, SEC00, turnOffLights);
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, heartbeat);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, getCodeVersion());
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(LIGHT_RELAY_ID, S_BINARY, "Gate Light Relay");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(lightRelayMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(thingspeakMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
}

void loop()
{
	if (sendLightRelayStatusRequest)
	{
		sendLightRelayStatusRequest = false;
		request(LIGHT_RELAY_ID, V_STATUS);
		requestTimer = Alarm.timerOnce(REQUEST_INTERVAL, checkLightRelayStatusRequest);
		lightRelayStatusRequestCount++;
		if (lightRelayStatusRequestCount == 10)
		{
			lightRelayMessage.set(RELAY_OFF);
			send(lightRelayMessage);
			wait(WAIT_AFTER_SEND_MESSAGE);
		}
	}
	Alarm.delay(1);
}

void receive(const MyMessage &message)
{
	switch (message.type)
	{
	case V_STATUS:
		switch (message.sender)
		{
		case THINGSPEAK_NODE_ID:
			if (message.getBool())
				turnOnLights();
			else
				turnOffLights();
			break;
		default:
			if (!lightRelayStatusReceived)
			{
				lightRelayStatusReceived = true;
				Alarm.free(requestTimer);
				request(LIGHT_RELAY_ID, V_STATUS);
				wait(WAIT_AFTER_SEND_MESSAGE);
			}
			if (message.getInt())
				turnOnLights();
			else
				turnOffLights();
			requestTime();
			break;
		}
		break;
	}
}

void turnOnLights()
{
	digitalWrite(LIGHT_RELAY_PIN, RELAY_ON);
	send(lightRelayMessage.set(RELAY_ON));
	wait(WAIT_AFTER_SEND_MESSAGE);
	send(thingspeakMessage.set(RELAY_ON));
	wait(WAIT_AFTER_SEND_MESSAGE);
	send(staircaseLightRelayMessage.set(RELAY_ON));
	wait(WAIT_AFTER_SEND_MESSAGE);
}

void turnOffLights()
{
	digitalWrite(LIGHT_RELAY_PIN, RELAY_OFF);
	send(lightRelayMessage.set(RELAY_OFF));
	wait(WAIT_AFTER_SEND_MESSAGE);
	send(thingspeakMessage.set(RELAY_OFF));
	wait(WAIT_AFTER_SEND_MESSAGE);
	send(staircaseLightRelayMessage.set(RELAY_OFF));
	wait(WAIT_AFTER_SEND_MESSAGE);
}

void checkLightRelayStatusRequest()
{
	if (!lightRelayStatusReceived)
		sendLightRelayStatusRequest = true;
}

void heartbeat()
{
	sendHeartbeat();
}

time_t getTimeFromGateway()
{
	requestTime();
	return 0;
}

void receiveTime(unsigned long controllerTime)
{
	setTime(controllerTime);
	Alarm.free(onTimer);
	Alarm.free(offTimer);
	onTimer = Alarm.alarmRepeat(HRS19, MIN00, SEC00, turnOnLights);
	offTimer = Alarm.alarmRepeat(HRS23, MIN45, SEC00, turnOffLights);
}