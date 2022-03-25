#include "newradio-enb-mac.h"
#include "newradio-phy-mac-common.h"
#include "newradio-mac-pdu-header.h"
#include "newradio-mac-sched-sap.h"
#include "newradio-mac-scheduler.h"
#include <ns3/lte-mac-sap.h>
#include <ns3/lte-enb-cmac-sap.h>
#include <ns3/log.h>

namespace ns3 {

namespace newradio {

NS_LOG_COMPONENT_DEFINE ("newradioEnbMac");

NS_OBJECT_ENSURE_REGISTERED (newradioEnbMac);



// //////////////////////////////////////
// member SAP forwarders
// //////////////////////////////////////


class newradioEnbMacMemberEnbCmacSapProvider : public LteEnbCmacSapProvider
{
public:
  newradioEnbMacMemberEnbCmacSapProvider (newradioEnbMac* mac);

  // inherited from LteEnbCmacSapProvider
  virtual void ConfigureMac (uint8_t ulBandwidth, uint8_t dlBandwidth);
  virtual void AddUe (uint16_t rnti);
  virtual void RemoveUe (uint16_t rnti);
  virtual void AddLc (LcInfo lcinfo, LteMacSapUser* msu);
  virtual void ReconfigureLc (LcInfo lcinfo);
  virtual void ReleaseLc (uint16_t rnti, uint8_t lcid);
  virtual void UeUpdateConfigurationReq (UeConfig params);
  virtual RachConfig GetRachConfig ();
  virtual AllocateNcRaPreambleReturnValue AllocateNcRaPreamble (uint16_t rnti);


private:
  newradioEnbMac* m_mac;
};


newradioEnbMacMemberEnbCmacSapProvider::newradioEnbMacMemberEnbCmacSapProvider (newradioEnbMac* mac)
  : m_mac (mac)
{
}

void
newradioEnbMacMemberEnbCmacSapProvider::ConfigureMac (uint8_t ulBandwidth, uint8_t dlBandwidth)
{
  m_mac->DoConfigureMac (ulBandwidth, dlBandwidth);
}

void
newradioEnbMacMemberEnbCmacSapProvider::AddUe (uint16_t rnti)
{
  m_mac->DoAddUe (rnti);
}

void
newradioEnbMacMemberEnbCmacSapProvider::RemoveUe (uint16_t rnti)
{
  m_mac->DoRemoveUe (rnti);
}

void
newradioEnbMacMemberEnbCmacSapProvider::AddLc (LcInfo lcinfo, LteMacSapUser* msu)
{
  m_mac->DoAddLc (lcinfo, msu);
}

void
newradioEnbMacMemberEnbCmacSapProvider::ReconfigureLc (LcInfo lcinfo)
{
  m_mac->DoReconfigureLc (lcinfo);
}

void
newradioEnbMacMemberEnbCmacSapProvider::ReleaseLc (uint16_t rnti, uint8_t lcid)
{
  m_mac->DoReleaseLc (rnti, lcid);
}

void
newradioEnbMacMemberEnbCmacSapProvider::UeUpdateConfigurationReq (UeConfig params)
{
  m_mac->UeUpdateConfigurationReq (params);
}

LteEnbCmacSapProvider::RachConfig
newradioEnbMacMemberEnbCmacSapProvider::GetRachConfig ()
{
  return m_mac->DoGetRachConfig ();
}

LteEnbCmacSapProvider::AllocateNcRaPreambleReturnValue
newradioEnbMacMemberEnbCmacSapProvider::AllocateNcRaPreamble (uint16_t rnti)
{
  return m_mac->DoAllocateNcRaPreamble (rnti);
}



// SAP
// ENB MAC-Phy
class newradioMacEnbMemberPhySapUser : public newradioEnbPhySapUser
{
public:
  newradioMacEnbMemberPhySapUser (newradioEnbMac* mac);

  virtual void ReceivePhyPdu (Ptr<Packet> p);

  virtual void ReceiveControlMessage (Ptr<newradioControlMessage> msg);

  virtual void SubframeIndication (SfnSf);

  virtual void UlCqiReport (newradioMacSchedSapProvider::SchedUlCqiInfoReqParameters cqi);

  virtual void ReceiveRachPreamble (uint32_t raId);

  virtual void UlHarqFeedback (UlHarqInfo params);

private:
  newradioEnbMac* m_mac;
};

newradioMacEnbMemberPhySapUser::newradioMacEnbMemberPhySapUser (newradioEnbMac* mac)
  : m_mac (mac)
{

}

void
newradioMacEnbMemberPhySapUser::ReceivePhyPdu (Ptr<Packet> p)
{
  m_mac->DoReceivePhyPdu (p);
}

void
newradioMacEnbMemberPhySapUser::ReceiveControlMessage (Ptr<newradioControlMessage> msg)
{
  m_mac->DoReceiveControlMessage (msg);
}

void
newradioMacEnbMemberPhySapUser::SubframeIndication (SfnSf sfn)
{
  m_mac->DoSubframeIndication (sfn);
}

void
newradioMacEnbMemberPhySapUser::UlCqiReport (newradioMacSchedSapProvider::SchedUlCqiInfoReqParameters ulcqi)
{
  m_mac->DoUlCqiReport (ulcqi);
}

void
newradioMacEnbMemberPhySapUser::ReceiveRachPreamble (uint32_t raId)
{
  m_mac->ReceiveRachPreamble (raId);
}

void
newradioMacEnbMemberPhySapUser::UlHarqFeedback (UlHarqInfo params)
{
  m_mac->DoUlHarqFeedback (params);
}

// MAC Sched

class newradioMacMemberMacSchedSapUser : public newradioMacSchedSapUser
{
public:
  newradioMacMemberMacSchedSapUser (newradioEnbMac* mac);
  virtual void SchedConfigInd (const struct SchedConfigIndParameters& params);
private:
  newradioEnbMac* m_mac;
};

newradioMacMemberMacSchedSapUser::newradioMacMemberMacSchedSapUser (newradioEnbMac* mac)
  : m_mac (mac)
{
//	 Some blank spaces
}

void
newradioMacMemberMacSchedSapUser::SchedConfigInd (const struct SchedConfigIndParameters& params)
{
  m_mac->DoSchedConfigIndication (params);
}


class newradioMacMemberMacCschedSapUser : public newradioMacCschedSapUser
{
public:
  newradioMacMemberMacCschedSapUser (newradioEnbMac* mac);

