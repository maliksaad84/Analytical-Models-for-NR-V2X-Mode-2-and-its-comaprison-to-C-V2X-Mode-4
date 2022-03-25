#define newradio_COMPONENT_CARRIER_H

#include <ns3/object.h>
#include <ns3/packet.h>
#include <ns3/nstime.h>
#include "ns3/newradio-phy.h"
#include <ns3/newradio-enb-phy.h>
#include <ns3/pointer.h>

namespace ns3 {

namespace newradio {

/**
 * \ingroup lte
 *
 * ComponentCarrier Object, it defines a single Carrier
 * This is the parent class for both ComponentCarrierEnb
 * and ComponentCarrierUe.
 * This class contains the main physical configuration
 * parameters for a carrier. Does not contain pointers to
 * the MAC/PHY objects of the carrier.

 */
class newradioComponentCarrier : public Object
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  newradioComponentCarrier ();

  virtual ~newradioComponentCarrier (void);
  virtual void DoDispose (void);

  /**
   * \return the bandwidth in RBs
   */
  uint32_t GetBandwidth () const;

  /**
   * \return the carrier frequency in Hz
   */
  double GetCenterFrequency () const;

  /**
   * \brief Returns the CSG ID of the eNodeB.
   * \return the Closed Subscriber Group identity
   * \sa LteEnbNetDevice::SetCsgId
   */
  uint32_t GetCsgId () const;

  /**
   * \brief Associate the eNodeB device with a particular CSG.
   * \param csgId the intended Closed Subscriber Group identity
   *
   * CSG identity is a number identifying a Closed Subscriber Group which the
   * cell belongs to. eNodeB is associated with a single CSG identity.
   *
   * The same CSG identity can also be associated to several UEs, which is
   * equivalent as enlisting these UEs as the members of this particular CSG.
   *
   * \sa LteEnbNetDevice::SetCsgIndication
   */
  void SetCsgId (uint32_t csgId);

  /**
   * \brief Returns the CSG indication flag of the eNodeB.
   * \return the CSG indication flag
   * \sa LteEnbNetDevice::SetCsgIndication
   */
  bool GetCsgIndication () const;

  /**
   * \brief Enable or disable the CSG indication flag.
   * \param csgIndication if TRUE, only CSG members are allowed to access this
   *                      cell
   *
   * When the CSG indication field is set to TRUE, only UEs which are members of
   * the CSG (i.e. same CSG ID) can gain access to the eNodeB, therefore
   * enforcing closed access mode. Otherwise, the eNodeB operates as a non-CSG
   * cell and implements open access mode.
   *
   * \note This restriction only applies to initial cell selection and
   *       EPC-enabled simulation.
   *
   * \sa LteEnbNetDevice::SetCsgIndication
   */
  void SetCsgIndication (bool csgIndication);

  /**
   * \brief Set as primary carrier
   * \param primaryCarrier true to set as primary carrier
   */
  void SetAsPrimary (bool primaryCarrier);

  /**
   * \brief Checks if the carrier is the primary carrier
   * \returns true if the carrier is primary
   */
  bool IsPrimary () const;


  void SetConfigurationParameters (Ptr<newradioPhyMacCommon> ptrConfig);

  Ptr<newradioPhyMacCommon> GetConfigurationParameters (void) const;

protected:
  // inherited from Object
  virtual void DoInitialize (void);
  uint16_t m_csgId; ///< CSG ID
  bool m_csgIndication; ///< CSG indication

  bool m_primaryCarrier; ///< whether the carrier is primary
  bool m_isConstructed; ///< whether the instance is constructed
//    bool m_isConfigured;
  Ptr<newradioPhyMacCommon> m_phyMacConfig;

};

} // namespace newradio

} // namespace ns3



#endif /* newradio_COMPONENT_CARRIER_H */
