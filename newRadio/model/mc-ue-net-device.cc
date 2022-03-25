#include "mc-ue-net-device.h"
#include <ns3/log.h>
#include "ns3/trace-source-accessor.h"
#include "ns3/net-device.h"
#include "ns3/uinteger.h"
#include "ns3/double.h" //TODO remove when removing rng
#include "ns3/pointer.h"
#include "ns3/node.h"
#include "ns3/packet.h"
#include <ns3/ipv4-l3-protocol.h>
#include <ns3/ipv6-l3-protocol.h>
#include "ns3/newradio-enb-net-device.h"
#include "ns3/lte-enb-net-device.h"
#include <ns3/object-map.h>
#include <ns3/lte-ue-component-carrier-manager.h>


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McUeNetDevice");

namespace newradio {

NS_OBJECT_ENSURE_REGISTERED (McUeNetDevice);

////////////////////////////////
// McUeNetDevice
////////////////////////////////

TypeId McUeNetDevice::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::McUeNetDevice")
    .SetParent<NetDevice> ()
    .AddConstructor<McUeNetDevice> ()
    .AddAttribute ("Mtu", "The MAC-level Maximum Transmission Unit",
                   UintegerValue (30000),
                   MakeUintegerAccessor (&McUeNetDevice::SetMtu,
                                         &McUeNetDevice::GetMtu),
                   MakeUintegerChecker<uint16_t> ())
    // Common attributes
    .AddAttribute ("EpcUeNas",
                   "The NAS associated to the this NetDevice",
                   PointerValue (),
                   MakePointerAccessor (&McUeNetDevice::m_nas),
                   MakePointerChecker <EpcUeNas> ())
    .AddAttribute ("Imsi",
                   "International Mobile Subscriber Identity assigned to this UE",
                   UintegerValue (0),
                   MakeUintegerAccessor (&McUeNetDevice::m_imsi),
                   MakeUintegerChecker<uint64_t> ())
    .AddAttribute ("CsgId",
                   "The Closed Subscriber Group (CSG) identity that this UE is associated with, "
                   "i.e., giving the UE access to cells which belong to this particular CSG. "
                   "This restriction only applies to initial cell selection and EPC-enabled simulation. "
                   "This does not revoke the UE's access to non-CSG cells. ",
                   UintegerValue (0),
                   MakeUintegerAccessor (&McUeNetDevice::SetCsgId,
                                         &McUeNetDevice::GetCsgId),
                   MakeUintegerChecker<uint32_t> ())
    // LTE stack
    .AddAttribute ("LteUeRrc",
                   "The RRC associated to the LTE stack of this NetDevice",
                   PointerValue (),
                   MakePointerAccessor (&McUeNetDevice::m_lteRrc),
                   MakePointerChecker <LteUeRrc> ())
    .AddAttribute ("LteUeComponentCarrierManager",
                   "The LteComponentCarrierManager associated to this McUeNetDevice",
                   PointerValue (),
                   MakePointerAccessor (&McUeNetDevice::m_lteComponentCarrierManager),
                   MakePointerChecker <LteUeComponentCarrierManager> ())
    .AddAttribute ("LteComponentCarrierMapUe", "List of all LTE CCs.",
                   ObjectMapValue (),
                   MakeObjectMapAccessor (&McUeNetDevice::m_lteCcMap),
                   MakeObjectMapChecker<ComponentCarrierUe> ())
    .AddAttribute ("LteDlEarfcn",
                   "Downlink E-UTRA Absolute Radio Frequency Channel Number (EARFCN) "
                   "as per 3GPP 36.101 Section 5.7.3. ",
                   UintegerValue (100),
                   MakeUintegerAccessor (&McUeNetDevice::SetLteDlEarfcn,
                                         &McUeNetDevice::GetLteDlEarfcn),
                   MakeUintegerChecker<uint16_t> (0, 6149))
    // newradio stack attributes
    .AddAttribute ("newradioUeRrc",
                   "The RRC associated to the newradio stack of this NetDevice",
                   PointerValue (),
                   MakePointerAccessor (&McUeNetDevice::m_newradioRrc),
                   MakePointerChecker <LteUeRrc> ())
    .AddAttribute ("newradioUeComponentCarrierManager",
                   "The newradioComponentCarrierManager associated to this McUeNetDevice",
                   PointerValue (),
                   MakePointerAccessor (&McUeNetDevice::m_newradioComponentCarrierManager),
                   MakePointerChecker <LteUeComponentCarrierManager> ())
    .AddAttribute ("newradioComponentCarrierMapUe", "List of all newradio CCs.",
                   ObjectMapValue (),
                   MakeObjectMapAccessor (&McUeNetDevice::m_newradioCcMap),
                   MakeObjectMapChecker<newradioComponentCarrierUe> ())
    .AddAttribute ("AntennaNum",
                   "Antenna number of the device",
                   UintegerValue (16),
                   MakeUintegerAccessor (&McUeNetDevice::SetAntennaNum,
                                         &McUeNetDevice::GetAntennaNum),
                   MakeUintegerChecker<uint16_t> ())
  ;
  return tid;
}

