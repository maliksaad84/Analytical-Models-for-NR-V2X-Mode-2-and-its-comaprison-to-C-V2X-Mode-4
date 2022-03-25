#ifndef newradio_HELPER_H
#define newradio_HELPER_H

#include <ns3/config.h>
#include <ns3/simulator.h>
#include <ns3/names.h>
#include <ns3/net-device.h>
#include <ns3/net-device-container.h>
#include <ns3/node.h>
#include <ns3/node-container.h>
#include <ns3/mobility-model.h>
#include <ns3/spectrum-phy.h>
#include <ns3/newradio-ue-net-device.h>
#include <ns3/mc-ue-net-device.h>
#include <ns3/newradio-enb-net-device.h>
#include <ns3/newradio-phy.h>
#include <ns3/newradio-ue-phy.h>
#include <ns3/newradio-enb-phy.h>
#include <ns3/newradio-spectrum-value-helper.h>
#include <ns3/newradio-phy-mac-common.h>
#include <ns3/antenna-array-model.h>
#include <ns3/newradio-rrc-protocol-ideal.h>
#include "newradio-phy-rx-trace.h"
#include <ns3/epc-helper.h>
#include <ns3/epc-ue-nas.h>
#include <ns3/lte-enb-rrc.h>
#include <ns3/lte-ue-rrc.h>
#include <ns3/boolean.h>
#include <ns3/epc-helper.h>
#include <ns3/lte-ffr-algorithm.h>
#include <ns3/newradio-beamforming.h>
#include <ns3/newradio-channel-matrix.h>
#include <ns3/newradio-bearer-stats-calculator.h>
#include <ns3/mc-stats-calculator.h>
#include <ns3/newradio-bearer-stats-connector.h>
#include <ns3/propagation-loss-model.h>
#include <ns3/newradio-channel-raytracing.h>

#include <ns3/lte-enb-mac.h>
#include <ns3/lte-enb-net-device.h>
#include <ns3/lte-enb-phy.h>
#include <ns3/ff-mac-scheduler.h>
#include <ns3/lte-handover-algorithm.h>
#include <ns3/epc-enb-s1-sap.h>
#include <ns3/lte-anr.h>
#include <ns3/lte-spectrum-value-helper.h>
#include <ns3/core-network-stats-calculator.h>
#include <ns3/newradio-los-tracker.h>

#include <ns3/buildings-obstacle-propagation-loss-model.h>
#include <ns3/newradio-3gpp-channel.h>

#include <ns3/newradio-component-carrier-enb.h>


namespace ns3 {

class SpectrumChannel;
class SpectrumpropagationLossModel;
class PropagationLossModel;

namespace newradio {

/* ... */
class newradioUePhy;
class newradioEnbPhy;
class newradioSpectrumValueHelper;
//class newradio3gppChannel;

class newradioHelper : public Object
{
public:
  newradioHelper (void);
  virtual ~newradioHelper (void);

  static TypeId GetTypeId (void);
  virtual void DoDispose (void);

  NetDeviceContainer InstallUeDevice (NodeContainer c);
  NetDeviceContainer InstallMcUeDevice (NodeContainer c);
  NetDeviceContainer InstallInterRatHoCapableUeDevice (NodeContainer c);
  NetDeviceContainer InstallEnbDevice (NodeContainer c);
  NetDeviceContainer InstallLteEnbDevice (NodeContainer c);
  void SetAntenna (uint16_t Nrx, uint16_t Ntx);
  void SetPathlossModelType (std::string type);
  void SetChannelModelType (std::string type);
  void SetLtePathlossModelType (std::string type);

  /**
   * This method is used to set the newradioComponentCarrier map.
   * The structure will be used within InstallSingleEnbDevice,
   * InstallSingleUeNetDevice and InstallSingleMcUeDevice.
   *
   * \param ccmap the component carrier map
   */
  void SetCcPhyParams ( std::map< uint8_t, newradioComponentCarrier> ccMapParams);

