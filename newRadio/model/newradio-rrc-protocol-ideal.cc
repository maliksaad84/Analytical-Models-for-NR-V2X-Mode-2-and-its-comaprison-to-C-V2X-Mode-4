#include "newradio-rrc-protocol-ideal.h"

#include <ns3/fatal-error.h>
#include <ns3/log.h>
#include <ns3/nstime.h>
#include <ns3/node-list.h>
#include <ns3/node.h>
#include <ns3/simulator.h>

#include "ns3/lte-ue-rrc.h"
#include "ns3/lte-enb-rrc.h"
#include "newradio-ue-net-device.h"
#include "mc-ue-net-device.h"
#include "newradio-enb-net-device.h"

NS_LOG_COMPONENT_DEFINE ("newradioRrcProtocolIdeal");


namespace ns3 {

namespace newradio {


static const Time RRC_IDEAL_MSG_DELAY = MicroSeconds (500);

NS_OBJECT_ENSURE_REGISTERED (newradioUeRrcProtocolIdeal);

newradioUeRrcProtocolIdeal::newradioUeRrcProtocolIdeal ()
  :  m_ueRrcSapProvider (0),
    m_enbRrcSapProvider (0)
{
  m_ueRrcSapUser = new MemberLteUeRrcSapUser<newradioUeRrcProtocolIdeal> (this);
}

newradioUeRrcProtocolIdeal::~newradioUeRrcProtocolIdeal ()
{
}

void
newradioUeRrcProtocolIdeal::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_ueRrcSapUser;
  m_rrc = 0;
}

TypeId
newradioUeRrcProtocolIdeal::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::newradioUeRrcProtocolIdeal")
    .SetParent<Object> ()
    .AddConstructor<newradioUeRrcProtocolIdeal> ()
  ;
  return tid;
}

void
newradioUeRrcProtocolIdeal::SetLteUeRrcSapProvider (LteUeRrcSapProvider* p)
{
  m_ueRrcSapProvider = p;
}

LteUeRrcSapUser*
newradioUeRrcProtocolIdeal::GetLteUeRrcSapUser ()
{
  return m_ueRrcSapUser;
}

void
newradioUeRrcProtocolIdeal::SetUeRrc (Ptr<LteUeRrc> rrc)
{
  m_rrc = rrc;
}

void
newradioUeRrcProtocolIdeal::DoSetup (LteUeRrcSapUser::SetupParameters params)
{
  NS_LOG_FUNCTION (this);
  // We don't care about SRB0/SRB1 since we use ideal RRC messages.
}

void
newradioUeRrcProtocolIdeal::DoSendRrcConnectionRequest (LteRrcSap::RrcConnectionRequest msg)
{
  // initialize the RNTI and get the EnbLteRrcSapProvider for the
  // eNB we are currently attached to
  m_rnti = m_rrc->GetRnti ();
  SetEnbRrcSapProvider ();

  Simulator::Schedule (RRC_IDEAL_MSG_DELAY,
                       &LteEnbRrcSapProvider::RecvRrcConnectionRequest,
                       m_enbRrcSapProvider,
                       m_rnti,
                       msg);
}

void
newradioUeRrcProtocolIdeal::DoSendRrcConnectionSetupCompleted (LteRrcSap::RrcConnectionSetupCompleted msg)
{
  Simulator::Schedule (RRC_IDEAL_MSG_DELAY,
                       &LteEnbRrcSapProvider::RecvRrcConnectionSetupCompleted,
                       m_enbRrcSapProvider,
                       m_rnti,
                       msg);
}

void
newradioUeRrcProtocolIdeal::DoSendRrcConnectionReconfigurationCompleted (LteRrcSap::RrcConnectionReconfigurationCompleted msg)
{
  // re-initialize the RNTI and get the EnbLteRrcSapProvider for the
  // eNB we are currently attached to
  m_rnti = m_rrc->GetRnti ();
  SetEnbRrcSapProvider ();

  Simulator::Schedule (RRC_IDEAL_MSG_DELAY,
                       &LteEnbRrcSapProvider::RecvRrcConnectionReconfigurationCompleted,
                       m_enbRrcSapProvider,
                       m_rnti,
                       msg);
}

