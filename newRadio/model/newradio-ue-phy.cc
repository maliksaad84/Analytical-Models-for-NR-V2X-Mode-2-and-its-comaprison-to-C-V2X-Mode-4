#include <ns3/object-factory.h>
#include <ns3/log.h>
#include <cfloat>
#include <cmath>
#include <ns3/simulator.h>
#include <ns3/double.h>
#include "newradio-ue-phy.h"
#include "newradio-ue-net-device.h"
#include "mc-ue-net-device.h"
#include "newradio-spectrum-value-helper.h"
#include <ns3/pointer.h>
#include <ns3/node.h>

namespace ns3 {

namespace newradio {

NS_LOG_COMPONENT_DEFINE ("newradioUePhy");

NS_OBJECT_ENSURE_REGISTERED (newradioUePhy);

newradioUePhy::newradioUePhy ()
{
  NS_LOG_FUNCTION (this);
  NS_FATAL_ERROR ("This constructor should not be called");
}

newradioUePhy::newradioUePhy (Ptr<newradioSpectrumPhy> dlPhy, Ptr<newradioSpectrumPhy> ulPhy)
  : newradioPhy (dlPhy, ulPhy),
    m_prevSlot (0),
    m_rnti (0)
{
  NS_LOG_FUNCTION (this);
  m_wbCqiLast = Simulator::Now ();
  m_cellSinrMap.clear ();
  m_ueCphySapProvider = new MemberLteUeCphySapProvider<newradioUePhy> (this);
  Simulator::ScheduleNow (&newradioUePhy::SubframeIndication, this, 0, 0);
}

newradioUePhy::~newradioUePhy ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
newradioUePhy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::newradioUePhy")
    .SetParent<newradioPhy> ()
    .AddConstructor<newradioUePhy> ()
    .AddAttribute ("TxPower",
                   "Transmission power in dBm",
                   DoubleValue (30.0),         //TBD zml
                   MakeDoubleAccessor (&newradioUePhy::SetTxPower,
                                       &newradioUePhy::GetTxPower),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("DlSpectrumPhy",
                   "The downlink newradioSpectrumPhy associated to this newradioPhy",
                   TypeId::ATTR_GET,
                   PointerValue (),
                   MakePointerAccessor (&newradioUePhy::GetDlSpectrumPhy),
                   MakePointerChecker <newradioSpectrumPhy> ())
    .AddAttribute ("UlSpectrumPhy",
                   "The uplink newradioSpectrumPhy associated to this newradioPhy",
                   TypeId::ATTR_GET,
                   PointerValue (),
                   MakePointerAccessor (&newradioUePhy::GetUlSpectrumPhy),
                   MakePointerChecker <newradioSpectrumPhy> ())
    .AddTraceSource ("ReportCurrentCellRsrpSinr",
                     "RSRP and SINR statistics.",
                     MakeTraceSourceAccessor (&newradioUePhy::m_reportCurrentCellRsrpSinrTrace),
                     "ns3::CurrentCellRsrpSinr::TracedCallback")
    .AddTraceSource ("ReportUplinkTbSize",
                     "Report allocated uplink TB size for trace.",
                     MakeTraceSourceAccessor (&newradioUePhy::m_reportUlTbSize),
                     "ns3::UlTbSize::TracedCallback")
    .AddTraceSource ("ReportDownlinkTbSize",
                     "Report allocated downlink TB size for trace.",
                     MakeTraceSourceAccessor (&newradioUePhy::m_reportDlTbSize),
                     "ns3::DlTbSize::TracedCallback")
    .AddAttribute ("OutageThreshold",
                   "SNR threshold for outage events [dB]",
                   DoubleValue (-5.0),
                   MakeDoubleAccessor (&newradioUePhy::m_outageThreshold),
                   MakeDoubleChecker<long double> (-70.0, 10.0))
    .AddAttribute ("n310",
                   "Counter for SINR below threshold events",
                   UintegerValue (2),
                   MakeUintegerAccessor (&newradioUePhy::m_n310),
                   MakeUintegerChecker<uint32_t> ())
  ;

  return tid;
}

void
newradioUePhy::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);
  m_dlCtrlPeriod = NanoSeconds (1000 * m_phyMacConfig->GetDlCtrlSymbols () * m_phyMacConfig->GetSymbolPeriod ());
  m_ulCtrlPeriod = NanoSeconds (1000 * m_phyMacConfig->GetUlCtrlSymbols () * m_phyMacConfig->GetSymbolPeriod ());

  for (unsigned i = 0; i < m_phyMacConfig->GetSubframesPerFrame (); i++)
    {
      m_sfAllocInfo.push_back (SfAllocInfo (SfnSf (0, i, 0)));
      SlotAllocInfo dlCtrlSlot;
      dlCtrlSlot.m_slotType = SlotAllocInfo::CTRL;
      dlCtrlSlot.m_numCtrlSym = 1;
      dlCtrlSlot.m_tddMode = SlotAllocInfo::DL_slotAllocInfo;
      dlCtrlSlot.m_dci.m_numSym = 1;
      dlCtrlSlot.m_dci.m_symStart = 0;
      SlotAllocInfo ulCtrlSlot;
      ulCtrlSlot.m_slotType = SlotAllocInfo::CTRL;
      ulCtrlSlot.m_numCtrlSym = 1;
      ulCtrlSlot.m_tddMode = SlotAllocInfo::UL_slotAllocInfo;
      ulCtrlSlot.m_slotIdx = 0xFF;
      ulCtrlSlot.m_dci.m_numSym = 1;
      ulCtrlSlot.m_dci.m_symStart = m_phyMacConfig->GetSymbolsPerSubframe () - 1;
      m_sfAllocInfo[i].m_slotAllocInfo.push_back (dlCtrlSlot);
      m_sfAllocInfo[i].m_slotAllocInfo.push_back (ulCtrlSlot);
    }

  for (unsigned i = 0; i < m_phyMacConfig->GetTotalNumChunk (); i++)
    {
      m_channelChunks.push_back (i);
    }

  m_sfPeriod = NanoSeconds (1000.0 * m_phyMacConfig->GetSubframePeriod ());

