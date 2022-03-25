#ifndef newradio_SIDELINK_SPECTRUM_SIGNAL_PARAMETERS_H
#define newradio_SIDELINK_SPECTRUM_SIGNAL_PARAMETERS_H

#include <ns3/spectrum-signal-parameters.h>

namespace ns3 {

class PacketBurst;

namespace millicar {

class newradioSidelinkControlMessage;

struct newradioSidelinkSpectrumSignalParameters : public SpectrumSignalParameters
{

  // inherited from SpectrumSignalParameters
  virtual Ptr<SpectrumSignalParameters> Copy ();

  /**
  * default constructor
  */
  newradioSidelinkSpectrumSignalParameters ();

  /**
  * copy constructor
  */
  newradioSidelinkSpectrumSignalParameters (const newradioSidelinkSpectrumSignalParameters& p);

  Ptr<PacketBurst> packetBurst;

  //std::list<Ptr<newradioSidelinkControlMessage>> ctrlMsgList;

  uint8_t slotInd;

  uint8_t mcs; ///< the modulation and coding scheme index to be used to transmit the transport block

  uint8_t numSym; ///< the number of symbols associated to a specific transport block

  uint16_t senderRnti; ///< the RNTI which identifies the sender device

  uint16_t destinationRnti; ///< the RNTI which identifies the destination device

  uint32_t size; ///< the size of the corresponding transport block

  std::vector<int> rbBitmap; ///< the resource blocks bitmap associated to the transport block

  bool pss;

};

} // namespace millicar

}  // namespace ns3


#endif /* newradio_SIDELINK_SPECTRUM_SIGNAL_PARAMETERS_H */
