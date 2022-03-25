#include <ns3/llc-snap-header.h>
#include <ns3/simulator.h>
#include <ns3/callback.h>
#include <ns3/node.h>
#include <ns3/packet.h>
#include "newradio-net-device.h"
#include <ns3/packet-burst.h>
#include <ns3/uinteger.h>
#include <ns3/trace-source-accessor.h>
#include <ns3/pointer.h>
#include <ns3/enum.h>
#include <ns3/uinteger.h>
#include "newradio-enb-net-device.h"
#include "newradio-ue-net-device.h"
#include <ns3/lte-enb-rrc.h>
#include <ns3/ipv4-l3-protocol.h>
#include <ns3/ipv6-l3-protocol.h>
#include <ns3/abort.h>
#include <ns3/log.h>
#include <ns3/lte-enb-component-carrier-manager.h>
#include <ns3/object-map.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("newradioEnbNetDevice");

namespace newradio {

NS_OBJECT_ENSURE_REGISTERED ( newradioEnbNetDevice);

TypeId newradioEnbNetDevice::GetTypeId ()
{
  static TypeId
    tid =
    TypeId ("ns3::newradioEnbNetDevice")
    .SetParent<newradioNetDevice> ()
    .AddConstructor<newradioEnbNetDevice> ()
    .AddAttribute ("LteEnbComponentCarrierManager",
                   "The ComponentCarrierManager associated to this EnbNetDevice",
                   PointerValue (),
                   MakePointerAccessor (&newradioEnbNetDevice::m_componentCarrierManager),
                   MakePointerChecker <LteEnbComponentCarrierManager> ())
    .AddAttribute ("ComponentCarrierMap", "List of component carriers.",
                   ObjectMapValue (),
                   MakeObjectMapAccessor (&newradioEnbNetDevice::m_ccMap),
                   MakeObjectMapChecker<newradioComponentCarrierEnb> ())
    .AddAttribute ("LteEnbRrc",
                   "The RRC layer associated with the ENB",
                   PointerValue (),
                   MakePointerAccessor (&newradioEnbNetDevice::m_rrc),
                   MakePointerChecker <LteEnbRrc> ())
    .AddAttribute ("CellId",
                   "Cell Identifier",
                   UintegerValue (0),
                   MakeUintegerAccessor (&newradioEnbNetDevice::m_cellId),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("AntennaNum",
                   "Antenna number of the device",
                   UintegerValue (64),
                   MakeUintegerAccessor (&newradioEnbNetDevice::SetAntennaNum,
                                         &newradioEnbNetDevice::GetAntennaNum),
                   MakeUintegerChecker<uint16_t> ())
  ;
  return tid;
}

newradioEnbNetDevice::newradioEnbNetDevice ()
//:m_cellId(0),
// m_Bandwidth (72),
// m_Earfcn(1),
  : m_componentCarrierManager (0),
    m_isConstructed (false),
    m_isConfigured (false)
{
  NS_LOG_FUNCTION (this);
}

newradioEnbNetDevice::~newradioEnbNetDevice ()
{
  NS_LOG_FUNCTION (this);
}

void
newradioEnbNetDevice::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);
  m_isConstructed = true;
  UpdateConfig ();
  std::map< uint8_t, Ptr<newradioComponentCarrierEnb> >::iterator it;
  for (it = m_ccMap.begin (); it != m_ccMap.end (); ++it)
    {
      it->second->Initialize ();
    }
  m_rrc->Initialize ();
  m_componentCarrierManager->Initialize ();
}

void
newradioEnbNetDevice::DoDispose ()
{
  NS_LOG_FUNCTION (this);

  m_rrc->Dispose ();
  m_rrc = 0;

  m_componentCarrierManager->Dispose ();
  m_componentCarrierManager = 0;
  // newradioComponentCarrierEnb::DoDispose() will call DoDispose
  // of its PHY, MAC, FFR and scheduler instance
  for (uint32_t i = 0; i < m_ccMap.size (); i++)
    {
      m_ccMap.at (i)->Dispose ();
      m_ccMap.at (i) = 0;
    }

  newradioNetDevice::DoDispose ();
}

Ptr<newradioEnbPhy>
newradioEnbNetDevice::GetPhy (void) const
{
  NS_LOG_FUNCTION (this);
  return m_ccMap.at (0)->GetPhy ();
}

Ptr<newradioEnbPhy>
newradioEnbNetDevice::GetPhy (uint8_t index)
{
  return m_ccMap.at (index)->GetPhy ();
}