  m_phyReset = true;
  newradioPhy::DoInitialize ();
}

void
newradioUePhy::DoDispose (void)
{
  m_registeredEnb.clear ();
}

void
newradioUePhy::SetUeCphySapUser (LteUeCphySapUser* s)
{
  NS_LOG_FUNCTION (this);
  m_ueCphySapUser = s;
}

LteUeCphySapProvider*
newradioUePhy::GetUeCphySapProvider ()
{
  NS_LOG_FUNCTION (this);
  return (m_ueCphySapProvider);
}

void
newradioUePhy::SetImsi (uint64_t imsi)
{
  m_imsi = imsi;
}

uint64_t
newradioUePhy::GetImsi (void) const
{
  return m_imsi;
}

void
newradioUePhy::SetTxPower (double pow)
{
  m_txPower = pow;
}
double
newradioUePhy::GetTxPower () const
{
  return m_txPower;
}

void
newradioUePhy::SetNoiseFigure (double pf)
{

}

double
newradioUePhy::GetNoiseFigure () const
{
  return m_noiseFigure;
}

Ptr<SpectrumValue>
newradioUePhy::CreateTxPowerSpectralDensity ()
{
  Ptr<SpectrumValue> psd =
    newradioSpectrumValueHelper::CreateTxPowerSpectralDensity (m_phyMacConfig, m_txPower, m_subChannelsForTx );
  return psd;
}

void
newradioUePhy::DoSetSubChannels ()
{

}

void
newradioUePhy::SetSubChannelsForReception (std::vector <int> mask)
{

}

void
newradioUePhy::UpdateSinrEstimate (uint16_t cellId, double sinr)
{
  NS_LOG_FUNCTION (this);
  if (m_cellSinrMap.find (cellId) != m_cellSinrMap.end ())
    {
      m_cellSinrMap.find (cellId)->second = sinr;
    }
  else
    {
      m_cellSinrMap.insert (std::pair<uint16_t, double> (cellId, sinr));
    }

  if (cellId == m_cellId)      // update for SNR of the current cell
    {
      long double currentCellSinr = 10 * std::log10 (m_cellSinrMap.find (m_cellId)->second);
      if (currentCellSinr < m_outageThreshold)
        {
          m_consecutiveSinrBelowThreshold++;
          if (m_consecutiveSinrBelowThreshold > m_n310)
            {
              // TODO raise a call to upper layers
              NS_LOG_DEBUG ("Phy layer detects SNR below threshold for " << m_n310 << " times");
            }
        }
      else
        {
          m_consecutiveSinrBelowThreshold = 0;
        }
      NS_LOG_DEBUG ("Phy layers: update sinr value for cell " << m_cellId << " to " << currentCellSinr << " m_consecutiveSinrBelowThreshold " << (uint16_t)m_consecutiveSinrBelowThreshold << " at time " << Simulator::Now ());
    }
}

std::vector <int>
newradioUePhy::GetSubChannelsForReception (void)
{
  std::vector <int> vec;

  return vec;
}

void
newradioUePhy::SetSubChannelsForTransmission (std::vector <int> mask)
{
  m_subChannelsForTx = mask;
  Ptr<SpectrumValue> txPsd = CreateTxPowerSpectralDensity ();
  NS_ASSERT (txPsd);
  m_downlinkSpectrumPhy->SetTxPowerSpectralDensity (txPsd);
}

std::vector <int>
newradioUePhy::GetSubChannelsForTransmission (void)
{
  std::vector <int> vec;

  return vec;
}

void
newradioUePhy::DoSendControlMessage (Ptr<newradioControlMessage> msg)
{
  NS_LOG_FUNCTION (this << msg);
  SetControlMessage (msg);
}


