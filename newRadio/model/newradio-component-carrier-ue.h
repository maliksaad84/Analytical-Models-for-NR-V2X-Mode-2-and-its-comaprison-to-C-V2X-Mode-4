#ifndef newradio_COMPONENT_CARRIER_UE_H
#define newradio_COMPONENT_CARRIER_UE_H

#include <ns3/object.h>
#include <ns3/packet.h>
#include <ns3/nstime.h>
#include "ns3/newradio-phy.h"
#include <ns3/newradio-ue-phy.h>
#include <ns3/newradio-component-carrier.h>

namespace ns3 {

namespace newradio {

class newradioUeMac;
/**
 * \ingroup lte
 *
 * ComponentCarrierUe Object, it defines a single Carrier for the Ue
 */
class newradioComponentCarrierUe : public newradioComponentCarrier
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  newradioComponentCarrierUe ();

  virtual ~newradioComponentCarrierUe (void);
  virtual void DoDispose (void);


  /**
   * \return a pointer to the physical layer.
   */
  Ptr<newradioUePhy> GetPhy (void) const;

  /**
   * \return a pointer to the MAC layer.
   */
  Ptr<newradioUeMac> GetMac (void) const;

  /**
   * Set newradioUePhy
   * \param s a pointer to the newradioUePhy
   */
  void SetPhy (Ptr<newradioUePhy> s);

  /**
   * Set the newradioEnbMac
   * \param s a pointer to the newradioEnbMac
   */
  void SetMac (Ptr<newradioUeMac> s);

protected:
  // inherited from Object
  virtual void DoInitialize (void);

private:
  Ptr<newradioUePhy> m_phy; ///< the Phy instance of this eNodeB component carrier
  Ptr<newradioUeMac> m_mac; ///< the MAC instance of this eNodeB component carrier

};

} // namespace newradio

} // namespace ns3



#endif /* newradio_COMPONENT_CARRIER_UE_H */