McUeNetDevice::McUeNetDevice ()
  : m_isConstructed (false),
    m_csgId (0)
{
  NS_LOG_FUNCTION (this);
  m_random = CreateObject<UniformRandomVariable> ();
  m_random->SetAttribute ("Min", DoubleValue (0.0));
  m_random->SetAttribute ("Max", DoubleValue (1.0));
}

McUeNetDevice::~McUeNetDevice ()
{
  NS_LOG_FUNCTION (this);
}

void
McUeNetDevice::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);
  m_isConstructed = true;
  UpdateConfig ();

  std::map< uint8_t, Ptr<ComponentCarrierUe> >::iterator lteIt;
  for (lteIt = m_lteCcMap.begin (); lteIt != m_lteCcMap.end (); ++lteIt)
    {
      lteIt->second->GetPhy ()->Initialize ();
      lteIt->second->GetMac ()->Initialize ();
    }

  std::map< uint8_t, Ptr<newradioComponentCarrierUe> >::iterator newradioIt;
  for (newradioIt = m_newradioCcMap.begin (); newradioIt != m_newradioCcMap.end (); ++newradioIt)
    {
      newradioIt->second->GetPhy ()->Initialize ();
      newradioIt->second->GetMac ()->Initialize ();
    }

  if (m_newradioRrc != 0)
    {
      m_newradioRrc->Initialize ();
    }

  m_lteRrc->Initialize ();
}


void
McUeNetDevice::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_lteTargetEnb = 0;
  m_lteRrc->Dispose ();
  m_lteRrc = 0;
  m_nas->Dispose ();
  m_nas = 0;

  for (uint32_t i = 0; i < m_lteCcMap.size (); i++)
    {
      m_lteCcMap.at (i)->Dispose ();
    }
  m_lteComponentCarrierManager->Dispose ();

  m_newradioTargetEnb = 0;
  if (m_newradioRrc != 0)
    {
      m_newradioRrc->Dispose ();
    }
  m_newradioRrc = 0;

  for (uint32_t i = 0; i < m_newradioCcMap.size (); i++)
    {
      m_newradioCcMap.at (i)->Dispose ();
    }
  m_newradioComponentCarrierManager->Dispose ();

  m_node = 0;
  NetDevice::DoDispose ();
}


Ptr<Channel>
McUeNetDevice::GetChannel (void) const
{
  NS_LOG_FUNCTION (this);
  // we can't return a meaningful channel here, because LTE devices using FDD
  // have actually two channels + one channel for newradio TDD.
  return 0;
}

void
McUeNetDevice::SetAddress (Address address)
{
  NS_LOG_FUNCTION (this << address);
  m_macaddress = Mac64Address::ConvertFrom (address);
}


Address
McUeNetDevice::GetAddress (void) const
{
  NS_LOG_FUNCTION (this);
  return m_macaddress;
}

void
McUeNetDevice::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this << node);
  m_node = node;
}


Ptr<Node>
McUeNetDevice::GetNode (void) const
{
  NS_LOG_FUNCTION (this);
  return m_node;
}

void
McUeNetDevice::SetReceiveCallback (ReceiveCallback cb)
{
  NS_LOG_FUNCTION (this);
  m_rxCallback = cb;
}


bool
McUeNetDevice::SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber)
{
  NS_FATAL_ERROR ("SendFrom () not supported");
  return false;
}


bool
McUeNetDevice::SupportsSendFrom (void) const
{
  NS_LOG_FUNCTION (this);
  return false;
}


bool
McUeNetDevice::SetMtu (const uint16_t mtu)
{
  NS_LOG_FUNCTION (this << mtu);
  m_mtu = mtu;
  return true;
}

uint16_t
McUeNetDevice::GetMtu (void) const
{
  NS_LOG_FUNCTION (this);
  return m_mtu;
}


void
McUeNetDevice::SetIfIndex (const uint32_t index)
{
  NS_LOG_FUNCTION (this << index);
  m_ifIndex = index;
}

uint32_t
McUeNetDevice::GetIfIndex (void) const
{
  NS_LOG_FUNCTION (this);
  return m_ifIndex;
}


bool
McUeNetDevice::IsLinkUp (void) const
{
  NS_LOG_FUNCTION (this);
  return m_linkUp;
}


bool
McUeNetDevice::IsBroadcast (void) const
{
  NS_LOG_FUNCTION (this);
  return true; // TODO newradio is false, LTE is true
}