  virtual void CschedCellConfigCnf (const struct newradioMacCschedSapUser::CschedCellConfigCnfParameters& params);
  virtual void CschedUeConfigCnf (const struct newradioMacCschedSapUser::CschedUeConfigCnfParameters& params);
  virtual void CschedLcConfigCnf (const struct newradioMacCschedSapUser::CschedLcConfigCnfParameters& params);
  virtual void CschedLcReleaseCnf (const struct newradioMacCschedSapUser::CschedLcReleaseCnfParameters& params);
  virtual void CschedUeReleaseCnf (const struct newradioMacCschedSapUser::CschedUeReleaseCnfParameters& params);
  virtual void CschedUeConfigUpdateInd (const struct newradioMacCschedSapUser::CschedUeConfigUpdateIndParameters& params);
  virtual void CschedCellConfigUpdateInd (const struct newradioMacCschedSapUser::CschedCellConfigUpdateIndParameters& params);

private:
  newradioEnbMac* m_mac;
};


newradioMacMemberMacCschedSapUser::newradioMacMemberMacCschedSapUser (newradioEnbMac* mac)
  : m_mac (mac)
{
}

void
newradioMacMemberMacCschedSapUser::CschedCellConfigCnf (const struct CschedCellConfigCnfParameters& params)
{
  m_mac->DoCschedCellConfigCnf (params);
}

void
newradioMacMemberMacCschedSapUser::CschedUeConfigCnf (const struct CschedUeConfigCnfParameters& params)
{
  m_mac->DoCschedUeConfigCnf (params);
}

void
newradioMacMemberMacCschedSapUser::CschedLcConfigCnf (const struct CschedLcConfigCnfParameters& params)
{
  m_mac->DoCschedLcConfigCnf (params);
}

void
newradioMacMemberMacCschedSapUser::CschedLcReleaseCnf (const struct CschedLcReleaseCnfParameters& params)
{
  m_mac->DoCschedLcReleaseCnf (params);
}

void
newradioMacMemberMacCschedSapUser::CschedUeReleaseCnf (const struct CschedUeReleaseCnfParameters& params)
{
  m_mac->DoCschedUeReleaseCnf (params);
}

void
newradioMacMemberMacCschedSapUser::CschedUeConfigUpdateInd (const struct CschedUeConfigUpdateIndParameters& params)
{
  m_mac->DoCschedUeConfigUpdateInd (params);
}

void
newradioMacMemberMacCschedSapUser::CschedCellConfigUpdateInd (const struct CschedCellConfigUpdateIndParameters& params)
{
  m_mac->DoCschedCellConfigUpdateInd (params);
}

// Enb Mac Sap Provider

template <class C>
class EnbMacMembernewradioMacSapProvider : public LteMacSapProvider
{
public:
  EnbMacMembernewradioMacSapProvider (C* mac);

  // inherited from LteMacSapProvider
  virtual void TransmitPdu (TransmitPduParameters params);
  virtual void ReportBufferStatus (ReportBufferStatusParameters params);

private:
  C* m_mac;
};


template <class C>
EnbMacMembernewradioMacSapProvider<C>::EnbMacMembernewradioMacSapProvider (C* mac)
  : m_mac (mac)
{
}

template <class C>
void EnbMacMembernewradioMacSapProvider<C>::TransmitPdu (TransmitPduParameters params)
{
  m_mac->DoTransmitPdu (params);
}

template <class C>
void EnbMacMembernewradioMacSapProvider<C>::ReportBufferStatus (ReportBufferStatusParameters params)
{
  m_mac->DoReportBufferStatus (params);
}





TypeId
newradioEnbMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::newradioEnbMac")
    .SetParent<newradioMac> ()
    .AddConstructor<newradioEnbMac> ()
    .AddAttribute ("NumberOfRaPreambles",
                   "how many random access preambles are available for the contention based RACH process",
                   UintegerValue (50),
                   MakeUintegerAccessor (&newradioEnbMac::m_numberOfRaPreambles),
                   MakeUintegerChecker<uint8_t> (4, 64))
    .AddAttribute ("PreambleTransMax",
                   "Maximum number of random access preamble transmissions",
                   UintegerValue (50),
                   MakeUintegerAccessor (&newradioEnbMac::m_preambleTransMax),
                   MakeUintegerChecker<uint8_t> (3, 200))
    .AddAttribute ("RaResponseWindowSize",
                   "length of the window (in TTIs) for the reception of the random access response (RAR); the resulting RAR timeout is this value + 3 ms",
                   UintegerValue (3),
                   MakeUintegerAccessor (&newradioEnbMac::m_raResponseWindowSize),
                   MakeUintegerChecker<uint8_t> (2, 10))
    .AddAttribute ("ComponentCarrierId",
                   "ComponentCarrier Id, needed to reply on the appropriate sap.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&newradioEnbMac::m_componentCarrierId),
                   MakeUintegerChecker<uint8_t> (0,4))
    .AddTraceSource ("DlMacTxCallback",
                     "MAC transmission with tb size and number of retx.",
                     MakeTraceSourceAccessor (&newradioEnbMac::m_macDlTxSizeRetx),
                     "ns3::LteRlc::RetransmissionCountCallback")
    .AddTraceSource ("TxMacPacketTraceEnb",
                     "Packets transmitted by EnbMac",
                     MakeTraceSourceAccessor (&newradioEnbMac::m_txMacPacketTraceEnb),
                     "ns3::EnbTxRxPacketCount::TracedCallback")
  ;
  return tid;
}

newradioEnbMac::newradioEnbMac (void)
  : m_ccmMacSapUser (0),
    m_frameNum (0),
    m_sfNum (0),
    m_slotNum (0),
    m_tbUid (0)
{
  NS_LOG_FUNCTION (this);
  m_cmacSapProvider = new newradioEnbMacMemberEnbCmacSapProvider (this);
  m_macSapProvider = new EnbMacMembernewradioMacSapProvider<newradioEnbMac> (this);
  m_phySapUser = new newradioMacEnbMemberPhySapUser (this);
  m_macSchedSapUser = new newradioMacMemberMacSchedSapUser (this);
  m_macCschedSapUser = new newradioMacMemberMacCschedSapUser (this);

  m_ccmMacSapProvider = new MemberLteCcmMacSapProvider<newradioEnbMac> (this);
  Initialize ();
}

newradioEnbMac::~newradioEnbMac (void)
{
  NS_LOG_FUNCTION (this);
}

void
newradioEnbMac::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_dlCqiReceived.clear ();
  m_ulCqiReceived.clear ();
  m_ulCeReceived.clear ();
  //  m_dlHarqInfoListReceived.clear ();
  //  m_ulHarqInfoListReceived.clear ();
  m_miDlHarqProcessesPackets.clear ();
  delete m_macSapProvider;
  delete m_cmacSapProvider;
  delete m_macSchedSapUser;
  //  delete m_macCschedSapUser;
  delete m_phySapUser;
  delete m_ccmMacSapProvider;
}

