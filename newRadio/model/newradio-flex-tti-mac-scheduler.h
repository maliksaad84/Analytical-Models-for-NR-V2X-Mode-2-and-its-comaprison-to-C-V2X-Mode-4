#ifndef SRC_newradio_MODEL_newradio_RR_MAC_SCHEDULER_H_
#define SRC_newradio_MODEL_newradio_RR_MAC_SCHEDULER_H_


#include "newradio-mac-sched-sap.h"
#include "newradio-mac-csched-sap.h"
#include "newradio-mac-scheduler.h"
#include "newradio-amc.h"
#include "string"
#include <vector>
#include <set>

namespace ns3 {

namespace newradio {

class newradioFlexTtiMacScheduler : public newradioMacScheduler
{
public:
  typedef std::vector < uint8_t > DlHarqProcessesStatus_t;
  typedef std::vector < uint8_t > DlHarqProcessesTimer_t;
  typedef std::vector < DciInfoElementTdma > DlHarqProcessesDciInfoList_t;
  typedef std::vector < std::vector <struct RlcPduInfo> > DlHarqRlcPduList_t;       // vector of the LCs per per UE HARQ process
  //	typedef std::vector < RlcPduElement > DlHarqRlcPduList_t; // vector of the 8 HARQ processes per UE

  typedef std::vector < uint8_t > UlHarqProcessesTimer_t;
  typedef std::vector < DciInfoElementTdma > UlHarqProcessesDciInfoList_t;
  typedef std::vector < uint8_t > UlHarqProcessesStatus_t;


  newradioFlexTtiMacScheduler ();

  virtual ~newradioFlexTtiMacScheduler ();
  virtual void DoDispose (void);
  static TypeId GetTypeId (void);

  virtual void SetMacSchedSapUser (newradioMacSchedSapUser* sap);
  virtual void SetMacCschedSapUser (newradioMacCschedSapUser* s);

  virtual newradioMacSchedSapProvider* GetMacSchedSapProvider ();
  virtual newradioMacCschedSapProvider* GetMacCschedSapProvider ();

  virtual void ConfigureCommonParameters (Ptr<newradioPhyMacCommon> config);

  static bool SortRlcBufferReq (newradioMacSchedSapProvider::SchedDlRlcBufferReqParameters i, newradioMacSchedSapProvider::SchedDlRlcBufferReqParameters j);

  void RefreshDlCqiMaps (void);
  void RefreshUlCqiMaps (void);

  void UpdateDlRlcBufferInfo (uint16_t rnti, uint8_t lcid, uint16_t size);
  void UpdateUlRlcBufferInfo (uint16_t rnti, uint16_t size);

  friend class newradioFlexTtiMacSchedSapProvider;
  friend class newradioFlexTtiMacCschedSapProvider;

private:
  struct UeSchedInfo
  {
    UeSchedInfo ()
      : m_dlMcs (0),
        m_ulMcs (0),
        m_maxDlBufSize (0),
        m_maxUlBufSize (0),
        m_maxDlSymbols (0),
        m_maxUlSymbols (0),
        m_dlSymbols (0),
        m_ulSymbols (0),
        m_dlSymbolsRetx (0),
        m_ulSymbolsRetx (0),
        m_dlTbSize (0),
        m_ulTbSize (0),
        m_dlAllocDone (false),
        m_ulAllocDone (false)
    {
    }

    uint8_t         m_dlMcs;             // DL MCS
    uint8_t         m_ulMcs;             // UL MCS
    uint32_t        m_maxDlBufSize;             // DL TB size needed to encode the DL buffer (this parameter is also used to temporarily store the DL buffer size)
    uint32_t        m_maxUlBufSize;             // UL TB size needed to encode the UL buffer (this parameter is also used to temporarily store the UL buffer size)
    uint8_t         m_maxDlSymbols;             // Number of symbols needed to encode the DL buffer given the DL MCS
    uint8_t         m_maxUlSymbols;             // Number of symbols needed to encode the UL buffer given the UL MCS
    uint8_t         m_dlSymbols;
    uint8_t         m_ulSymbols;
    uint8_t         m_dlSymbolsRetx;
    uint8_t         m_ulSymbolsRetx;
    uint32_t        m_dlTbSize;
    uint32_t        m_ulTbSize;
    std::vector <struct RlcPduInfo> m_rlcPduInfo;
    bool                    m_dlAllocDone;
    bool                    m_ulAllocDone;
  };

