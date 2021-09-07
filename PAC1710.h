#ifndef PAC1710_H
#define PAC1710_H

#include <Arduino.h>

/**
 * @brief Module for handling the PAC1710 I²C communication
 */
class PAC1710 {
  public:
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
     * @brief Initializes the PAC1710 module.
     * The sense scale has to be divided by the sense resistor value to
     * get the maximum current. Choose as small a sense scale as possible.
     * 
     * @param ss The sense scale to use
     * @param senseMilliOhm The current measuring resistor value in milliohms
     */
    void Init(SenseScale ss, float senseMilliOhm);

    /**
     * @brief Get measured power in watts
     * 
     * @return float Power in watts
     */
    float GetPower();

    /**
     * @brief Get measured current in amps
     * 
     * @return float Current in amps
     */
    float GetCurrent();

    /**
     * @brief Get measured voltage in volts
     * 
     * @return float Voltage in volts
     */
    float GetVoltage();
    
  private:
    static constexpr uint8_t i2cAddr = 0x4c;

    uint16_t GetPowerRatio();
    int16_t GetCurrentRatio();
    uint16_t GetVoltageRatio();

    uint8_t readByte(uint8_t address);
    void writeByte(uint8_t address, uint8_t value);

    float fsc;
};

#endif
