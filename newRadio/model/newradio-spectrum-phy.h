#ifndef SRC_newradio_MODEL_newradio_SPECTRUM_PHY_H_
#define SRC_newradio_MODEL_newradio_SPECTRUM_PHY_H_


#include <ns3/object-factory.h>
#include <ns3/event-id.h>
#include <ns3/spectrum-value.h>
#include <ns3/mobility-model.h>
#include <ns3/packet.h>
#include <ns3/nstime.h>
#include <ns3/net-device.h>
#include <ns3/spectrum-phy.h>
#include <ns3/spectrum-channel.h>
#include <ns3/spectrum-interference.h>
#include <ns3/data-rate.h>
#include <ns3/generic-phy.h>
#include <ns3/packet-burst.h>
#include "newradio-spectrum-signal-parameters.h"
#include "ns3/random-variable-stream.h"
#include "ns3/newradio-beamforming.h"
#include "newradio-interference.h"
#include "newradio-control-messages.h"
#include "newradio-harq-phy.h"

namespace ns3 {

namespace newradio {

struct ExpectedTbInfo_t
{
  uint8_t ndi;
  uint32_t size;
  uint8_t mcs;
  std::vector<int> rbBitmap;
  uint8_t harqProcessId;
  uint8_t rv;
  double mi;
  bool downlink;
  bool corrupt;
  bool harqFeedbackSent;
  double tbler;
  uint8_t               symStart;
  uint8_t               numSym;
};

typedef std::map<uint16_t, ExpectedTbInfo_t> ExpectedTbMap_t;

typedef Callback< void, Ptr<Packet> > newradioPhyRxDataEndOkCallback;
typedef Callback< void, std::list<Ptr<newradioControlMessage> > > newradioPhyRxCtrlEndOkCallback;

/**
* This method is used by the LteSpectrumPhy to notify the PHY about
* the status of a certain DL HARQ process
*/
typedef Callback< void, DlHarqInfo > newradioPhyDlHarqFeedbackCallback;

/**
* This method is used by the LteSpectrumPhy to notify the PHY about
* the status of a certain UL HARQ process
*/
typedef Callback< void, UlHarqInfo > newradioPhyUlHarqFeedbackCallback;


class newradioSpectrumPhy : public SpectrumPhy
{
public:
  newradioSpectrumPhy ();
  virtual ~newradioSpectrumPhy ();

  enum State
  {
    IDLE = 0,
    TX,
    RX_DATA,
    RX_CTRL
  };

  static TypeId GetTypeId (void);
  virtual void DoDispose ();

  void Reset ();
  void ResetSpectrumModel ();

  void SetDevice (Ptr<NetDevice> d);
  Ptr<NetDevice> GetDevice () const;
  void SetMobility (Ptr<MobilityModel> m);
  Ptr<MobilityModel> GetMobility ();
  void SetChannel (Ptr<SpectrumChannel> c);
  Ptr<const SpectrumModel> GetRxSpectrumModel () const;

  Ptr<AntennaModel> GetRxAntenna ();
  void SetAntenna (Ptr<AntennaModel> a);

  void SetState (State newState);

  void SetNoisePowerSpectralDensity (Ptr<const SpectrumValue> noisePsd);
  void SetTxPowerSpectralDensity (Ptr<SpectrumValue> TxPsd);
  void StartRx (Ptr<SpectrumSignalParameters> params);
  void StartRxData (Ptr<newradioSpectrumSignalParametersDataFrame> params);
  void StartRxCtrl (Ptr<SpectrumSignalParameters> params);
  Ptr<SpectrumChannel> GetSpectrumChannel ();
  void SetCellId (uint16_t cellId);

  /**
   *
   * \param componentCarrierId the component carrier id
   */
  void SetComponentCarrierId (uint8_t componentCarrierId);


  bool StartTxDataFrames (Ptr<PacketBurst> pb, std::list<Ptr<newradioControlMessage> > ctrlMsgList, Time duration, uint8_t slotInd);