  /**
   * This method is used to get the newradioComponentCarrier map.
   *
   * \return ccmap the component carrier map
   */
  std::map< uint8_t, newradioComponentCarrier> GetCcPhyParams ();

  /**
   * This method is used to set the ComponentCarrier map.
   * The structure will be used within InstallSingleLteEnbDevice,
   * and InstallSingleMcUeDevice.
   *
   * \param ccmap the component carrier map
   */
  void SetLteCcPhyParams ( std::map< uint8_t, ComponentCarrier> ccMapParams);

  /**
   * Attach newradio-only ueDevices to the closest enbDevice
   */
  void AttachToClosestEnb (NetDeviceContainer ueDevices, NetDeviceContainer enbDevices);
  /**
   * Attach MC ueDevices to the closest LTE enbDevice, register all newradio eNBs to the newradioUePhy
   */
  void AttachToClosestEnb (NetDeviceContainer ueDevices, NetDeviceContainer newradioEnbDevices, NetDeviceContainer lteEnbDevices);

  /**
   * Attach MC ueDevices to the closest newradio eNB device, register all newradio eNBs to the newradioUePhy,
   * store all cellId in each LteUeRrc layer.
   * This must be used when attaching InterRatHandover capable devices
   */
  void AttachIrToClosestEnb (NetDeviceContainer ueDevices, NetDeviceContainer newradioEnbDevices, NetDeviceContainer lteEnbDevices);


  void EnableTraces ();

  void SetSchedulerType (std::string type);
  std::string GetSchedulerType () const;

  void SetLteSchedulerType (std::string type);
  std::string GetLteSchedulerType () const;

  void ActivateDataRadioBearer (NetDeviceContainer ueDevices, EpsBearer bearer);
  void ActivateDataRadioBearer (Ptr<NetDevice> ueDevice, EpsBearer bearer);
  void SetEpcHelper (Ptr<EpcHelper> epcHelper);

  void SetHarqEnabled (bool harqEnabled);
  bool GetHarqEnabled ();
  void SetSnrTest (bool snrTest);
  bool GetSnrTest ();
  Ptr<PropagationLossModel> GetPathLossModel (uint8_t index);

  /**
  * Set the type of FFR algorithm to be used by LTE eNodeB devices.
  *
  * \param type type of FFR algorithm, must be a type name of any class
  *             inheriting from ns3::LteFfrAlgorithm, for example:
  *             "ns3::LteFrNoOpAlgorithm"
  *
  * Equivalent with setting the `FfrAlgorithm` attribute.
  */
  void SetLteFfrAlgorithmType (std::string type);

  /**
  *
  * \return the LTE FFR algorithm type
  */
  std::string GetLteFfrAlgorithmType () const;

  /**
  * Set the type of handover algorithm to be used by LTE eNodeB devices.
  *
  * \param type type of handover algorithm, must be a type name of any class
  *             inheriting from ns3::LteHandoverAlgorithm, for example:
  *             "ns3::NoOpHandoverAlgorithm"
  *
  * Equivalent with setting the `HandoverAlgorithm` attribute.
  */
  void SetLteHandoverAlgorithmType (std::string type);

  /**
  *
  * \return the LTE handover algorithm type
  */
  std::string GetLteHandoverAlgorithmType () const;

  void AddX2Interface (NodeContainer enbNodes);
  void AddX2Interface (NodeContainer lteEnbNodes, NodeContainer newradioEnbNodes);
  void AddX2Interface (Ptr<Node> enbNode1, Ptr<Node> enbNode2);

  /**
* Set the type of carrier component algorithm to be used by gNodeB devices.
*
* \param type type of carrier component manager
*
*/
  void SetEnbComponentCarrierManagerType (std::string type);

  /**
   *
   * \return the carrier gnb component carrier manager type
   */
  std::string GetEnbComponentCarrierManagerType () const;

  /**
* Set the type of carrier component algorithm to be used by eNodeB devices.
*
* \param type type of carrier component manager
*
*/
  void SetLteEnbComponentCarrierManagerType (std::string type);

