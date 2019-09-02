#include <Wire.h>
#define EEPROM_DATA
#include <MyNodes.h>

byte multiplier = 100;
byte nodeId = 0;
boolean nodeIdUpdated = false;

void setup()
{
	Serial.begin(115200);
	Wire.begin();
	clearEEPROM(EEPROM_ADDRESS, memoryAddress);
}

void loop()
{
	Serial.flush();
	if (Serial.available())
	{
		byte inValue = 0;
		inValue = Serial.read() - '0';
		nodeId = nodeId + (inValue * multiplier);
		if (!nodeIdUpdated & multiplier == 1)
		{
			writeEEPROM(EEPROM_ADDRESS, memoryAddress, nodeId);
			nodeIdUpdated = true;
			Serial.print("The raw value read from EEPROM is : ");
			Serial.println(readEEPROM(EEPROM_ADDRESS, memoryAddress));
		}
		multiplier = multiplier / 10;
	}
 }

void clearEEPROM(int deviceaddress, unsigned int eeaddress)
{
	byte data = 0;
	Wire.beginTransmission(deviceaddress);
	Wire.write((int)(eeaddress >> 8));   // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.write(data);
	Wire.endTransmission();
	delay(5);
}

void writeEEPROM(int deviceaddress, unsigned int eeaddress, byte data)
{
	Wire.beginTransmission(deviceaddress);
	Wire.write((int)(eeaddress >> 8));   // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.write(data);
	Wire.endTransmission();
	delay(5);
}