void
newradioUePhy::RegisterToEnb (uint16_t cellId, Ptr<newradioPhyMacCommon> config)
{
  m_cellId = cellId;
  m_phyReset = false;
  //TBD how to assign bandwitdh and earfcn
  m_noiseFigure = 5.0;
  m_phyMacConfig = config;
  m_phySapUser->SetConfigurationParameters (config);

  Ptr<newradioEnbNetDevice> enbNetDevice = m_registeredEnb.find (cellId)->second.second;
  if (DynamicCast<newradio::newradioUeNetDevice> (m_netDevice))
    {
      DynamicCast<newradio::newradioUeNetDevice> (m_netDevice)->SetTargetEnb (enbNetDevice);
    }
  else if (DynamicCast<McUeNetDevice> (m_netDevice))
    {
      DynamicCast<McUeNetDevice> (m_netDevice)->SetnewradioTargetEnb (enbNetDevice);
    }
  NS_LOG_UNCOND ("UE register to enb " << m_cellId);
  // call antennaarrya to change the bf vector
  Ptr<AntennaArrayModel> txAntennaArray = DynamicCast<AntennaArrayModel> (GetDlSpectrumPhy ()->GetRxAntenna ());
  if (txAntennaArray != 0)
    {
      txAntennaArray->ChangeBeamformingVectorPanel (enbNetDevice);
    }
  else
    {
      NS_FATAL_ERROR ("UE is not using an AntennaArrayModel");
    }

  if (m_frameNum != 0)
    {
      m_sfAllocInfo.clear ();          //clear the no more valid DCI, then rebuild the structure
      uint8_t nextSf = m_sfNum + 1;
      for (unsigned i = 0; i < nextSf; i++)
        {
          NS_LOG_INFO ("SfAllocInfo for subframe " << i << " frame " << m_frameNum + 1);
          m_sfAllocInfo.push_back (SfAllocInfo (SfnSf (m_frameNum + 1, i, 0)));
          SlotAllocInfo dlCtrlSlot;
          dlCtrlSlot.m_slotType = SlotAllocInfo::CTRL;
          dlCtrlSlot.m_numCtrlSym = 1;
          dlCtrlSlot.m_tddMode = SlotAllocInfo::DL_slotAllocInfo;
          dlCtrlSlot.m_dci.m_numSym = 1;
          dlCtrlSlot.m_dci.m_symStart = 0;
          SlotAllocInfo ulCtrlSlot;
          ulCtrlSlot.m_slotType = SlotAllocInfo::CTRL;
          ulCtrlSlot.m_numCtrlSym = 1;
          ulCtrlSlot.m_tddMode = SlotAllocInfo::UL_slotAllocInfo;
          ulCtrlSlot.m_slotIdx = 0xFF;
          ulCtrlSlot.m_dci.m_numSym = 1;
          ulCtrlSlot.m_dci.m_symStart = m_phyMacConfig->GetSymbolsPerSubframe () - 1;
          m_sfAllocInfo[i].m_slotAllocInfo.push_back (dlCtrlSlot);
          m_sfAllocInfo[i].m_slotAllocInfo.push_back (ulCtrlSlot);
        }
      for (unsigned i = nextSf; i < m_phyMacConfig->GetSubframesPerFrame (); i++)
        {
          NS_LOG_INFO ("SfAllocInfo for subframe " << i << " frame " << m_frameNum);
          m_sfAllocInfo.push_back (SfAllocInfo (SfnSf (m_frameNum, i, 0)));
          SlotAllocInfo dlCtrlSlot;
          dlCtrlSlot.m_slotType = SlotAllocInfo::CTRL;
          dlCtrlSlot.m_numCtrlSym = 1;
          dlCtrlSlot.m_tddMode = SlotAllocInfo::DL_slotAllocInfo;
          dlCtrlSlot.m_dci.m_numSym = 1;
          dlCtrlSlot.m_dci.m_symStart = 0;
          SlotAllocInfo ulCtrlSlot;
          ulCtrlSlot.m_slotType = SlotAllocInfo::CTRL;
          ulCtrlSlot.m_numCtrlSym = 1;
          ulCtrlSlot.m_tddMode = SlotAllocInfo::UL_slotAllocInfo;
          ulCtrlSlot.m_slotIdx = 0xFF;
          ulCtrlSlot.m_dci.m_numSym = 1;
          ulCtrlSlot.m_dci.m_symStart = m_phyMacConfig->GetSymbolsPerSubframe () - 1;
          m_sfAllocInfo[i].m_slotAllocInfo.push_back (dlCtrlSlot);
          m_sfAllocInfo[i].m_slotAllocInfo.push_back (ulCtrlSlot);
        }
    }

  m_downlinkSpectrumPhy->ResetSpectrumModel ();
  Ptr<SpectrumValue> noisePsd =
    newradioSpectrumValueHelper::CreateNoisePowerSpectralDensity (m_phyMacConfig, m_noiseFigure);
  m_downlinkSpectrumPhy->SetNoisePowerSpectralDensity (noisePsd);
  m_downlinkSpectrumPhy->GetSpectrumChannel ()->AddRx (m_downlinkSpectrumPhy);
  m_downlinkSpectrumPhy->SetCellId (m_cellId);
  NS_LOG_INFO ("Registered to eNB with CellId " << m_cellId);
}

void
newradioUePhy::RegisterOtherEnb (uint16_t cellId, Ptr<newradioPhyMacCommon> config, Ptr<newradioEnbNetDevice> enbNetDevice)
{
  NS_ASSERT_MSG (m_registeredEnb.find (cellId) == m_registeredEnb.end (), "Enb already registered");
  std::pair<Ptr<newradioPhyMacCommon>, Ptr<newradioEnbNetDevice> > pair (config, enbNetDevice);
  m_registeredEnb[cellId] = pair;
}

Ptr<newradioSpectrumPhy>
newradioUePhy::GetDlSpectrumPhy () const
{
  return m_downlinkSpectrumPhy;
}

Ptr<newradioSpectrumPhy>
newradioUePhy::GetUlSpectrumPhy () const
{
  return m_uplinkSpectrumPhy;
}