uint16_t
newradioEnbNetDevice::GetCellId () const
{
  NS_LOG_FUNCTION (this);
  return m_cellId;
}

bool
newradioEnbNetDevice::HasCellId (uint16_t cellId) const
{
  for (auto &it : m_ccMap)
    {
      if (it.second->GetCellId () == cellId)
        {
          return true;
        }
    }
  return false;
}

uint8_t
newradioEnbNetDevice::GetBandwidth () const
{
  NS_LOG_FUNCTION (this);
  return m_Bandwidth;
}

void
newradioEnbNetDevice::SetBandwidth (uint8_t bw)
{
  NS_LOG_FUNCTION (this << bw);
  m_Bandwidth = bw;
}

void
newradioEnbNetDevice::SetEarfcn (uint16_t earfcn)
{
  NS_LOG_FUNCTION (this << earfcn);
  m_Earfcn = earfcn;
}

uint16_t
newradioEnbNetDevice::GetEarfcn () const
{
  NS_LOG_FUNCTION (this);
  return m_Earfcn;

}

Ptr<newradioEnbMac>
newradioEnbNetDevice::GetMac (void)
{
  return m_ccMap.at (0)->GetMac ();
}

Ptr<newradioEnbMac>
newradioEnbNetDevice::GetMac (uint8_t index)
{
  return m_ccMap.at (index)->GetMac ();
}

void
newradioEnbNetDevice::SetRrc (Ptr<LteEnbRrc> rrc)
{
  m_rrc = rrc;
}

Ptr<LteEnbRrc>
newradioEnbNetDevice::GetRrc (void)
{
  return m_rrc;
}

void
newradioEnbNetDevice::SetAntennaNum (uint16_t antennaNum)
{
  NS_ASSERT_MSG (std::floor (std::sqrt(antennaNum)) == std::sqrt(antennaNum), "Only square antenna arrays are currently supported.");
  m_antennaNum = antennaNum;
}
uint16_t
newradioEnbNetDevice::GetAntennaNum () const
{
  return m_antennaNum;
}

bool
newradioEnbNetDevice::DoSend (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (this << packet   << dest << protocolNumber);
  NS_ABORT_MSG_IF (protocolNumber != Ipv4L3Protocol::PROT_NUMBER
                   && protocolNumber != Ipv6L3Protocol::PROT_NUMBER,
                   "unsupported protocol " << protocolNumber << ", only IPv4/IPv6 is supported");
  return m_rrc->SendData (packet);
}

void
newradioEnbNetDevice::UpdateConfig (void)
{
  NS_LOG_FUNCTION (this);

  if (m_isConstructed)
    {
      if (!m_isConfigured)
        {
          NS_LOG_LOGIC (this << " Configure cell " << m_cellId);
          // we have to make sure that this function is called only once
          //m_rrc->ConfigureCell (m_Bandwidth, m_Bandwidth, m_Earfcn, m_Earfcn, m_cellId);
          NS_ASSERT (!m_ccMap.empty ());

          // create the newradioComponentCarrierConf map used for the RRC setup
          std::map<uint8_t, LteEnbRrc::newradioComponentCarrierConf> ccConfMap;
          for (std::map<uint8_t,Ptr<newradioComponentCarrierEnb> >::iterator it = m_ccMap.begin (); it != m_ccMap.end (); ++it)
            {
              LteEnbRrc::newradioComponentCarrierConf ccConf;
              ccConf.m_ccId = it->second->GetConfigurationParameters ()->GetCcId ();
              ccConf.m_cellId = it->second->GetCellId ();
              ccConf.m_bandwidth = it->second->GetBandwidth ();

              ccConfMap[it->first] = ccConf;
            }

          m_rrc->ConfigureCell (ccConfMap);
          m_isConfigured = true;
        }

      //m_rrc->SetCsgId (m_csgId, m_csgIndication);
    }
  else
    {
      /*
      * Lower layers are not ready yet, so do nothing now and expect
      * ``DoInitialize`` to re-invoke this function.
      */
    }
}

std::map < uint8_t, Ptr<newradioComponentCarrierEnb> >
newradioEnbNetDevice::GetCcMap ()
{
  return m_ccMap;
}

void
newradioEnbNetDevice::SetCcMap (std::map< uint8_t, Ptr<newradioComponentCarrierEnb> > ccm)
{
  NS_ASSERT_MSG (!m_isConfigured, "attempt to set CC map after configuration");
  m_ccMap = ccm;
}

}
}
