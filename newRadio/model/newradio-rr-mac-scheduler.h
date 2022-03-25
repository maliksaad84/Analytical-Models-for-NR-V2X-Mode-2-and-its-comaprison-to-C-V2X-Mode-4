#ifndef SRC_newradio_MODEL_newradio_RR_MAC_SCHEDULER_H_
#define SRC_newradio_MODEL_newradio_RR_MAC_SCHEDULER_H_


#include "newradio-mac-sched-sap.h"
#include "newradio-mac-scheduler.h"
#include "newradio-amc.h"
#include "string"
#include <vector>
#include <set>

namespace ns3 {

namespace newradio {

class newradioRrMacScheduler : public newradioMacScheduler
{
public:
  typedef std::vector < uint8_t > DlHarqProcessesStatus_t;
  typedef std::vector < uint8_t > DlHarqProcessesTimer_t;
  typedef std::vector < TbInfoElement > DlHarqProcessesTbInfoList_t;
  typedef std::vector < std::vector <struct RlcPduInfo> > DlHarqRlcPduList_t;       // vector of the LCs per per UE HARQ process
//	typedef std::vector < RlcPduElement > DlHarqRlcPduList_t; // vector of the 8 HARQ processes per UE

  typedef std::vector < TbInfoElement > UlHarqProcessesTbInfoList_t;
  typedef std::vector < uint8_t > UlHarqProcessesStatus_t;


  newradioRrMacScheduler ();

  virtual ~newradioRrMacScheduler ();
  virtual void DoDispose (void);
  static TypeId GetTypeId (void);

  virtual void SetMacSchedSapUser (newradioMacSchedSapUser* sap);

  virtual newradioMacSchedSapProvider* GetMacSchedSapProvider ();

  virtual void ConfigureCommonParameters (Ptr<newradioPhyMacCommon> config);

  void DoSchedDlRlcBufferReq (const struct newradioMacSchedSapProvider::SchedDlRlcBufferReqParameters& params);

  void DoSchedDlCqiInfoReq (const newradioMacSchedSapProvider::SchedDlCqiInfoReqParameters& params);       // Put in UML

  void DoSchedUlCqiInfoReq (const struct newradioMacSchedSapProvider::SchedUlCqiInfoReqParameters& params);

  void DoSchedUlMacCtrlInfoReq (const struct newradioMacSchedSapProvider::SchedUlMacCtrlInfoReqParameters& params);

  static bool SortRlcBufferReq (newradioMacSchedSapProvider::SchedDlRlcBufferReqParameters i, newradioMacSchedSapProvider::SchedDlRlcBufferReqParameters j);

  void RefreshDlCqiMaps (void);
  void RefreshUlCqiMaps (void);

  void UpdateDlRlcBufferInfo (uint16_t rnti, uint8_t lcid, uint16_t size);
  void UpdateUlRlcBufferInfo (uint16_t rnti, uint16_t size);

  friend class newradioRrMacSchedSapProvider;

private:
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


  /**
  * \brief Refresh HARQ processes according to the timers
  *
  */
  void RefreshHarqProcesses ();

  void SetTBSizeAssigned ();
  SfAllocInfo ScheduleUsersInTime (uint32_t slotNum);
  void SetScheduleDirection  (std::string patt);

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

  uint32_t m_cqiTimersThreshold; // # of TTIs for which a CQI can be considered valid

  /*
  * Map of UEs' UL-CQI per RBG
  */
  std::map <uint16_t, std::vector <double> > m_ueUlCqi;
  /*
  * Map of UEs' timers on UL-CQI per RBG
  */
  std::map <uint16_t, uint32_t> m_ueCqiTimers;

  /*
  * Map of UE's buffer status reports received
  */
  std::map <uint16_t,uint32_t> m_ceBsrRxed;

  std::string m_directions;       //UL or DL Need to to this in a better way
  bool m_isDirnUpdated;

  uint64_t m_nextRntiDl;
  uint64_t m_nextRntiUl;

  uint32_t m_tddBitmap;

  uint32_t m_subframeNo;
  uint32_t m_frameNo;

  uint8_t m_tbUid;

  newradioMacSchedSapProvider* m_macSchedSapProvider;
  newradioMacSchedSapUser* m_macSchedSapUser;

  uint32_t m_numChunks;

  /*
  * Map of previous allocated UE per RBG
  * (used to retrieve info from UL-CQI)
  */
  std::map <uint32_t, std::vector <uint16_t> > m_allocationMaps;

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
  // x>0: process Id equal to `x` transmission count
  std::map <uint16_t, DlHarqProcessesStatus_t> m_dlHarqProcessesStatus;
  std::map <uint16_t, DlHarqProcessesTimer_t> m_dlHarqProcessesTimer;
  std::map <uint16_t, DlHarqProcessesTbInfoList_t> m_dlHarqProcessesTbInfoMap;
  std::map <uint16_t, DlHarqRlcPduList_t> m_dlHarqProcessesRlcPduMap;
  std::vector <DlHarqInfo> m_dlHarqInfoList; // HARQ retx buffered

  std::map <uint16_t, uint8_t> m_ulHarqCurrentProcessId;
  //HARQ status
  // 0: process Id available
  // x>0: process Id equal to `x` trasmission count
  std::map <uint16_t, UlHarqProcessesStatus_t> m_ulHarqProcessesStatus;
  std::map <uint16_t, UlHarqProcessesTbInfoList_t> m_ulHarqProcessesTbInfoMap;

  std::list <SfAllocInfo> m_ulSfAllocInfo;       // queue for storing uplink allocations in later scheduling intervals
};

}

}


#endif /* SRC_newradio_MODEL_newradio_RR_MAC_SCHEDULER_H_ */