void
newradioUePhy::ReceiveControlMessageList (std::list<Ptr<newradioControlMessage> > msgList)
{
  NS_LOG_FUNCTION (this);

  std::list<Ptr<newradioControlMessage> >::iterator it;
  for (it = msgList.begin (); it != msgList.end (); it++)
    {
      Ptr<newradioControlMessage> msg = (*it);

      if (msg->GetMessageType () == newradioControlMessage::DCI_TDMA)
        {
          NS_ASSERT_MSG (m_slotNum == 0, "UE" << m_rnti << " got DCI on slot != 0");
          Ptr<newradioTdmaDciMessage> dciMsg = DynamicCast<newradioTdmaDciMessage> (msg);
          DciInfoElementTdma dciInfoElem = dciMsg->GetDciInfoElement ();
          SfnSf dciSfn = dciMsg->GetSfnSf ();

          if (dciSfn.m_frameNum != m_frameNum || dciSfn.m_sfNum != m_sfNum)
            {
              NS_FATAL_ERROR ("DCI intended for different subframe (dci= "
                              << dciSfn.m_frameNum << " " << dciSfn.m_sfNum << ", actual= " << m_frameNum << " " << m_sfNum);
            }

          NS_LOG_DEBUG ("UE" << m_rnti << " DCI received for RNTI " << dciInfoElem.m_rnti << " in frame " << m_frameNum << " subframe " << (unsigned)m_sfNum << " slot " << (unsigned)m_slotNum << " format " << (unsigned)dciInfoElem.m_format << " symStart " << (unsigned)dciInfoElem.m_symStart << " numSym " << (unsigned)dciInfoElem.m_numSym);

          if (dciInfoElem.m_rnti != m_rnti)
            {
              continue;                   // DCI not for me
            }

          if (dciInfoElem.m_format == DciInfoElementTdma::DL_dci)               // set downlink slot schedule for current slot
            {
              NS_LOG_DEBUG ("UE" << m_rnti << " DL-DCI received for frame " << m_frameNum << " subframe " << (unsigned)m_sfNum
                                 << " symStart " << (unsigned)dciInfoElem.m_symStart << " numSym " << (unsigned)dciInfoElem.m_numSym  << " tbs " << dciInfoElem.m_tbSize
                                 << " harqId " << (unsigned)dciInfoElem.m_harqProcess);

              SlotAllocInfo slotInfo;
              slotInfo.m_tddMode = SlotAllocInfo::DL_slotAllocInfo;
              slotInfo.m_dci = dciInfoElem;
              slotInfo.m_slotIdx = 0;
              std::deque <SlotAllocInfo>::iterator itSlot;
              for (itSlot = m_currSfAllocInfo.m_slotAllocInfo.begin ();
                   itSlot != m_currSfAllocInfo.m_slotAllocInfo.end (); itSlot++)
                {
                  if (itSlot->m_tddMode == SlotAllocInfo::UL_slotAllocInfo)
                    {
                      break;
                    }
                  slotInfo.m_slotIdx++;
                }
              //m_currSfAllocInfo.m_slotAllocInfo.push_back (slotInfo);  // add SlotAllocInfo to current SfAllocInfo
              m_currSfAllocInfo.m_slotAllocInfo.insert (itSlot, slotInfo);
            }
          else if (dciInfoElem.m_format == DciInfoElementTdma::UL_dci)               // set downlink slot schedule for t+Tul_sched slot
            {
              uint8_t ulSfIdx = (m_sfNum + m_phyMacConfig->GetUlSchedDelay ()) % m_phyMacConfig->GetSubframesPerFrame ();
              uint16_t dciFrame = (ulSfIdx > m_sfNum) ? m_frameNum : m_frameNum + 1;

              NS_LOG_DEBUG ("UE" << m_rnti << " UL-DCI received for frame " << dciFrame << " subframe " << (unsigned)ulSfIdx
                                 << " symStart " << (unsigned)dciInfoElem.m_symStart << " numSym " << (unsigned)dciInfoElem.m_numSym << " tbs " << dciInfoElem.m_tbSize
                                 << " harqId " << (unsigned)dciInfoElem.m_harqProcess);

              SlotAllocInfo slotInfo;
              slotInfo.m_tddMode = SlotAllocInfo::UL_slotAllocInfo;
              slotInfo.m_dci = dciInfoElem;
              SlotAllocInfo ulCtrlSlot = m_sfAllocInfo[ulSfIdx].m_slotAllocInfo.back ();
              m_sfAllocInfo[ulSfIdx].m_slotAllocInfo.pop_back ();
              //ulCtrlSlot.m_slotIdx++;
              slotInfo.m_slotIdx = m_sfAllocInfo[ulSfIdx].m_slotAllocInfo.size ();
              m_sfAllocInfo[ulSfIdx].m_slotAllocInfo.push_back (slotInfo);
              m_sfAllocInfo[ulSfIdx].m_slotAllocInfo.push_back (ulCtrlSlot);
            }

          m_phySapUser->ReceiveControlMessage (msg);
        }
      else if (msg->GetMessageType () == newradioControlMessage::MIB)
        {
          NS_LOG_INFO ("received MIB");
          NS_ASSERT (m_cellId > 0);
          Ptr<newradioMibMessage> msg2 = DynamicCast<newradioMibMessage> (msg);
          m_ueCphySapUser->RecvMasterInformationBlock (m_cellId, msg2->GetMib ());
        }
      else if (msg->GetMessageType () == newradioControlMessage::SIB1)
        {
          NS_ASSERT (m_cellId > 0);
          Ptr<newradioSib1Message> msg2 = DynamicCast<newradioSib1Message> (msg);
          m_ueCphySapUser->RecvSystemInformationBlockType1 (m_cellId, msg2->GetSib1 ());
        }
      else if (msg->GetMessageType () == newradioControlMessage::RAR)
        {
          NS_LOG_INFO ("received RAR");
          NS_ASSERT (m_cellId > 0);

          Ptr<newradioRarMessage> rarMsg = DynamicCast<newradioRarMessage> (msg);

          for (std::list<newradioRarMessage::Rar>::const_iterator it = rarMsg->RarListBegin ();
               it != rarMsg->RarListEnd ();
               ++it)
            {
              if (it->rapId == m_raPreambleId)
                {
                  m_phySapUser->ReceiveControlMessage (rarMsg);
                }
            }
        }
      else
        {
          NS_LOG_DEBUG ("Control message not handled. Type: " << msg->GetMessageType ());
        }
    }
}

void
newradioUePhy::QueueUlTbAlloc (TbAllocInfo m)
{
  NS_LOG_FUNCTION (this);
//  NS_LOG_DEBUG ("UL TB Info Elem queue size == " << m_ulTbAllocQueue.size ());
  m_ulTbAllocQueue.at (m_phyMacConfig->GetUlSchedDelay () - 1).push_back (m);
}

std::list<TbAllocInfo>
newradioUePhy::DequeueUlTbAlloc (void)
{
  NS_LOG_FUNCTION (this);

  if (m_ulTbAllocQueue.empty ())
    {
      std::list<TbAllocInfo> emptylist;
      return (emptylist);
    }

  if (m_ulTbAllocQueue.at (0).size () > 0)
    {
      std::list<TbAllocInfo> ret = m_ulTbAllocQueue.at (0);
      m_ulTbAllocQueue.erase (m_ulTbAllocQueue.begin ());
      std::list<TbAllocInfo> l;
      m_ulTbAllocQueue.push_back (l);
      return (ret);
    }
  else
    {
      m_ulTbAllocQueue.erase (m_ulTbAllocQueue.begin ());
      std::list<TbAllocInfo> l;
      m_ulTbAllocQueue.push_back (l);
      std::list<TbAllocInfo> emptylist;
      return (emptylist);
    }
}

