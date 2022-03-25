#include "newradio-component-carrier-enb.h"
#include <ns3/uinteger.h>
#include <ns3/boolean.h>
#include <ns3/simulator.h>
#include <ns3/log.h>
#include <ns3/abort.h>
#include <ns3/newradio-enb-phy.h>
#include <ns3/pointer.h>
#include <ns3/newradio-enb-mac.h>
#include <ns3/lte-ffr-algorithm.h>
#include <ns3/newradio-mac-scheduler.h>

namespace ns3 {

namespace newradio {

NS_LOG_COMPONENT_DEFINE ("newradioComponentCarrierEnb");
NS_OBJECT_ENSURE_REGISTERED (newradioComponentCarrierEnb);

TypeId newradioComponentCarrierEnb::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::newradioComponentCarrierEnb")
    .SetParent<newradioComponentCarrier> ()
    .AddConstructor<newradioComponentCarrierEnb> ()
    .AddAttribute ("newradioEnbPhy",
                   "The PHY associated to this ComponentCarrierEnb",
                   PointerValue (),
                   MakePointerAccessor (&newradioComponentCarrierEnb::m_phy),
                   MakePointerChecker <newradioEnbPhy> ())
    .AddAttribute ("newradioEnbMac",
                   "The MAC associated to this ComponentCarrierEnb",
                   PointerValue (),
                   MakePointerAccessor (&newradioComponentCarrierEnb::m_mac),
                   MakePointerChecker <newradioEnbMac> ())
    .AddAttribute ("newradioMacScheduler",
                   "The scheduler associated to this ComponentCarrierEnb",
                   PointerValue (),
                   MakePointerAccessor (&newradioComponentCarrierEnb::m_scheduler),
                   MakePointerChecker <newradioMacScheduler> ())
    /*
    .AddAttribute ("LteFfrAlgorithm",
                   "The FFR algorithm associated to this ComponentCarrierEnb",
                   PointerValue (),
                   MakePointerAccessor (&ComponentCarrierEnb::m_ffrAlgorithm),
                   MakePointerChecker <LteFfrAlgorithm> ())
                   */
  ;
  return tid;
}
newradioComponentCarrierEnb::newradioComponentCarrierEnb ()
{
  NS_LOG_FUNCTION (this);
}

newradioComponentCarrierEnb::~newradioComponentCarrierEnb (void)
{
  NS_LOG_FUNCTION (this);
}

void
newradioComponentCarrierEnb::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  if (m_phy)
    {
      m_phy->Dispose ();
      m_phy = 0;
    }
  if (m_mac)
    {
      m_mac->Dispose ();
      m_mac = 0;
    }
  if (m_scheduler)
    {
      m_scheduler->Dispose ();
      m_scheduler = 0;
    }
  /*
  if (m_ffrAlgorithm)
    {
      m_ffrAlgorithm->Dispose ();
      m_ffrAlgorithm = 0;
    }
 */
  Object::DoDispose ();
}


void
newradioComponentCarrierEnb::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);
  m_isConstructed = true;
  m_phy->Initialize ();
  m_mac->Initialize ();
  //m_ffrAlgorithm->Initialize ();
  m_scheduler->Initialize ();

}

uint16_t
newradioComponentCarrierEnb::GetCellId ()
{
  return m_cellId;
}

Ptr<newradioEnbPhy>
newradioComponentCarrierEnb::GetPhy ()
{
  NS_LOG_FUNCTION (this);
  return m_phy;
}

void
newradioComponentCarrierEnb::SetCellId (uint16_t cellId)
{
  NS_LOG_FUNCTION (this << cellId);
  m_cellId = cellId;
}

void
newradioComponentCarrierEnb::SetPhy (Ptr<newradioEnbPhy> s)
{
  NS_LOG_FUNCTION (this);
  m_phy = s;
}

Ptr<newradioEnbMac>
newradioComponentCarrierEnb::GetMac ()
{
  NS_LOG_FUNCTION (this);
  return m_mac;
}
void
newradioComponentCarrierEnb::SetMac (Ptr<newradioEnbMac> s)
{
  NS_LOG_FUNCTION (this);
  m_mac = s;
}

/*
Ptr<LteFfrAlgorithm>
ComponentCarrierEnb::GetFfrAlgorithm ()
{
  NS_LOG_FUNCTION (this);
  return m_ffrAlgorithm;
}

void
ComponentCarrierEnb::SetFfrAlgorithm (Ptr<LteFfrAlgorithm> s)
{
  NS_LOG_FUNCTION (this);
  m_ffrAlgorithm = s;
}
*/

Ptr<newradioMacScheduler>
newradioComponentCarrierEnb::GetMacScheduler ()
{
  NS_LOG_FUNCTION (this);
  return m_scheduler;
}

void
newradioComponentCarrierEnb::SetMacScheduler (Ptr<newradioMacScheduler> s)
{
  NS_LOG_FUNCTION (this);
  m_scheduler = s;
}

} // namespace newradio

} // namespace ns3