void
newradioUeRrcProtocolIdeal::DoSendRrcConnectionReestablishmentRequest (LteRrcSap::RrcConnectionReestablishmentRequest msg)
{
  Simulator::Schedule (RRC_IDEAL_MSG_DELAY,
                       &LteEnbRrcSapProvider::RecvRrcConnectionReestablishmentRequest,
                       m_enbRrcSapProvider,
                       m_rnti,
                       msg);
}

void
newradioUeRrcProtocolIdeal::DoSendRrcConnectionReestablishmentComplete (LteRrcSap::RrcConnectionReestablishmentComplete msg)
{
  Simulator::Schedule (RRC_IDEAL_MSG_DELAY,
                       &LteEnbRrcSapProvider::RecvRrcConnectionReestablishmentComplete,
                       m_enbRrcSapProvider,
                       m_rnti,
                       msg);
}

void
newradioUeRrcProtocolIdeal::DoSendMeasurementReport (LteRrcSap::MeasurementReport msg)
{
  Simulator::Schedule (RRC_IDEAL_MSG_DELAY,
                       &LteEnbRrcSapProvider::RecvMeasurementReport,
                       m_enbRrcSapProvider,
                       m_rnti,
                       msg);
}

void
newradioUeRrcProtocolIdeal::DoSendNotifySecondaryCellConnected (uint16_t newradioRnti,uint16_t newradioCellId)
{
  Simulator::Schedule (RRC_IDEAL_MSG_DELAY,
                       &LteEnbRrcSapProvider::RecvRrcSecondaryCellInitialAccessSuccessful,
                       m_enbRrcSapProvider,
                       m_rnti,
                       newradioRnti,
                       newradioCellId);
}

void
newradioUeRrcProtocolIdeal::SetEnbRrcSapProvider ()
{
  uint16_t cellId = m_rrc->GetCellId ();

  // walk list of all nodes to get the peer eNB
  Ptr<newradioEnbNetDevice> enbDev;
  NodeList::Iterator listEnd = NodeList::End ();
  bool found = false;
  for (NodeList::Iterator i = NodeList::Begin ();
       (i != listEnd) && (!found);
       ++i)
    {
      Ptr<Node> node = *i;
      int nDevs = node->GetNDevices ();
      for (int j = 0;
           (j < nDevs) && (!found);
           j++)
        {
          enbDev = node->GetDevice (j)->GetObject <newradioEnbNetDevice> ();
          if (enbDev == 0)
            {
              continue;
            }
          else
            {
              if (enbDev->HasCellId (cellId))
                {
                  found = true;
                  break;
                }
            }
        }
    }
  NS_ASSERT_MSG (found, " Unable to find eNB with CellId =" << cellId);
  m_enbRrcSapProvider = enbDev->GetRrc ()->GetLteEnbRrcSapProvider ();
  Ptr<newradioEnbRrcProtocolIdeal> enbRrcProtocolIdeal = enbDev->GetRrc ()->GetObject<newradioEnbRrcProtocolIdeal> ();
  enbRrcProtocolIdeal->SetUeRrcSapProvider (m_rnti, m_ueRrcSapProvider);
}


NS_OBJECT_ENSURE_REGISTERED (newradioEnbRrcProtocolIdeal);

newradioEnbRrcProtocolIdeal::newradioEnbRrcProtocolIdeal ()
  :  m_enbRrcSapProvider (0)
{
  NS_LOG_FUNCTION (this);
  m_enbRrcSapUser = new MemberLteEnbRrcSapUser<newradioEnbRrcProtocolIdeal> (this);
}

newradioEnbRrcProtocolIdeal::~newradioEnbRrcProtocolIdeal ()
{
  NS_LOG_FUNCTION (this);
}

void
newradioEnbRrcProtocolIdeal::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_enbRrcSapUser;
}

