#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define LIGHT_NODE
#define NODE_HAS_RELAY

#define MY_RADIO_RF24
#define MY_NODE_ID BALCONY_LIGHT_NODE_ID
#define MY_PARENT_NODE_ID REPEATER_01_NODE_ID
#define MY_PARENT_NODE_IS_STATIC

#define STATUS_LEDS
#define APPLICATION_NAME "Balcony Light"

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

byte lightRelayStatusRequestCount;

boolean lightRelayStatusReceived;
boolean sendLightRelayStatusRequest;

AlarmId requestTimer;
AlarmId heartbeatTimer;

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
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, heartbeat);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, getCodeVersion());
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(LIGHT_RELAY_ID, S_BINARY, "Balcony Light Relay");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(lightRelayMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(staircaseLightRelayMessage.set(RELAY_OFF));
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
	int newLightOnDuration;
	switch (message.type)
	{
	case V_STATUS:
		switch (message.sender)
		{
		case STAIRCASE_LIGHT_NODE_ID:
			if (digitalRead(LIGHT_RELAY_PIN))
				send(staircaseLightRelayMessage.set(RELAY_ON));
			else
				send(staircaseLightRelayMessage.set(RELAY_OFF));
			wait(WAIT_AFTER_SEND_MESSAGE);
			break;
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

void sendLightStatus()
{
	if (digitalRead(LIGHT_RELAY_PIN))
		send(staircaseLightRelayMessage.set(RELAY_ON));
	else
		send(staircaseLightRelayMessage.set(RELAY_OFF));
	wait(WAIT_AFTER_SEND_MESSAGE);
}

void heartbeat()
{
	sendHeartbeat();
}