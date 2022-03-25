#ifndef SRC_newradio_MODEL_newradio_UE_PHY_H_
#define SRC_newradio_MODEL_newradio_UE_PHY_H_

#include <ns3/newradio-phy.h>
#include "newradio-phy-mac-common.h"
#include <ns3/ptr.h>
#include "newradio-amc.h"
#include <map>
#include <ns3/lte-ue-phy-sap.h>
#include <ns3/lte-ue-cphy-sap.h>
#include <ns3/newradio-harq-phy.h>
#include "newradio-enb-net-device.h"



namespace ns3 {

class PacketBurst;

namespace newradio {

class mmwEnbPhy;

class newradioUePhy : public newradioPhy
{
  friend class UeMemberLteUePhySapProvider;
  friend class MemberLteUeCphySapProvider<newradioUePhy>;

public:
  newradioUePhy ();

  newradioUePhy (Ptr<newradioSpectrumPhy> ulPhy, Ptr<newradioSpectrumPhy> dlPhy);

  virtual ~newradioUePhy ();

  // inherited from Object
  static TypeId GetTypeId (void);
  virtual void DoInitialize (void);
  virtual void DoDispose (void);

  LteUeCphySapProvider* GetUeCphySapProvider ();
  void SetUeCphySapUser (LteUeCphySapUser* s);

  void SetTxPower (double pow);
  double GetTxPower () const;

  void SetNoiseFigure (double pf);
  double GetNoiseFigure () const;

  bool SendPacket (Ptr<Packet> packet);

  Ptr<SpectrumValue> CreateTxPowerSpectralDensity ();

  void DoSetSubChannels ();

  void SetSubChannelsForReception (std::vector <int> mask);
  std::vector <int> GetSubChannelsForReception (void);

  void SetSubChannelsForTransmission (std::vector <int> mask);
  std::vector <int> GetSubChannelsForTransmission (void);

  void DoSendControlMessage (Ptr<newradioControlMessage> msg);

  void RegisterToEnb (uint16_t cellId, Ptr<newradioPhyMacCommon> config);
  void RegisterOtherEnb (uint16_t cellId, Ptr<newradioPhyMacCommon> config, Ptr<newradioEnbNetDevice> enb);
  Ptr<newradioSpectrumPhy> GetDlSpectrumPhy () const;
  Ptr<newradioSpectrumPhy> GetUlSpectrumPhy () const;

  void ReceiveControlMessageList (std::list<Ptr<newradioControlMessage> > msgList);

  void SubframeIndication (uint16_t frameNum, uint8_t subframeNum);
  void StartSlot ();
  void EndSlot ();


  uint32_t GetSubframeNumber (void);

  void PhyDataPacketReceived (Ptr<Packet> p);
  void DelayPhyDataPacketReceived (Ptr<Packet> p);

  void SendDataChannels (Ptr<PacketBurst> pb, std::list<Ptr<newradioControlMessage> > ctrlMsg, Time duration, uint8_t slotInd);

  void SendCtrlChannels (std::list<Ptr<newradioControlMessage> > ctrlMsg, Time prd);

  uint32_t GetAbsoluteSubframeNo ();       // Used for tracing purposes

  Ptr<newradioDlCqiMessage> CreateDlCqiFeedbackMessage (const SpectrumValue& sinr);

  void GenerateDlCqiReport (const SpectrumValue& sinr);

  void SetImsi (uint64_t imsi);
  uint64_t GetImsi () const;

  bool IsReceptionEnabled ();
  void ResetReception ();

  uint16_t GetRnti ();

  void SetPhySapUser (newradioUePhySapUser* ptr);

  void SetHarqPhyModule (Ptr<newradioHarqPhy> harq);

  void ReceiveLteDlHarqFeedback (DlHarqInfo m);

  void UpdateSinrEstimate (uint16_t cellId, double sinr);


private:
  void DoReset ();
  void DoStartCellSearch (uint16_t dlEarfcn);
  void DoSynchronizeWithEnb (uint16_t cellId);
  void DoSynchronizeWithEnb (uint16_t cellId, uint16_t dlEarfcn);
  void DoSetPa (double pa);
  void DoSetRsrpFilterCoefficient (uint8_t rsrpFilterCoefficient);
  void DoSetDlBandwidth (uint8_t ulBandwidth);
  void DoConfigureUplink (uint16_t ulEarfcn, uint8_t ulBandwidth);
  void DoConfigureReferenceSignalPower (int8_t referenceSignalPower);
  void DoSetRnti (uint16_t rnti);
  void DoSetTransmissionMode (uint8_t txMode);
  void DoSetSrsConfigurationIndex (uint16_t srcCi);

  void ReceiveDataPeriod (uint32_t slotNum);
  void QueueUlTbAlloc (TbAllocInfo tbAllocInfo);
  std::list<TbAllocInfo> DequeueUlTbAlloc ();

  newradioUePhySapUser* m_phySapUser;

  LteUeCphySapProvider* m_ueCphySapProvider;
  LteUeCphySapUser* m_ueCphySapUser;

  Ptr<newradioAmc> m_amc;
  std::vector <int> m_subChannelsForTx;
  std::vector <int> m_subChannelsforRx;

  uint32_t m_numRbg;

  Time m_wbCqiPeriod;       /**< Wideband Periodic CQI: 2, 5, 10, 16, 20, 32, 40, 64, 80 or 160 ms */
  Time m_wbCqiLast;

  SlotAllocInfo::TddMode m_prevSlotDir;

  SfAllocInfo m_currSfAllocInfo;
  std::vector< std::list<TbAllocInfo> > m_ulTbAllocQueue;       // for storing info on future UL TB transmissions
  bool m_ulGrant;               // true if no uplink grant in subframe, need to transmit UL control in PUCCH instead
  bool m_sfAllocInfoUpdated;
  Time m_dlCtrlPeriod;
  Time m_ulCtrlPeriod;

  Time m_dataPeriod;            // data period length in microseconds
  Time m_sfPeriod;
  Time m_lastSfStart;

  bool m_dlConfigured;
  bool m_ulConfigured;

  TracedCallback< uint64_t, SpectrumValue&, SpectrumValue& > m_reportCurrentCellRsrpSinrTrace;

  TracedCallback<uint64_t, uint64_t> m_reportUlTbSize;
  TracedCallback<uint64_t, uint64_t> m_reportDlTbSize;
  uint8_t m_prevSlot;

  bool m_receptionEnabled;
  uint16_t m_rnti;
  uint64_t m_imsi;

  Ptr<newradioHarqPhy> m_harqPhyModule;

  std::vector<int> m_channelChunks;

  SlotAllocInfo m_currSlot;

  std::map<uint16_t, std::pair<Ptr<newradioPhyMacCommon>, Ptr<newradioEnbNetDevice> > > m_registeredEnb;

  EventId m_sendDataChannelEvent;
  EventId m_sendDlHarqFeedbackEvent;
  bool m_phyReset;

  std::map<uint16_t, double> m_cellSinrMap;

  uint8_t m_consecutiveSinrBelowThreshold;
  long double m_outageThreshold;
  uint8_t m_n310;

};


}

}

#endif /* SRC_newradio_MODEL_newradio_UE_PHY_H_ */