TypeId
newradioEnbRrcProtocolIdeal::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::newradioEnbRrcProtocolIdeal")
    .SetParent<Object> ()
    .AddConstructor<newradioEnbRrcProtocolIdeal> ()
  ;
  return tid;
}

void
newradioEnbRrcProtocolIdeal::SetLteEnbRrcSapProvider (LteEnbRrcSapProvider* p)
{
  m_enbRrcSapProvider = p;
}

LteEnbRrcSapUser*
newradioEnbRrcProtocolIdeal::GetLteEnbRrcSapUser ()
{
  return m_enbRrcSapUser;
}

void
newradioEnbRrcProtocolIdeal::SetCellId (uint16_t cellId)
{
  m_cellId = cellId;
}

LteUeRrcSapProvider*
newradioEnbRrcProtocolIdeal::GetUeRrcSapProvider (uint16_t rnti)
{
  std::map<uint16_t, LteUeRrcSapProvider*>::const_iterator it;
  it = m_enbRrcSapProviderMap.find (rnti);
  NS_ASSERT_MSG (it != m_enbRrcSapProviderMap.end (), "could not find RNTI = " << rnti);
  return it->second;
}

void
newradioEnbRrcProtocolIdeal::SetUeRrcSapProvider (uint16_t rnti, LteUeRrcSapProvider* p)
{
  std::map<uint16_t, LteUeRrcSapProvider*>::iterator it;
  it = m_enbRrcSapProviderMap.find (rnti);
  NS_ASSERT_MSG (it != m_enbRrcSapProviderMap.end (), "could not find RNTI = " << rnti);
  it->second = p;
}

void
newradioEnbRrcProtocolIdeal::DoSetupUe (uint16_t rnti, LteEnbRrcSapUser::SetupUeParameters params)
{
  NS_LOG_FUNCTION (this << rnti);
  m_enbRrcSapProviderMap[rnti] = 0;

}

void
newradioEnbRrcProtocolIdeal::DoRemoveUe (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << rnti);
  m_enbRrcSapProviderMap.erase (rnti);
}

void
newradioEnbRrcProtocolIdeal::DoSendSystemInformation (uint16_t cellId, LteRrcSap::SystemInformation msg)
{
  NS_LOG_FUNCTION (this << cellId);
  // walk list of all nodes to get UEs with this cellId
  Ptr<LteUeRrc> ueRrc;
  for (NodeList::Iterator i = NodeList::Begin (); i != NodeList::End (); ++i)
    {
      Ptr<Node> node = *i;
      int nDevs = node->GetNDevices ();
      for (int j = 0; j < nDevs; ++j)
        {
          Ptr<newradio::newradioUeNetDevice> newradioUeDev = node->GetDevice (j)->GetObject <newradio::newradioUeNetDevice> ();
          if (newradioUeDev != 0)
            {
              ueRrc = newradioUeDev->GetRrc ();
              NS_LOG_LOGIC ("considering UE IMSI " << newradioUeDev->GetImsi () << " that has cellId " << ueRrc->GetCellId ());
              if (ueRrc->GetCellId () == cellId)
                {
                  NS_LOG_LOGIC ("sending SI to IMSI " << newradioUeDev->GetImsi ());
                  ueRrc->GetLteUeRrcSapProvider ()->RecvSystemInformation (msg);
                  Simulator::Schedule (RRC_IDEAL_MSG_DELAY,
                                       &LteUeRrcSapProvider::RecvSystemInformation,
                                       ueRrc->GetLteUeRrcSapProvider (),
                                       msg);
                }
            }
          else
            {
              // it may be a McUeNetDevice
              Ptr<McUeNetDevice> mcUeDev = node->GetDevice (j)->GetObject <McUeNetDevice> ();
              if (mcUeDev != 0)
                {
                  NS_FATAL_ERROR ("-----------This implementation does not support McUeNetDevice. Use newradioRrcProtocolReal.");
                  ueRrc = mcUeDev->GetnewradioRrc ();
                  if (ueRrc != 0) // actually is using 2 connections
                    {
                      NS_LOG_LOGIC ("considering UE IMSI " << mcUeDev->GetImsi () << " that has cellId " << ueRrc->GetCellId ());
                      NS_LOG_LOGIC ("UE cellId " << (uint32_t)ueRrc->GetCellId () << " ENB cellId " << (uint32_t)cellId);
                      if (ueRrc->GetCellId () == m_cellId)
                        {
                          NS_LOG_LOGIC ("sending SI to IMSI " << mcUeDev->GetImsi ());
                          ueRrc->GetLteUeRrcSapProvider ()->RecvSystemInformation (msg);
                          Simulator::Schedule (RRC_IDEAL_MSG_DELAY,
                                               &LteUeRrcSapProvider::RecvSystemInformation,
                                               ueRrc->GetLteUeRrcSapProvider (),
                                               msg);
                        }
                    }
                  else // it may have just a double stack up to MAC layer
                    {
                      ueRrc = mcUeDev->GetLteRrc ();
                      if (ueRrc != 0)
                        {
                          NS_LOG_LOGIC ("considering UE IMSI " << mcUeDev->GetImsi () << " that has cellId " << ueRrc->GetCellId ());
                          NS_LOG_LOGIC ("UE cellId " << (uint32_t)ueRrc->GetCellId () << " ENB cellId " << (uint32_t)cellId);
                          if (ueRrc->GetCellId () == m_cellId)
                            {
                              NS_LOG_LOGIC ("sending SI to IMSI " << mcUeDev->GetImsi ());
                              ueRrc->GetLteUeRrcSapProvider ()->RecvSystemInformation (msg);
                              Simulator::Schedule (RRC_IDEAL_MSG_DELAY,
                                                   &LteUeRrcSapProvider::RecvSystemInformation,
                                                   ueRrc->GetLteUeRrcSapProvider (),
                                                   msg);
                            }
                        }
                    }
                }
            }
        }
    }
}

