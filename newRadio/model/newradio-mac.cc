#include "newradio-mac.h"

#include <ns3/object-factory.h>
#include <ns3/log.h>
#include "newradio-radio-bearer-tag.h"
#include "newradio-phy-sap.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("newradioMac");

namespace newradio {

NS_OBJECT_ENSURE_REGISTERED (newradioMac);

newradioMac::newradioMac ()
{
  m_macQueue = CreateObject <PacketBurst> ();
}

newradioMac::~newradioMac ()
{

}


void
newradioMac::SetConfigurationParameters (Ptr<newradioPhyMacCommon> ptrConfig)
{
  m_phyMacConfig = ptrConfig;
}

Ptr<newradioPhyMacCommon>
newradioMac::GetConfigurationParameters (void) const
{
  return m_phyMacConfig;
}

bool
newradioMac::QueueData (Ptr<Packet> packet)
{
  NS_LOG_INFO ("Queue in Mac");

  m_macQueue->AddPacket (packet);

  return true;
}

Ptr<PacketBurst>
newradioMac::GetPacketBurstFromMacQueue ()
{
  NS_LOG_FUNCTION (this);

  Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
  if (m_macQueue->GetNPackets () > 0)
    {
      pb = m_macQueue->Copy ();
      m_macQueue->Dispose ();
      m_macQueue = CreateObject <PacketBurst> ();
    }

  return pb;
}


}

}
