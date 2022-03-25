#ifndef SRC_newradio_MODEL_newradio_ENB_MAC_H_
#define SRC_newradio_MODEL_newradio_ENB_MAC_H_

#include "newradio-mac.h"
#include "newradio-enb-mac.h"
#include <ns3/lte-enb-cmac-sap.h>
#include <ns3/lte-mac-sap.h>
#include "newradio-phy-mac-common.h"
#include <ns3/lte-ccm-mac-sap.h>

namespace ns3 {

namespace newradio {

struct newradioDlHarqProcessInfo
{
  Ptr<PacketBurst> m_pktBurst;
  // maintain list of LCs contained in this TB
  // used to signal HARQ failure to RLC handlers
  std::vector<uint8_t> m_lcidList;
};

typedef std::vector <newradioDlHarqProcessInfo> newradioDlHarqProcessesBuffer_t;

class newradioEnbMac : public Object
{
  friend class newradioEnbMacMemberEnbCmacSapProvider;
  friend class newradioMacEnbMemberPhySapUser;

public:
  static TypeId GetTypeId (void);
  newradioEnbMac (void);
  virtual ~newradioEnbMac (void);
  virtual void DoDispose (void);

/*	struct SchedConfigIndParameters
        {
                uint32_t m_sfn;
                TddSlotTypeList m_tddPattern;
                SfAllocInfo m_allocationList;
        };*/

  struct TransmitPduParameters
  {
    Ptr<Packet> pdu;  /**< the RLC PDU */
    uint16_t    rnti; /**< the C-RNTI identifying the UE */
    uint8_t     lcid; /**< the logical channel id corresponding to the sending RLC instance */
    uint8_t     layer; /**< the layer value that was passed by the MAC in the call to NotifyTxOpportunity that generated this PDU */
    uint8_t     harqProcessId; /**< the HARQ process id that was passed by the MAC in the call to NotifyTxOpportunity that generated this PDU */
  };

  struct ReportBufferStatusParameters
  {
    uint16_t rnti;  /**< the C-RNTI identifying the UE */
    uint8_t lcid;  /**< the logical channel id corresponding to the sending RLC instance */
    uint32_t txQueueSize;  /**< the current size of the RLC transmission queue */
    uint16_t txQueueHolDelay;  /**< the Head Of Line delay of the transmission queue */
    uint32_t retxQueueSize;  /**<  the current size of the RLC retransmission queue in bytes */
    uint16_t retxQueueHolDelay;  /**<  the Head Of Line delay of the retransmission queue */
    uint16_t statusPduSize;  /**< the current size of the pending STATUS RLC  PDU message in bytes */
  };

/*
  struct RachConfig
  {
        uint8_t numberOfRaPreambles;
        uint8_t preambleTransMax;
        uint8_t raResponseWindowSize;
  };

  struct AllocateNcRaPreambleReturnValue
  {
        bool valid; ///< true if a valid RA config was allocated, false otherwise
        uint8_t raPreambleId; ///< random access preamble id
        uint8_t raPrachMaskIndex; /// PRACH mask index
  };
*/
  void SetComponentCarrierId (uint8_t index);
  void SetConfigurationParameters (Ptr<newradioPhyMacCommon> ptrConfig);
  Ptr<newradioPhyMacCommon> GetConfigurationParameters (void) const;

  void SetCellId (uint16_t cellId);

  // forwarded from LteMacSapProvider
  void DoTransmitPdu (LteMacSapProvider::TransmitPduParameters);
  void DoReportBufferStatus (LteMacSapProvider::ReportBufferStatusParameters);
  void DoUlCqiReport (newradioMacSchedSapProvider::SchedUlCqiInfoReqParameters ulcqi);

  void DoSubframeIndication (SfnSf sfnSf);

  void SetMcs (int mcs);

  void AssociateUeMAC (uint64_t imsi);

  void SetForwardUpCallback (Callback <void, Ptr<Packet> > cb);

//	void PhyPacketRx (Ptr<Packet> p);

  void DoReceivePhyPdu (Ptr<Packet> p);

  void DoReceiveControlMessage  (Ptr<newradioControlMessage> msg);

  void DoSchedConfigIndication (newradioMacSchedSapUser::SchedConfigIndParameters ind);

  newradioEnbPhySapUser* GetPhySapUser ();
  void SetPhySapProvider (newradioPhySapProvider* ptr);

  newradioMacSchedSapUser* GetnewradioMacSchedSapUser ();
  void SetnewradioMacSchedSapProvider (newradioMacSchedSapProvider* ptr);

  newradioMacCschedSapUser* GetnewradioMacCschedSapUser ();
  void SetnewradioMacCschedSapProvider (newradioMacCschedSapProvider* ptr);

  LteMacSapProvider* GetUeMacSapProvider (void);
  LteEnbCmacSapProvider* GetEnbCmacSapProvider (void);

  void SetEnbCmacSapUser (LteEnbCmacSapUser* s);
  void ReceiveRachPreamble (uint32_t raId);

