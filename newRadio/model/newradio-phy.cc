#include <ns3/simulator.h>
#include <ns3/callback.h>
#include <ns3/node.h>
#include <ns3/packet.h>
#include <ns3/log.h>
#include "newradio-phy.h"
#include "newradio-phy-sap.h"
#include "newradio-mac-pdu-tag.h"
#include "newradio-mac-pdu-header.h"
#include <sstream>
#include <vector>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("newradioPhy");

namespace newradio {

NS_OBJECT_ENSURE_REGISTERED ( newradioPhy);

/*   SAP   */
class newradioMemberPhySapProvider : public newradioPhySapProvider
{
public:
  newradioMemberPhySapProvider (newradioPhy* phy);

  virtual void SendMacPdu (Ptr<Packet> p );

  virtual void SendControlMessage (Ptr<newradioControlMessage> msg);

  virtual void SendRachPreamble (uint8_t PreambleId, uint8_t Rnti);

  virtual void SetDlSfAllocInfo (SfAllocInfo sfAllocInfo);

  virtual void SetUlSfAllocInfo (SfAllocInfo sfAllocInfo);

private:
  newradioPhy* m_phy;
};

newradioMemberPhySapProvider::newradioMemberPhySapProvider (newradioPhy* phy)
  : m_phy (phy)
{
//	 Nothing more to do
}

void
newradioMemberPhySapProvider::SendMacPdu (Ptr<Packet> p)
{
  m_phy->SetMacPdu (p);
}

void
newradioMemberPhySapProvider::SendControlMessage (Ptr<newradioControlMessage> msg)
{
  m_phy->SetControlMessage (msg);      //May need to change
}

void
newradioMemberPhySapProvider::SendRachPreamble (uint8_t PreambleId, uint8_t Rnti)
{
  m_phy->SendRachPreamble (PreambleId, Rnti);
}

void
newradioMemberPhySapProvider::SetDlSfAllocInfo (SfAllocInfo sfAllocInfo)
{
  m_phy->SetDlSfAllocInfo (sfAllocInfo);
}

void
newradioMemberPhySapProvider::SetUlSfAllocInfo (SfAllocInfo sfAllocInfo)
{
  m_phy->SetUlSfAllocInfo (sfAllocInfo);
}

/* ======= */

TypeId
newradioPhy::GetTypeId ()
{
  static TypeId
    tid =
    TypeId ("ns3::newradioPhy")
    .SetParent<Object> ()
  ;

  return tid;
}

newradioPhy::newradioPhy ()
{
  NS_LOG_FUNCTION (this);
  NS_FATAL_ERROR ("This constructor should not be called");
}

newradioPhy::newradioPhy (Ptr<newradioSpectrumPhy> dlChannelPhy, Ptr<newradioSpectrumPhy> ulChannelPhy)
  : m_downlinkSpectrumPhy (dlChannelPhy),
    m_uplinkSpectrumPhy (ulChannelPhy),
    m_cellId (0),
    m_frameNum (0),
    m_sfNum (0),
    m_slotNum (0),
    m_sfAllocInfoUpdated (false),
    m_componentCarrierId (0)
{
  NS_LOG_FUNCTION (this);
  m_phySapProvider = new newradioMemberPhySapProvider (this);
}

newradioPhy::~newradioPhy ()
{

}

void
newradioPhy::DoInitialize ()
{
}

void
newradioPhy::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_controlMessageQueue.clear ();

  Object::DoDispose ();
}

void
newradioPhy::SetDevice (Ptr<NetDevice> d)
{
  m_netDevice = d;
}

Ptr<NetDevice>
newradioPhy::GetDevice ()
{
  return m_netDevice;
}

void
newradioPhy::SetChannel (Ptr<SpectrumChannel> c)
{

}

double
newradioPhy::GetTti (void) const
{
  NS_LOG_FUNCTION (this);
  return m_phyMacConfig->GetTti ();
}

void
newradioPhy::DoSetCellId (uint16_t cellId)
{

  NS_LOG_FUNCTION (this);
  m_cellId = cellId;
  m_downlinkSpectrumPhy->SetCellId (cellId);
  m_uplinkSpectrumPhy->SetCellId (cellId);
}


void
newradioPhy::SetNoiseFigure (double nf)
{
  m_noiseFigure = nf;
}

double
newradioPhy::GetNoiseFigure (void) const
{
  return m_noiseFigure;
}

void
newradioPhy::SendRachPreamble (uint32_t PreambleId, uint32_t Rnti)
{
  m_raPreambleId = PreambleId;
  Ptr<newradioRachPreambleMessage> msg = Create<newradioRachPreambleMessage> ();
  msg->SetRapId (PreambleId);
  SetControlMessage (msg);
}