Address
McUeNetDevice::GetBroadcast (void) const
{
  NS_LOG_FUNCTION (this);
  return Mac48Address::GetBroadcast ();
}

bool
McUeNetDevice::IsMulticast (void) const
{
  NS_LOG_FUNCTION (this);
  return false;
}


bool
McUeNetDevice::IsPointToPoint (void) const
{
  NS_LOG_FUNCTION (this);
  return false;
}


bool
McUeNetDevice::NeedsArp (void) const
{
  NS_LOG_FUNCTION (this);
  return false;
}


bool
McUeNetDevice::IsBridge (void) const
{
  NS_LOG_FUNCTION (this);
  return false;
}

Address
McUeNetDevice::GetMulticast (Ipv4Address multicastGroup) const
{
  NS_LOG_FUNCTION (this << multicastGroup);

  Mac48Address ad = Mac48Address::GetMulticast (multicastGroup);

  //
  // Implicit conversion (operator Address ()) is defined for Mac48Address, so
  // use it by just returning the EUI-48 address which is automagically converted
  // to an Address.
  //
  NS_LOG_LOGIC ("multicast address is " << ad);

  return ad;
}

Address
McUeNetDevice::GetMulticast (Ipv6Address addr) const
{
  NS_LOG_FUNCTION (this << addr);
  Mac48Address ad = Mac48Address::GetMulticast (addr);

  NS_LOG_LOGIC ("MAC IPv6 multicast address is " << ad);
  return ad;
}

void
McUeNetDevice::AddLinkChangeCallback (Callback<void> callback)
{
  NS_LOG_FUNCTION (this);
  m_linkChangeCallbacks.ConnectWithoutContext (callback);
}


void
McUeNetDevice::SetPromiscReceiveCallback (PromiscReceiveCallback cb)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_WARN ("Promisc mode not supported");
}


void
McUeNetDevice::Receive (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << p);
  Ipv4Header ipv4Header;
  Ipv6Header ipv6Header;

  if (p->PeekHeader (ipv4Header) != 0)
    {
      NS_LOG_LOGIC ("IPv4 stack...");
      m_rxCallback (this, p, Ipv4L3Protocol::PROT_NUMBER, Address ());
    }
  else if  (p->PeekHeader (ipv6Header) != 0)
    {
      NS_LOG_LOGIC ("IPv6 stack...");
      m_rxCallback (this, p, Ipv6L3Protocol::PROT_NUMBER, Address ());
    }
  else
    {
      NS_ABORT_MSG ("McUeNetDevice::Receive - Unknown IP type...");
    }
}


bool
McUeNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
  bool ret = DoSend ( packet, dest, protocolNumber);
  return ret;
}

Ipv4Address
McUeNetDevice::GetPacketDestination (Ptr<Packet> packet)
{
  Ipv4Address dest_ip;
  Ptr<Packet> q = packet->Copy ();

  Ipv4Header ipHeader;
  q->PeekHeader (ipHeader);
  dest_ip = ipHeader.GetDestination ();
  return dest_ip;
}

void
McUeNetDevice::UpdateConfig (void)
{
  NS_LOG_FUNCTION (this);

  if (m_isConstructed)
    {
      NS_LOG_LOGIC (this << " Updating configuration: IMSI " << m_imsi
                         << " CSG ID " << m_csgId);
      m_nas->SetImsi (m_imsi);

      m_lteRrc->SetImsi (m_imsi);
      if (m_newradioRrc != 0)
        {
          m_newradioRrc->SetImsi (m_imsi);
        }

      m_nas->SetCsgId (m_csgId);           // TODO this also handles propagation to RRC (LTE only for now)
    }
  else
    {
      /*
       * NAS and RRC instances are not be ready yet, so do nothing now and
       * expect ``DoInitialize`` to re-invoke this function.
       */
    }
}


Ptr<LteUeMac>
McUeNetDevice::GetLteMac (void) const
{
  NS_LOG_FUNCTION (this);
  return m_lteCcMap.at (0)->GetMac ();
}

Ptr<LteUeMac>
McUeNetDevice::GetLteMac (uint8_t index) const
{
  NS_LOG_FUNCTION (this);
  return m_lteCcMap.at (index)->GetMac ();
}


Ptr<LteUeRrc>
McUeNetDevice::GetLteRrc (void) const
{
  NS_LOG_FUNCTION (this);
  return m_lteRrc;
}


Ptr<LteUePhy>
McUeNetDevice::GetLtePhy (void) const
{
  NS_LOG_FUNCTION (this);
  return m_lteCcMap.at (0)->GetPhy ();
}

Ptr<LteUePhy>
McUeNetDevice::GetLtePhy (uint8_t index) const
{
  NS_LOG_FUNCTION (this);
  return m_lteCcMap.at (index)->GetPhy ();
}