  // forwarded from FfMacCchedSapUser
  void DoCschedCellConfigCnf (newradioMacCschedSapUser::CschedCellConfigCnfParameters params);
  void DoCschedUeConfigCnf (newradioMacCschedSapUser::CschedUeConfigCnfParameters params);
  void DoCschedLcConfigCnf (newradioMacCschedSapUser::CschedLcConfigCnfParameters params);
  void DoCschedLcReleaseCnf (newradioMacCschedSapUser::CschedLcReleaseCnfParameters params);
  void DoCschedUeReleaseCnf (newradioMacCschedSapUser::CschedUeReleaseCnfParameters params);
  void DoCschedUeConfigUpdateInd (newradioMacCschedSapUser::CschedUeConfigUpdateIndParameters params);
  void DoCschedCellConfigUpdateInd (newradioMacCschedSapUser::CschedCellConfigUpdateIndParameters params);

  void SetLteCcmMacSapUser (LteCcmMacSapUser* s);
  LteCcmMacSapProvider* GetLteCcmMacSapProvider ();

  //forwarded from LteCcmMacSapProvider
  void DoReportMacCeToScheduler (MacCeListElement_s bsr);

  /**
   * TracedCallback signature for
   *
   * \param [in] rnti C-RNTI scheduled.
   * \param [in] the cellId
   * \param [in] the allocated transport block size
   * \param [in] the number of HARQ retransmissions for that packet
   */
  typedef void (*HarqRetxCallback)
    (uint16_t rnti, uint16_t cellId, uint32_t tbSize, uint8_t numRetx);

private:
  // forwarded from LteEnbCmacSapProvider
  void DoConfigureMac (uint8_t ulBandwidth, uint8_t dlBandwidth);
  void DoAddUe (uint16_t rnti);
  void DoRemoveUe (uint16_t rnti);
  void DoAddLc (LteEnbCmacSapProvider::LcInfo lcinfo, LteMacSapUser* msu);
  void DoReconfigureLc (LteEnbCmacSapProvider::LcInfo lcinfo);
  void DoReleaseLc (uint16_t  rnti, uint8_t lcid);
  void UeUpdateConfigurationReq (LteEnbCmacSapProvider::UeConfig params);
  LteEnbCmacSapProvider::RachConfig DoGetRachConfig ();
  LteEnbCmacSapProvider::AllocateNcRaPreambleReturnValue DoAllocateNcRaPreamble (uint16_t rnti);
  uint8_t AllocateTbUid ();

  void DoDlHarqFeedback (DlHarqInfo params);
  void DoUlHarqFeedback (UlHarqInfo params);

  Ptr<newradioPhyMacCommon> m_phyMacConfig;

  LteMacSapProvider* m_macSapProvider;
  LteEnbCmacSapProvider* m_cmacSapProvider;
  LteEnbCmacSapUser* m_cmacSapUser;

  // Sap For ComponentCarrierManager 'Uplink case'
  LteCcmMacSapProvider* m_ccmMacSapProvider; ///< CCM MAC SAP provider
  LteCcmMacSapUser* m_ccmMacSapUser; ///< CCM MAC SAP user

  /// component carrier Id used to address sap
  uint8_t m_componentCarrierId;

  uint32_t m_frameNum;
  uint32_t m_sfNum;
  uint32_t m_slotNum;

  uint8_t m_tbUid;
  std::map<uint32_t, struct MacPduInfo> m_macPduMap;

  std::list <uint16_t> m_associatedUe;

  Callback <void, Ptr<Packet> > m_forwardUpCallback;

  std::vector <DlCqiInfo> m_dlCqiReceived;
  std::vector <newradioMacSchedSapProvider::SchedUlCqiInfoReqParameters> m_ulCqiReceived;
  std::vector <MacCeElement> m_ulCeReceived;       // CE received (BSR up to now)

  newradioPhySapProvider* m_phySapProvider;
  newradioEnbPhySapUser* m_phySapUser;

  newradioMacSchedSapProvider* m_macSchedSapProvider;
  newradioMacSchedSapUser* m_macSchedSapUser;
  newradioMacCschedSapProvider* m_macCschedSapProvider;
  newradioMacCschedSapUser* m_macCschedSapUser;

  std::map<uint8_t, uint32_t> m_receivedRachPreambleCount;

  std::map <uint16_t, std::map<uint8_t, LteMacSapUser*> > m_rlcAttached;

  std::vector <DlHarqInfo> m_dlHarqInfoReceived;       // DL HARQ feedback received
  std::vector <UlHarqInfo> m_ulHarqInfoReceived;       // UL HARQ feedback received
  std::map <uint16_t, newradioDlHarqProcessesBuffer_t> m_miDlHarqProcessesPackets;       // Packet under trasmission of the DL HARQ process

  /**
  * info associated with a preamble allocated for non-contention based RA
  *
  */
  struct NcRaPreambleInfo
  {
    uint16_t rnti;             ///< rnti previously allocated for this non-contention based RA procedure
    Time expiryTime;             ///< value the expiration time of this allocation (so that stale preambles can be reused)
  };

  uint8_t m_numberOfRaPreambles;
  uint8_t m_preambleTransMax;
  uint8_t m_raResponseWindowSize;

  /**
  * map storing as key the random acccess preamble IDs allocated for
  * non-contention based access, and as value the associated info
  *
  */
  std::map<uint8_t, NcRaPreambleInfo> m_allocatedNcRaPreambleMap;

  uint16_t m_cellId;

  TracedCallback<uint16_t, uint16_t, uint32_t, uint8_t> m_macDlTxSizeRetx;

  TracedCallback<uint16_t, uint8_t, uint32_t> m_txMacPacketTraceEnb;

};

} // namespace newradio

} // namespace ns3



#endif /* SRC_newradio_MODEL_newradio_ENB_MAC_H_ */
