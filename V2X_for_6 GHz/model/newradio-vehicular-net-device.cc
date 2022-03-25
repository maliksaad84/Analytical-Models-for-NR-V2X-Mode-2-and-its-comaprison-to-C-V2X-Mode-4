#include <ns3/uinteger.h>
#include <ns3/log.h>
#include <ns3/object-map.h>
#include <ns3/ipv4-header.h>
#include <ns3/ipv4-l3-protocol.h>
#include <ns3/ipv6-header.h>
#include <ns3/ipv6-l3-protocol.h>
#include "ns3/epc-tft.h"
#include "ns3/lte-rlc-um.h"
#include "ns3/lte-rlc-tm.h"
#include "ns3/lte-radio-bearer-tag.h"
#include "newradio-sidelink-mac.h"
#include "newradio-vehicular-net-device.h"

namespace ns3 {

namespace millicar {

PdcpSpecificSidelinkPdcpSapUser::PdcpSpecificSidelinkPdcpSapUser (Ptr<newradioVehicularNetDevice> netDevice)
  : m_netDevice (netDevice)
{

}

void
PdcpSpecificSidelinkPdcpSapUser::ReceivePdcpSdu (ReceivePdcpSduParameters params)
{
  m_netDevice->Receive (params.pdcpSdu);
}

//-----------------------------------------------------------------------

NS_LOG_COMPONENT_DEFINE ("newradioVehicularNetDevice");

NS_OBJECT_ENSURE_REGISTERED (newradioVehicularNetDevice);

TypeId newradioVehicularNetDevice::GetTypeId ()
{
  static TypeId
    tid =
    TypeId ("ns3::newradioVehicularNetDevice")
    .SetParent<NetDevice> ()
    .AddAttribute ("SidelinkRadioBearerMap", "List of SidelinkRadioBearerMap by BID",
                   ObjectMapValue (),
                   MakeObjectMapAccessor (&newradioVehicularNetDevice::m_bearerToInfoMap),
                   MakeObjectMapChecker<SidelinkRadioBearerInfo> ())
    .AddAttribute ("Mtu", "The MAC-level Maximum Transmission Unit",
                   UintegerValue (30000),
                   MakeUintegerAccessor (&newradioVehicularNetDevice::SetMtu,
                                         &newradioVehicularNetDevice::GetMtu),
                   MakeUintegerChecker<uint16_t> ())
     .AddAttribute ("RlcType",
                  "Set the RLC mode to use (AM not supported for now)",
                   StringValue ("LteRlcTm"),
                   MakeStringAccessor (&newradioVehicularNetDevice::m_rlcType),
                   MakeStringChecker ())
  ;

  return tid;
}

newradioVehicularNetDevice::newradioVehicularNetDevice (void)
{
  NS_LOG_FUNCTION (this);
}

newradioVehicularNetDevice::newradioVehicularNetDevice (Ptr<newradioSidelinkPhy> phy, Ptr<newradioSidelinkMac> mac)
{
  NS_LOG_FUNCTION (this);
  m_phy = phy;
  m_mac = mac;
}

newradioVehicularNetDevice::~newradioVehicularNetDevice (void)
{
  NS_LOG_FUNCTION (this);
}

void
newradioVehicularNetDevice::DoDispose (void)
{
  NetDevice::DoDispose ();
}

void
newradioVehicularNetDevice::SetIfIndex (const uint32_t index)
{
  m_ifIndex = index;
}

uint32_t
newradioVehicularNetDevice::GetIfIndex (void) const
{
  return m_ifIndex;
}

Ptr<Channel>
newradioVehicularNetDevice::GetChannel (void) const
{
  return 0;
}

bool
newradioVehicularNetDevice::IsLinkUp (void) const
{
  return m_linkUp;
}

void
newradioVehicularNetDevice::AddLinkChangeCallback (Callback<void> callback)
{
}

bool
newradioVehicularNetDevice::IsBroadcast (void) const
{
  return true;
}

Address
newradioVehicularNetDevice::GetBroadcast (void) const
{
  return Mac48Address::GetBroadcast ();
}

bool
newradioVehicularNetDevice::IsMulticast (void) const
{
  return false;
}

Address
newradioVehicularNetDevice::GetMulticast (Ipv4Address multicastGroup) const
{
  return Mac48Address ("01:00:5e:00:00:00");
}

bool
newradioVehicularNetDevice::IsBridge (void) const
{
  return false;
}

bool
newradioVehicularNetDevice::IsPointToPoint (void) const
{
  return false;
}

bool
newradioVehicularNetDevice::SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber)
{
  NS_FATAL_ERROR ("Send from not supported");
  return false;
}

Ptr<Node>
newradioVehicularNetDevice::GetNode (void) const
{
  return m_node;
}

void
newradioVehicularNetDevice::SetNode (Ptr<Node> node)
{
  m_node = node;
}

bool
newradioVehicularNetDevice::NeedsArp (void) const
{
  return false;
}

Address
newradioVehicularNetDevice::GetMulticast (Ipv6Address addr) const
{
  Address dummy;
  return dummy;
}

void
newradioVehicularNetDevice::SetReceiveCallback (ReceiveCallback cb)
{
  NS_LOG_FUNCTION (this);
  m_rxCallback = cb;
}

void
newradioVehicularNetDevice::SetPromiscReceiveCallback (PromiscReceiveCallback cb)
{

}

bool
newradioVehicularNetDevice::SupportsSendFrom (void) const
{
  return false;
}

void
newradioVehicularNetDevice::SetAddress (Address address)
{
  NS_LOG_FUNCTION (this << address);
  m_macAddr = Mac64Address::ConvertFrom (address);
}

Address
newradioVehicularNetDevice::GetAddress (void) const
{
  NS_LOG_FUNCTION (this);
  return m_macAddr;
}

Ptr<newradioSidelinkMac>
newradioVehicularNetDevice::GetMac (void) const
{
  NS_LOG_FUNCTION (this);
  return m_mac;
}

Ptr<newradioSidelinkPhy>
newradioVehicularNetDevice::GetPhy (void) const
{
  NS_LOG_FUNCTION (this);
  return m_phy;
}

bool
newradioVehicularNetDevice::SetMtu (const uint16_t mtu)
{
  m_mtu = mtu;
  return true;
}

uint16_t
newradioVehicularNetDevice::GetMtu (void) const
{
  return m_mtu;
}

TypeId
newradioVehicularNetDevice::GetRlcType (std::string rlcType)
{
  if (rlcType == "LteRlcSm")
  {
    return LteRlcSm::GetTypeId ();
  }
  else if (rlcType == "LteRlcUm")
  {
    return LteRlcUm::GetTypeId ();
  }
  else if (rlcType == "LteRlcTm")
  {
    return LteRlcTm::GetTypeId ();
  }
  else
  {
    NS_FATAL_ERROR ("Unknown RLC type");
  }
}

void
newradioVehicularNetDevice::ActivateBearer(const uint8_t bearerId, const uint16_t destRnti, const Address& dest)
{
  NS_LOG_FUNCTION(this << bearerId);
  uint8_t lcid = bearerId; // set the LCID to be equal to the bearerId
  // future extensions could consider a different mapping
  // and will actually exploit the following map
  m_bid2lcid.insert(std::make_pair(bearerId, lcid));

  NS_ASSERT_MSG(m_bearerToInfoMap.find (bearerId) == m_bearerToInfoMap.end (),
    "There's another bearer associated to this bearerId: " << uint32_t(bearerId));

  EpcTft::PacketFilter slFilter;
  slFilter.remoteAddress= Ipv4Address::ConvertFrom(dest);

  Ptr<Node> node = GetNode ();
  Ptr<Ipv4> nodeIpv4 = node->GetObject<Ipv4> ();
  int32_t interface =  nodeIpv4->GetInterfaceForDevice (this);
  Ipv4Address src = nodeIpv4->GetAddress (interface, 0).GetLocal ();
  slFilter.localAddress= Ipv4Address::ConvertFrom(src);
  //slFilter.direction= EpcTft::DOWNLINK;
  slFilter.remoteMask= Ipv4Mask("255.255.255.255");
  slFilter.localMask= Ipv4Mask("255.255.255.255");

  NS_LOG_DEBUG(this << " Add filter for " << Ipv4Address::ConvertFrom(dest));

  Ptr<EpcTft> tft = Create<EpcTft> (); // Create a new tft
  tft->Add (slFilter); // Add the packet filter

  m_tftClassifier.Add(tft, bearerId);

  // Create RLC instance with specific RNTI and LCID
  ObjectFactory rlcObjectFactory;
  rlcObjectFactory.SetTypeId (GetRlcType(m_rlcType));
  Ptr<LteRlc> rlc = rlcObjectFactory.Create ()->GetObject<LteRlc> ();

  rlc->SetLteMacSapProvider (m_mac->GetMacSapProvider());
  rlc->SetRnti (destRnti); // this is the rnti of the destination
  rlc->SetLcId (lcid);

  // Call to the MAC method that created the SAP for binding the MAC instance on this node to the RLC instance just created
  m_mac->AddMacSapUser(lcid, rlc->GetLteMacSapUser());

  Ptr<LtePdcp> pdcp = CreateObject<LtePdcp> ();
  pdcp->SetRnti (destRnti); // this is the rnti of the destination
  pdcp->SetLcId (lcid);

  // Create the PDCP SAP that connects the PDCP instance to this NetDevice
  LtePdcpSapUser* pdcpSapUser = new PdcpSpecificSidelinkPdcpSapUser (this);
  pdcp->SetLtePdcpSapUser (pdcpSapUser);
  pdcp->SetLteRlcSapProvider (rlc->GetLteRlcSapProvider ());
  rlc->SetLteRlcSapUser (pdcp->GetLteRlcSapUser ());
  rlc->Initialize (); // this is needed to trigger the BSR procedure if RLC SM is selected

  Ptr<SidelinkRadioBearerInfo> rbInfo = CreateObject<SidelinkRadioBearerInfo> ();
  rbInfo->m_rlc= rlc;
  rbInfo->m_pdcp = pdcp;
  rbInfo->m_rnti = destRnti;

  NS_LOG_DEBUG(this << " newradioVehicularNetDevice::ActivateBearer() bid: " << (uint32_t)bearerId << " rnti: " << destRnti);

  // insert the tuple <lcid, pdcpSapProvider> in the map of this NetDevice, so that we are able to associate it to them later
  m_bearerToInfoMap.insert (std::make_pair (bearerId, rbInfo));
}

void
newradioVehicularNetDevice::Receive (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << p);
  NS_LOG_DEBUG ("Received packet at: " << Simulator::Now().GetSeconds() << "s");
  uint8_t ipType;