void
newradioPhy::SetMacPdu (Ptr<Packet> p)
{
  newradioMacPduTag tag;
  if (p->PeekPacketTag (tag))
    {
      NS_ASSERT ((tag.GetSfn ().m_sfNum >= 0) && (tag.GetSfn ().m_sfNum < m_phyMacConfig->GetSymbolsPerSubframe ()));
      std::map<uint32_t, Ptr<PacketBurst> >::iterator it = m_packetBurstMap.find (tag.GetSfn ().Encode ());
      if (it == m_packetBurstMap.end ())
        {
          it = m_packetBurstMap.insert (std::pair<uint32_t, Ptr<PacketBurst> > (tag.GetSfn ().Encode (), CreateObject<PacketBurst> ())).first;
        }
      else
        {
          NS_FATAL_ERROR ("Packet burst map entry already exists");
        }
      it->second->AddPacket (p);
    }
  else
    {
      NS_FATAL_ERROR ("No MAC packet PDU header available");
    }
}

Ptr<PacketBurst>
newradioPhy::GetPacketBurst (SfnSf sfn)
{
  Ptr<PacketBurst> pburst;
  std::map<uint32_t, Ptr<PacketBurst> >::iterator it = m_packetBurstMap.find (sfn.Encode ());
  if (it == m_packetBurstMap.end ())
    {
      NS_LOG_ERROR ("GetPacketBurst(): Packet burst not found for subframe " << (unsigned)sfn.m_sfNum << " slot/sym start "  << (unsigned)sfn.m_slotNum);
      return pburst;
    }
  else
    {
      pburst = it->second;
      m_packetBurstMap.erase (it);
    }
  return pburst;
}

void
newradioPhy::SetControlMessage (Ptr<newradioControlMessage> m)
{
  if (m_controlMessageQueue.empty ())
    {
      std::list<Ptr<newradioControlMessage> > l;
      l.push_back (m);
      m_controlMessageQueue.push_back (l);
    }
  else
    {
      m_controlMessageQueue.at (m_controlMessageQueue.size () - 1).push_back (m);
    }
}

std::list<Ptr<newradioControlMessage> >
newradioPhy::GetControlMessages (void)
{
  NS_LOG_FUNCTION (this);
  if (m_controlMessageQueue.empty ())
    {
      std::list<Ptr<newradioControlMessage> > emptylist;
      return (emptylist);
    }

  if (m_controlMessageQueue.at (0).size () > 0)
    {
      std::list<Ptr<newradioControlMessage> > ret = m_controlMessageQueue.front ();
      m_controlMessageQueue.erase (m_controlMessageQueue.begin ());
      std::list<Ptr<newradioControlMessage> > newlist;
      m_controlMessageQueue.push_back (newlist);
      return (ret);
    }
  else
    {
      m_controlMessageQueue.erase (m_controlMessageQueue.begin ());
      std::list<Ptr<newradioControlMessage> > newlist;
      m_controlMessageQueue.push_back (newlist);
      std::list<Ptr<newradioControlMessage> > emptylist;
      return (emptylist);
    }
}

void
newradioPhy::SetConfigurationParameters (Ptr<newradioPhyMacCommon> ptrConfig)
{
  m_phyMacConfig = ptrConfig;
}

Ptr<newradioPhyMacCommon>
newradioPhy::GetConfigurationParameters (void) const
{
  return m_phyMacConfig;
}


newradioPhySapProvider*
newradioPhy::GetPhySapProvider ()
{
  return m_phySapProvider;
}


SfAllocInfo
newradioPhy::GetSfAllocInfo (uint8_t subframeNum)
{
  return m_sfAllocInfo[subframeNum];
}

void
newradioPhy::SetDlSfAllocInfo (SfAllocInfo sfAllocInfo)
{
  // get previously enqueued SfAllocInfo and set DL slot allocations
  //SfAllocInfo &sf = m_sfAllocInfo[sfAllocInfo.m_sfnSf.m_sfNum];
  // merge slot lists
  //sf.m_dlSlotAllocInfo = sfAllocInfo.m_dlSlotAllocInfo;
  m_sfAllocInfo[sfAllocInfo.m_sfnSf.m_sfNum] = sfAllocInfo;
  //m_sfAllocInfoUpdated = true;
}

void
newradioPhy::SetUlSfAllocInfo (SfAllocInfo sfAllocInfo)
{
  // add new SfAllocInfo with UL slot allocation
  //m_sfAllocInfo[sfAllocInfo.m_sfnSf.m_sfNum] = sfAllocInfo;
}

void
newradioPhy::AddPropagationLossModel (Ptr<PropagationLossModel> model)
{
  m_propagationLoss = model;
}

void
newradioPhy::AddLosTracker (Ptr<newradioLosTracker> losTracker)
{
  m_losTracker = losTracker;
}

void
newradioPhy::AddSpectrumPropagationLossModel (Ptr<SpectrumPropagationLossModel> model)
{
  m_spectrumPropagationLossModel = model;
}

void
newradioPhy::SetComponentCarrierId (uint8_t index)
{
  m_componentCarrierId = index;
  m_downlinkSpectrumPhy->SetComponentCarrierId (index);
  m_uplinkSpectrumPhy->SetComponentCarrierId (index);
}

uint8_t
newradioPhy::GetComponentCarrierId ()
{
  return m_componentCarrierId;
}

}

}