void
newradioUePhy::SubframeIndication (uint16_t frameNum, uint8_t sfNum)
{
  NS_LOG_DEBUG ("frameNum " << frameNum << " subframe " << (uint16_t)sfNum << " current frame " << m_frameNum << " current frame " << (uint16_t)m_sfNum);
  m_frameNum = frameNum;
  m_sfNum = sfNum;
  m_lastSfStart = Simulator::Now ();
  m_currSfAllocInfo = m_sfAllocInfo[m_sfNum];
  NS_ASSERT ((m_currSfAllocInfo.m_sfnSf.m_frameNum == m_frameNum));
  NS_ASSERT ((m_currSfAllocInfo.m_sfnSf.m_sfNum == m_sfNum));
  m_sfAllocInfo[m_sfNum] = SfAllocInfo (SfnSf (m_frameNum + 1, m_sfNum, 0));
  SlotAllocInfo dlCtrlSlot;
  dlCtrlSlot.m_slotType = SlotAllocInfo::CTRL;
  dlCtrlSlot.m_numCtrlSym = 1;
  dlCtrlSlot.m_tddMode = SlotAllocInfo::DL_slotAllocInfo;
  dlCtrlSlot.m_dci.m_numSym = 1;
  dlCtrlSlot.m_dci.m_symStart = 0;
  SlotAllocInfo ulCtrlSlot;
  ulCtrlSlot.m_slotType = SlotAllocInfo::CTRL;
  ulCtrlSlot.m_numCtrlSym = 1;
  ulCtrlSlot.m_tddMode = SlotAllocInfo::UL_slotAllocInfo;
  ulCtrlSlot.m_slotIdx = 0xFF;
  ulCtrlSlot.m_dci.m_numSym = 1;
  ulCtrlSlot.m_dci.m_symStart = m_phyMacConfig->GetSymbolsPerSubframe () - 1;
  m_sfAllocInfo[m_sfNum].m_slotAllocInfo.push_front (dlCtrlSlot);
  m_sfAllocInfo[m_sfNum].m_slotAllocInfo.push_back (ulCtrlSlot);

  // else
  // {
  //    m_currSfAllocInfo = SfAllocInfo (SfnSf (m_frameNum, m_sfNum, 0));
  //    NS_ASSERT ((m_currSfAllocInfo.m_sfnSf.m_frameNum == m_frameNum) &&
  //               (m_currSfAllocInfo.m_sfnSf.m_sfNum == m_sfNum));
  //    SlotAllocInfo dlCtrlSlot;
  //    dlCtrlSlot.m_slotType = SlotAllocInfo::CTRL;
  //    dlCtrlSlot.m_numCtrlSym = 1;
  //    dlCtrlSlot.m_tddMode = SlotAllocInfo::DL_slotAllocInfo;
  //    dlCtrlSlot.m_dci.m_numSym = 1;
  //    dlCtrlSlot.m_dci.m_symStart = 0;
  //    SlotAllocInfo ulCtrlSlot;
  //    ulCtrlSlot.m_slotType = SlotAllocInfo::CTRL;
  //    ulCtrlSlot.m_numCtrlSym = 1;
  //    ulCtrlSlot.m_tddMode = SlotAllocInfo::UL_slotAllocInfo;
  //    ulCtrlSlot.m_slotIdx = 0xFF;
  //    ulCtrlSlot.m_dci.m_numSym = 1;
  //    ulCtrlSlot.m_dci.m_symStart = m_phyMacConfig->GetSymbolsPerSubframe()-1;
  //    m_currSfAllocInfo.m_slotAllocInfo.push_front (dlCtrlSlot);
  //    m_currSfAllocInfo.m_slotAllocInfo.push_back (ulCtrlSlot);

  //    m_sfAllocInfo[m_sfNum] = SfAllocInfo (SfnSf (m_frameNum+1, m_sfNum, 0));
  //    m_sfAllocInfo[m_sfNum].m_slotAllocInfo.push_front (dlCtrlSlot);
  //    m_sfAllocInfo[m_sfNum].m_slotAllocInfo.push_back (ulCtrlSlot);
  // }

  StartSlot ();
}

