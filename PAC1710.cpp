#include "PAC1710.h"
#include <Wire.h>

void PAC1710::SetSenseScale(SenseScale ss) {
	_ss = ss;
	writeConfigRegisters();
}

void PAC1710::SetSamplingTimesMs(unsigned voltage, unsigned current) {
	if(voltage < 5) _samplingU = 0x0u;
	else {
		voltage /= 10;
		_samplingU = 0x1u;
		while(voltage > 0) {
			voltage >>= 1u;
			++_samplingU;
		}
	}
	_samplingU &= 0x3u;
	if(current < 5) _samplingI = 0x0u;
	else {
		current /= 10;
		_samplingI = 0x1u;
		while(current > 0) {
			current >>= 1u;
			++_samplingI;
		}
	}
	_samplingI &= 0x7u;
	writeConfigRegisters();
}

void PAC1710::SetAveraging(Averaging voltage, Averaging current) {
	_avgU = voltage;
	_avgI = current;
	writeConfigRegisters();
}

void PAC1710::SetStandby(bool standby) {
	_inStandby = standby;
	writeByte(0x00, _inStandby ? 0x03 : 0x00);
	writeConfigRegisters();
}

void PAC1710::ReadOnce(ReadSchedule schedule) {
	if(_inStandby) writeByte(0x02, 0x00);	// Write to one-shot
	if(schedule & READ_POWER) _powerRatio = (readByte(0x15) << 8u) | readByte(0x16);
	if(schedule & READ_CURRENT) _currentRatio = (int16_t)((readByte(0x0d) << 8u) | readByte(0x0e)) >> 4;
	if(schedule & READ_VOLTAGE) _voltageRatio = (readByte(0x11) << 3u) | readByte(0x12) / 32;
}

void PAC1710::writeConfigRegisters() const {
	byte avgU = _avgU, avgI = _avgI;
	if(_inStandby) {
		avgU = AVG_NONE;
		avgI = AVG_NONE;
	}
	writeByte(0x0a, (_samplingU << 2) | avgU);
	writeByte(0x0b, (_samplingI << 4) | (avgI << 2) | _ss);
}

uint8_t PAC1710::readByte(uint8_t address) const {
	Wire.beginTransmission(i2cAddr);
	Wire.write(address);
	Wire.endTransmission();
	if(Wire.requestFrom(i2cAddr, 1u) != 1u) return -1;
	return (uint8_t)Wire.read();
}

void PAC1710::writeByte(uint8_t address, uint8_t value) const {
	Wire.beginTransmission(i2cAddr);
	Wire.write(address);
	Wire.write(value);
	Wire.endTransmission();
}
