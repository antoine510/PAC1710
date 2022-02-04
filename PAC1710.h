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

    template<byte senseMilliOhm, SenseScale ss>
    class ValueReader {
    public:
      /**
       * @brief Get measured power in watts
       *
       * @return float Power in watts
       */
      static float GetPower(const PAC1710& pac) { return pac._powerRatio * _powerScale; }

      /**
       * @brief Get Power as integer (for performance)
       *
       * @return unsigned Power in deciWatts
       */
      static unsigned GetPowerI(const PAC1710& pac) { return pac._powerRatio * _powerIScale; }

      /**
       * @brief Get measured current in amps
       *
       * @return float Current in amps
       */
      static float GetCurrent(const PAC1710& pac) { return pac._currentRatio * _currentScale; }

      /**
       * @brief Get Current as integer (for performance)
       *
       * @return int Current in milliamps
       */
      static int GetCurrentI(const PAC1710& pac) { return pac._currentRatio * _currentIScale; }

      /**
       * @brief Get measured voltage in volts
       *
       * @return float Voltage in volts
       */
      static float GetVoltage(const PAC1710& pac) { return pac._voltageRatio * _voltageScale; }

      /**
       * @brief Get Voltage as integer (for performance)
       *
       * @return unsigned Voltage in millivolts
       */
      static unsigned GetVoltageI(const PAC1710& pac) { return pac._voltageRatio * _voltageIScale; }

    private:
      static constexpr float _fsc = 10.f * (1u << ss) / senseMilliOhm;
      static constexpr float _fsv = 40.f * 2047 / 2048;

      static constexpr float _powerScale = _fsc * _fsv / 65535;
      static constexpr float _currentScale = _fsc / 2047;
      static constexpr float _voltageScale = _fsv / 2047;

      static constexpr float _powerIScale = _powerScale * 10;
      static constexpr float _currentIScale = _currentScale * 1000;
      static constexpr float _voltageIScale = _voltageScale * 1000;
    };

    /**
     * @brief Sets the sense scale for the module
     * The sense scale has to be divided by the sense resistor value to
     * get the maximum current. Choose as small a sense scale as possible.
     *
     * @param ss The sense scale to use
     */
    void SetSenseScale(SenseScale ss);

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
     * @brief Enter the standby state
     * Disables averaging and enters standby state
     */
    void SetStandby(bool standby);

    /**
     * @brief Triggers a read on the PAC1710 hardware
     * This call is necessary before accessing the power, current or voltage
     */
    void ReadOnce(ReadSchedule schedule = READ_ALL);

  private:
    template<byte senseMilliOhm, SenseScale ss>
    friend class ValueReader;

    static constexpr uint8_t i2cAddr = 0x4c;

    void writeConfigRegisters() const;

    uint8_t readByte(uint8_t address) const;
    void writeByte(uint8_t address, uint8_t value) const;

    SenseScale _ss = SS_80MV;  // 80mV sense scale, as per datasheet
    byte _samplingU = 0x2u, _samplingI = 0x5u;  // 10ms voltage, 80ms current, as per datasheet.
    Averaging _avgU = AVG_NONE, _avgI = AVG_NONE; // No averaging, as per datasheet.
    bool _inStandby = false;

    int16_t _currentRatio;
    uint16_t _voltageRatio, _powerRatio;
};

#endif