void
newradioEnbMac::SetComponentCarrierId (uint8_t index)
{
  m_componentCarrierId = index;
}

void
newradioEnbMac::SetConfigurationParameters (Ptr<newradioPhyMacCommon> ptrConfig)
{
  m_phyMacConfig = ptrConfig;
}

Ptr<newradioPhyMacCommon>
newradioEnbMac::GetConfigurationParameters (void) const
{
  return m_phyMacConfig;
}

void
newradioEnbMac::ReceiveRachPreamble (uint32_t raId)
{
  ++m_receivedRachPreambleCount[raId];
}

LteMacSapProvider*
newradioEnbMac::GetUeMacSapProvider (void)
{
  return m_macSapProvider;
}

LteEnbCmacSapProvider*
newradioEnbMac::GetEnbCmacSapProvider (void)
{
  return m_cmacSapProvider;
}
void
newradioEnbMac::SetEnbCmacSapUser (LteEnbCmacSapUser* s)
{
  m_cmacSapUser = s;
}

void
newradioEnbMac::DoSubframeIndication (SfnSf sfnSf)
{
  m_frameNum = sfnSf.m_frameNum;
  m_sfNum = sfnSf.m_sfNum;
  m_slotNum = sfnSf.m_slotNum;

  // --- DOWNLINK ---
  // Send Dl-CQI info to the scheduler	if(m_dlCqiReceived.size () > 0)
  {
    newradioMacSchedSapProvider::SchedDlCqiInfoReqParameters dlCqiInfoReq;
    dlCqiInfoReq.m_sfnsf = sfnSf;

    dlCqiInfoReq.m_cqiList.insert (dlCqiInfoReq.m_cqiList.begin (), m_dlCqiReceived.begin (), m_dlCqiReceived.end ());
    m_dlCqiReceived.erase (m_dlCqiReceived.begin (), m_dlCqiReceived.end ());

    m_macSchedSapProvider->SchedDlCqiInfoReq (dlCqiInfoReq);
  }

  if (!m_receivedRachPreambleCount.empty ())
    {
      // process received RACH preambles and notify the scheduler
      Ptr<newradioRarMessage> rarMsg = Create<newradioRarMessage> ();

      for (std::map<uint8_t, uint32_t>::const_iterator it = m_receivedRachPreambleCount.begin ();
           it != m_receivedRachPreambleCount.end ();
           ++it)
        {
          uint16_t rnti;
          std::map<uint8_t, NcRaPreambleInfo>::iterator jt = m_allocatedNcRaPreambleMap.find (it->first);
          NS_LOG_LOGIC ("received RapId " << (uint16_t)it->first);
          if (jt != m_allocatedNcRaPreambleMap.end ())
            {
              rnti = jt->second.rnti;
              NS_LOG_INFO ("preambleId previously allocated for NC based RA, RNTI =" << (uint32_t) rnti << ", sending RAR");
              m_allocatedNcRaPreambleMap.erase (jt);
            }
          else
            {
              rnti = m_cmacSapUser->AllocateTemporaryCellRnti ();
              NS_LOG_INFO ("preambleId " << (uint32_t) it->first << ": allocated T-C-RNTI " << (uint32_t) rnti << ", sending RAR");
            }
          newradioRarMessage::Rar rar;
          rar.rapId = (*it).first;
          rar.rarPayload.m_rnti = rnti;
          rarMsg->AddRar (rar);
          //NS_ASSERT_MSG((*it).second ==1, "Two user send the same Rach ID, collision detected");
        }
      m_phySapProvider->SendControlMessage (rarMsg);
      m_receivedRachPreambleCount.clear ();
    }

  // --- UPLINK ---
  // Send UL-CQI info to the scheduler
  std::vector <newradioMacSchedSapProvider::SchedUlCqiInfoReqParameters>::iterator itCqi;
  for (uint16_t i = 0; i < m_ulCqiReceived.size (); i++)
    {
      //m_ulCqiReceived.at (i).m_sfnSf = ((0x3FF & frameNum) << 16) | ((0xFF & subframeNum) << 8) | (0xFF & slotNum);
      m_macSchedSapProvider->SchedUlCqiInfoReq (m_ulCqiReceived.at (i));
    }
  m_ulCqiReceived.clear ();

  // Send UL BSR reports to the scheduler
  if (m_ulCeReceived.size () > 0)
    {
      newradioMacSchedSapProvider::SchedUlMacCtrlInfoReqParameters ulMacReq;
      ulMacReq.m_sfnSf = sfnSf;
      NS_LOG_DEBUG ("ulMacReq.m_macCeList size " << m_ulCeReceived.size ());
      ulMacReq.m_macCeList.insert (ulMacReq.m_macCeList.begin (), m_ulCeReceived.begin (), m_ulCeReceived.end ());
      m_ulCeReceived.erase (m_ulCeReceived.begin (), m_ulCeReceived.end ());
      m_macSchedSapProvider->SchedUlMacCtrlInfoReq (ulMacReq);
    }

  if (m_slotNum == 0)
    {
      // trigger scheduler
      uint32_t dlSchedframeNum = m_frameNum;
      uint32_t dlSchedSubframeNum = m_sfNum;

      if (dlSchedSubframeNum + m_phyMacConfig->GetL1L2CtrlLatency () >= m_phyMacConfig->GetSubframesPerFrame ())
        {
          dlSchedframeNum++;
          dlSchedSubframeNum = (dlSchedSubframeNum + m_phyMacConfig->GetL1L2CtrlLatency ()) - m_phyMacConfig->GetSubframesPerFrame ();
        }
      else
        {
          dlSchedSubframeNum = dlSchedSubframeNum + m_phyMacConfig->GetL1L2CtrlLatency ();
        }

      newradioMacSchedSapProvider::SchedTriggerReqParameters params;
      SfnSf schedSfn (dlSchedframeNum, dlSchedSubframeNum, 0);
      params.m_snfSf = schedSfn;

      // Forward DL HARQ feebacks collected during last subframe TTI
      if (m_dlHarqInfoReceived.size () > 0)
        {
          params.m_dlHarqInfoList = m_dlHarqInfoReceived;
          // empty local buffer
          m_dlHarqInfoReceived.clear ();
        }

      // Forward UL HARQ feebacks collected during last TTI
      if (m_ulHarqInfoReceived.size () > 0)
        {
          params.m_ulHarqInfoList = m_ulHarqInfoReceived;
          // empty local buffer
          m_ulHarqInfoReceived.clear ();
        }

      params.m_ueList = m_associatedUe;
      m_macSchedSapProvider->SchedTriggerReq (params);
    }
}

