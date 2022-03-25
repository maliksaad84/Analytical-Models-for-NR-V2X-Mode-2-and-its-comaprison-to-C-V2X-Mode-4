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
#include "newradio-enb-net-device.h"
#include "newradio-ue-net-device.h"
#include <ns3/ipv4-header.h>
#include <ns3/ipv4.h>
#include "newradio-ue-phy.h"
#include <ns3/ipv4-l3-protocol.h>
#include <ns3/ipv6-l3-protocol.h>
#include <ns3/log.h>
#include <ns3/lte-ue-component-carrier-manager.h>
#include <ns3/object-map.h>


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("newradioUeNetDevice");

namespace newradio {

NS_OBJECT_ENSURE_REGISTERED (newradioUeNetDevice);


TypeId
newradioUeNetDevice::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::newradioUeNetDevice")
    .SetParent<newradioNetDevice> ()
    .AddConstructor<newradioUeNetDevice> ()
    .AddAttribute ("EpcUeNas",
                   "The NAS associated to this UeNetDevice",
                   PointerValue (),
                   MakePointerAccessor (&newradioUeNetDevice::m_nas),
                   MakePointerChecker <EpcUeNas> ())
    .AddAttribute ("newradioUeRrc",
                   "The RRC associated to this UeNetDevice",
                   PointerValue (),
                   MakePointerAccessor (&newradioUeNetDevice::m_rrc),
                   MakePointerChecker <LteUeRrc> ())
    .AddAttribute ("LteUeComponentCarrierManager",
                   "The ComponentCarrierManager associated to this UeNetDevice",
                   PointerValue (),
                   MakePointerAccessor (&newradioUeNetDevice::m_componentCarrierManager),
                   MakePointerChecker <LteUeComponentCarrierManager> ())
    .AddAttribute ("ComponentCarrierMapUe", "List of all component Carrier.",
                   ObjectMapValue (),
                   MakeObjectMapAccessor (&newradioUeNetDevice::m_ccMap),
                   MakeObjectMapChecker<newradioComponentCarrierUe> ())
    .AddAttribute ("Imsi",
                   "International Mobile Subscriber Identity assigned to this UE",
                   UintegerValue (0),
                   MakeUintegerAccessor (&newradioUeNetDevice::m_imsi),
                   MakeUintegerChecker<uint64_t> ())
    .AddAttribute ("AntennaNum",
                   "Antenna number of the device",
                   UintegerValue (16),
                   MakeUintegerAccessor (&newradioUeNetDevice::SetAntennaNum,
                                         &newradioUeNetDevice::GetAntennaNum),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("LteUeRrc",
                   "The RRC layer associated with the ENB",
                   PointerValue (),
                   MakePointerAccessor (&newradioUeNetDevice::m_rrc),
                   MakePointerChecker <LteUeRrc> ())
  ;
  return tid;
}

newradioUeNetDevice::newradioUeNetDevice (void)
  : m_isConstructed (false)

{
  NS_LOG_FUNCTION (this);
}

newradioUeNetDevice::~newradioUeNetDevice (void)
{

}

void
newradioUeNetDevice::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);
  m_isConstructed = true;
  UpdateConfig ();

  std::map< uint8_t, Ptr<newradioComponentCarrierUe> >::iterator it;
  for (it = m_ccMap.begin (); it != m_ccMap.end (); ++it)
    {
      it->second->GetPhy ()->Initialize ();
      it->second->GetMac ()->Initialize ();
    }
  m_rrc->Initialize ();

}
void
newradioUeNetDevice::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_targetEnb = 0;

  m_rrc->Dispose ();
  m_rrc = 0;

  m_nas->Dispose ();
  m_nas = 0;

  for (uint32_t i = 0; i < m_ccMap.size (); i++)
    {
      m_ccMap.at (i)->Dispose ();
    }
  m_componentCarrierManager->Dispose ();
}

uint32_t
newradioUeNetDevice::GetCsgId () const
{
  NS_LOG_FUNCTION (this);
  return m_csgId;
}

