#ifndef SRC_newradio_MODEL_newradio_RADIO_BEARER_TAG_H_
#define SRC_newradio_MODEL_newradio_RADIO_BEARER_TAG_H_


#include "ns3/tag.h"

namespace ns3 {

class Tag;

namespace newradio {


/**
 * Tag used to define the RNTI and LC id for each MAC packet trasmitted
 */

class newradioRadioBearerTag : public Tag
{
public:
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  /**
   * Create an empty newradioRadioBearerTag
   */
  newradioRadioBearerTag ();

  /**
   * Create a newradioRadioBearerTag with the given RNTI and LC id
   */
  newradioRadioBearerTag (uint16_t  rnti, uint8_t lcId, uint32_t size);

  /**
  * Create a newradioRadioBearerTag with the given RNTI, LC id and layer
  */
  newradioRadioBearerTag (uint16_t  rnti, uint8_t lcId, uint32_t size, uint8_t layer);

  /**
   * Set the RNTI to the given value.
   *
   * @param rnti the value of the RNTI to set
   */
  void SetRnti (uint16_t rnti);

  /**
   * Set the LC id to the given value.
   *
   * @param lcid the value of the RNTI to set
   */
  void SetLcid (uint8_t lcid);

  /**
  * Set the layer id to the given value.
  *
  * @param layer the value of the layer to set
  */
  void SetLayer (uint8_t layer);

  /**
  * Set the size of the RLC PDU in bytes.
  *
  * @param size the size of the RLC PDU in bytes
  */
  void SetSize (uint32_t size);


  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual uint32_t GetSerializedSize () const;
  virtual void Print (std::ostream &os) const;

  uint16_t GetRnti (void) const;
  uint8_t GetLcid (void) const;
  uint8_t GetLayer (void) const;
  uint32_t GetSize (void) const;

private:
  uint16_t m_rnti;
  uint8_t m_lcid;
  uint8_t m_layer;
  uint32_t        m_size;        // size in bytes of RLC PDU
};

} // namespace newradio

} // namespace ns3



#endif /* SRC_newradio_MODEL_newradio_RADIO_BEARER_TAG_H_ */