Ptr<LteUeComponentCarrierManager>
McUeNetDevice::GetLteComponentCarrierManager (void) const
{
  NS_LOG_FUNCTION (this);
  return m_lteComponentCarrierManager;
}

Ptr<EpcUeNas>
McUeNetDevice::GetNas (void) const
{
  NS_LOG_FUNCTION (this);
  return m_nas;
}

uint64_t
McUeNetDevice::GetImsi () const
{
  NS_LOG_FUNCTION (this);
  return m_imsi;
}

uint16_t
McUeNetDevice::GetLteDlEarfcn () const
{
  NS_LOG_FUNCTION (this);
  return m_lteDlEarfcn;
}

void
McUeNetDevice::SetLteDlEarfcn (uint16_t earfcn)
{
  NS_LOG_FUNCTION (this << earfcn);
  m_lteDlEarfcn = earfcn;
}

uint32_t
McUeNetDevice::GetCsgId () const
{
  NS_LOG_FUNCTION (this);
  return m_csgId;
}

void
McUeNetDevice::SetCsgId (uint32_t csgId)
{
  NS_LOG_FUNCTION (this << csgId);
  m_csgId = csgId;
  UpdateConfig (); // propagate the change down to NAS and RRC
}

void
McUeNetDevice::SetLteTargetEnb (Ptr<LteEnbNetDevice> enb)
{
  NS_LOG_FUNCTION (this << enb);
  m_lteTargetEnb = enb;
}


Ptr<LteEnbNetDevice>
McUeNetDevice::GetLteTargetEnb (void)
{
  NS_LOG_FUNCTION (this);
  return m_lteTargetEnb;
}

std::map < uint8_t, Ptr<ComponentCarrierUe> >
McUeNetDevice::GetLteCcMap ()
{
  return m_lteCcMap;
}

void
McUeNetDevice::SetLteCcMap (std::map< uint8_t, Ptr<ComponentCarrierUe> > ccm)
{
  m_lteCcMap = ccm;
}


Ptr<newradioUePhy>
McUeNetDevice::GetnewradioPhy (void) const
{
  return m_newradioCcMap.at (0)->GetPhy ();
}

Ptr<newradioUePhy>
McUeNetDevice::GetnewradioPhy (uint8_t index) const
{
  return m_newradioCcMap.at (index)->GetPhy ();
}

Ptr<newradioUeMac>
McUeNetDevice::GetnewradioMac (void) const
{
  return m_newradioCcMap.at (0)->GetMac ();
}

Ptr<newradioUeMac>
McUeNetDevice::GetnewradioMac (uint8_t index) const
{
  return m_newradioCcMap.at (index)->GetMac ();
}

Ptr<LteUeComponentCarrierManager>
McUeNetDevice::GetnewradioComponentCarrierManager (void) const
{
  NS_LOG_FUNCTION (this);
  return m_newradioComponentCarrierManager;
}


Ptr<LteUeRrc>
McUeNetDevice::GetnewradioRrc (void) const
{
  NS_LOG_FUNCTION (this);
  return m_newradioRrc;
}

uint16_t
McUeNetDevice::GetnewradioEarfcn () const
{
  return m_newradioEarfcn;
}

void
McUeNetDevice::SetnewradioEarfcn (uint16_t earfcn)
{
  m_newradioEarfcn = earfcn;
}

void
McUeNetDevice::SetnewradioTargetEnb (Ptr<newradioEnbNetDevice> enb)
{
  m_newradioTargetEnb = enb;
}

Ptr<newradioEnbNetDevice>
McUeNetDevice::GetnewradioTargetEnb (void)
{
  return m_newradioTargetEnb;
}

std::map < uint8_t, Ptr<newradioComponentCarrierUe> >
McUeNetDevice::GetnewradioCcMap ()
{
  return m_newradioCcMap;
}

void
McUeNetDevice::SetnewradioCcMap (std::map< uint8_t, Ptr<newradioComponentCarrierUe> > ccm)
{
  m_newradioCcMap = ccm;
}


uint16_t
McUeNetDevice::GetAntennaNum () const
{
  return m_newradioAntennaNum;
}

void
McUeNetDevice::SetAntennaNum (uint16_t antennaNum)
{
  NS_ASSERT_MSG (std::floor (std::sqrt(antennaNum)) == std::sqrt(antennaNum), "Only square antenna arrays are currently supported.");
  m_newradioAntennaNum = antennaNum;
}

bool
McUeNetDevice::DoSend (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (this << dest << protocolNumber);
  NS_ABORT_MSG_IF (protocolNumber != Ipv4L3Protocol::PROT_NUMBER
  		             && protocolNumber != Ipv6L3Protocol::PROT_NUMBER,
  		             "unsupported protocol " << protocolNumber << ", only IPv4 and IPv6 are supported");
  return m_nas->Send (packet, protocolNumber);
}

}
}