void
newradioEnbRrcProtocolIdeal::DoSendRrcConnectionSetup (uint16_t rnti, LteRrcSap::RrcConnectionSetup msg)
{
  Simulator::Schedule (RRC_IDEAL_MSG_DELAY,
                       &LteUeRrcSapProvider::RecvRrcConnectionSetup,
                       GetUeRrcSapProvider (rnti),
                       msg);
}

void
newradioEnbRrcProtocolIdeal::DoSendRrcConnectionReconfiguration (uint16_t rnti, LteRrcSap::RrcConnectionReconfiguration msg)
{
  Simulator::Schedule (RRC_IDEAL_MSG_DELAY,
                       &LteUeRrcSapProvider::RecvRrcConnectionReconfiguration,
                       GetUeRrcSapProvider (rnti),
                       msg);
}

void
newradioEnbRrcProtocolIdeal::DoSendRrcConnectionReestablishment (uint16_t rnti, LteRrcSap::RrcConnectionReestablishment msg)
{
  Simulator::Schedule (RRC_IDEAL_MSG_DELAY,
                       &LteUeRrcSapProvider::RecvRrcConnectionReestablishment,
                       GetUeRrcSapProvider (rnti),
                       msg);
}

void
newradioEnbRrcProtocolIdeal::DoSendRrcConnectionReestablishmentReject (uint16_t rnti, LteRrcSap::RrcConnectionReestablishmentReject msg)
{
  Simulator::Schedule (RRC_IDEAL_MSG_DELAY,
                       &LteUeRrcSapProvider::RecvRrcConnectionReestablishmentReject,
                       GetUeRrcSapProvider (rnti),
                       msg);
}

void
newradioEnbRrcProtocolIdeal::DoSendRrcConnectionRelease (uint16_t rnti, LteRrcSap::RrcConnectionRelease msg)
{
  Simulator::Schedule (RRC_IDEAL_MSG_DELAY,
                       &LteUeRrcSapProvider::RecvRrcConnectionRelease,
                       GetUeRrcSapProvider (rnti),
                       msg);
}

