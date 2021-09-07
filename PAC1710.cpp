#include "PAC1710.h"
#include <Wire.h>

void PAC1710::Init(SenseScale ss, float senseMilliOhm) {
	uint8_t ssval = ss;
	writeByte(0x0a, 0x0e); // Vsource configuration
	writeByte(0x0b, 0x78 + ssval); // Vsense configuration
	fsc = (float)(10 * (1u << ssval)) / senseMilliOhm;
}

float PAC1710::GetPower() {
	// FSV * 1/65535 factored in as 0.00061
	return GetPowerRatio() * 0.00061f * fsc;
}

uint16_t PAC1710::GetPowerRatio() {
	// Power ratio is in registers 0x15 << 8 + 0x16
	return (readByte(0x15) << 8u) | readByte(0x16);
}

float PAC1710::GetCurrent() {
	// 1/2047 factored in as 0.00048852
	return GetCurrentRatio() * fsc * 0.00048852f;
}

int16_t PAC1710::GetCurrentRatio() {
	return (int16_t)((readByte(0x0d) << 8u) | readByte(0x0e)) >> 4;
}

float PAC1710::GetVoltage() {
	// FSV * 1/2047 factored in as 0.01953125
	return GetVoltageRatio() * 0.01953125f;
}

uint16_t PAC1710::GetVoltageRatio() {
	return (readByte(0x11) << 3u) | (readByte(0x12) >> 5u);
}

uint8_t PAC1710::readByte(uint8_t address) {
	Wire.beginTransmission(i2cAddr);
	Wire.write(address);
	Wire.endTransmission();
	if(Wire.requestFrom(i2cAddr, 1u) != 1u) return -1;
	return (uint8_t)Wire.read();
}

void PAC1710::writeByte(uint8_t address, uint8_t value) {
	Wire.beginTransmission(i2cAddr);
	Wire.write(address);
	Wire.write(value);
	Wire.endTransmission();
}
