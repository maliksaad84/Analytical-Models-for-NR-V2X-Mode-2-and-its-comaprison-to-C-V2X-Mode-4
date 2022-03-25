#include "newradio-component-carrier.h"
#include <ns3/uinteger.h>
#include <ns3/boolean.h>
#include <ns3/simulator.h>
#include <ns3/log.h>
#include <ns3/abort.h>
#include <ns3/newradio-enb-phy.h>
#include <ns3/pointer.h>

namespace ns3 {

namespace newradio {

NS_LOG_COMPONENT_DEFINE ("newradioComponentCarrier");

NS_OBJECT_ENSURE_REGISTERED ( newradioComponentCarrier);

TypeId newradioComponentCarrier::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::newradioComponentCarrier")
    .SetParent<Object> ()
    .AddConstructor<newradioComponentCarrier> ()
    .AddAttribute ("CsgId",
                   "The Closed Subscriber Group (CSG) identity that this eNodeB belongs to",
                   UintegerValue (0),
                   MakeUintegerAccessor (&newradioComponentCarrier::SetCsgId,
                                         &newradioComponentCarrier::GetCsgId),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("CsgIndication",
                   "If true, only UEs which are members of the CSG (i.e. same CSG ID) "
                   "can gain access to the eNodeB, therefore enforcing closed access mode. "
                   "Otherwise, the eNodeB operates as a non-CSG cell and implements open access mode.",
                   BooleanValue (false),
                   MakeBooleanAccessor (&newradioComponentCarrier::SetCsgIndication,
                                        &newradioComponentCarrier::GetCsgIndication),
                   MakeBooleanChecker ())
    .AddAttribute ("PrimaryCarrier",
                   "If true, this Carrier Component will be the Primary Carrier Component (PCC) "
                   "Only one PCC per eNodeB is (currently) allowed",
                   BooleanValue (false),
                   MakeBooleanAccessor (&newradioComponentCarrier::SetAsPrimary,
                                        &newradioComponentCarrier::IsPrimary),
                   MakeBooleanChecker ())
  ;
  return tid;
}
newradioComponentCarrier::newradioComponentCarrier ()
  : m_isConstructed (false)
{
  NS_LOG_FUNCTION (this);
}

newradioComponentCarrier::~newradioComponentCarrier (void)
{
  NS_LOG_FUNCTION (this);
}

void
newradioComponentCarrier::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  Object::DoDispose ();
}

uint32_t
newradioComponentCarrier::GetBandwidth () const
{
  return m_phyMacConfig->GetNumRb ();
}

double
newradioComponentCarrier::GetCenterFrequency () const
{
  return m_phyMacConfig->GetCenterFrequency ();
}

uint32_t
newradioComponentCarrier::GetCsgId () const
{
  return m_csgId;
}

void
newradioComponentCarrier::SetCsgId (uint32_t csgId)
{
  NS_LOG_FUNCTION (this << csgId);
  m_csgId = csgId;
}

bool
newradioComponentCarrier::GetCsgIndication () const
{
  return m_csgIndication;
}

void
newradioComponentCarrier::SetCsgIndication (bool csgIndication)
{
  NS_LOG_FUNCTION (this << csgIndication);
  m_csgIndication = csgIndication;
}

bool
newradioComponentCarrier::IsPrimary () const
{
  return m_primaryCarrier;
}

void
newradioComponentCarrier::SetAsPrimary (bool primaryCarrier)
{
  NS_LOG_FUNCTION (this << primaryCarrier);
  m_primaryCarrier = primaryCarrier;
}

void
newradioComponentCarrier::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);
  m_isConstructed = true;
}

void
newradioComponentCarrier::SetConfigurationParameters (Ptr<newradioPhyMacCommon> ptrConfig)
{
  NS_LOG_FUNCTION (this);
  m_phyMacConfig = ptrConfig;
}

Ptr<newradioPhyMacCommon>
newradioComponentCarrier::GetConfigurationParameters (void) const
{
  NS_LOG_FUNCTION (this);
  return m_phyMacConfig;
}

} // namespace newradio

} // namespace ns3
