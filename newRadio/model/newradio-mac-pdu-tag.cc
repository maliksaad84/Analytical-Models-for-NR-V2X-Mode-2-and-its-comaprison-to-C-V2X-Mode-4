#include "newradio-mac-pdu-tag.h"
#include "newradio-phy-mac-common.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3 {

namespace newradio {

NS_OBJECT_ENSURE_REGISTERED (newradioMacPduTag);

newradioMacPduTag::newradioMacPduTag () : m_sfnSf (SfnSf ()),
                                      m_symStart (0),
                                      m_numSym (0),
                                      m_tagSize (6)
{
}

newradioMacPduTag::newradioMacPduTag (SfnSf sfn)
  :  m_sfnSf (sfn),
    m_symStart (0),
    m_numSym (0),
    m_tagSize (6)
{
}

newradioMacPduTag::newradioMacPduTag (SfnSf sfn, uint8_t symStart, uint8_t numSym)
  :  m_sfnSf (sfn),
    m_symStart (symStart),
    m_numSym (numSym),
    m_tagSize (6)
{
}

TypeId
newradioMacPduTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::newradioMacPduTag")
    .SetParent<Tag> ()
    .AddConstructor<newradioMacPduTag> ();
  return tid;
}

TypeId
newradioMacPduTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
newradioMacPduTag::GetSerializedSize (void) const
{
  return (m_tagSize);
}

void
newradioMacPduTag::Serialize (TagBuffer i) const
{
  i.WriteU16 (m_sfnSf.m_frameNum);
  i.WriteU8 (m_sfnSf.m_sfNum);
  i.WriteU8 (m_sfnSf.m_slotNum);
  i.WriteU8 (m_symStart);
  i.WriteU8 (m_numSym);
}

void
newradioMacPduTag::Deserialize (TagBuffer i)
{
  m_sfnSf.m_frameNum = (uint16_t)i.ReadU16 ();
  m_sfnSf.m_sfNum = (uint8_t)i.ReadU8 ();
  m_sfnSf.m_slotNum = (uint8_t)i.ReadU8 ();
  m_symStart = (uint8_t)i.ReadU8 ();
  m_numSym = (uint8_t)i.ReadU8 ();
  m_tagSize = 6;
}

void
newradioMacPduTag::Print (std::ostream &os) const
{
  os << m_sfnSf.m_sfNum << " " << m_sfnSf.m_slotNum;
}

} // namespace newradio

} // namespace ns3
