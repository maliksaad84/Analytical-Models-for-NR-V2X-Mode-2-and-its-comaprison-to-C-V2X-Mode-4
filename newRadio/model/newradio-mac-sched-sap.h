#ifndef SRC_newradio_MODEL_newradio_MAC_SCHED_SAP_H_
#define SRC_newradio_MODEL_newradio_MAC_SCHED_SAP_H_

#include "newradio-phy-mac-common.h"
#include <set>
#include <map>

namespace ns3 {

namespace newradio {

class newradioMacSchedSapProvider
{
public:
  virtual ~newradioMacSchedSapProvider ();

  struct SchedDlRlcBufferReqParameters
  {
    uint16_t  m_rnti;
    uint8_t   m_logicalChannelIdentity;
    uint32_t  m_rlcTransmissionQueueSize;
    uint16_t  m_rlcTransmissionQueueHolDelay;
    uint32_t  m_rlcRetransmissionQueueSize;
    uint16_t  m_rlcRetransmissionHolDelay;
    uint16_t  m_rlcStatusPduSize;

    std::list<uint32_t>     m_txPacketSizes;
    std::list<uint32_t>     m_retxPacketSizes;
    std::list<double>       m_txPacketDelays;
    std::list<double>       m_retxPacketDelays;
    double m_arrivalRate;
  };

  struct SchedDlCqiInfoReqParameters
  {
    SfnSf m_sfnsf;
    std::vector <struct DlCqiInfo> m_cqiList;
  };

  struct SchedUlMacCtrlInfoReqParameters
  {
    SfnSf  m_sfnSf;
    std::vector <struct MacCeElement> m_macCeList;
  };

  struct SchedUlCqiInfoReqParameters
  {
    SfnSf  m_sfnSf;
    struct UlCqiInfo m_ulCqi;
  };

  struct SchedTriggerReqParameters
  {
    SfnSf m_snfSf;
    std::vector <struct DlHarqInfo> m_dlHarqInfoList;
    std::vector <struct UlHarqInfo> m_ulHarqInfoList;
    std::list <uint16_t> m_ueList;
  };

  virtual void SchedDlRlcBufferReq (const struct SchedDlRlcBufferReqParameters& params) = 0;

  virtual void SchedDlCqiInfoReq (const SchedDlCqiInfoReqParameters& params) = 0;

  virtual void SchedUlCqiInfoReq (const struct SchedUlCqiInfoReqParameters& params) = 0;

  virtual void SchedTriggerReq (const struct SchedTriggerReqParameters& params) = 0;

  virtual void SchedUlMacCtrlInfoReq (const struct SchedUlMacCtrlInfoReqParameters& params) = 0;

  virtual void SchedSetMcs (int mcs)
  {
  }

private:
};

class newradioMacSchedSapUser
{
public:
  virtual ~newradioMacSchedSapUser ();

  struct SchedConfigIndParameters
  {
    SfnSf m_sfnSf;
//		TddSlotTypeList m_tddPattern;
    SfAllocInfo m_sfAllocInfo;
    SfAllocInfo m_dlSfAllocInfo;
    SfAllocInfo m_ulSfAllocInfo;

    std::map<uint16_t, SchedInfo> m_schedInfoMap;
  };

  virtual void SchedConfigInd (const struct SchedConfigIndParameters& params) = 0;
private:
};

}

}

#endif /* SRC_newradio_MODEL_newradio_MAC_SCHED_SAP_H_ */