void
newradioEnbRrcProtocolIdeal::DoSendRrcConnectionReject (uint16_t rnti, LteRrcSap::RrcConnectionReject msg)
{
  Simulator::Schedule (RRC_IDEAL_MSG_DELAY,
                       &LteUeRrcSapProvider::RecvRrcConnectionReject,
                       GetUeRrcSapProvider (rnti),
                       msg);
}


void
newradioEnbRrcProtocolIdeal::DoSendRrcConnectionSwitch (uint16_t rnti, LteRrcSap::RrcConnectionSwitch msg)
{
  NS_FATAL_ERROR ("A newradio eNB should not use this primitive");
}

void
newradioEnbRrcProtocolIdeal::DoSendRrcConnectTonewradio (uint16_t rnti, uint16_t newradioCellId)
{
  Simulator::Schedule (RRC_IDEAL_MSG_DELAY,
                       &LteUeRrcSapProvider::RecvRrcConnectTonewradio,
                       GetUeRrcSapProvider (rnti),
                       newradioCellId);
}

/*
 * The purpose of newradioEnbRrcProtocolIdeal is to avoid encoding
 * messages. In order to do so, we need to have some form of encoding for
 * inter-node RRC messages like HandoverPreparationInfo and HandoverCommand. Doing so
 * directly is not practical (these messages includes a lot of
 * information elements, so encoding all of them would defeat the
 * purpose of newradioEnbRrcProtocolIdeal. The workaround is to store the
 * actual message in a global map, so that then we can just encode the
 * key in a header and send that between eNBs over X2.
 *
 */

static std::map<uint32_t, LteRrcSap::HandoverPreparationInfo> g_handoverPreparationInfoMsgMap;
static uint32_t g_handoverPreparationInfoMsgIdCounter = 0;

/*
 * This header encodes the map key discussed above. We keep this
 * private since it should not be used outside this file.
 *
 */
class newradioIdealHandoverPreparationInfoHeader : public Header
{
public:
  uint32_t GetMsgId ();
  void SetMsgId (uint32_t id);
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  uint32_t m_msgId;
};

uint32_t
newradioIdealHandoverPreparationInfoHeader::GetMsgId ()
{
  return m_msgId;
}

void
newradioIdealHandoverPreparationInfoHeader::SetMsgId (uint32_t id)
{
  m_msgId = id;
}


TypeId
newradioIdealHandoverPreparationInfoHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::newradioIdealHandoverPreparationInfoHeader")
    .SetParent<Header> ()
    .AddConstructor<newradioIdealHandoverPreparationInfoHeader> ()
  ;
  return tid;
}

TypeId
newradioIdealHandoverPreparationInfoHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void newradioIdealHandoverPreparationInfoHeader::Print (std::ostream &os)  const
{
  os << " msgId=" << m_msgId;
}

uint32_t newradioIdealHandoverPreparationInfoHeader::GetSerializedSize (void) const
{
  return 4;
}

void newradioIdealHandoverPreparationInfoHeader::Serialize (Buffer::Iterator start) const
{
  start.WriteU32 (m_msgId);
}

uint32_t newradioIdealHandoverPreparationInfoHeader::Deserialize (Buffer::Iterator start)
{
  m_msgId = start.ReadU32 ();
  return GetSerializedSize ();
}



Ptr<Packet>
newradioEnbRrcProtocolIdeal::DoEncodeHandoverPreparationInformation (LteRrcSap::HandoverPreparationInfo msg)
{
  uint32_t msgId = ++g_handoverPreparationInfoMsgIdCounter;
  NS_ASSERT_MSG (g_handoverPreparationInfoMsgMap.find (msgId) == g_handoverPreparationInfoMsgMap.end (), "msgId " << msgId << " already in use");
  NS_LOG_INFO (" encoding msgId = " << msgId);
  g_handoverPreparationInfoMsgMap.insert (std::pair<uint32_t, LteRrcSap::HandoverPreparationInfo> (msgId, msg));
  newradioIdealHandoverPreparationInfoHeader h;
  h.SetMsgId (msgId);
  Ptr<Packet> p = Create<Packet> ();
  p->AddHeader (h);
  return p;
}