  bool StartTxDlControlFrames (std::list<Ptr<newradioControlMessage> > ctrlMsgList, Time duration);       // control frames from enb to ue
  bool StartTxUlControlFrames (void);       // control frames from ue to enb

  void SetPhyRxDataEndOkCallback (newradioPhyRxDataEndOkCallback c);
  void SetPhyRxCtrlEndOkCallback (newradioPhyRxCtrlEndOkCallback c);
  void SetPhyDlHarqFeedbackCallback (newradioPhyDlHarqFeedbackCallback c);
  void SetPhyUlHarqFeedbackCallback (newradioPhyUlHarqFeedbackCallback c);

  void AddDataPowerChunkProcessor (Ptr<newradioChunkProcessor> p);
  void AddDataSinrChunkProcessor (Ptr<newradioChunkProcessor> p);

  void UpdateSinrPerceived (const SpectrumValue& sinr);

  /**
   * Add the transport block that the spectrum should expect to receive.
   *
   * \param rnti the RNTI of the UE
   * \param ndi the New Data Indicator
   * \param tbSize the size of the transport block
   * \param mcs the modulation and coding scheme to use
   * \param map a map of the resource blocks used
   * \param harqId the ID of the HARQ process
   * \param rv the number of retransmissions
   * \param downlink a boolean flag for a downlink transmission
   * \param symStart the first symbol of this TB
   * \param numSym the number of symbols of the TB
   */
  void AddExpectedTb (uint16_t rnti, uint8_t ndi, uint32_t tbSize, uint8_t mcs, std::vector<int> map, uint8_t harqId,
                      uint8_t rv, bool downlink, uint8_t symStart, uint8_t numSym);

  void SetHarqPhyModule (Ptr<newradioHarqPhy> harq);


private:
  void ChangeState (State newState);
  void EndTx ();
  void EndRxData ();
  void EndRxCtrl ();

  Ptr<newradioInterference> m_interferenceData;
  Ptr<MobilityModel> m_mobility;
  Ptr<NetDevice> m_device;
  Ptr<SpectrumChannel> m_channel;
  Ptr<const SpectrumModel> m_rxSpectrumModel;
  Ptr<SpectrumValue> m_txPsd;
  //Ptr<PacketBurst> m_txPacketBurst;
  std::list<Ptr<PacketBurst> > m_rxPacketBurstList;
  std::list<Ptr<newradioControlMessage> > m_rxControlMessageList;

  Time m_firstRxStart;
  Time m_firstRxDuration;

  Ptr<AntennaModel> m_antenna;

  uint16_t m_cellId;

  State m_state;

  uint8_t m_componentCarrierId; ///< the component carrier ID

  newradioPhyRxCtrlEndOkCallback    m_phyRxCtrlEndOkCallback;
  newradioPhyRxDataEndOkCallback            m_phyRxDataEndOkCallback;

  newradioPhyDlHarqFeedbackCallback m_phyDlHarqFeedbackCallback;
  newradioPhyUlHarqFeedbackCallback m_phyUlHarqFeedbackCallback;

  TracedCallback<RxPacketTraceParams> m_rxPacketTraceEnb;
  TracedCallback<RxPacketTraceParams> m_rxPacketTraceUe;

  SpectrumValue m_sinrPerceived;

  ExpectedTbMap_t m_expectedTbs;

  Ptr<UniformRandomVariable> m_random;

  bool m_dataErrorModelEnabled;       // when true (default) the phy error model is enabled
  bool m_ctrlErrorModelEnabled;       // when true (default) the phy error model is enabled for DL ctrl frame

  Ptr<newradioHarqPhy> m_harqPhyModule;

  bool m_isEnb;

  EventId m_endTxEvent;
  EventId m_endRxDataEvent;
  EventId m_endRxDlCtrlEvent;
  std::string m_fileName;

};

}

}


#endif /* SRC_newradio_MODEL_newradio_SPECTRUM_PHY_H_ */
