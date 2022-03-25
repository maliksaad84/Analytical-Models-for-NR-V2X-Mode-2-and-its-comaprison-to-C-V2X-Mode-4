#ifndef SRC_newradio_MODEL_newradio_MAC_H_
#define SRC_newradio_MODEL_newradio_MAC_H_


#include <vector>
#include "newradio-enb-phy.h"
#include "newradio-phy-mac-common.h"
#include "newradio-mac-scheduler.h"
#include "newradio-control-messages.h"
#include <ns3/packet.h>
#include <ns3/packet-burst.h>
#include <ns3/lte-mac-sap.h>
#include <map>
#include <list>
#include "newradio-mac-sched-sap.h"
#include <ns3/lte-radio-bearer-tag.h>
#include "newradio-mac-pdu-header.h"
#include "newradio-mac-pdu-tag.h"


namespace ns3 {

namespace newradio {

struct MacPduInfo
{
  MacPduInfo (SfnSf sfn, uint32_t size, uint8_t numRlcPdu)
    : m_sfnSf (sfn),
      m_size (size),
      m_numRlcPdu (numRlcPdu)
  {
    m_pdu = Create<Packet> ();
    m_macHeader = newradioMacPduHeader ();
    newradioMacPduTag tag (sfn);
    m_pdu->AddPacketTag (tag);
  }

  MacPduInfo (SfnSf sfn, uint32_t size, uint8_t numRlcPdu, DciInfoElementTdma dci)
    : m_sfnSf (sfn),
      m_size (size),
      m_numRlcPdu (numRlcPdu)
  {
    m_pdu = Create<Packet> ();
    m_macHeader = newradioMacPduHeader ();
    newradioMacPduTag tag (sfn, dci.m_symStart, dci.m_numSym);
    m_pdu->AddPacketTag (tag);
  }

  SfnSf m_sfnSf;
  uint32_t m_size;
  uint8_t m_numRlcPdu;
  Ptr<Packet> m_pdu;
  newradioMacPduHeader m_macHeader;
};

class newradioMac : public Object
{
public:
  /* Do not put the set TypeId function. */
  newradioMac ();
  ~newradioMac ();


  void SetConfigurationParameters (Ptr<newradioPhyMacCommon> ptrConfig);
  Ptr<newradioPhyMacCommon> GetConfigurationParameters (void) const;

  bool QueueData (Ptr<Packet> packet);
  Ptr<PacketBurst> GetPacketBurstFromMacQueue ();

protected:
  Ptr<newradioPhyMacCommon> m_phyMacConfig;

  Ptr<PacketBurst>  m_macQueue;


};


}

}


#endif /* SRC_newradio_MODEL_newradio_MAC_H_ */