void
newradioEnbMac::SetCellId (uint16_t cellId)
{
  m_cellId = cellId;
}

void
newradioEnbMac::SetMcs (int mcs)
{
  m_macSchedSapProvider->SchedSetMcs (mcs);
}

void
newradioEnbMac::AssociateUeMAC (uint64_t imsi)
{
  //NS_LOG_UNCOND (this<<"Associate UE (imsi:"<< imsi<<" ) with enb");

  //m_associatedUe.push_back (imsi);

}

void
newradioEnbMac::SetForwardUpCallback (Callback <void, Ptr<Packet> > cb)
{
  m_forwardUpCallback = cb;
}

void
newradioEnbMac::DoReceivePhyPdu (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this);
  LteRadioBearerTag tag;
  p->RemovePacketTag (tag);
  uint16_t rnti = tag.GetRnti ();
  newradioMacPduHeader macHeader;
  p->RemoveHeader (macHeader);
  std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator rntiIt = m_rlcAttached.find (rnti);
  NS_ASSERT_MSG (rntiIt != m_rlcAttached.end (), "could not find RNTI" << rnti);
  std::vector<MacSubheader> macSubheaders = macHeader.GetSubheaders ();
  uint32_t currPos = 0;
  for (unsigned ipdu = 0; ipdu < macSubheaders.size (); ipdu++)
    {
      if (macSubheaders[ipdu].m_size == 0)
        {
          continue;
        }
      std::map<uint8_t, LteMacSapUser*>::iterator lcidIt = rntiIt->second.find (macSubheaders[ipdu].m_lcid);
      NS_ASSERT_MSG (lcidIt != rntiIt->second.end (), "could not find LCID" << macSubheaders[ipdu].m_lcid);
      Ptr<Packet> rlcPdu;
      if ((p->GetSize () - currPos) < (uint32_t)macSubheaders[ipdu].m_size)
        {
          NS_LOG_ERROR ("Packet size less than specified in MAC header (actual= " \
                        << p->GetSize () << " header= " << (uint32_t)macSubheaders[ipdu].m_size << ")" );
        }
      else if ((p->GetSize () - currPos) > (uint32_t)macSubheaders[ipdu].m_size)
        {
          NS_LOG_DEBUG ("Fragmenting MAC PDU (packet size greater than specified in MAC header (actual= " \
                        << p->GetSize () << " header= " << (uint32_t)macSubheaders[ipdu].m_size << ")" );
          rlcPdu = p->CreateFragment (currPos, macSubheaders[ipdu].m_size);
          currPos += macSubheaders[ipdu].m_size;

          LteMacSapUser::ReceivePduParameters rxPduParams;
          rxPduParams.p = rlcPdu;
          rxPduParams.rnti = rnti;
          rxPduParams.lcid = macSubheaders[ipdu].m_lcid;
          (*lcidIt).second->ReceivePdu (rxPduParams);
        }
      else
        {
          rlcPdu = p->CreateFragment (currPos, p->GetSize () - currPos);
          currPos = p->GetSize ();

          LteMacSapUser::ReceivePduParameters rxPduParams;
          rxPduParams.p = rlcPdu;
          rxPduParams.rnti = rnti;
          rxPduParams.lcid = macSubheaders[ipdu].m_lcid;
          (*lcidIt).second->ReceivePdu (rxPduParams);
        }
      NS_LOG_INFO ("newradio Enb Mac Rx Packet, Rnti:" << rnti << " lcid:" << (uint32_t)macSubheaders[ipdu].m_lcid << " size:" << macSubheaders[ipdu].m_size);
    }
}

newradioEnbPhySapUser*
newradioEnbMac::GetPhySapUser ()
{
  return m_phySapUser;
}

void
newradioEnbMac::SetLteCcmMacSapUser (LteCcmMacSapUser* s)
{
  m_ccmMacSapUser = s;
}

LteCcmMacSapProvider*
newradioEnbMac::GetLteCcmMacSapProvider ()
{
  return m_ccmMacSapProvider;
}

void
newradioEnbMac::SetPhySapProvider (newradioPhySapProvider* ptr)
{
  m_phySapProvider = ptr;
}

newradioMacSchedSapUser*
newradioEnbMac::GetnewradioMacSchedSapUser ()
{
  return m_macSchedSapUser;
}

void
newradioEnbMac::SetnewradioMacSchedSapProvider (newradioMacSchedSapProvider* ptr)
{
  m_macSchedSapProvider = ptr;
}

newradioMacCschedSapUser*
newradioEnbMac::GetnewradioMacCschedSapUser ()
{
  return m_macCschedSapUser;
}

void
newradioEnbMac::SetnewradioMacCschedSapProvider (newradioMacCschedSapProvider* ptr)
{
  m_macCschedSapProvider = ptr;
}

void
newradioEnbMac::DoUlCqiReport (newradioMacSchedSapProvider::SchedUlCqiInfoReqParameters ulcqi)
{
  if (ulcqi.m_ulCqi.m_type == UlCqiInfo::PUSCH)
    {
      NS_LOG_DEBUG (this << " eNB rxed an PUSCH UL-CQI");
    }
  else if (ulcqi.m_ulCqi.m_type == UlCqiInfo::SRS)
    {
      NS_LOG_DEBUG (this << " eNB rxed an SRS UL-CQI");
    }
  //ulcqi.m_sfnSf = SfnSf(m_frameNum, m_sfNum, m_slotNum);
  NS_LOG_INFO ("*** UL CQI report SINR " << LteFfConverter::fpS11dot3toDouble (ulcqi.m_ulCqi.m_sinr[0]) << " frame " << m_frameNum << " subframe " << m_sfNum << " slot " << m_slotNum );

  m_ulCqiReceived.push_back (ulcqi);
}