  unsigned CalcMinTbSizeNumSym (unsigned mcs, unsigned bufSize, unsigned &tbSize);

  uint32_t
  BsrId2BufferSize (uint8_t val)
  {
    NS_ABORT_MSG_UNLESS (val < 64, "val = " << val << " is out of range");
    return BufferSizeLevelBsrTable[val];
  }

  uint8_t
  BufferSize2BsrId (uint32_t val)
  {
    int index = 0;
    if (BufferSizeLevelBsrTable[63] < val)
      {
        index = 63;
      }
    else
      {
        while (BufferSizeLevelBsrTable[index] < val)
          {
            NS_ASSERT (index < 64);
            index++;
          }
      }

    return (index);
  }

  uint8_t UpdateDlHarqProcessId (uint16_t rnti);
  uint8_t UpdateUlHarqProcessId (uint16_t rnti);

  //
  // Implementation of the CSCHED API primitives
  // (See 4.1 for description of the primitives)
  //

  void DoCschedCellConfigReq (const struct newradioMacCschedSapProvider::CschedCellConfigReqParameters& params);

  void DoCschedUeConfigReq (const struct newradioMacCschedSapProvider::CschedUeConfigReqParameters& params);

  void DoCschedLcConfigReq (const struct newradioMacCschedSapProvider::CschedLcConfigReqParameters& params);

  void DoCschedLcReleaseReq (const struct newradioMacCschedSapProvider::CschedLcReleaseReqParameters& params);

  void DoCschedUeReleaseReq (const struct newradioMacCschedSapProvider::CschedUeReleaseReqParameters& params);

  //
  // Implementation of the SCHED API primitives
  // (See 4.2 for description of the primitives)
  //

  void DoSchedDlRlcBufferReq (const struct newradioMacSchedSapProvider::SchedDlRlcBufferReqParameters& params);

  void DoSchedDlCqiInfoReq (const newradioMacSchedSapProvider::SchedDlCqiInfoReqParameters& params);       // Put in UML

  void DoSchedUlCqiInfoReq (const struct newradioMacSchedSapProvider::SchedUlCqiInfoReqParameters& params);

  void DoSchedUlMacCtrlInfoReq (const struct newradioMacSchedSapProvider::SchedUlMacCtrlInfoReqParameters& params);

  void DoSchedTriggerReq (const struct newradioMacSchedSapProvider::SchedTriggerReqParameters& params);

  bool DoSchedDlTriggerReq (const struct newradioMacSchedSapProvider::SchedTriggerReqParameters& params,
                            newradioMacSchedSapUser::SchedConfigIndParameters& ret,
                            unsigned int frameNum,
                            unsigned int sfNum,
                            unsigned int islot);

  bool DoSchedUlTriggerReq (const struct newradioMacSchedSapProvider::SchedTriggerReqParameters& params,
                            newradioMacSchedSapUser::SchedConfigIndParameters& ret,
                            unsigned int frameNum,
                            unsigned int sfNum,
                            unsigned int islot);

  void DoSchedSetMcs (int mcs);

  /**
   * \brief Refresh HARQ processes according to the timers
   *
   */
  void RefreshHarqProcesses ();

  TddSlotTypeList m_tddMap;

  Ptr<newradioAmc> m_amc;

  /*
   * Vectors of UE's RLC info
   */
  std::list <newradioMacSchedSapProvider::SchedDlRlcBufferReqParameters> m_rlcBufferReq;

  /*
   * Map of UE's DL CQI WB received
   */
  std::map <uint16_t,uint8_t> m_wbCqiRxed;
  /*
   * Map of UE's timers on DL CQI WB received
   */
  std::map <uint16_t,uint32_t> m_wbCqiTimers;

  uint32_t m_cqiTimersThreshold;       // # of TTIs for which a CQI can be considered valid

  /*
   * Map of UEs' UL-CQI per RBG
   */
  struct UlCqiMapElem
  {
    UlCqiMapElem (std::vector<double> ulCqi, uint8_t nSym, uint32_t tbs)
      : m_ueUlCqi (ulCqi),
        m_numSym (nSym),
        m_tbSize (tbs)
    {
    }
    std::vector <double> m_ueUlCqi;
    uint8_t         m_numSym;
    uint32_t        m_tbSize;
  };