  p->CopyData (&ipType, 1);
  ipType = (ipType>>4) & 0x0f;

  if (ipType == 0x04)
  {
    m_rxCallback (this, p, Ipv4L3Protocol::PROT_NUMBER, Address ());
  }
  else if (ipType == 0x06)
  {
    m_rxCallback (this, p, Ipv6L3Protocol::PROT_NUMBER, Address ());
  }
  else
  {
    NS_ABORT_MSG ("newradioVehicularNetDevice::Receive - Unknown IP type...");
  }
}

bool
newradioVehicularNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (this);

  // classify the incoming packet
  uint32_t id = m_tftClassifier.Classify (packet, EpcTft::UPLINK, protocolNumber);
  NS_ASSERT ((id & 0xFFFFFF00) == 0);
  uint8_t bid = (uint8_t) (id & 0x000000FF);
  uint8_t lcid = BidToLcid(bid);

  // get the SidelinkRadioBearerInfo
  NS_ASSERT_MSG(m_bearerToInfoMap.find (bid) != m_bearerToInfoMap.end (), "No logical channel associated to this communication");
  auto bearerInfo = m_bearerToInfoMap.find (bid)->second;

  LtePdcpSapProvider::TransmitPdcpSduParameters params;
  params.pdcpSdu = packet;
  params.rnti = bearerInfo->m_rnti;
  params.lcid = lcid;

  NS_LOG_DEBUG(this << " newradioVehicularNetDevice::Send() bid " << (uint32_t)bid << " lcid " << (uint32_t)lcid << " rnti " << bearerInfo->m_rnti);

  packet->RemoveAllPacketTags (); // remove all tags in case there is any

  params.pdcpSdu = packet;
  bearerInfo->m_pdcp->GetLtePdcpSapProvider()->TransmitPdcpSdu (params);

  return true;
}

uint8_t
newradioVehicularNetDevice::BidToLcid(const uint8_t bearerId) const
{
  NS_ASSERT_MSG(m_bid2lcid.find(bearerId) != m_bid2lcid.end(),
    "BearerId to LCID mapping not found " << bearerId);
  return m_bid2lcid.find(bearerId)->second;
}

}

}