void
newradioUePhy::StartSlot ()
{
  Ptr<AntennaArrayModel> txAntennaArray = DynamicCast<AntennaArrayModel> (GetDlSpectrumPhy ()->GetRxAntenna ());
  if (m_cellId > 0)
    {
      if (txAntennaArray != 0)
        {
          txAntennaArray->ChangeBeamformingVectorPanel (m_registeredEnb.find (m_cellId)->second.second);
        }
      else
        {
          NS_FATAL_ERROR ("UE is not using an AntennaArrayModel");
        }
    }

  //unsigned slotInd = 0;
  SlotAllocInfo currSlot;
  /*if (m_slotNum >= m_currSfAllocInfo.m_dlSlotAllocInfo.size ())
  {
          if (m_currSfAllocInfo.m_ulSlotAllocInfo.size () > 0)
          {
                  slotInd = m_slotNum - m_currSfAllocInfo.m_dlSlotAllocInfo.size ();
                  currSlot = m_currSfAllocInfo.m_ulSlotAllocInfo[slotInd];
          }
  }
  else
  {
          if (m_currSfAllocInfo.m_ulSlotAllocInfo.size () > 0)
          {
                  slotInd = m_slotNum;
                  currSlot = m_currSfAllocInfo.m_dlSlotAllocInfo[slotInd];
          }
  }*/

  currSlot = m_currSfAllocInfo.m_slotAllocInfo[m_slotNum];
  m_currSlot = currSlot;

  NS_LOG_INFO ("newradio UE " << m_rnti << " frame " << m_frameNum << " subframe " << (uint16_t) m_sfNum << " slot " << (uint16_t) m_slotNum);

  Time slotPeriod;
  m_receptionEnabled = false;

  if (m_slotNum == 0)        // reserved DL control
    {
      slotPeriod = NanoSeconds (1000.0 * m_phyMacConfig->GetSymbolPeriod () * m_phyMacConfig->GetDlCtrlSymbols ());
      NS_LOG_DEBUG ("UE" << m_rnti << " imsi" << m_imsi << " RXing DL CTRL frame " << m_frameNum << " subframe " << (unsigned)m_sfNum << " symbols "
                         << (unsigned)currSlot.m_dci.m_symStart << "-" << (unsigned)(currSlot.m_dci.m_symStart + currSlot.m_dci.m_numSym - 1) <<
                    "\t start " << Simulator::Now () << " end " << (Simulator::Now () + slotPeriod));
    }
  else if (m_slotNum == m_currSfAllocInfo.m_slotAllocInfo.size () - 1)    // reserved UL control
    {
      SetSubChannelsForTransmission (m_channelChunks);
      slotPeriod = NanoSeconds (1000.0 * m_phyMacConfig->GetSymbolPeriod () * m_phyMacConfig->GetUlCtrlSymbols ());
      std::list<Ptr<newradioControlMessage> > ctrlMsg = GetControlMessages ();
      NS_LOG_DEBUG ("UE" << m_rnti << " imsi" << m_imsi << " TXing UL CTRL frame " << m_frameNum << " subframe " << (unsigned)m_sfNum << " symbols "
                         << (unsigned)currSlot.m_dci.m_symStart << "-" << (unsigned)(currSlot.m_dci.m_symStart + currSlot.m_dci.m_numSym - 1) <<
                    "\t start " << Simulator::Now () << " end " << (Simulator::Now () + slotPeriod - NanoSeconds (1.0)));
      SendCtrlChannels (ctrlMsg, slotPeriod - NanoSeconds (1.0));
    }
  else if (currSlot.m_dci.m_format == DciInfoElementTdma::DL_dci)        // scheduled DL data slot
    {
      m_receptionEnabled = true;
      slotPeriod = NanoSeconds (1000.0 * m_phyMacConfig->GetSymbolPeriod () * currSlot.m_dci.m_numSym);
      m_downlinkSpectrumPhy->AddExpectedTb (currSlot.m_dci.m_rnti, currSlot.m_dci.m_ndi, currSlot.m_dci.m_tbSize, currSlot.m_dci.m_mcs,
                                            m_channelChunks, currSlot.m_dci.m_harqProcess, currSlot.m_dci.m_rv, true,
                                            currSlot.m_dci.m_symStart, currSlot.m_dci.m_numSym);
      m_reportDlTbSize (m_imsi, currSlot.m_dci.m_tbSize);
      NS_LOG_DEBUG ("UE" << m_rnti << " imsi" << m_imsi << " RXing DL DATA frame " << m_frameNum << " subframe " << (unsigned)m_sfNum << " symbols "
                         << (unsigned)currSlot.m_dci.m_symStart << "-" << (unsigned)(currSlot.m_dci.m_symStart + currSlot.m_dci.m_numSym - 1) <<
                    "\t start " << Simulator::Now () << " end " << (Simulator::Now () + slotPeriod));
    }
  else if (currSlot.m_dci.m_format == DciInfoElementTdma::UL_dci)       // scheduled UL data slot
    {
      SetSubChannelsForTransmission (m_channelChunks);
      slotPeriod = NanoSeconds (1000.0 * m_phyMacConfig->GetSymbolPeriod () * currSlot.m_dci.m_numSym);
      Ptr<PacketBurst> pktBurst = GetPacketBurst (SfnSf (m_frameNum, m_sfNum, currSlot.m_dci.m_symStart));
      if (pktBurst && pktBurst->GetNPackets () > 0)
        {
          std::list< Ptr<Packet> > pkts = pktBurst->GetPackets ();
          newradioMacPduTag tag;
          pkts.front ()->PeekPacketTag (tag);
          NS_ASSERT ((tag.GetSfn ().m_sfNum == m_sfNum) && (tag.GetSfn ().m_slotNum == currSlot.m_dci.m_symStart));

          LteRadioBearerTag bearerTag;
          if (!pkts.front ()->PeekPacketTag (bearerTag))
            {
              NS_FATAL_ERROR ("No radio bearer tag");
            }
        }
      else
        {
          // sometimes the UE will be scheduled when no data is queued
          // in this case, send an empty PDU
          // newradioMacPduTag tag (SfnSf(m_frameNum, m_sfNum, currSlot.m_dci.m_symStart));
          // Ptr<Packet> emptyPdu = Create <Packet> ();
          // newradioMacPduHeader header;
          // MacSubheader subheader (3, 0);  // lcid = 3, size = 0
          // header.AddSubheader (subheader);
          // emptyPdu->AddHeader (header);
          // emptyPdu->AddPacketTag (tag);
          // LteRadioBearerTag bearerTag (m_rnti, 3, 0);
          // emptyPdu->AddPacketTag (bearerTag);
          // pktBurst = CreateObject<PacketBurst> ();
          // pktBurst->AddPacket (emptyPdu);
        }
      m_reportUlTbSize (m_imsi, currSlot.m_dci.m_tbSize);
      NS_LOG_DEBUG ("UE" << m_rnti << " imsi" << m_imsi << " TXing UL DATA frame " << m_frameNum << " subframe " << (unsigned)m_sfNum << " symbols "
                         << (unsigned)currSlot.m_dci.m_symStart << "-" << (unsigned)(currSlot.m_dci.m_symStart + currSlot.m_dci.m_numSym - 1)
                         << "\t start " << Simulator::Now () << " end " << (Simulator::Now () + slotPeriod));
      if (pktBurst != 0)
        {
          std::list<Ptr<newradioControlMessage> > ctrlMsg = GetControlMessages ();
          m_sendDataChannelEvent = Simulator::Schedule (NanoSeconds (1.0), &newradioUePhy::SendDataChannels, this, pktBurst, ctrlMsg, slotPeriod - NanoSeconds (2.0), m_slotNum);
        }
    }

  m_prevSlotDir = currSlot.m_tddMode;

  m_phySapUser->SubframeIndication (SfnSf (m_frameNum, m_sfNum, m_slotNum));            // trigger mac

  NS_LOG_DEBUG ("newradioUePhy: Scheduling slot end for " << slotPeriod);
  Simulator::Schedule (slotPeriod, &newradioUePhy::EndSlot, this);
}