LteRrcSap::HandoverPreparationInfo
newradioEnbRrcProtocolIdeal::DoDecodeHandoverPreparationInformation (Ptr<Packet> p)
{
  newradioIdealHandoverPreparationInfoHeader h;
  p->RemoveHeader (h);
  uint32_t msgId = h.GetMsgId ();
  NS_LOG_INFO (" decoding msgId = " << msgId);
  std::map<uint32_t, LteRrcSap::HandoverPreparationInfo>::iterator it = g_handoverPreparationInfoMsgMap.find (msgId);
  NS_ASSERT_MSG (it != g_handoverPreparationInfoMsgMap.end (), "msgId " << msgId << " not found");
  LteRrcSap::HandoverPreparationInfo msg = it->second;
  g_handoverPreparationInfoMsgMap.erase (it);
  return msg;
}



static std::map<uint32_t, LteRrcSap::RrcConnectionReconfiguration> g_handoverCommandMsgMap;
static uint32_t g_handoverCommandMsgIdCounter = 0;

/*
 * This header encodes the map key discussed above. We keep this
 * private since it should not be used outside this file.
 *
 */
class newradioIdealHandoverCommandHeader : public Header
{
public:
  uint32_t GetMsgId ();
  void SetMsgId (uint32_t id);
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  uint32_t m_msgId;
};

uint32_t
newradioIdealHandoverCommandHeader::GetMsgId ()
{
  return m_msgId;
}

void
newradioIdealHandoverCommandHeader::SetMsgId (uint32_t id)
{
  m_msgId = id;
}


TypeId
newradioIdealHandoverCommandHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::newradioIdealHandoverCommandHeader")
    .SetParent<Header> ()
    .AddConstructor<newradioIdealHandoverCommandHeader> ()
  ;
  return tid;
}

TypeId
newradioIdealHandoverCommandHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void newradioIdealHandoverCommandHeader::Print (std::ostream &os)  const
{
  os << " msgId=" << m_msgId;
}

uint32_t newradioIdealHandoverCommandHeader::GetSerializedSize (void) const
{
  return 4;
}

void newradioIdealHandoverCommandHeader::Serialize (Buffer::Iterator start) const
{
  start.WriteU32 (m_msgId);
}

uint32_t newradioIdealHandoverCommandHeader::Deserialize (Buffer::Iterator start)
{
  m_msgId = start.ReadU32 ();
  return GetSerializedSize ();
}



Ptr<Packet>
newradioEnbRrcProtocolIdeal::DoEncodeHandoverCommand (LteRrcSap::RrcConnectionReconfiguration msg)
{
  uint32_t msgId = ++g_handoverCommandMsgIdCounter;
  NS_ASSERT_MSG (g_handoverCommandMsgMap.find (msgId) == g_handoverCommandMsgMap.end (), "msgId " << msgId << " already in use");
  NS_LOG_INFO (" encoding msgId = " << msgId);
  g_handoverCommandMsgMap.insert (std::pair<uint32_t, LteRrcSap::RrcConnectionReconfiguration> (msgId, msg));
  newradioIdealHandoverCommandHeader h;
  h.SetMsgId (msgId);
  Ptr<Packet> p = Create<Packet> ();
  p->AddHeader (h);
  return p;
}

LteRrcSap::RrcConnectionReconfiguration
newradioEnbRrcProtocolIdeal::DoDecodeHandoverCommand (Ptr<Packet> p)
{
  newradioIdealHandoverCommandHeader h;
  p->RemoveHeader (h);
  uint32_t msgId = h.GetMsgId ();
  NS_LOG_INFO (" decoding msgId = " << msgId);
  std::map<uint32_t, LteRrcSap::RrcConnectionReconfiguration>::iterator it = g_handoverCommandMsgMap.find (msgId);
  NS_ASSERT_MSG (it != g_handoverCommandMsgMap.end (), "msgId " << msgId << " not found");
  LteRrcSap::RrcConnectionReconfiguration msg = it->second;
  g_handoverCommandMsgMap.erase (it);
  return msg;
}



} // namespace newradio

} // namespace ns3