void
newradioEnbMac::DoReceiveControlMessage  (Ptr<newradioControlMessage> msg)
{
  NS_LOG_FUNCTION (this << msg);

  switch (msg->GetMessageType ())
    {
    case (newradioControlMessage::DL_CQI):
      {
        Ptr<newradioDlCqiMessage> cqi = DynamicCast<newradioDlCqiMessage> (msg);
        DlCqiInfo cqiElement = cqi->GetDlCqi ();
        NS_ASSERT (cqiElement.m_rnti != 0);
        m_dlCqiReceived.push_back (cqiElement);
        break;
      }
    case (newradioControlMessage::BSR):
      {
        Ptr<newradioBsrMessage> bsr = DynamicCast<newradioBsrMessage> (msg);
        MacCeElement macCeElement = bsr->GetBsr ();

        //Conversion from MacCeElement to MacCeListElement_s needed to use the lte CCM-MAC SAP
        MacCeValue_u macCeValue;
        macCeValue.m_phr = macCeElement.m_macCeValue.m_phr;
        macCeValue.m_crnti = macCeElement.m_macCeValue.m_crnti;
        macCeValue.m_bufferStatus = macCeElement.m_macCeValue.m_bufferStatus;

        MacCeListElement_s macCeListElement;
        macCeListElement.m_rnti = macCeElement.m_rnti;
        macCeListElement.m_macCeType = MacCeListElement_s::BSR;
        macCeListElement.m_macCeValue = macCeValue;

        m_ccmMacSapUser->UlReceiveMacCe (macCeListElement, m_componentCarrierId);

        break;
      }
    case (newradioControlMessage::DL_HARQ):
      {
        Ptr<newradioDlHarqFeedbackMessage> dlharq = DynamicCast<newradioDlHarqFeedbackMessage> (msg);
        DoDlHarqFeedback (dlharq->GetDlHarqFeedback ());
        break;
      }
    default:
      NS_LOG_INFO ("Control message not supported/expected");
    }

}

void
newradioEnbMac::DoReportMacCeToScheduler (MacCeListElement_s bsr)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG (this << " bsr Size " << (uint16_t) m_ulCeReceived.size ());

  //Conversion from MacCeListElement_s to MacCeElement
  MacCeElement macCeElement;
  macCeElement.m_rnti = bsr.m_rnti;
  macCeElement.m_macCeType = MacCeElement::BSR;

  MacCeValue macCeValue;
  macCeValue.m_phr = bsr.m_macCeValue.m_phr;
  macCeValue.m_crnti = bsr.m_macCeValue.m_crnti;
  macCeValue.m_bufferStatus = bsr.m_macCeValue.m_bufferStatus;
  macCeElement.m_macCeValue = macCeValue;

  //send to LteCcmMacSapUser
  m_ulCeReceived.push_back (macCeElement); // this to called when LteUlCcmSapProvider::ReportMacCeToScheduler is called
  NS_LOG_DEBUG (this << " bsr Size after push_back " << (uint16_t) m_ulCeReceived.size ());
}

void
newradioEnbMac::DoUlHarqFeedback (UlHarqInfo params)
{
  NS_LOG_FUNCTION (this);
  m_ulHarqInfoReceived.push_back (params);
}

void
newradioEnbMac::DoDlHarqFeedback (DlHarqInfo params)
{
  NS_LOG_FUNCTION (this);
  // Update HARQ buffer
  std::map <uint16_t, newradioDlHarqProcessesBuffer_t>::iterator it =  m_miDlHarqProcessesPackets.find (params.m_rnti);
  NS_ASSERT (it != m_miDlHarqProcessesPackets.end ());

  if (params.m_harqStatus == DlHarqInfo::ACK)
    {
      // discard buffer
      Ptr<PacketBurst> emptyBuf = CreateObject <PacketBurst> ();
      (*it).second.at (params.m_harqProcessId).m_pktBurst = emptyBuf;
      NS_LOG_DEBUG (this << " HARQ-ACK UE " << params.m_rnti << " harqId " << (uint16_t)params.m_harqProcessId);
    }
  else if (params.m_harqStatus == DlHarqInfo::NACK)
    {
      /*if (params.m_numRetx == 3)
      {
              std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator rntiIt = m_rlcAttached.find (params.m_rnti);
              for (unsigned i = 0; i < (*it).second.at (params.m_harqProcessId).m_lcidList.size (); i++)
              {
                              std::map<uint8_t, LteMacSapUser*>::iterator lcidIt =
                                              rntiIt->second.find ((*it).second.at (params.m_harqProcessId).m_lcidList[i]);
                              NS_ASSERT (lcidIt != rntiIt->second.end ());
                              lcidIt->second->NotifyDlHarqDeliveryFailure (params.m_harqProcessId);
              }
      }*/
      NS_LOG_DEBUG (this << " HARQ-NACK UE " << params.m_rnti << " harqId " << (uint16_t)params.m_harqProcessId);
    }
  else
    {
      NS_FATAL_ERROR (" HARQ functionality not implemented");
    }

  m_dlHarqInfoReceived.push_back (params);
}

void
newradioEnbMac::DoReportBufferStatus (LteMacSapProvider::ReportBufferStatusParameters params)
{
  NS_LOG_FUNCTION (this);
  newradioMacSchedSapProvider::SchedDlRlcBufferReqParameters schedParams;
  schedParams.m_logicalChannelIdentity = params.lcid;
  schedParams.m_rlcRetransmissionHolDelay = params.retxQueueHolDelay;
  schedParams.m_rlcRetransmissionQueueSize = params.retxQueueSize;
  schedParams.m_rlcStatusPduSize = params.statusPduSize;
  schedParams.m_rlcTransmissionQueueHolDelay = params.txQueueHolDelay;
  schedParams.m_rlcTransmissionQueueSize = params.txQueueSize;
  schedParams.m_rnti = params.rnti;

  schedParams.m_txPacketSizes = params.txPacketSizes;
  schedParams.m_txPacketDelays = params.txPacketDelays;
  schedParams.m_retxPacketSizes = params.retxPacketSizes;
  schedParams.m_retxPacketDelays = params.retxPacketDelays;
  schedParams.m_arrivalRate = params.arrivalRate;

  NS_LOG_LOGIC ("ReportBufferStatus for lcid " << (uint16_t)params.lcid << " rnti " << params.rnti << " txPacketSizes " << params.txPacketSizes.size ());

  m_macSchedSapProvider->SchedDlRlcBufferReq (schedParams);
}

