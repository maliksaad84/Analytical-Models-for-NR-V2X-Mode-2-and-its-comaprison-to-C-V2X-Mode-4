#ifndef SRC_newradio_MODEL_newradio_UE_MAC_H_
#define SRC_newradio_MODEL_newradio_UE_MAC_H_

#include "newradio-mac.h"
#include <ns3/lte-ue-cmac-sap.h>
#include <ns3/lte-mac-sap.h>
#include <ns3/lte-radio-bearer-tag.h>


namespace ns3 {

namespace newradio {

class newradioControlMessage;

class newradioUeMac : public Object
{
  friend class UeMembernewradioUeCmacSapProvider;
  friend class UeMembernewradioMacSapProvider;
  friend class MacUeMemberPhySapUser;

public:
  static TypeId GetTypeId (void);

  newradioUeMac (void);
  ~newradioUeMac (void);
  virtual void DoDispose (void);

  void  SetUeCmacSapUser (LteUeCmacSapUser* s);
  LteUeCmacSapProvider*  GetUeCmacSapProvider (void);
  LteMacSapProvider*  GetUeMacSapProvider (void);

  void SetConfigurationParameters (Ptr<newradioPhyMacCommon> ptrConfig);
  Ptr<newradioPhyMacCommon> GetConfigurationParameters (void) const;

  void DoSubframeIndication (SfnSf sfn);

  newradioUePhySapUser* GetPhySapUser ();
  void SetPhySapProvider (newradioPhySapProvider* ptr);

  void RecvRaResponse (BuildRarListElement_s raResponse);

  void SetComponentCarrierId (uint8_t index);

  /**
  * Assign a fixed random variable stream number to the random variables
  * used by this model.  Return the number of streams (possibly zero) that
  * have been assigned.
  *
  * \param stream first stream index to use
  * \return the number of stream indices assigned by this model
  */
  int64_t AssignStreams (int64_t stream);

private:
  void DoTransmitPdu (LteMacSapProvider::TransmitPduParameters params);
  void DoReportBufferStatus (LteMacSapProvider::ReportBufferStatusParameters params);

  // forwarded from PHY SAP
  void DoReceivePhyPdu (Ptr<Packet> p);
  void DoReceiveControlMessage  (Ptr<newradioControlMessage> msg);
  //void DoNotifyHarqDeliveryFailure (uint8_t harqId);

  // forwarded from UE CMAC SAP
  void DoConfigureRach (LteUeCmacSapProvider::RachConfig rc);
  void DoStartContentionBasedRandomAccessProcedure ();
  void DoSetRnti (uint16_t rnti);
  void DoStartNonContentionBasedRandomAccessProcedure (uint16_t rnti, uint8_t rapId, uint8_t prachMask);
  void AddLc (uint8_t lcId, LteUeCmacSapProvider::LogicalChannelConfig lcConfig, LteMacSapUser* msu);
  void DoRemoveLc (uint8_t lcId);
  void DoReset ();

  void RandomlySelectAndSendRaPreamble ();
  void SendRaPreamble (bool contention);
  void SendReportBufferStatus (void);
  void RefreshHarqProcessesPacketBuffer (void);

  std::map<uint32_t, struct MacPduInfo>::iterator AddToMacPduMap (DciInfoElementTdma dci, unsigned activeLcs);

  /// component carrier Id --> used to address sap
  uint8_t m_componentCarrierId;

  Ptr<newradioPhyMacCommon> m_phyMacConfig;

  LteUeCmacSapUser* m_cmacSapUser;
  LteUeCmacSapProvider* m_cmacSapProvider;

  TddSlotTypeList m_DataTxTDDMap;
  SfAllocInfo m_DataTxAllocationList;

  newradioPhySapProvider* m_phySapProvider;
  newradioUePhySapUser* m_phySapUser;
  LteMacSapProvider* m_macSapProvider;

  uint32_t m_frameNum;
  uint32_t m_sfNum;
  uint32_t m_slotNum;

  //uint8_t	m_tbUid;
  std::map<uint32_t, struct MacPduInfo> m_macPduMap;

  std::map <uint8_t, LteMacSapProvider::ReportBufferStatusParameters> m_ulBsrReceived;       // BSR received from RLC (the last one)
  Time m_bsrPeriodicity;
  Time m_bsrLast;
  bool m_freshUlBsr;       // true when a BSR has been received in the last TTI


  Ptr<UniformRandomVariable> m_raPreambleUniformVariable;
  uint8_t m_raPreambleId;
  uint8_t m_raRnti;

  struct UlHarqProcessInfo
  {
    Ptr<PacketBurst> m_pktBurst;
    // maintain list of LCs contained in this TB
    // used to signal HARQ failure to RLC handlers
    std::vector<uint8_t> m_lcidList;
  };

  //uint8_t m_harqProcessId;
  std::vector < UlHarqProcessInfo > m_miUlHarqProcessesPacket; // Packets under trasmission of the UL HARQ processes
  std::vector < uint8_t > m_miUlHarqProcessesPacketTimer; // timer for packet life in the buffer

  struct LcInfo
  {
    LteUeCmacSapProvider::LogicalChannelConfig lcConfig;
    LteMacSapUser* macSapUser;
  };

  std::map <uint8_t, LcInfo> m_lcInfoMap;
  uint16_t m_rnti;

  bool m_waitingForRaResponse;
  static uint8_t g_raPreambleId;
  Ptr<UniformRandomVariable> m_randomAccessProcedureDelay;
  double m_ueUpdateSinrPeriod;
  bool m_interRatHoCapable;

  TracedCallback<uint16_t, uint8_t, uint32_t> m_txMacPacketTraceUe;

};

}

}


#endif /* SRC_newradio_MODEL_newradio_UE_MAC_H_ */
