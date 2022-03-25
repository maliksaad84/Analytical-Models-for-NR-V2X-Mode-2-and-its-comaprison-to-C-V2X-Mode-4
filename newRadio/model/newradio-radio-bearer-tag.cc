#include "newradio-radio-bearer-tag.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3 {

namespace newradio {

NS_OBJECT_ENSURE_REGISTERED (newradioRadioBearerTag);

TypeId
newradioRadioBearerTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::newradioRadioBearerTag")
    .SetParent<Tag> ()
    .AddConstructor<newradioRadioBearerTag> ()
    .AddAttribute ("rnti", "The rnti that indicates the UE to which packet belongs",
                   UintegerValue (0),
                   MakeUintegerAccessor (&newradioRadioBearerTag::GetRnti),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("lcid", "The id whithin the UE identifying the logical channel to which the packet belongs",
                   UintegerValue (0),
                   MakeUintegerAccessor (&newradioRadioBearerTag::GetLcid),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("size", "Size in bytes of the RLC PDU",
                   UintegerValue (0),
                   MakeUintegerAccessor (&newradioRadioBearerTag::GetSize),
                   MakeUintegerChecker<uint32_t> ())
  ;
  return tid;
}

TypeId
newradioRadioBearerTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

newradioRadioBearerTag::newradioRadioBearerTag ()
  : m_rnti (0),
    m_lcid (0),
    m_layer (0),
    m_size (0)
{
}
newradioRadioBearerTag::newradioRadioBearerTag (uint16_t rnti, uint8_t lcid, uint32_t size)
  : m_rnti (rnti),
    m_lcid (lcid),
    m_size (size)
{
}

newradioRadioBearerTag::newradioRadioBearerTag (uint16_t rnti, uint8_t lcid, uint32_t size, uint8_t layer)
  : m_rnti (rnti),
    m_lcid (lcid),
    m_layer (layer),
    m_size (size)
{
}

void
newradioRadioBearerTag::SetRnti (uint16_t rnti)
{
  m_rnti = rnti;
}

void
newradioRadioBearerTag::SetLcid (uint8_t lcid)
{
  m_lcid = lcid;
}

void
newradioRadioBearerTag::SetLayer (uint8_t layer)
{
  m_layer = layer;
}

void
newradioRadioBearerTag::SetSize (uint32_t size)
{
  m_size = size;
}

uint32_t
newradioRadioBearerTag::GetSerializedSize (void) const
{
  return 4;
}

void
newradioRadioBearerTag::Serialize (TagBuffer i) const
{
  i.WriteU16 (m_rnti);
  i.WriteU8 (m_lcid);
  i.WriteU8 (m_layer);
  i.WriteU32 (m_size);
}

void
newradioRadioBearerTag::Deserialize (TagBuffer i)
{
  m_rnti = (uint16_t) i.ReadU16 ();
  m_lcid = (uint8_t) i.ReadU8 ();
  m_layer = (uint8_t) i.ReadU8 ();
}

uint16_t
newradioRadioBearerTag::GetRnti () const
{
  return m_rnti;
}

uint8_t
newradioRadioBearerTag::GetLcid () const
{
  return m_lcid;
}

uint8_t
newradioRadioBearerTag::GetLayer () const
{
  return m_layer;
}

uint32_t
newradioRadioBearerTag::GetSize () const
{
  return m_size;
}

void
newradioRadioBearerTag::Print (std::ostream &os) const
{
  os << "rnti=" << m_rnti << ", lcid=" << (uint16_t) m_lcid << ", layer=" << (uint16_t)m_layer;
}

} // namespace newradio

} // namespace ns3