void
newradioUePhy::EndSlot ()
{
  if (m_slotNum == m_currSfAllocInfo.m_slotAllocInfo.size () - 1)
    {           // end of subframe
      uint16_t frameNum;
      uint8_t sfNum;
      if (m_sfNum == m_phyMacConfig->GetSubframesPerFrame () - 1)
        {
          sfNum = 0;
          frameNum = m_frameNum + 1;
        }
      else
        {
          frameNum = m_frameNum;
          sfNum = m_sfNum + 1;
        }
      m_slotNum = 0;
      NS_LOG_INFO ("newradioUePhy: Next subframe scheduled for " << m_lastSfStart + m_sfPeriod - Simulator::Now () << " first if");
      Simulator::Schedule (m_lastSfStart + m_sfPeriod - Simulator::Now (), &newradioUePhy::SubframeIndication, this, frameNum, sfNum);
    }
  else
    {
      Time nextSlotStart;
      /*uint8_t slotInd = m_slotNum+1;
      if (slotInd >= m_currSfAllocInfo.m_dlSlotAllocInfo.size ())
      {
              if (m_currSfAllocInfo.m_ulSlotAllocInfo.size () > 0)
              {
                      slotInd = slotInd - m_currSfAllocInfo.m_dlSlotAllocInfo.size ();
                      nextSlotStart = NanoSeconds (1000.0 * m_phyMacConfig->GetSymbolPeriod () *
                                                   m_currSfAllocInfo.m_ulSlotAllocInfo[slotInd].m_dci.m_symStart);
              }
      }
      else
      {
              if (m_currSfAllocInfo.m_ulSlotAllocInfo.size () > 0)
              {
                      nextSlotStart = NanoSeconds (1000.0 * m_phyMacConfig->GetSymbolPeriod () *
                                                   m_currSfAllocInfo.m_dlSlotAllocInfo[slotInd].m_dci.m_symStart);
              }
      }*/
      m_slotNum++;
      nextSlotStart = NanoSeconds (1000.0 * m_phyMacConfig->GetSymbolPeriod () *
                                   m_currSfAllocInfo.m_slotAllocInfo[m_slotNum].m_dci.m_symStart);
      NS_LOG_INFO ("m_slotNum " << (uint16_t)m_slotNum);
      NS_LOG_INFO ("m_phyMacConfig->GetSymbolPeriod () " << m_phyMacConfig->GetSymbolPeriod () << " other part " << (uint16_t) m_currSfAllocInfo.m_slotAllocInfo[m_slotNum].m_dci.m_symStart);
      NS_LOG_INFO ("nextSlotStart " << nextSlotStart << " m_lastSfStart " << m_lastSfStart << " now " << Simulator::Now ());
      NS_LOG_INFO ("newradioUePhy: Next subframe scheduled for " << nextSlotStart + m_lastSfStart - Simulator::Now () << " in else");
      Simulator::Schedule (nextSlotStart + m_lastSfStart - Simulator::Now (), &newradioUePhy::StartSlot, this);
    }

  if (m_receptionEnabled)
    {
      m_receptionEnabled = false;
    }
}


uint32_t
newradioUePhy::GetSubframeNumber (void)
{
  return m_slotNum;
}

void
newradioUePhy::PhyDataPacketReceived (Ptr<Packet> p)
{
  if (!m_phyReset)
    {
      Simulator::Schedule (MicroSeconds (m_phyMacConfig->GetTbDecodeLatency ()), &newradioUePhy::DelayPhyDataPacketReceived, this, p);
    }
  //Simulator::ScheduleWithContext (m_netDevice->GetNode()->GetId(),
  //                              MicroSeconds(m_phyMacConfig->GetTbDecodeLatency()),
  //                              &newradioUePhySapUser::ReceivePhyPdu,
  //                              m_phySapUser,
  //                              p);
}

void
newradioUePhy::DelayPhyDataPacketReceived (Ptr<Packet> p)
{
  m_phySapUser->ReceivePhyPdu (p);
}

void
newradioUePhy::SendDataChannels (Ptr<PacketBurst> pb, std::list<Ptr<newradioControlMessage> > ctrlMsg, Time duration, uint8_t slotInd)
{

  //Ptr<AntennaArrayModel> antennaArray = DynamicCast<AntennaArrayModel> (GetDlSpectrumPhy ()->GetRxAntenna());
  /* set beamforming vector;
   * for UE, you can choose 16 antenna with 0-7 sectors, or 4 antenna with 0-3 sectors
   * input is (sector, antenna number)
   *
   * */
  //antennaArray->SetSector (3,16);

  if (pb->GetNPackets () > 0)
    {
      LteRadioBearerTag tag;
      if (!pb->GetPackets ().front ()->PeekPacketTag (tag))
        {
          NS_FATAL_ERROR ("No radio bearer tag");
        }
      // call only if the packet burst is > 0
      m_downlinkSpectrumPhy->StartTxDataFrames (pb, ctrlMsg, duration, slotInd);
    }
}

void
newradioUePhy::SendCtrlChannels (std::list<Ptr<newradioControlMessage> > ctrlMsg, Time prd)
{
  m_downlinkSpectrumPhy->StartTxDlControlFrames (ctrlMsg,prd);
}


uint32_t
newradioUePhy::GetAbsoluteSubframeNo ()
{
  return ((m_frameNum - 1) * 8 + m_slotNum);
}

Ptr<newradioDlCqiMessage>
newradioUePhy::CreateDlCqiFeedbackMessage (const SpectrumValue& sinr)
{
  if (!m_amc)
    {
      m_amc = CreateObject <newradioAmc> (m_phyMacConfig);
    }
  NS_LOG_FUNCTION (this);
  SpectrumValue newSinr = sinr;
  // CREATE DlCqiLteControlMessage
  Ptr<newradioDlCqiMessage> msg = Create<newradioDlCqiMessage> ();
  DlCqiInfo dlcqi;

  dlcqi.m_rnti = m_rnti;
  dlcqi.m_cqiType = DlCqiInfo::WB;
  dlcqi.m_ri = 0;
  dlcqi.m_wbPmi = 0;

  std::vector<int> cqi;

  //uint8_t dlBandwidth = m_phyMacConfig->GetNumChunkPerRb () * m_phyMacConfig->GetNumRb ();
  NS_ASSERT (m_currSlot.m_dci.m_format == 0);
  int mcs;
  dlcqi.m_wbCqi = m_amc->CreateCqiFeedbackWbTdma (newSinr, m_currSlot.m_dci.m_numSym, m_currSlot.m_dci.m_tbSize, mcs);

//	int activeSubChannels = newSinr.GetSpectrumModel()->GetNumBands ();
  /*cqi = m_amc->CreateCqiFeedbacksTdma (newSinr, m_currNumSym);
  int nbSubChannels = cqi.size ();
  double cqiSum = 0.0;
  // average the CQIs of the different RBs
  for (int i = 0; i < nbSubChannels; i++)
  {
          if (cqi.at (i) != -1)
          {
                  cqiSum += cqi.at (i);
                  activeSubChannels++;
          }
//		NS_LOG_DEBUG (this << " subch " << i << " cqi " <<  cqi.at (i));
  }*/
//	if (activeSubChannels > 0)
//	{
//		dlcqi.m_wbCqi = ((uint16_t) cqiSum / activeSubChannels);
//	}
//	else
//	{
//		// approximate with the worst case -> CQI = 1
//		dlcqi.m_wbCqi = 1;
//	}
  msg->SetDlCqi (dlcqi);
  return msg;
}

