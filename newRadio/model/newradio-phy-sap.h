#ifndef SRC_newradio_MODEL_newradio_PHY_SAP_H_
#define SRC_newradio_MODEL_newradio_PHY_SAP_H_

#include <ns3/packet-burst.h>
#include <ns3/newradio-phy-mac-common.h>
#include <ns3/newradio-mac-sched-sap.h>

namespace ns3 {

namespace newradio {

class newradioControlMessage;
/* Mac to Phy comm*/
class newradioPhySapProvider
{
public:
  virtual ~newradioPhySapProvider ();

  virtual void SendMacPdu (Ptr<Packet> p ) = 0;

  virtual void SendControlMessage (Ptr<newradioControlMessage> msg) = 0;

  virtual void SendRachPreamble (uint8_t PreambleId, uint8_t Rnti) = 0;

  virtual void SetDlSfAllocInfo (SfAllocInfo sfAllocInfo) = 0;

  virtual void SetUlSfAllocInfo (SfAllocInfo sfAllocInfo) = 0;
};

/* Phy to Mac comm */
class newradioEnbPhySapUser
{
public:
  virtual ~newradioEnbPhySapUser ()
  {
  }

  /**
   * Called by the Phy to notify the MAC of the reception of a new PHY-PDU
   *
   * \param p
   */
  virtual void ReceivePhyPdu (Ptr<Packet> p) = 0;

  /**
   * \brief Receive SendLteControlMessage (PDCCH map, CQI feedbacks) using the ideal control channel
   * \param msg the Ideal Control Message to receive
   */
  virtual void ReceiveControlMessage (Ptr<newradioControlMessage> msg) = 0;

  /**
   * \brief Trigger the start from a new frame (input from Phy layer)
   * \param frameNo frame number
   * \param subframeNo subframe number
   */
  virtual void SubframeIndication (SfnSf) = 0;

  /**
   * \brief Returns to MAC level the UL-CQI evaluated
   * \param ulcqi the UL-CQI (see FF MAC API 4.3.29)
   */
  virtual void UlCqiReport (newradioMacSchedSapProvider::SchedUlCqiInfoReqParameters ulcqi) = 0;

  /**
   * notify the reception of a RACH preamble on the PRACH
   *
   * \param prachId the ID of the preamble
   */
  virtual void ReceiveRachPreamble (uint32_t raId) = 0;

  /**
   * Notify the HARQ on the UL tranmission status
   *
   * \param params
   */
  virtual void UlHarqFeedback (UlHarqInfo params) = 0;
};

class newradioUePhySapUser
{
public:
  virtual ~newradioUePhySapUser ()
  {
  }

  /**
   * Called by the Phy to notify the MAC of the reception of a new PHY-PDU
   *
   * \param p
   */
  virtual void ReceivePhyPdu (Ptr<Packet> p) = 0;

  /**
   * \brief Receive SendLteControlMessage (PDCCH map, CQI feedbacks) using the ideal control channel
   * \param msg the Ideal Control Message to receive
   */
  virtual void ReceiveControlMessage (Ptr<newradioControlMessage> msg) = 0;

  /**
   * \brief Trigger the start from a new frame (input from Phy layer)
   * \param frameNo frame number
   * \param subframeNo subframe number
   */
  virtual void SubframeIndication (SfnSf) = 0;

  /**
   * When PHY connects to a new eNB, update the configuration parameters
   */
  virtual void SetConfigurationParameters (Ptr<newradioPhyMacCommon> params) = 0;

  //virtual void NotifyHarqDeliveryFailure (uint8_t harqId) = 0;
};

}

}



#endif /* SRC_newradio_MODEL_newradio_PHY_SAP_H_ */