  /**
   *
   * \return the carrier enb component carrier manager type
   */
  std::string GetLteEnbComponentCarrierManagerType () const;

  /**
* Set the type of Component Carrier Manager to be used by Ue devices.
*
* \param type type of UE Component Carrier Manager
*
*/
  void SetUeComponentCarrierManagerType (std::string type);

  /**
   *
   * \return the carrier ue component carrier manager type
   */
  std::string GetUeComponentCarrierManagerType () const;

  /**
   * Set the blockage attribute of each channel if newradio3gppChannel is used.
   *
   * \parmam map (CC ID, blockage attribute value)
   *
   */
  void SetBlockageMap (std::map<uint8_t, bool> blockageMap);

  void EnablePdcpTraces (void);
  void EnableMcTraces (void);
  void EnableRlcTraces (void);
  void EnableDlPhyTrace ();
  void EnableUlPhyTrace ();

protected:
  virtual void DoInitialize ();

private:
  void newradioChannelModelInitialization ();
  void LteChannelModelInitialization ();

  Ptr<NetDevice> InstallSingleUeDevice (Ptr<Node> n);
  Ptr<NetDevice> InstallSingleMcUeDevice (Ptr<Node> n);
  Ptr<NetDevice> InstallSingleEnbDevice (Ptr<Node> n);
  Ptr<NetDevice> InstallSingleLteEnbDevice (Ptr<Node> n);
  Ptr<NetDevice> InstallSingleInterRatHoCapableUeDevice (Ptr<Node> n);

  void AttachToClosestEnb (Ptr<NetDevice> ueDevice, NetDeviceContainer enbDevices);
  void AttachMcToClosestEnb (Ptr<NetDevice> ueDevice, NetDeviceContainer newradioEnbDevices, NetDeviceContainer lteEnbDevices);
  void AttachIrToClosestEnb (Ptr<NetDevice> ueDevice, NetDeviceContainer newradioEnbDevices, NetDeviceContainer lteEnbDevices);

  //void EnableDlPhyTrace ();
  //void EnableUlPhyTrace ();
  void EnableEnbPacketCountTrace ();
  void EnableUePacketCountTrace ();
  void EnableTransportBlockTrace ();
  //void EnableRlcTraces (void);
  Ptr<newradioBearerStatsCalculator> GetRlcStats (void);
  //void EnablePdcpTraces (void);
  Ptr<newradioBearerStatsCalculator> GetPdcpStats (void);
  //void EnableMcTraces (void);
  Ptr<McStatsCalculator> GetMcStats (void);

  std::map< uint8_t, Ptr<SpectrumChannel> > m_channel;       // newradio TDD channel
  Ptr<SpectrumChannel> m_downlinkChannel;       /// The downlink LTE channel used in the simulation.
  Ptr<SpectrumChannel> m_uplinkChannel;         /// The uplink LTE channel used in the simulation.

  std::map< uint8_t, Ptr<newradioBeamforming> > m_beamforming;
  std::map< uint8_t, Ptr<newradioLosTracker> > m_losTracker;
  std::map< uint8_t, Ptr<newradioChannelMatrix> > m_channelMatrix;
  std::map< uint8_t, Ptr<newradioChannelRaytracing> > m_raytracing;
  std::map< uint8_t, Ptr<newradio3gppChannel> > m_3gppChannel;

  std::map< uint8_t, Ptr<Object> > m_pathlossModel;
  std::string m_pathlossModelType;
  Ptr<Object> m_downlinkPathlossModel;            /// The path loss model used in the LTE downlink channel.
  Ptr<Object> m_uplinkPathlossModel;         /// The path loss model used in the LTE uplink channel.


  std::string m_channelModelType;

