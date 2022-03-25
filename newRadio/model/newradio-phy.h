#ifndef SRC_newradio_MODEL_newradio_PHY_H_
#define SRC_newradio_MODEL_newradio_PHY_H_

#include <ns3/spectrum-value.h>
#include <ns3/mobility-model.h>
#include <ns3/packet.h>
#include <ns3/nstime.h>
#include <ns3/spectrum-phy.h>
#include <ns3/spectrum-channel.h>
#include <ns3/spectrum-signal-parameters.h>
#include <ns3/spectrum-interference.h>
#include <ns3/propagation-loss-model.h>
#include <ns3/generic-phy.h>
#include <ns3/antenna-array-model.h>
#include "newradio-phy-mac-common.h"
#include "newradio-spectrum-phy.h"
#include "newradio-net-device.h"
#include "newradio-phy-sap.h"
#include <string>
#include <map>
#include <ns3/newradio-los-tracker.h>


namespace ns3 {

namespace newradio {

class newradioNetDevice;
class newradioControlMessage;

class newradioPhy : public Object
{
public:
  newradioPhy ();

  newradioPhy (Ptr<newradioSpectrumPhy> dlChannelPhy, Ptr<newradioSpectrumPhy> ulChannelPhy);

  virtual ~newradioPhy ();

  static TypeId GetTypeId (void);

  void SetDevice (Ptr<NetDevice> d);

  Ptr<NetDevice> GetDevice ();

  void SetChannel (Ptr<SpectrumChannel> c);

  /**
   * \brief Compute the TX Power Spectral Density
   * \return a pointer to a newly allocated SpectrumValue representing the TX Power Spectral Density in W/Hz for each Resource Block
   */
  virtual Ptr<SpectrumValue> CreateTxPowerSpectralDensity () = 0;

  void DoDispose ();

  virtual void DoInitialize (void);

  /**
   * \returns transmission time interval
   */
  double GetTti (void) const;

  void DoSetCellId (uint16_t cellId);


  void SetNoiseFigure (double nf);
  double GetNoiseFigure (void) const;

  void SetControlMessage (Ptr<newradioControlMessage> m);
  std::list<Ptr<newradioControlMessage> > GetControlMessages (void);

  virtual void SetMacPdu (Ptr<Packet> pb);

  virtual void SendRachPreamble (uint32_t PreambleId, uint32_t Rnti);


//	virtual Ptr<PacketBurst> GetPacketBurst (void);
  virtual Ptr<PacketBurst> GetPacketBurst (SfnSf);

  void SetConfigurationParameters (Ptr<newradioPhyMacCommon> ptrConfig);
  Ptr<newradioPhyMacCommon> GetConfigurationParameters (void) const;

  newradioPhySapProvider* GetPhySapProvider ();
//	void SetPhySapUser (newradioPhySapUser* ptr);

  void UpdateCurrentAllocationAndSchedule (uint32_t frame, uint32_t sf);

  SfAllocInfo GetSfAllocInfo (uint8_t subframeNum);
  void SetDlSfAllocInfo (SfAllocInfo sfAllocInfo);
  void SetUlSfAllocInfo (SfAllocInfo sfAllocInfo);

  // hacks needed to compute SINR at eNB for each UE, without pilots
  void AddSpectrumPropagationLossModel (Ptr<SpectrumPropagationLossModel> model);
  void AddPropagationLossModel (Ptr<PropagationLossModel> model);
  void AddLosTracker (Ptr<newradioLosTracker>);

  /**
* Set the component carrier ID
*
* \param index the component carrier ID index
*/
  void SetComponentCarrierId (uint8_t index);

  /**
  * Get the component carrier ID
  *
  * \returns the component carrier ID index
  */
  uint8_t GetComponentCarrierId ();

protected:
  Ptr<NetDevice> m_netDevice;

  Ptr<newradioSpectrumPhy> m_spectrumPhy;
  Ptr<newradioSpectrumPhy> m_downlinkSpectrumPhy;
  Ptr<newradioSpectrumPhy> m_uplinkSpectrumPhy;

  double m_txPower;
  double m_noiseFigure;

  uint16_t m_cellId;

  Ptr<newradioPhyMacCommon> m_phyMacConfig;

  std::map<uint32_t, Ptr<PacketBurst> > m_packetBurstMap;
  std::vector< std::list<Ptr<newradioControlMessage> > > m_controlMessageQueue;

  TddSlotTypeList m_currTddMap;
//	std::list<SfAllocInfo> m_sfAllocInfoList;
  SfAllocInfo m_currSfAllocInfo;

  std::vector <SfAllocInfo> m_sfAllocInfo;                      // maps subframe num to allocation info

  uint16_t m_frameNum;
  uint8_t m_sfNum;
  uint8_t m_slotNum;

  Time m_ctrlPeriod;
  Time m_dataPeriod;

  std::map <uint32_t,TddSlotTypeList> m_tddPatternForSlotMap;

  std::map <uint32_t,SfAllocInfo> m_slotAllocInfoMap;

  newradioPhySapProvider* m_phySapProvider;

  uint32_t m_raPreambleId;

  bool m_sfAllocInfoUpdated;

  // hack to allow eNB to compute the SINR, periodically, without pilots
  Ptr<SpectrumPropagationLossModel> m_spectrumPropagationLossModel;
  Ptr<PropagationLossModel> m_propagationLoss;
  Ptr<newradioLosTracker> m_losTracker;

  /// component carrier Id used to address sap
  uint8_t m_componentCarrierId;


private:
};

}

}


#endif /* SRC_newradio_MODEL_newradio_PHY_H_ */
