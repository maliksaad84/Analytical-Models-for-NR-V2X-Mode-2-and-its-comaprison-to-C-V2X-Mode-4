#include "newradio-component-carrier-ue.h"
#include <ns3/uinteger.h>
#include <ns3/boolean.h>
#include <ns3/simulator.h>
#include <ns3/log.h>
#include <ns3/abort.h>
#include <ns3/newradio-ue-phy.h>
#include <ns3/newradio-ue-mac.h>
#include <ns3/pointer.h>

namespace ns3 {

namespace newradio {

NS_LOG_COMPONENT_DEFINE ("newradioComponentCarrierUe");

NS_OBJECT_ENSURE_REGISTERED ( newradioComponentCarrierUe);

TypeId newradioComponentCarrierUe::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::newradioComponentCarrierUe")
    .SetParent<newradioComponentCarrier> ()
    .AddConstructor<newradioComponentCarrierUe> ()
    .AddAttribute ("newradioUePhy",
                   "The PHY associated to this ComponentCarrierUe",
                   PointerValue (),
                   MakePointerAccessor (&newradioComponentCarrierUe::m_phy),
                   MakePointerChecker <newradioUePhy> ())
    .AddAttribute ("newradioUeMac",
                   "The MAC associated to this ComponentCarrierUe",
                   PointerValue (),
                   MakePointerAccessor (&newradioComponentCarrierUe::m_mac),
                   MakePointerChecker <newradioUeMac> ())
  ;
  return tid;
}
newradioComponentCarrierUe::newradioComponentCarrierUe ()
{
  NS_LOG_FUNCTION (this);
}

newradioComponentCarrierUe::~newradioComponentCarrierUe (void)
{
  NS_LOG_FUNCTION (this);
}

void
newradioComponentCarrierUe::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_phy->Dispose ();
  m_phy = 0;
  m_mac->Dispose ();
  m_mac = 0;
  Object::DoDispose ();
}


void
newradioComponentCarrierUe::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);
  m_isConstructed = true;
  m_phy->Initialize ();
  m_mac->Initialize ();
}

void
newradioComponentCarrierUe::SetPhy (Ptr<newradioUePhy> s)
{
  NS_LOG_FUNCTION (this);
  m_phy = s;
}


Ptr<newradioUePhy>
newradioComponentCarrierUe::GetPhy () const
{
  NS_LOG_FUNCTION (this);
  return m_phy;
}

void
newradioComponentCarrierUe::SetMac (Ptr<newradioUeMac> s)
{
  NS_LOG_FUNCTION (this);
  m_mac = s;
}

Ptr<newradioUeMac>
newradioComponentCarrierUe::GetMac () const
{
  NS_LOG_FUNCTION (this);
  return m_mac;
}

} // namespace newradio

} // namespace ns3
