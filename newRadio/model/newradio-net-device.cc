#include <ns3/simulator.h>
#include <ns3/callback.h>
#include <ns3/node.h>
#include <ns3/packet.h>
#include <ns3/log.h>
#include "ns3/ipv4-header.h"
#include <ns3/ipv4-l3-protocol.h>
#include "ns3/ipv6-header.h"
#include <ns3/ipv6-l3-protocol.h>
#include "newradio-net-device.h"


namespace ns3 {

namespace newradio {

NS_LOG_COMPONENT_DEFINE ("newradioNetDevice");

NS_OBJECT_ENSURE_REGISTERED (newradioNetDevice);

TypeId newradioNetDevice::GetTypeId ()
{
  static TypeId
    tid =
    TypeId ("ns3::newradioNetDevice")
    .SetParent<NetDevice> ()
    .AddAttribute ("Mtu", "The MAC-level Maximum Transmission Unit",
                   UintegerValue (30000),
                   MakeUintegerAccessor (&newradioNetDevice::SetMtu,
                                         &newradioNetDevice::GetMtu),
                   MakeUintegerChecker<uint16_t> ())
  ;

  return tid;
}

newradioNetDevice::newradioNetDevice (void)
{
  NS_LOG_FUNCTION (this);
}


newradioNetDevice::~newradioNetDevice (void)
{
  NS_LOG_FUNCTION (this);
}

void
newradioNetDevice::DoDispose (void)
{
  m_node = 0;
  NetDevice::DoDispose ();
}

void
newradioNetDevice::SetIfIndex (const uint32_t index)
{
  m_ifIndex = index;
}
uint32_t
newradioNetDevice::GetIfIndex (void) const
{
  return m_ifIndex;
}
Ptr<Channel>
newradioNetDevice::GetChannel (void) const
{
  return 0;
}
void
newradioNetDevice::SetAddress (Address address)
{
  NS_LOG_FUNCTION (this << address);
  m_macaddress = Mac64Address::ConvertFrom (address);
}
Address
newradioNetDevice::GetAddress (void) const
{
  NS_LOG_FUNCTION (this);
  return m_macaddress;
}
bool
newradioNetDevice::SetMtu (const uint16_t mtu)
{
  m_mtu = mtu;
  return true;
}
uint16_t
newradioNetDevice::GetMtu (void) const
{
  return m_mtu;
}
bool
newradioNetDevice::IsLinkUp (void) const
{
  return m_linkUp;
}
void
newradioNetDevice::AddLinkChangeCallback (Callback<void> callback)
{

}
bool
newradioNetDevice::IsBroadcast (void) const
{
  return false;
}
Address
newradioNetDevice::GetBroadcast (void) const
{
  return Mac48Address::GetBroadcast ();
}
bool
newradioNetDevice::IsMulticast (void) const
{
  return false;
}
Address
newradioNetDevice::GetMulticast (Ipv4Address multicastGroup) const
{
  return Mac48Address ("01:00:5e:00:00:00");
}
bool
newradioNetDevice::IsBridge (void) const
{
  return false;
}
bool
newradioNetDevice::IsPointToPoint (void) const
{
  return false;
}

bool
newradioNetDevice::SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber)
{
  NS_FATAL_ERROR ("Send from not supported");
  return false;
}

Ptr<Node>
newradioNetDevice::GetNode (void) const
{
  return m_node;
}

void
newradioNetDevice::SetNode (Ptr<Node> node)
{
  m_node = node;
}

bool
newradioNetDevice::NeedsArp (void) const
{
  return false;
}

Address
newradioNetDevice::GetMulticast (Ipv6Address addr) const
{
  Address dummy;
  return dummy;
}

void
newradioNetDevice::SetReceiveCallback (ReceiveCallback cb)
{
  NS_LOG_FUNCTION (this);
  m_rxCallback = cb;
}

void
newradioNetDevice::SetPromiscReceiveCallback (PromiscReceiveCallback cb)
{

}

bool
newradioNetDevice::SupportsSendFrom (void) const
{
  return false;
}

void
newradioNetDevice::Receive (Ptr<Packet> p)
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
      NS_ABORT_MSG ("newradioNetDevice::Receive - Unknown IP type...");
    }
}

bool
newradioNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
  bool ret = DoSend ( packet, dest, protocolNumber);
  return ret;
}

Ipv4Address
newradioNetDevice::GetPacketDestination (Ptr<Packet> packet)
{
  Ipv4Address dest_ip;
  Ptr<Packet> q = packet->Copy ();

  Ipv4Header ipHeader;
  q->PeekHeader (ipHeader);
  dest_ip = ipHeader.GetDestination ();
  return dest_ip;
}


}

}
