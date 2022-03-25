#ifndef newradio_MAC_PDU_HEADER_H
#define newradio_MAC_PDU_HEADER_H

#include "ns3/packet.h"
#include "ns3/nstime.h"

namespace ns3 {

class Tag;

namespace newradio {

struct MacSubheader
{
  MacSubheader ()
    : m_lcid (0),
      m_size (0)
  {

  }

  MacSubheader (uint8_t lcid, uint32_t size)
    : m_lcid (lcid),
      m_size (size)
  {
  }

  uint32_t GetSize ()
  {
    if (m_size > 127)
      {
        return 3;
      }
    else
      {
        return 2;
      }
  }

  uint8_t   m_lcid;
  uint32_t  m_size;             // 22 bits
};

class newradioMacPduHeader : public Header
{
public:
  static TypeId  GetTypeId (void);
  virtual TypeId  GetInstanceTypeId (void) const;

  /**
   * Create an empty Mac header
   */
  newradioMacPduHeader ();

  newradioMacPduHeader (uint16_t frameNo, uint8_t sfNo, uint8_t slotNo);

  virtual void  Serialize (Buffer::Iterator i) const;
  virtual uint32_t  Deserialize (Buffer::Iterator i);
  virtual uint32_t  GetSerializedSize () const;
  virtual void Print (std::ostream &os) const;
  void  AddSubheader (MacSubheader rlcPduInfo);

  void SetSubheaders (std::vector<MacSubheader> macSubheaderList)
  {
    m_subheaderList = macSubheaderList;
  }

  std::vector<MacSubheader> GetSubheaders (void)
  {
    return m_subheaderList;
  }

protected:
  std::vector<MacSubheader> m_subheaderList;
  uint32_t m_headerSize;
};

} // namespace newradio

} // namespace ns3

#endif /* newradio_MAC_PDU_HEADER_H */
