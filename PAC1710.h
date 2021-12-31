#ifndef PAC1710_H
#define PAC1710_H

#include <Arduino.h>

/**
 * @brief Module for handling the PAC1710 I²C communication
 */
class PAC1710 {
  public:
    /**
     * @brief This is used to read a subset of all values available on the PAC1710
     */
    enum ReadSchedule {
      READ_VOLTAGE  = 0x1u,
      READ_CURRENT  = 0x2u,
      READ_POWER    = 0x4u,
      READ_ALL      = 0x7u
    };

    /**
     * @brief Maximum sense voltage. Chosen depending on sense resistor value
     * and maximum measured current.
     */
    enum SenseScale : byte {
      SS_10MV = 0,
      SS_20MV = 1,
      SS_40MV = 2,
      SS_80MV = 3
    };

    /**
     * @brief Averaging of samples in hardware.
     */
    enum Averaging : byte {
      AVG_NONE  = 0,
      AVG_2     = 1,
      AVG_4     = 2,
      AVG_8     = 3
    };

    /**
     * @brief Initializes the PAC1710 module.
     * The sense scale has to be divided by the sense resistor value to
     * get the maximum current. Choose as small a sense scale as possible.
     * 
     * @param ss The sense scale to use
     * @param senseMilliOhm The current measuring resistor value in milliohms
     */
    void Init(SenseScale ss, float senseMilliOhm);

    /**
     * @brief Sets the sampling time for voltage and current
     * The sampling time will be rounded down to an available value:
     * 2.5 x 2^n milliseconds
     * Max is 20ms for voltage and 320ms for current
     * @param voltage The voltage sampling time
     * @param current The current sampling time
     */
    void SetSamplingTimesMs(unsigned voltage, unsigned current);

    /**
     * @brief Sets the number of samples to average for voltage and current
     * @param voltage The voltage averaging sample count
     * @param current The current averaging sample count
     */
    void SetAveraging(Averaging voltage, Averaging current);

    /**
     * @brief Triggers a read on the PAC1710 hardware
     * This call is necessary before accessing the power, current or voltage
     */
    void ReadOnce(ReadSchedule schedule = READ_ALL);

    /**
     * @brief Get measured power in watts
     * FSV * 1/65535 factored in as 0.00061
     * 
     * @return float Power in watts
     */
    float GetPower() { return _powerRatio * 0.00061f * _fsc; }

    /**
     * @brief Get measured current in amps
     * 1/2047 factored in as 0.00048852
     * 
     * @return float Current in amps
     */
    float GetCurrent() { return _currentRatio * _fsc * 0.00048852f; }

    /**
     * @brief Get measured voltage in volts
     * FSV * 1/2047 factored in as 0.01953125
     * 
     * @return float Voltage in volts
     */
    float GetVoltage() { return _voltageRatio * 0.01953125f; }
    
  private:
    static constexpr uint8_t i2cAddr = 0x4c;

    void writeConfigRegisters();

    uint8_t readByte(uint8_t address);
    void writeByte(uint8_t address, uint8_t value);

    float _fsc;
    SenseScale _ss = SS_80MV;  // 80mV sense scale, as per datasheet
    byte _samplingU = 0x2u, _samplingI = 0x5u;  // 10ms voltage, 80ms current, as per datasheet.
    Averaging _avgU = AVG_NONE, _avgI = AVG_NONE; // No averaging, as per datasheet.

    int16_t _currentRatio;
    uint16_t _voltageRatio, _powerRatio;
};

#endif