// forwarded from LteMacSapProvider
void
newradioEnbMac::DoTransmitPdu (LteMacSapProvider::TransmitPduParameters params)
{
  // TB UID passed back along with RLC data as HARQ process ID
  uint32_t tbMapKey = ((params.rnti & 0xFFFF) << 8) | (params.harqProcessId & 0xFF);
  NS_LOG_LOGIC ("Tx RLC PDU for rnti " << params.rnti << " lcid " << (uint32_t) params.lcid);
  std::map<uint32_t, struct MacPduInfo>::iterator it = m_macPduMap.find (tbMapKey);
  if (it == m_macPduMap.end ())
    {
      NS_FATAL_ERROR ("No MAC PDU storage element found for this TB UID/RNTI");
    }
  else
    {
      if (it->second.m_pdu == 0)
        {
          it->second.m_pdu = params.pdu;
        }
      else
        {
          it->second.m_pdu->AddAtEnd (params.pdu);               // append to MAC PDU
        }
      MacSubheader subheader (params.lcid, params.pdu->GetSize ());
      it->second.m_macHeader.AddSubheader (subheader);           // add RLC PDU sub-header into MAC header
      it->second.m_numRlcPdu++;
    }
}

void
newradioEnbMac::DoSchedConfigIndication (newradioMacSchedSapUser::SchedConfigIndParameters ind)
{
  m_phySapProvider->SetDlSfAllocInfo (ind.m_sfAllocInfo);
  //m_phySapProvider->SetUlSfAllocInfo (ind.m_ulSfAllocInfo);
  LteMacSapUser::TxOpportunityParameters txOpParams;

  for (unsigned islot = 0; islot < ind.m_sfAllocInfo.m_slotAllocInfo.size (); islot++)
    {
      SlotAllocInfo &slotAllocInfo = ind.m_sfAllocInfo.m_slotAllocInfo[islot];
      if (slotAllocInfo.m_slotType != SlotAllocInfo::CTRL && slotAllocInfo.m_tddMode == SlotAllocInfo::DL_slotAllocInfo)
        {
          uint16_t rnti = slotAllocInfo.m_dci.m_rnti;
          // here log all the packets sent in downlink
          m_macDlTxSizeRetx (rnti, m_cellId, slotAllocInfo.m_dci.m_tbSize, slotAllocInfo.m_dci.m_rv);

          std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator rntiIt = m_rlcAttached.find (rnti);
          if (rntiIt == m_rlcAttached.end ())
            {
              NS_FATAL_ERROR ("Scheduled UE " << rntiIt->first << " not attached");
            }
          else
            {
              // Call RLC entities to generate RLC PDUs
              DciInfoElementTdma &dciElem = slotAllocInfo.m_dci;
              uint8_t tbUid = dciElem.m_harqProcess;

              // update Harq Processes
              if (dciElem.m_ndi == 1)
                {
                  NS_ASSERT (dciElem.m_format == DciInfoElementTdma::DL_dci);
                  std::vector<RlcPduInfo> &rlcPduInfo = slotAllocInfo.m_rlcPduInfo;
                  NS_ASSERT (rlcPduInfo.size () > 0);
                  SfnSf pduSfn = ind.m_sfnSf;
                  pduSfn.m_slotNum = slotAllocInfo.m_dci.m_symStart;
                  MacPduInfo macPduInfo (pduSfn, slotAllocInfo.m_dci.m_tbSize, rlcPduInfo.size (), dciElem);
                  // insert into MAC PDU map
                  uint32_t tbMapKey = ((rnti & 0xFFFF) << 8) | (tbUid & 0xFF);
                  std::pair <std::map<uint32_t, struct MacPduInfo>::iterator, bool> mapRet =
                    m_macPduMap.insert (std::pair<uint32_t, struct MacPduInfo> (tbMapKey, macPduInfo));
                  if (!mapRet.second)
                    {
                      NS_FATAL_ERROR ("MAC PDU map element exists");
                    }

                  // new data -> force emptying correspondent harq pkt buffer
                  std::map <uint16_t, newradioDlHarqProcessesBuffer_t>::iterator harqIt = m_miDlHarqProcessesPackets.find (rnti);
                  NS_ASSERT (harqIt != m_miDlHarqProcessesPackets.end ());
                  Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
                  harqIt->second.at (tbUid).m_pktBurst = pb;
                  harqIt->second.at (tbUid).m_lcidList.clear ();

                  std::map<uint32_t, struct MacPduInfo>::iterator pduMapIt = mapRet.first;
                  pduMapIt->second.m_numRlcPdu = 0;
                  for (unsigned int ipdu = 0; ipdu < rlcPduInfo.size (); ipdu++)
                    {
                      NS_ASSERT_MSG (rntiIt != m_rlcAttached.end (), "could not find RNTI" << rnti);
                      std::map<uint8_t, LteMacSapUser*>::iterator lcidIt = rntiIt->second.find (rlcPduInfo[ipdu].m_lcid);
                      NS_ASSERT_MSG (lcidIt != rntiIt->second.end (), "could not find LCID" << rlcPduInfo[ipdu].m_lcid);
                      NS_LOG_DEBUG ("Notifying RLC of TX opportunity for TB " << (unsigned int)tbUid << " PDU num " << ipdu << " size " << (unsigned int) rlcPduInfo[ipdu].m_size);
                      MacSubheader subheader (rlcPduInfo[ipdu].m_lcid, rlcPduInfo[ipdu].m_size);

                      txOpParams.bytes = (rlcPduInfo[ipdu].m_size) - subheader.GetSize ();
                      txOpParams.layer = 0;
                      txOpParams.harqId = tbUid;
                      txOpParams.componentCarrierId = m_componentCarrierId;
                      txOpParams.rnti = rnti;
                      txOpParams.lcid = rlcPduInfo[ipdu].m_lcid;
                      (*lcidIt).second->NotifyTxOpportunity (txOpParams);
                      harqIt->second.at (tbUid).m_lcidList.push_back (rlcPduInfo[ipdu].m_lcid);
                    }

                  if (pduMapIt->second.m_numRlcPdu == 0)
                    {
                      MacSubheader subheader (3, 0);                            // add subheader for empty packet
                      pduMapIt->second.m_macHeader.AddSubheader (subheader);
                    }
                  pduMapIt->second.m_pdu->AddHeader (pduMapIt->second.m_macHeader);

                  newradioMacPduHeader hdrTst;
                  pduMapIt->second.m_pdu->PeekHeader (hdrTst);

                  NS_ASSERT (pduMapIt->second.m_pdu->GetSize () > 0);
                  LteRadioBearerTag bearerTag (rnti, pduMapIt->second.m_size, 0);
                  pduMapIt->second.m_pdu->AddPacketTag (bearerTag);
                  NS_LOG_DEBUG ("eNB sending MAC pdu size " << pduMapIt->second.m_pdu->GetSize ());
                  for (unsigned i = 0; i < pduMapIt->second.m_macHeader.GetSubheaders ().size (); i++)
                    {
                      NS_LOG_DEBUG ("Subheader " << i << " size " << pduMapIt->second.m_macHeader.GetSubheaders ().at (i).m_size);
                    }
                  NS_LOG_DEBUG ("Total MAC PDU size " << pduMapIt->second.m_pdu->GetSize ());
                  harqIt->second.at (tbUid).m_pktBurst->AddPacket (pduMapIt->second.m_pdu);

                  m_txMacPacketTraceEnb (rnti, m_componentCarrierId, pduMapIt->second.m_pdu->GetSize ());
                  m_phySapProvider->SendMacPdu (pduMapIt->second.m_pdu);
                  m_macPduMap.erase (pduMapIt);                        // delete map entry
                }
              else
                {
                  NS_LOG_INFO ("DL retransmission");
                  if (dciElem.m_tbSize > 0)
                    {
                      // HARQ retransmission -> retrieve TB from HARQ buffer
                      std::map <uint16_t, newradioDlHarqProcessesBuffer_t>::iterator it = m_miDlHarqProcessesPackets.find (rnti);
                      NS_ASSERT (it != m_miDlHarqProcessesPackets.end ());
                      Ptr<PacketBurst> pb = it->second.at (tbUid).m_pktBurst;
                      for (std::list<Ptr<Packet> >::const_iterator j = pb->Begin (); j != pb->End (); ++j)
                        {
                          Ptr<Packet> pkt = (*j)->Copy ();
                          newradioMacPduTag tag;                                                                          // update PDU tag for retransmission
                          if (!pkt->RemovePacketTag (tag))
                            {
                              NS_FATAL_ERROR ("No MAC PDU tag");
                            }
                          tag.SetSfn (SfnSf (ind.m_sfnSf.m_frameNum, ind.m_sfnSf.m_sfNum, dciElem.m_symStart));
                          tag.SetSymStart (dciElem.m_symStart);
                          tag.SetNumSym (dciElem.m_numSym);
                          pkt->AddPacketTag (tag);

                          m_txMacPacketTraceEnb (rnti, m_componentCarrierId, pkt->GetSize ());
                          m_phySapProvider->SendMacPdu (pkt);
                        }
                    }
                }
            }
        }
    }
}