  ObjectFactory m_enbNetDeviceFactory;
  ObjectFactory m_lteEnbNetDeviceFactory;
  ObjectFactory m_ueNetDeviceFactory;
  ObjectFactory m_mcUeNetDeviceFactory;
  ObjectFactory m_channelFactory;               // TODO check if one factory for the channel is enough
  ObjectFactory m_pathlossModelFactory;         // Each channel (newradio, LteUl & LteDl) may have a different pathloss with diff attributes
  ObjectFactory m_schedulerFactory;
  ObjectFactory m_lteSchedulerFactory;       // Factory for LTE scheduler
  ObjectFactory m_ffrAlgorithmFactory;
  ObjectFactory m_lteFfrAlgorithmFactory;
  ObjectFactory m_lteHandoverAlgorithmFactory;

  ObjectFactory m_lteChannelFactory;            /// Factory of both the downlink and uplink LTE channels.
  ObjectFactory m_dlPathlossModelFactory;               /// Factory of path loss model object for the downlink channel.
  ObjectFactory m_ulPathlossModelFactory;               /// Factory of path loss model object for the uplink channel.

  ObjectFactory m_enbComponentCarrierManagerFactory;            /// Factory of enb component carrier manager object.
  ObjectFactory m_ueComponentCarrierManagerFactory;         /// Factory of ue component carrier manager object.

  ObjectFactory m_lteEnbComponentCarrierManagerFactory;         /// Factory of enb component carrier manager object.

  uint64_t m_imsiCounter;
  uint16_t m_cellIdCounter;

  uint16_t m_noTxAntenna;
  uint16_t m_noRxAntenna;

  uint16_t m_noEnbPanels;
  uint16_t m_noUePanels;
  Ptr<newradioPhyRxTrace> m_phyStats;

  ObjectFactory m_enbAntennaModelFactory;
  ObjectFactory m_ueAntennaModelFactory;        // Factory of antenna object for newradio UE
  ObjectFactory m_lteUeAntennaModelFactory;             /// Factory of antenna object for Lte UE.
  ObjectFactory m_lteEnbAntennaModelFactory;       /// Factory of antenna objects for Lte eNB.

  /**
  * From lte-helper.h
  * The `UsePdschForCqiGeneration` attribute. If true, DL-CQI will be
  * calculated from PDCCH as signal and PDSCH as interference. If false,
  * DL-CQI will be calculated from PDCCH as signal and PDCCH as interference.
  */
  bool m_usePdschForCqiGeneration;
  bool m_isAnrEnabled;

  Ptr<EpcHelper> m_epcHelper;

  bool m_harqEnabled;
  bool m_rlcAmEnabled;
  bool m_snrTest;
  bool m_useIdealRrc;       // Initialized as true in the constructor

  Ptr<newradioBearerStatsCalculator> m_rlcStats;
  Ptr<newradioBearerStatsCalculator> m_pdcpStats;
  Ptr<McStatsCalculator> m_mcStats;
  Ptr<newradioBearerStatsConnector> m_radioBearerStatsConnector;
  Ptr<CoreNetworkStatsCalculator> m_cnStats;

  /**
* The `LteUseCa` attribute. If true, Carrier Aggregation is enabled in the
   * LTE stack.
*/
  bool m_lteUseCa;

  /**
* The `UseCa` attribute. If true, Carrier Aggregation is enabled in the newradio
   * stack.
*/
  bool m_useCa;

  /**
* This contains all the informations about each LTE component carrier
*/
  std::map< uint8_t, ComponentCarrier > m_lteComponentCarrierPhyParams;

  /**
* This contains all the informations about each newradio component carrier
*/
  std::map< uint8_t, newradioComponentCarrier > m_componentCarrierPhyParams;

  /**
   * Number of LTE component carriers that will be installed by default at LTE
         * eNodeB and MC-UE devices.
   */
  uint16_t m_noOfLteCcs;

  /**
* Number of component carriers that will be installed by default at gNodeB
   * and UE devices.
*/
  uint16_t m_noOfCcs;

  /**
   * This map is used to set the Blockage attribute of each channel if newradio3gppChannel
   * is used
  **/
  std::map< uint8_t, bool > m_3gppBlockage;

};

}

}

#endif /* newradio_HELPER_H */