void
newradioUeNetDevice::SetCsgId (uint32_t csgId)
{
  NS_LOG_FUNCTION (this << csgId);
  m_csgId = csgId;
  UpdateConfig (); // propagate the change down to NAS and RRC
}

void
newradioUeNetDevice::UpdateConfig (void)
{
  NS_LOG_FUNCTION (this);

  if (m_isConstructed)
    {
      NS_LOG_LOGIC (this << " Updating configuration: IMSI " << m_imsi
                         << " CSG ID " << m_csgId);
      m_nas->SetImsi (m_imsi);
      m_rrc->SetImsi (m_imsi);
      m_nas->SetCsgId (m_csgId); // this also handles propagation to RRC
    }
  else
    {
      /*
       * NAS and RRC instances are not be ready yet, so do nothing now and
       * expect ``DoInitialize`` to re-invoke this function.
       */
    }
}

bool
newradioUeNetDevice::DoSend (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (this << dest << protocolNumber);
  NS_ABORT_MSG_IF (protocolNumber != Ipv4L3Protocol::PROT_NUMBER
  		             && protocolNumber != Ipv6L3Protocol::PROT_NUMBER,
  		             "unsupported protocol " << protocolNumber << ", only IPv4 and IPv6 are supported");

  return m_nas->Send (packet, protocolNumber);
}

Ptr<newradioUePhy>
newradioUeNetDevice::GetPhy (void) const
{
  NS_LOG_FUNCTION (this);
  return m_ccMap.at (0)->GetPhy ();
}

Ptr<newradioUePhy>
newradioUeNetDevice::GetPhy (uint8_t index) const
{
  NS_LOG_FUNCTION (this);
  return m_ccMap.at (index)->GetPhy ();
}

Ptr<LteUeComponentCarrierManager>
newradioUeNetDevice::GetComponentCarrierManager (void) const
{
  NS_LOG_FUNCTION (this);
  return m_componentCarrierManager;
}

Ptr<newradioUeMac>
newradioUeNetDevice::GetMac (void) const
{
  NS_LOG_FUNCTION (this);
  return m_ccMap.at (0)->GetMac ();
}

Ptr<EpcUeNas>
newradioUeNetDevice::GetNas (void) const
{
  NS_LOG_FUNCTION (this);
  return m_nas;
}


Ptr<LteUeRrc>
newradioUeNetDevice::GetRrc (void) const
{
  NS_LOG_FUNCTION (this);
  return m_rrc;
}

uint64_t
newradioUeNetDevice::GetImsi () const
{
  NS_LOG_FUNCTION (this);
  return m_imsi;
}

uint16_t
newradioUeNetDevice::GetEarfcn () const
{
  NS_LOG_FUNCTION (this);
  return m_earfcn;
}

void
newradioUeNetDevice::SetEarfcn (uint16_t earfcn)
{
  NS_LOG_FUNCTION (this << earfcn);
  m_earfcn = earfcn;
}

void
newradioUeNetDevice::SetTargetEnb (Ptr<newradioEnbNetDevice> enb)
{
  NS_LOG_FUNCTION (this << enb);
  m_targetEnb = enb;
}

Ptr<newradioEnbNetDevice>
newradioUeNetDevice::GetTargetEnb (void)
{
  NS_LOG_FUNCTION (this);
  return m_targetEnb;
}

std::map < uint8_t, Ptr<newradioComponentCarrierUe> >
newradioUeNetDevice::GetCcMap ()
{
  return m_ccMap;
}

void
newradioUeNetDevice::SetCcMap (std::map< uint8_t, Ptr<newradioComponentCarrierUe> > ccm)
{
  m_ccMap = ccm;
}


uint16_t
newradioUeNetDevice::GetAntennaNum () const
{
  NS_LOG_FUNCTION (this);
  return m_antennaNum;
}

void
newradioUeNetDevice::SetAntennaNum (uint16_t antennaNum)
{
  NS_LOG_FUNCTION (this << antennaNum);
  NS_ASSERT_MSG (std::floor (std::sqrt(antennaNum)) == std::sqrt(antennaNum), "Only square antenna arrays are currently supported.");
  m_antennaNum = antennaNum;
}

}
}
