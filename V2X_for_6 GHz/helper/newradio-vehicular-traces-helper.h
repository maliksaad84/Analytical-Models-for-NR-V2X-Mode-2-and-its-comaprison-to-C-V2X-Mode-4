#ifndef newradio_VEHICULAR_TRACES_HELPER_H
#define newradio_VEHICULAR_TRACES_HELPER_H

#include <fstream>
#include <string>
#include <ns3/object.h>
#include <ns3/spectrum-value.h>

namespace ns3 {

namespace millicar {

/**
 * Class that manages the connection to a trace
 * in newradioSidelinkSpectrumPhy and prints to a file
 */
class newradioVehicularTracesHelper : public Object
{
public:
  /**
   * Constructor for this class
   * \param filename the name of the file
   */
  newradioVehicularTracesHelper(std::string filename);

  /**
   * Destructor for this class
   */
  virtual ~newradioVehicularTracesHelper();

  /**
   * Method to be attached to the callback in the newradioSidelinkSpectrumPhy
   * \param sinr pointer to the SpectrumValue instance representing the SINR
            measured on all the spectrum chunks
   * \param rnti the RNTI of the tranmitting device
   * \param numSym size of the transport block that generated the report in
            number of OFDM symbols
   * \param tbSize size of the transport block that generated the report in bytes
   * \param mcs the MCS of the transmission
   */
  void McsSinrCallback(const SpectrumValue& sinr, uint16_t rnti, uint8_t numSym, uint32_t tbSize, uint8_t mcs);

private:
  std::string m_filename; //!< filename for the output
  std::ofstream m_outputFile; //!< output file

};

}
}

#endif