uint8_t newradioEnbMac::AllocateTbUid (void)
{
  return m_tbUid++;
}

// ////////////////////////////////////////////
// CMAC SAP
// ////////////////////////////////////////////

void
newradioEnbMac::DoConfigureMac (uint8_t ulBandwidth, uint8_t dlBandwidth)
{
  NS_LOG_FUNCTION (this << " ulBandwidth=" << (uint16_t) ulBandwidth << " dlBandwidth=" << (uint16_t) dlBandwidth);
  newradioMacCschedSapProvider::CschedCellConfigReqParameters params;
  // Configure the subset of parameters used by FfMacScheduler
  params.m_ulBandwidth = ulBandwidth;
  params.m_dlBandwidth = dlBandwidth;
  //m_macChTtiDelay = m_phySapProvider->GetMacChTtiDelay ();  // Gets set by newradioPhyMacCommon
  m_macCschedSapProvider->CschedCellConfigReq (params);
}


void
newradioEnbMac::DoAddUe (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << " DoAddUe rnti=" << rnti);
  std::map<uint8_t, LteMacSapUser*> empty;
  std::pair <std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator, bool>
  ret = m_rlcAttached.insert (std::pair <uint16_t,  std::map<uint8_t, LteMacSapUser*> >
                                (rnti, empty));
  NS_ASSERT_MSG (ret.second, "element already present, RNTI already existed");
  //m_associatedUe.push_back (rnti);

  newradioMacCschedSapProvider::CschedUeConfigReqParameters params;
  params.m_rnti = rnti;
  params.m_transmissionMode = 0;       // set to default value (SISO) for avoiding random initialization (valgrind error)
  m_macCschedSapProvider->CschedUeConfigReq (params);

  // Create DL transmission HARQ buffers
  newradioDlHarqProcessesBuffer_t buf;
  uint16_t harqNum = m_phyMacConfig->GetNumHarqProcess ();
  buf.resize (harqNum);
  for (uint8_t i = 0; i < harqNum; i++)
    {
      Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
      buf.at (i).m_pktBurst = pb;
    }
  m_miDlHarqProcessesPackets.insert (std::pair <uint16_t, newradioDlHarqProcessesBuffer_t> (rnti, buf));

}

void
newradioEnbMac::DoRemoveUe (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << " rnti=" << rnti);
  newradioMacCschedSapProvider::CschedUeReleaseReqParameters params;
  params.m_rnti = rnti;
  m_macCschedSapProvider->CschedUeReleaseReq (params);
  m_miDlHarqProcessesPackets.erase (rnti);
  // for(std::vector<UlHarqInfo>::iterator iter = m_ulHarqInfoReceived.begin(); iter != m_ulHarqInfoReceived.end(); ++iter)
  // {
  //    if(iter->m_rnti == rnti)
  //    {
  //            iter = m_ulHarqInfoReceived.erase(iter);
  //    }
  // }
  m_rlcAttached.erase (rnti);
}

