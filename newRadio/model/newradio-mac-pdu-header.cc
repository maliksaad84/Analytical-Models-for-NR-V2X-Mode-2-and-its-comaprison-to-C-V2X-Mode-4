#include "newradio-mac-pdu-header.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3 {

namespace newradio {

NS_OBJECT_ENSURE_REGISTERED (newradioMacPduHeader);

newradioMacPduHeader::newradioMacPduHeader () : m_headerSize (0)
{
}

TypeId
newradioMacPduHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::newradioMacPduHeader")
    .SetParent<Header> ()
    .AddConstructor<newradioMacPduHeader> ();
  return tid;
}

TypeId
newradioMacPduHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
newradioMacPduHeader::GetSerializedSize (void) const
{
  return (m_headerSize);
}

void
newradioMacPduHeader::Serialize (Buffer::Iterator i) const
{
  // RDF TODO: implement BSR MAC control elements

  // this part builds the MAC sub-header format from 36.321 sec 6.1.2
  for (unsigned ipdu = 0; ipdu < m_subheaderList.size (); ipdu++)
    {
      // first octet R/R/E/LCID (R=reserved bit, E=extension bit)
      uint8_t octet1 = m_subheaderList[ipdu].m_lcid & 0x1F;
      if (ipdu < (m_subheaderList.size () - 1))          // not the last subheader
        {
          octet1 |= (1 << 5);
        }
      i.WriteU8 (octet1);
      // second octet F/Length (F=1 if length > 127 bits)
      uint8_t octet2 = (m_subheaderList[ipdu].m_size & 0x7F);
      // third octet = upper 8 bits of length if length > 127
      uint8_t octet3 = 0;
      if (m_subheaderList[ipdu].m_size >  0x7F)
        {
          octet2 |= (1 << 7);
          octet3 = ((m_subheaderList[ipdu].m_size >> 7) & 0x7F);
          i.WriteU8 (octet2);
          if (m_subheaderList[ipdu].m_size > 0x3FFF)
            {
              octet3 |= (1 << 7);                               // write extension flag 2
              uint8_t octet4 = ((m_subheaderList[ipdu].m_size >> 14) & 0xFF);
              i.WriteU8 (octet3);
              i.WriteU8 (octet4);
            }
          else
            {
              i.WriteU8 (octet3);
            }
        }
      else
        {
          i.WriteU8 (octet2);
        }
    }
}

uint32_t
newradioMacPduHeader::Deserialize (Buffer::Iterator i)
{
  // decode sub-headers and create RLC info elements
  m_headerSize = 0;
  bool done = false;
  while (!done)
    {
      uint8_t octet1 = (uint8_t)i.ReadU8 ();
      uint8_t lcid = (octet1 & 0x1F);
      if (!(octet1 >> 5))            // extension bit not set
        {
          done = true;
        }
      uint8_t octet2 = (uint8_t)i.ReadU8 ();
      bool flag = octet2 >> 7;
      uint32_t size = (octet2 & 0x7F);
      if (flag)           // size > 127
        {
          uint8_t octet3 = (uint8_t)i.ReadU8 ();
          size |= (octet3 & 0x7F) << 7;
          bool flag2 = (octet3 >> 7);
          if (flag2)
            {
              uint8_t octet4 = (uint8_t)i.ReadU8 ();
              size |= (octet4 << 14);
              m_headerSize += 4;
            }
          else
            {
              m_headerSize += 3;
            }
        }
      else
        {
          m_headerSize += 2;
        }
      m_subheaderList.push_back (MacSubheader (lcid, size));
    }

  return m_headerSize;
}

void
newradioMacPduHeader::Print (std::ostream &os) const
{
}

void
newradioMacPduHeader::AddSubheader (MacSubheader macSubheader)
{
  this->m_subheaderList.push_back (macSubheader);
  if (macSubheader.m_size > 0x3FFF)
    {
      m_headerSize += 4;
    }
  else if (macSubheader.m_size > 0x7F)
    {
      m_headerSize += 3;
    }
  else
    {
      m_headerSize += 2;
    }
}

} // namespace newradio

} // namespace ns3
