#ifndef SRC_newradio_MODEL_newradio_SAP_H_
#define SRC_newradio_MODEL_newradio_SAP_H_

#include <ns3/packet-burst.h>
#include <ns3/lte-mac-sap.h>
#include <ns3/lte-rlc-am.h>
#include <ns3/spectrum-value.h>
#include <ns3/newradio-phy-mac-common.h>

namespace ns3 {

namespace millicar {

class newradioSidelinkPhySapProvider
{
public:
  virtual ~newradioSidelinkPhySapProvider ()
  {
  }

  /**
   * \brief Called by the upper layers to fill PHY's buffer
   * \param pb burst of packets to be forwarded to the PHY layer
   * \param info information about slot allocation necessary to determine the transmission parameters
   */
  virtual void AddTransportBlock (Ptr<PacketBurst> pb, newradio::SlotAllocInfo info) = 0;

  /**
   * \brief Called by the upper layer to prepare the PHY for the reception from
   *        another device
   * \param rnti the rnti of the transmitting device
   */
  virtual void PrepareForReception (uint16_t rnti) = 0;

};

class newradioSidelinkPhySapUser
{
public:
  virtual ~newradioSidelinkPhySapUser ()
  {
  }

  /**
   * \brief Called by the PHY to notify the MAC of the reception of a new PHY-PDU
   * \param p packet
   */
  virtual void ReceivePhyPdu (Ptr<Packet> p) = 0;

  /**
   * \brief Trigger the start from a new slot (input from PHY layer)
   * \param timingInfo the structure containing the timing information
   */
  virtual void SlotIndication (newradio::SfnSf timingInfo) = 0;

  /**
   * \brief Reports the SINR meausured with a certain device
   * \param sinr the SINR
   * \param rnti RNTI of the transmitting device
   * \param numSym size of the transport block that generated the report in
            number of OFDM symbols
   * \param tbSize size of the transport block that generated the report in
            number of bytes
   */
  virtual void SlSinrReport (const SpectrumValue& sinr, uint16_t rnti, uint8_t numSym, uint32_t tbSize) = 0;

};

} // newradio namespace

} // ns3 namespace

#endif /* SRC_newradio_MODEL_newradio_SAP_H_ */