void
newradioEnbMac::DoAddLc (LteEnbCmacSapProvider::LcInfo lcinfo, LteMacSapUser* msu)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_FUNCTION (this);
  NS_LOG_INFO ("Add LC for lcid " << lcinfo.lcId);

  std::map <LteFlowId_t, LteMacSapUser* >::iterator it;

  LteFlowId_t flow (lcinfo.rnti, lcinfo.lcId);

  std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator rntiIt = m_rlcAttached.find (lcinfo.rnti);
  NS_ASSERT_MSG (rntiIt != m_rlcAttached.end (), "RNTI not found");
  std::map<uint8_t, LteMacSapUser*>::iterator lcidIt = rntiIt->second.find (lcinfo.lcId);
  if (lcidIt == rntiIt->second.end ())
    {
      rntiIt->second.insert (std::pair<uint8_t, LteMacSapUser*> (lcinfo.lcId, msu));
    }
  else
    {
      NS_LOG_INFO ("LC already exists");
    }

  // CCCH (LCID 0) is pre-configured
  // see FF LTE MAC Scheduler
  // Interface Specification v1.11,
  // 4.3.4 logicalChannelConfigListElement
  if (true) //(lcinfo.lcId != 0)
    {
      struct newradioMacCschedSapProvider::CschedLcConfigReqParameters params;
      params.m_rnti = lcinfo.rnti;
      params.m_reconfigureFlag = false;

      struct LogicalChannelConfigListElement_s lccle;
      lccle.m_logicalChannelIdentity = lcinfo.lcId;
      lccle.m_logicalChannelGroup = lcinfo.lcGroup;
      lccle.m_direction = LogicalChannelConfigListElement_s::DIR_BOTH;
      lccle.m_qosBearerType = lcinfo.isGbr ? LogicalChannelConfigListElement_s::QBT_GBR : LogicalChannelConfigListElement_s::QBT_NON_GBR;
      lccle.m_qci = lcinfo.qci;
      lccle.m_eRabMaximulBitrateUl = lcinfo.mbrUl;
      lccle.m_eRabMaximulBitrateDl = lcinfo.mbrDl;
      lccle.m_eRabGuaranteedBitrateUl = lcinfo.gbrUl;
      lccle.m_eRabGuaranteedBitrateDl = lcinfo.gbrDl;
      params.m_logicalChannelConfigList.push_back (lccle);

      m_macCschedSapProvider->CschedLcConfigReq (params);
    }
}

void
newradioEnbMac::DoReconfigureLc (LteEnbCmacSapProvider::LcInfo lcinfo)
{
  NS_FATAL_ERROR ("not implemented");
}

void
newradioEnbMac::DoReleaseLc (uint16_t rnti, uint8_t lcid)
{
  //Find user based on rnti and then erase lcid stored against the same
  NS_LOG_INFO ("ReleaseLc");
  std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> >::iterator rntiIt = m_rlcAttached.find (rnti);
  rntiIt->second.erase (lcid);

  struct newradioMacCschedSapProvider::CschedLcReleaseReqParameters params;
  params.m_rnti = rnti;
  params.m_logicalChannelIdentity.push_back (lcid);
  m_macCschedSapProvider->CschedLcReleaseReq (params);
}

void
newradioEnbMac::UeUpdateConfigurationReq (LteEnbCmacSapProvider::UeConfig params)
{
  NS_LOG_FUNCTION (this);
  // propagates to scheduler
  newradioMacCschedSapProvider::CschedUeConfigReqParameters req;
  req.m_rnti = params.m_rnti;
  req.m_transmissionMode = params.m_transmissionMode;
  req.m_reconfigureFlag = true;
  m_macCschedSapProvider->CschedUeConfigReq (req);
}

LteEnbCmacSapProvider::RachConfig
newradioEnbMac::DoGetRachConfig ()
{
  struct LteEnbCmacSapProvider::RachConfig rc;
  rc.numberOfRaPreambles = m_numberOfRaPreambles;
  rc.preambleTransMax = m_preambleTransMax;
  rc.raResponseWindowSize = m_raResponseWindowSize;
  return rc;
}

LteEnbCmacSapProvider::AllocateNcRaPreambleReturnValue
newradioEnbMac::DoAllocateNcRaPreamble (uint16_t rnti)
{
  bool found = false;
  uint8_t preambleId;
  for (preambleId = m_numberOfRaPreambles; preambleId < 64; ++preambleId)
    {
      std::map<uint8_t, NcRaPreambleInfo>::iterator it = m_allocatedNcRaPreambleMap.find (preambleId);
      if ((it ==  m_allocatedNcRaPreambleMap.end ())
          || (it->second.expiryTime < Simulator::Now ()))
        {
          found = true;
          NcRaPreambleInfo preambleInfo;
          uint32_t expiryIntervalMs = (uint32_t) m_preambleTransMax * ((uint32_t) m_raResponseWindowSize + 5);

          preambleInfo.expiryTime = Simulator::Now () + MilliSeconds (expiryIntervalMs);
          preambleInfo.rnti = rnti;
          NS_LOG_INFO ("allocated preamble for NC based RA: preamble " << (uint16_t)preambleId << ", RNTI " << preambleInfo.rnti << ", exiryTime " << preambleInfo.expiryTime);
          m_allocatedNcRaPreambleMap[preambleId] = preambleInfo;     // create if not exist, update otherwise
          break;
        }
    }
  LteEnbCmacSapProvider::AllocateNcRaPreambleReturnValue ret;
  if (found)
    {
      ret.valid = true;
      ret.raPreambleId = preambleId;
      ret.raPrachMaskIndex = 0;
    }
  else
    {
      ret.valid = false;
      ret.raPreambleId = 0;
      ret.raPrachMaskIndex = 0;
    }
  return ret;
}

// ////////////////////////////////////////////
// CSCHED SAP
// ////////////////////////////////////////////


void
newradioEnbMac::DoCschedCellConfigCnf (newradioMacCschedSapUser::CschedCellConfigCnfParameters params)
{
  NS_LOG_FUNCTION (this);
}

void
newradioEnbMac::DoCschedUeConfigCnf (newradioMacCschedSapUser::CschedUeConfigCnfParameters params)
{
  NS_LOG_FUNCTION (this);
}

void
newradioEnbMac::DoCschedLcConfigCnf (newradioMacCschedSapUser::CschedLcConfigCnfParameters params)
{
  NS_LOG_FUNCTION (this);
  // Call the CSCHED primitive
  // m_cschedSap->LcConfigCompleted();
}

void
newradioEnbMac::DoCschedLcReleaseCnf (newradioMacCschedSapUser::CschedLcReleaseCnfParameters params)
{
  NS_LOG_FUNCTION (this);
}

void
newradioEnbMac::DoCschedUeReleaseCnf (newradioMacCschedSapUser::CschedUeReleaseCnfParameters params)
{
  NS_LOG_FUNCTION (this);
}

void
newradioEnbMac::DoCschedUeConfigUpdateInd (newradioMacCschedSapUser::CschedUeConfigUpdateIndParameters params)
{
  NS_LOG_FUNCTION (this);
  // propagates to RRC
  LteEnbCmacSapUser::UeConfig ueConfigUpdate;
  ueConfigUpdate.m_rnti = params.m_rnti;
  ueConfigUpdate.m_transmissionMode = params.m_transmissionMode;
  m_cmacSapUser->RrcConfigurationUpdateInd (ueConfigUpdate);
}

void
newradioEnbMac::DoCschedCellConfigUpdateInd (newradioMacCschedSapUser::CschedCellConfigUpdateIndParameters params)
{
  NS_LOG_FUNCTION (this);
}

} // namespace newradio

} // namespace ns3