void
newradioUePhy::GenerateDlCqiReport (const SpectrumValue& sinr)
{
  if (m_ulConfigured && (m_rnti > 0) && m_receptionEnabled)
    {
      if (Simulator::Now () > m_wbCqiLast + m_wbCqiPeriod)
        {
          SpectrumValue newSinr = sinr;
          Ptr<newradioDlCqiMessage> msg = CreateDlCqiFeedbackMessage (newSinr);

          if (msg)
            {
              DoSendControlMessage (msg);
            }
          m_reportCurrentCellRsrpSinrTrace (m_imsi, newSinr, newSinr);
        }
    }
}

void
newradioUePhy::ReceiveLteDlHarqFeedback (DlHarqInfo m)
{
  NS_LOG_FUNCTION (this);
  // generate feedback to eNB and send it through ideal PUCCH
  Ptr<newradioDlHarqFeedbackMessage> msg = Create<newradioDlHarqFeedbackMessage> ();
  msg->SetDlHarqFeedback (m);
  m_sendDlHarqFeedbackEvent = Simulator::Schedule (MicroSeconds (m_phyMacConfig->GetTbDecodeLatency ()), &newradioUePhy::DoSendControlMessage, this, msg);
//  if (m.m_harqStatus == DlHarqInfo::NACK)  // Notify MAC/RLC
//  {
//      m_phySapUser->NotifyHarqDeliveryFailure (m.m_harqProcessId);
//  }
}

bool
newradioUePhy::IsReceptionEnabled ()
{
  return m_receptionEnabled;
}

void
newradioUePhy::ResetReception ()
{
  m_receptionEnabled = false;
}

uint16_t
newradioUePhy::GetRnti ()
{
  return m_rnti;
}


void
newradioUePhy::DoReset ()
{
  NS_LOG_FUNCTION (this);
  m_rnti = 0;
  m_cellId = 0;
  m_raPreambleId = 255;       // value out of range

  m_packetBurstMap.clear ();
  m_controlMessageQueue.clear ();
  m_subChannelsForTx.clear ();

  //for (int i = 0; i < m_macChTtiDelay; i++)
  //  {
  //    Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
  //    m_packetBurstQueue.push_back (pb);
  //    std::list<Ptr<LteControlMessage> > l;
  //    m_controlMessagesQueue.push_back (l);
  //  }
  //std::vector <int> ulRb;
  //m_subChannelsForTransmissionQueue.resize (m_macChTtiDelay, ulRb);

  m_sendDataChannelEvent.Cancel ();
  m_sendDlHarqFeedbackEvent.Cancel ();
  m_downlinkSpectrumPhy->Reset ();
  // clear DCI
  m_phyReset = true;

  //m_currSfAllocInfo.m_slotAllocInfo.clear();
  //m_currSfAllocInfo.m_slotAllocInfo.clear();
}

void
newradioUePhy::DoStartCellSearch (uint16_t dlEarfcn)
{
  NS_LOG_FUNCTION (this << dlEarfcn);
}

void
newradioUePhy::DoSynchronizeWithEnb (uint16_t cellId, uint16_t dlEarfcn)
{
  NS_LOG_FUNCTION (this << cellId << dlEarfcn);
  DoSynchronizeWithEnb (cellId);
}

void
newradioUePhy::DoSetPa (double pa)
{
  NS_LOG_FUNCTION (this << pa);
}

void
newradioUePhy::DoSetRsrpFilterCoefficient (uint8_t rsrpFilterCoefficient)
{
  NS_LOG_WARN ("This method is not supported");
}

void
newradioUePhy::DoSynchronizeWithEnb (uint16_t cellId)
{
  NS_LOG_FUNCTION (this << cellId);
  if (cellId == 0)
    {
      NS_FATAL_ERROR ("Cell ID shall not be zero");
    }
  else
    {
      if (m_registeredEnb.find (cellId) != m_registeredEnb.end ())
        {
          RegisterToEnb (m_registeredEnb.find (cellId)->first, m_registeredEnb.find (cellId)->second.first);
        }
      else
        {
          NS_FATAL_ERROR ("Unknown eNB");
        }
    }
}

void
newradioUePhy::DoSetDlBandwidth (uint8_t dlBandwidth)
{
  NS_LOG_FUNCTION (this << (uint32_t) dlBandwidth);
}


void
newradioUePhy::DoConfigureUplink (uint16_t ulEarfcn, uint8_t ulBandwidth)
{
  NS_LOG_FUNCTION (this << ulEarfcn << ulBandwidth);
  m_ulConfigured = true;
}

void
newradioUePhy::DoConfigureReferenceSignalPower (int8_t referenceSignalPower)
{
  NS_LOG_FUNCTION (this << referenceSignalPower);
}

void
newradioUePhy::DoSetRnti (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << rnti);
  m_rnti = rnti;
}

void
newradioUePhy::DoSetTransmissionMode (uint8_t txMode)
{
  NS_LOG_FUNCTION (this << (uint16_t)txMode);
}

void
newradioUePhy::DoSetSrsConfigurationIndex (uint16_t srcCi)
{
  NS_LOG_FUNCTION (this << srcCi);
}

void
newradioUePhy::SetPhySapUser (newradioUePhySapUser* ptr)
{
  m_phySapUser = ptr;
}

void
newradioUePhy::SetHarqPhyModule (Ptr<newradioHarqPhy> harq)
{
  m_harqPhyModule = harq;
}

}

}