  std::map <uint16_t, struct UlCqiMapElem> m_ueUlCqi;
  /*
   * Map of UEs' timers on UL-CQI per RBG
   */
  std::map <uint16_t, uint32_t> m_ueCqiTimers;

  /*
   * Map of UE's buffer status reports received
   */
  std::map <uint16_t,uint32_t> m_ceBsrRxed;

  uint16_t m_nextRnti;
  uint64_t m_nextRntiDl;
  uint64_t m_nextRntiUl;

  uint32_t m_subframeNo;
  uint32_t m_frameNo;
  uint8_t m_tbUid;
  uint32_t m_numChunks;
  uint32_t m_numDataSymbols;

  newradioMacSchedSapProvider* m_macSchedSapProvider;
  newradioMacSchedSapUser* m_macSchedSapUser;
  newradioMacCschedSapUser* m_macCschedSapUser;
  newradioMacCschedSapProvider* m_macCschedSapProvider;

  newradioMacCschedSapProvider::CschedCellConfigReqParameters m_cschedCellConfig;

  struct AllocMapElem
  {
    AllocMapElem (std::vector<uint16_t> rntiMap, uint8_t nSym, uint32_t tbs)
      : m_rntiPerChunk (rntiMap),
        m_numSym (nSym),
        m_tbSize (tbs)
    {
    }

    std::vector <uint16_t> m_rntiPerChunk;
    uint8_t m_numSym;
    uint32_t m_tbSize;
  };
  /*
   * Map of previous allocated UE per RBG
   * (used to retrieve info from UL-CQI)
   */
  std::map <uint32_t, struct AllocMapElem> m_ulAllocationMap;

  // HARQ attributes
  /**
   * m_harqOn when false inhibit te HARQ mechanisms (by default active)
   */
  bool m_harqOn;
  uint8_t m_numHarqProcess;
  uint8_t m_harqTimeout;

  std::map <uint16_t, uint8_t> m_dlHarqCurrentProcessId;
  //HARQ status
  // 0: process Id available
  // x>0: process Id equal to `x` trasmission count
  std::map <uint16_t, DlHarqProcessesStatus_t> m_dlHarqProcessesStatus;
  std::map <uint16_t, DlHarqProcessesTimer_t> m_dlHarqProcessesTimer;
  std::map <uint16_t, DlHarqProcessesDciInfoList_t> m_dlHarqProcessesDciInfoMap;
  std::map <uint16_t, DlHarqRlcPduList_t> m_dlHarqProcessesRlcPduMap;
  std::vector <DlHarqInfo> m_dlHarqInfoList;       // HARQ retx buffered
  std::vector <UlHarqInfo> m_ulHarqInfoList;       // HARQ retx buffered

  std::map <uint16_t, uint8_t> m_ulHarqCurrentProcessId;
  //HARQ status
  // 0: process Id available
  // x>0: process Id equal to `x` trasmission count
  std::map <uint16_t, UlHarqProcessesStatus_t>    m_ulHarqProcessesStatus;
  std::map <uint16_t, UlHarqProcessesTimer_t>     m_ulHarqProcessesTimer;
  std::map <uint16_t, UlHarqProcessesDciInfoList_t> m_ulHarqProcessesDciInfoMap;

  // needed to keep track of uplink allocations in later slots
  std::list <struct SfAllocInfo> m_ulSfAllocInfo;

  static const unsigned m_macHdrSize;
  static const unsigned m_subHdrSize;
  static const unsigned m_rlcHdrSize;

  static const double m_berDl;
  bool            m_fixedMcsDl;
  bool            m_fixedMcsUl;
  uint8_t m_mcsDefaultDl;
  uint8_t m_mcsDefaultUl;

  bool m_dlOnly;
  bool m_ulOnly;       //for testing

  bool m_fixedTti;                      // one slot per TTI
  uint8_t m_symPerSlot;       // symbols per slot
};

} // namespace newradio

} // namespace ns3


#endif /* SRC_newradio_MODEL_newradio_RR_MAC_SCHEDULER_H_ */
