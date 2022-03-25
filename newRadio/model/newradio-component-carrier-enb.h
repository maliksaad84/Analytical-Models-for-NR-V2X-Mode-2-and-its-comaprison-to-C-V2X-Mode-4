#ifndef newradio_COMPONENT_CARRIER_ENB_H
#define newradio_COMPONENT_CARRIER_ENB_H

#include "newradio-component-carrier.h"
#include <ns3/object.h>
#include <ns3/packet.h>
#include <ns3/nstime.h>
#include "ns3/newradio-phy.h"
#include <ns3/newradio-enb-phy.h>
#include <ns3/pointer.h>
//#include <ns3/lte-enb-mac.h>


namespace ns3 {

namespace newradio {

class newradioEnbMac;
/**
 * \ingroup newradio
 *
 * Defines a single carrier for enb, and contains pointers to newradioEnbPhy,
 * newradioEnbMac objects.
 *
 */
class newradioComponentCarrierEnb : public newradioComponentCarrier
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  newradioComponentCarrierEnb ();

  virtual ~newradioComponentCarrierEnb (void);
  virtual void DoDispose (void);

  /**
   * Get cell identifier
   * \return cell identifer
   */
  uint16_t GetCellId ();

  /**
   * \return a pointer to the physical layer.
   */
  Ptr<newradioEnbPhy> GetPhy (void);

  /**
   * \return a pointer to the MAC layer.
   */
  Ptr<newradioEnbMac> GetMac (void);

  /**
   * \return a pointer to the Ffr Algorithm.
   */
  //Ptr<LteFfrAlgorithm> GetFfrAlgorithm ();

  /**
   * \return a pointer to the Mac Scheduler.
   */
  Ptr<newradioMacScheduler> GetMacScheduler ();

  /**
   * Set physical cell identifier
   * \param cellId cell identifier
   */
  void SetCellId (uint16_t cellId);

  /**
   * Set the LteEnbPhy
   * \param s a pointer to the LteEnbPhy
   */
  void SetPhy (Ptr<newradioEnbPhy> s);
  /**
   * Set the LteEnbMac
   * \param s a pointer to the LteEnbMac
   */
  void SetMac (Ptr<newradioEnbMac> s);

  /**
   * Set the FfMacScheduler Algorithm
   * \param s a pointer to the FfMacScheduler
   */
  void SetMacScheduler (Ptr<newradioMacScheduler> s);

  /**
   * Set the LteFfrAlgorithm
   * \param s a pointer to the LteFfrAlgorithm
   */
  //void SetFfrAlgorithm (Ptr<LteFfrAlgorithm> s);

protected:
  virtual void DoInitialize (void);

private:
  uint16_t m_cellId; ///< Cell identifer
  Ptr<newradioEnbPhy> m_phy; ///< the Phy instance of this eNodeB component carrier
  Ptr<newradioEnbMac> m_mac; ///< the MAC instance of this eNodeB component carrier
  Ptr<newradioMacScheduler> m_scheduler; ///< the scheduler instance of this eNodeB component carrier
  //Ptr<LteFfrAlgorithm> m_ffrAlgorithm; ///< the FFR algorithm instance of this eNodeB component carrier


};

} // namespace newradio

} // namespace ns3



#endif /* newradio_COMPONENT_CARRIER_H */
