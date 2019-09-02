#include <Wire.h>
#define EEPROM_DATA
#include <MyNodes.h>

void setup()
{
	Serial.begin(115200);
	Wire.begin();
	Serial.print("The raw value read from EEPROM is : ");
	Serial.println(readEEPROM(EEPROM_ADDRESS, memoryAddress));
}

void loop()
{

}