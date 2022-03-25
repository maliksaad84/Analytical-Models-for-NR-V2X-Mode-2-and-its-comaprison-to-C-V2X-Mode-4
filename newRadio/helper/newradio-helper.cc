#include <ns3/string.h>
#include <ns3/log.h>
#include <ns3/abort.h>
#include <ns3/pointer.h>
#include <iostream>
#include <string>
#include <sstream>
#include "newradio-helper.h"
#include <ns3/abort.h>
#include <ns3/multi-model-spectrum-channel.h>
#include <ns3/uinteger.h>
#include <ns3/double.h>
#include <ns3/ipv4.h>
#include <ns3/newradio-lte-rrc-protocol-real.h>
#include <ns3/epc-enb-application.h>
#include <ns3/epc-x2.h>
#include <ns3/friis-spectrum-propagation-loss.h>
#include <ns3/newradio-rrc-protocol-ideal.h>
#include <ns3/lte-spectrum-phy.h>
#include <ns3/lte-chunk-processor.h>
#include <ns3/isotropic-antenna-model.h>
#include <ns3/newradio-propagation-loss-model.h>
#include <ns3/newradio-3gpp-buildings-propagation-loss-model.h>
#include <ns3/lte-enb-component-carrier-manager.h>
#include <ns3/lte-ue-component-carrier-manager.h>
#include <ns3/cc-helper.h>
#include <ns3/object-map.h>


namespace ns3 {

/* ... */
NS_LOG_COMPONENT_DEFINE ("newradioHelper");

namespace newradio {

NS_OBJECT_ENSURE_REGISTERED (newradioHelper);

newradioHelper::newradioHelper (void)
  : m_imsiCounter (0),
    m_cellIdCounter (1),
    m_noTxAntenna (64),
    // TODO fix # antennas and # panels attributes
    m_noRxAntenna (16),
    m_noEnbPanels (3),
    m_noUePanels (2),
    m_harqEnabled (false),
    m_rlcAmEnabled (false),
    m_snrTest (false),
    m_useIdealRrc (false)
{
  NS_LOG_FUNCTION (this);
  m_channelFactory.SetTypeId (MultiModelSpectrumChannel::GetTypeId ());
  m_lteChannelFactory.SetTypeId (MultiModelSpectrumChannel::GetTypeId ());
  m_enbNetDeviceFactory.SetTypeId (newradioEnbNetDevice::GetTypeId ());
  m_lteEnbNetDeviceFactory.SetTypeId (LteEnbNetDevice::GetTypeId ());
  m_ueNetDeviceFactory.SetTypeId (newradioUeNetDevice::GetTypeId ());

  m_mcUeNetDeviceFactory.SetTypeId (McUeNetDevice::GetTypeId ());
  m_enbAntennaModelFactory.SetTypeId (AntennaArrayModel::GetTypeId ());
  m_ueAntennaModelFactory.SetTypeId (AntennaArrayModel::GetTypeId ());

  m_lteUeAntennaModelFactory.SetTypeId (IsotropicAntennaModel::GetTypeId ());
  m_lteEnbAntennaModelFactory.SetTypeId (IsotropicAntennaModel::GetTypeId ());

  m_3gppBlockage [0] = false;
  // TODO add Set methods for LTE antenna
}

newradioHelper::~newradioHelper (void)
{
  NS_LOG_FUNCTION (this);
}

TypeId
newradioHelper::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::newradioHelper")
    .SetParent<Object> ()
    .AddConstructor<newradioHelper> ()
    .AddAttribute ("PathlossModel",
                   "The type of path-loss model to be used. "
                   "The allowed values for this attributes are the type names "
                   "of any class inheriting from ns3::PropagationLossModel.",
                   StringValue ("ns3::newradioPropagationLossModel"),
                   MakeStringAccessor (&newradioHelper::SetPathlossModelType),
                   MakeStringChecker ())
    .AddAttribute ("ChannelModel",
                   "The type of MIMO channel model to be used. "
                   "The allowed values for this attributes are the type names "
                   "of any class inheriting from ns3::SpectrumPropagationLossModel.",
                   StringValue ("ns3::newradioBeamforming"),
                   MakeStringAccessor (&newradioHelper::SetChannelModelType),
                   MakeStringChecker ())
    .AddAttribute ("Scheduler",
                   "The type of scheduler to be used for newradio eNBs. "
                   "The allowed values for this attributes are the type names "
                   "of any class inheriting from ns3::newradioMacScheduler.",
                   StringValue ("ns3::newradioFlexTtiMacScheduler"),
                   MakeStringAccessor (&newradioHelper::SetSchedulerType,
                                       &newradioHelper::GetSchedulerType),
                   MakeStringChecker ())
    .AddAttribute ("HarqEnabled",
                   "Enable Hybrid ARQ",
                   BooleanValue (true),
                   MakeBooleanAccessor (&newradioHelper::m_harqEnabled),
                   MakeBooleanChecker ())
    .AddAttribute ("RlcAmEnabled",
                   "Enable RLC Acknowledged Mode",
                   BooleanValue (false),
                   MakeBooleanAccessor (&newradioHelper::m_rlcAmEnabled),
                   MakeBooleanChecker ())
    .AddAttribute ("LteScheduler",
                   "The type of scheduler to be used for LTE eNBs. "
                   "The allowed values for this attributes are the type names "
                   "of any class inheriting from ns3::FfMacScheduler.",
                   StringValue ("ns3::PfFfMacScheduler"),
                   MakeStringAccessor (&newradioHelper::SetLteSchedulerType,
                                       &newradioHelper::GetLteSchedulerType),
                   MakeStringChecker ())
    .AddAttribute ("LteFfrAlgorithm",
                   "The type of FFR algorithm to be used for LTE eNBs. "
                   "The allowed values for this attributes are the type names "
                   "of any class inheriting from ns3::LteFfrAlgorithm.",
                   StringValue ("ns3::LteFrNoOpAlgorithm"),
                   MakeStringAccessor (&newradioHelper::SetLteFfrAlgorithmType,
                                       &newradioHelper::GetLteFfrAlgorithmType),
                   MakeStringChecker ())
    .AddAttribute ("LteHandoverAlgorithm",
                   "The type of handover algorithm to be used for LTE eNBs. "
                   "The allowed values for this attributes are the type names "
                   "of any class inheriting from ns3::LteHandoverAlgorithm.",
                   StringValue ("ns3::NoOpHandoverAlgorithm"),
                   MakeStringAccessor (&newradioHelper::SetLteHandoverAlgorithmType,
                                       &newradioHelper::GetLteHandoverAlgorithmType),
                   MakeStringChecker ())
    .AddAttribute ("LtePathlossModel",
                   "The type of pathloss model to be used for the 2 LTE channels. "
                   "The allowed values for this attributes are the type names "
                   "of any class inheriting from ns3::PropagationLossModel.",
                   StringValue ("ns3::FriisPropagationLossModel"),
                   MakeStringAccessor (&newradioHelper::SetLtePathlossModelType),
                   MakeStringChecker ())
    .AddAttribute ("UsePdschForCqiGeneration",
                   "If true, DL-CQI will be calculated from PDCCH as signal and PDSCH as interference "
                   "If false, DL-CQI will be calculated from PDCCH as signal and PDCCH as interference  ",
                   BooleanValue (true),
                   MakeBooleanAccessor (&newradioHelper::m_usePdschForCqiGeneration),
                   MakeBooleanChecker ())
    .AddAttribute ("AnrEnabled",
                   "Activate or deactivate Automatic Neighbour Relation function",
                   BooleanValue (true),
                   MakeBooleanAccessor (&newradioHelper::m_isAnrEnabled),
                   MakeBooleanChecker ())
    .AddAttribute ("UseIdealRrc",
                   "Use Ideal or Real RRC",
                   BooleanValue (false),
                   MakeBooleanAccessor (&newradioHelper::m_useIdealRrc),
                   MakeBooleanChecker ())
    .AddAttribute ("BasicCellId",
                   "The next value will be the first cellId",
                   UintegerValue (1),
                   MakeUintegerAccessor (&newradioHelper::m_cellIdCounter),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("BasicImsi",
                   "The next value will be the first  imsi",
                   UintegerValue (0),
                   MakeUintegerAccessor (&newradioHelper::m_imsiCounter),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("NumUePanels",
                   "Number of panels for the UE",
                   UintegerValue (1),
                   MakeUintegerAccessor (&newradioHelper::m_noUePanels),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("NumEnbPanels",
                   "Number of panels for the eNB",
                   UintegerValue (1),
                   MakeUintegerAccessor (&newradioHelper::m_noEnbPanels),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("EnbComponentCarrierManager",
                   "The type of Component Carrier Manager to be used for gNBs. "
                   "The allowed values for this attributes are the type names "
                   "of any class inheriting ns3::LteEnbComponentCarrierManager.",
                   StringValue ("ns3::newradioNoOpComponentCarrierManager"),
                   MakeStringAccessor (&newradioHelper::SetEnbComponentCarrierManagerType,
                                       &newradioHelper::GetEnbComponentCarrierManagerType),
                   MakeStringChecker ())
    .AddAttribute ("UeComponentCarrierManager",
                   "The type of Component Carrier Manager to be used for UEs. "
                   "The allowed values for this attributes are the type names "
                   "of any class inheriting ns3::LteUeComponentCarrierManager.",
                   StringValue ("ns3::SimpleUeComponentCarrierManager"),
                   MakeStringAccessor (&newradioHelper::SetUeComponentCarrierManagerType,
                                       &newradioHelper::GetUeComponentCarrierManagerType),
                   MakeStringChecker ())
    .AddAttribute ("LteEnbComponentCarrierManager",
                   "The type of Component Carrier Manager to be used for eNBs. "
                   "The allowed values for this attributes are the type names "
                   "of any class inheriting ns3::LteEnbComponentCarrierManager.",
                   StringValue ("ns3::NoOpComponentCarrierManager"),
                   MakeStringAccessor (&newradioHelper::SetLteEnbComponentCarrierManagerType,
                                       &newradioHelper::GetLteEnbComponentCarrierManagerType),
                   MakeStringChecker ())
    .AddAttribute ("UseCa",
                   "If true, Carrier Aggregation feature is enabled in the newradio stack and a valid Component Carrier Map is expected."
                   "If false, single carrier simulation.",
                   BooleanValue (false),
                   MakeBooleanAccessor (&newradioHelper::m_useCa),
                   MakeBooleanChecker ())
    .AddAttribute ("LteUseCa",
                   "If true, Carrier Aggregation feature is enabled in the LTE stack and a valid Component Carrier Map is expected."
                   "If false, single carrier simulation.",
                   BooleanValue (false),
                   MakeBooleanAccessor (&newradioHelper::m_lteUseCa),
                   MakeBooleanChecker ())
    .AddAttribute ("NumberOfComponentCarriers",
                   "Set the number of newradio Component Carriers to use "
                   "If it is more than one and m_useCa is false, it will raise an error ",
                   UintegerValue (1),
                   MakeUintegerAccessor (&newradioHelper::m_noOfCcs),
                   MakeUintegerChecker<uint16_t> (MIN_NO_MMW_CC, MAX_NO_MMW_CC))
    .AddAttribute ("NumberOfLteComponentCarriers",
                   "Set the number of LTE Component Carriers to use "
                   "If it is more than one and m_lteUseCa is false, it will raise an error ",
                   UintegerValue (1),
                   MakeUintegerAccessor (&newradioHelper::m_noOfLteCcs),
                   MakeUintegerChecker<uint16_t> (MIN_NO_CC, MAX_NO_CC))
  ;

  return tid;
}

void
newradioHelper::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_channel.clear ();
  m_componentCarrierPhyParams.clear ();
  m_lteComponentCarrierPhyParams.clear ();
  Object::DoDispose ();
}

void
newradioHelper::DoInitialize ()
{
  NS_LOG_FUNCTION (this);

  // cc initialization
  // if useCa=false and SetCcPhyParams() has not been called, setup a default CC.
  if (!m_useCa && m_componentCarrierPhyParams.size () == 0)
    {
      NS_LOG_INFO ("useCa=false and empty CC map. Create the default CC.");
      Ptr<newradioPhyMacCommon> phyMacConfig = CreateObject<newradioPhyMacCommon> ();
      Ptr<newradioComponentCarrier> cc = CreateObject<newradioComponentCarrier> ();
      cc->SetConfigurationParameters (phyMacConfig);
      cc->SetAsPrimary (true);

      //create the ccMap
      std::map<uint8_t, newradioComponentCarrier > map;
      map [0] = *cc;

      this->SetCcPhyParams (map);
    }

  newradioChannelModelInitialization ();      // channel initialization

  m_phyStats = CreateObject<newradioPhyRxTrace> ();
  m_radioBearerStatsConnector = CreateObject<newradioBearerStatsConnector> ();

  // lte cc initialization
  // if m_lteUseCa=false and SetLteCcPhyParams() has not been called, setup a default LTE CC
  if (!m_lteUseCa && m_lteComponentCarrierPhyParams.size () == 0)
    {
      // create the map of LTE component carriers
      Ptr<CcHelper> lteCcHelper = CreateObject<CcHelper> ();
      lteCcHelper->SetNumberOfComponentCarriers (1);
      lteCcHelper->SetUlEarfcn (18100);
      lteCcHelper->SetDlEarfcn (100);
      lteCcHelper->SetDlBandwidth (100);
      lteCcHelper->SetUlBandwidth (100);
      std::map< uint8_t, ComponentCarrier > lteCcMap = lteCcHelper->EquallySpacedCcs ();
      lteCcMap.at (0).SetAsPrimary (true);
      this->SetLteCcPhyParams (lteCcMap);
    }

  LteChannelModelInitialization ();      // lte channel initialization

  m_cnStats = 0;       //core network stats calculator

  Object::DoInitialize ();
}

void
newradioHelper::SetBlockageMap (std::map<uint8_t, bool> blockageMap)
{
  m_3gppBlockage = blockageMap;
}

void
newradioHelper::newradioChannelModelInitialization (void)
{
  NS_LOG_FUNCTION (this);
  // setup of newradio channel & related
  //create a channel for each CC
  for (std::map<uint8_t, newradioComponentCarrier >::iterator it = m_componentCarrierPhyParams.begin (); it != m_componentCarrierPhyParams.end (); ++it)
    {
      Ptr<SpectrumChannel> channel = m_channelFactory.Create<SpectrumChannel> ();
      Ptr<newradioPhyMacCommon> phyMacCommon = m_componentCarrierPhyParams.at (it->first).GetConfigurationParameters ();

      if (!m_pathlossModelType.empty ())
        {
          Ptr<Object> pathlossModel = m_pathlossModelFactory.Create ();
          Ptr<PropagationLossModel> splm = pathlossModel->GetObject<PropagationLossModel> ();
          if ( splm )
            {
              NS_LOG_LOGIC (this << " using a PropagationLossModel");
              channel->AddPropagationLossModel (splm);
            }

          if (m_pathlossModelType == "ns3::BuildingsObstaclePropagationLossModel")
            {
              Ptr<newradioLosTracker> losTracker = CreateObject<newradioLosTracker> ();                  // create and initialize m_losTracker
              Ptr<BuildingsObstaclePropagationLossModel> building = pathlossModel->GetObject<BuildingsObstaclePropagationLossModel> ();
              building->SetConfigurationParameters (phyMacCommon);
              //building->SetBeamforming (m_beamforming); //dove viene creato m_beamforming??
              building->SetLosTracker (losTracker);                  // use m_losTracker in BuildingsObstaclePropagationLossModel

              m_losTracker [it->first] = losTracker;
            }
          else if (m_pathlossModelType == "ns3::newradioPropagationLossModel")
            {
              pathlossModel->GetObject<newradioPropagationLossModel> ()->SetConfigurationParameters (phyMacCommon);
            }
          else if (m_pathlossModelType == "ns3::newradio3gppPropagationLossModel")
            {
              pathlossModel->GetObject<newradio3gppPropagationLossModel> ()->SetConfigurationParameters (phyMacCommon);
            }
          else if (m_pathlossModelType == "ns3::newradio3gppBuildingsPropagationLossModel")
            {
              pathlossModel->GetObject<newradio3gppBuildingsPropagationLossModel> ()->SetConfigurationParameters (phyMacCommon);
            }

          m_pathlossModel [it->first] = pathlossModel;
        }
      else
        {
          NS_LOG_UNCOND (this << " No PropagationLossModel!");
        }

      if (m_channelModelType == "ns3::newradioBeamforming")
        {
          Ptr<newradioBeamforming> beamforming = CreateObject<newradioBeamforming> (m_noTxAntenna, m_noRxAntenna);
          channel->AddSpectrumPropagationLossModel (beamforming);
          beamforming->SetConfigurationParameters (phyMacCommon);

          m_beamforming [it->first] = beamforming;
        }
      else if (m_channelModelType == "ns3::newradioChannelMatrix")
        {
          Ptr<newradioChannelMatrix> channelMatrix = CreateObject<newradioChannelMatrix> ();
          channel->AddSpectrumPropagationLossModel (channelMatrix);
          channelMatrix->SetConfigurationParameters (phyMacCommon);

          m_channelMatrix [it->first] = channelMatrix;
        }
      else if (m_channelModelType == "ns3::newradioChannelRaytracing")
        {
          Ptr<newradioChannelRaytracing> raytracing = CreateObject<newradioChannelRaytracing> ();
          channel->AddSpectrumPropagationLossModel (raytracing);
          raytracing->SetConfigurationParameters (phyMacCommon);

          m_raytracing [it->first] = raytracing;
        }
      else if (m_channelModelType == "ns3::newradio3gppChannel")
        {
          Ptr<newradio3gppChannel> gppChannel = CreateObject<newradio3gppChannel> ();
          channel->AddSpectrumPropagationLossModel (gppChannel);
          gppChannel->SetConfigurationParameters (phyMacCommon);
          gppChannel->SetAttribute ("Blockage", BooleanValue (m_3gppBlockage [it->first]));

          if (m_pathlossModelType == "ns3::newradio3gppBuildingsPropagationLossModel" || m_pathlossModelType == "ns3::newradio3gppPropagationLossModel" )
            {
              Ptr<PropagationLossModel> pl = m_pathlossModel.at (it->first)->GetObject<PropagationLossModel> ();
              gppChannel->SetPathlossModel (pl);
            }
          else
            {
              NS_FATAL_ERROR ("The 3GPP channel and propagation loss should be enabled at the same time");
            }

          m_3gppChannel [it->first] = gppChannel;
        }
      m_channel [it->first] = channel;
    }    //end for
}

void
newradioHelper::LteChannelModelInitialization (void)
{
  NS_LOG_FUNCTION (this);
  // setup of LTE channels & related
  m_downlinkChannel = m_lteChannelFactory.Create<SpectrumChannel> ();
  m_uplinkChannel = m_lteChannelFactory.Create<SpectrumChannel> ();
  m_downlinkPathlossModel = m_dlPathlossModelFactory.Create ();
  Ptr<SpectrumPropagationLossModel> dlSplm = m_downlinkPathlossModel->GetObject<SpectrumPropagationLossModel> ();
  if (dlSplm != 0)
    {
      NS_LOG_LOGIC (this << " using a SpectrumPropagationLossModel in DL");
      m_downlinkChannel->AddSpectrumPropagationLossModel (dlSplm);
    }
  else
    {
      NS_LOG_LOGIC (this << " using a PropagationLossModel in DL");
      Ptr<PropagationLossModel> dlPlm = m_downlinkPathlossModel->GetObject<PropagationLossModel> ();
      NS_ASSERT_MSG (dlPlm != 0, " " << m_downlinkPathlossModel << " is neither PropagationLossModel nor SpectrumPropagationLossModel");
      m_downlinkChannel->AddPropagationLossModel (dlPlm);
    }

  m_uplinkPathlossModel = m_ulPathlossModelFactory.Create ();
  Ptr<SpectrumPropagationLossModel> ulSplm = m_uplinkPathlossModel->GetObject<SpectrumPropagationLossModel> ();
  if (ulSplm != 0)
    {
      NS_LOG_LOGIC (this << " using a SpectrumPropagationLossModel in UL");
      m_uplinkChannel->AddSpectrumPropagationLossModel (ulSplm);
    }
  else
    {
      NS_LOG_LOGIC (this << " using a PropagationLossModel in UL");
      Ptr<PropagationLossModel> ulPlm = m_uplinkPathlossModel->GetObject<PropagationLossModel> ();
      NS_ASSERT_MSG (ulPlm != 0, " " << m_uplinkPathlossModel << " is neither PropagationLossModel nor SpectrumPropagationLossModel");
      m_uplinkChannel->AddPropagationLossModel (ulPlm);
    }
  // TODO consider if adding LTE fading
  // TODO add mac & phy LTE stats
}

void
newradioHelper::SetAntenna (uint16_t Nrx, uint16_t Ntx)
{
  m_noTxAntenna = Ntx;
  m_noRxAntenna = Nrx;
}

void
newradioHelper::SetLtePathlossModelType (std::string type)
{
  NS_LOG_FUNCTION (this << type);
  m_dlPathlossModelFactory = ObjectFactory ();
  m_dlPathlossModelFactory.SetTypeId (type);
  m_ulPathlossModelFactory = ObjectFactory ();
  m_ulPathlossModelFactory.SetTypeId (type);
}

void
newradioHelper::SetPathlossModelType (std::string type)
{
  NS_LOG_FUNCTION (this << type);
  m_pathlossModelType = type;
  if (!type.empty ())
    {
      m_pathlossModelFactory = ObjectFactory ();
      m_pathlossModelFactory.SetTypeId (type);
    }
}

Ptr<PropagationLossModel>
newradioHelper::GetPathLossModel (uint8_t index)
{
  return m_pathlossModel.at (index)->GetObject<PropagationLossModel> ();
}

void
newradioHelper::SetChannelModelType (std::string type)
{
  NS_LOG_FUNCTION (this << type);
  m_channelModelType = type;
}

void
newradioHelper::SetSchedulerType (std::string type)
{
  NS_LOG_FUNCTION (this << type);
  m_schedulerFactory = ObjectFactory ();
  m_schedulerFactory.SetTypeId (type);
}
std::string
newradioHelper::GetSchedulerType () const
{
  return m_schedulerFactory.GetTypeId ().GetName ();
}

void
newradioHelper::SetLteSchedulerType (std::string type)
{
  NS_LOG_FUNCTION (this << type);
  m_lteSchedulerFactory = ObjectFactory ();
  m_lteSchedulerFactory.SetTypeId (type);
}

std::string
newradioHelper::GetLteSchedulerType () const
{
  return m_lteSchedulerFactory.GetTypeId ().GetName ();
}


std::string
newradioHelper::GetLteFfrAlgorithmType () const
{
  return m_lteFfrAlgorithmFactory.GetTypeId ().GetName ();
}

void
newradioHelper::SetLteFfrAlgorithmType (std::string type)
{
  NS_LOG_FUNCTION (this << type);
  m_lteFfrAlgorithmFactory = ObjectFactory ();
  m_lteFfrAlgorithmFactory.SetTypeId (type);
}

// TODO add attributes

std::string
newradioHelper::GetLteHandoverAlgorithmType () const
{
  return m_lteHandoverAlgorithmFactory.GetTypeId ().GetName ();
}

void
newradioHelper::SetLteHandoverAlgorithmType (std::string type)
{
  NS_LOG_FUNCTION (this << type);
  m_lteHandoverAlgorithmFactory = ObjectFactory ();
  m_lteHandoverAlgorithmFactory.SetTypeId (type);
}

void
newradioHelper::SetHarqEnabled (bool harqEnabled)
{
  m_harqEnabled = harqEnabled;
}

bool
newradioHelper::GetHarqEnabled ()
{
  return m_harqEnabled;
}

void
newradioHelper::SetSnrTest (bool snrTest)
{
  m_snrTest = snrTest;
}

bool
newradioHelper::GetSnrTest ()
{
  return m_snrTest;
}

void
newradioHelper::SetCcPhyParams ( std::map< uint8_t, newradioComponentCarrier> ccMapParams)
{
  NS_LOG_FUNCTION (this);
  m_componentCarrierPhyParams = ccMapParams;
}

std::map< uint8_t, newradioComponentCarrier>
newradioHelper::GetCcPhyParams ()
{
  NS_LOG_FUNCTION (this);
  return m_componentCarrierPhyParams;
}

void
newradioHelper::SetLteCcPhyParams ( std::map< uint8_t, ComponentCarrier> ccMapParams)
{
  NS_LOG_FUNCTION (this);
  m_lteComponentCarrierPhyParams = ccMapParams;
}

NetDeviceContainer
newradioHelper::InstallUeDevice (NodeContainer c)
{
  NS_LOG_FUNCTION (this);
  Initialize ();        // Run DoInitialize (), if necessary
  NetDeviceContainer devices;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<NetDevice> device = InstallSingleUeDevice (node);
      device->SetAddress (Mac64Address::Allocate ());
      devices.Add (device);
    }
  return devices;

}

NetDeviceContainer
newradioHelper::InstallMcUeDevice (NodeContainer c)
{
  NS_LOG_FUNCTION (this);
  Initialize ();        // Run DoInitialize (), if necessary
  NetDeviceContainer devices;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<NetDevice> device = InstallSingleMcUeDevice (node);
      device->SetAddress (Mac64Address::Allocate ());
      devices.Add (device);
    }
  return devices;
}

NetDeviceContainer
newradioHelper::InstallInterRatHoCapableUeDevice (NodeContainer c)
{
  NS_LOG_FUNCTION (this);
  Initialize ();        // Run DoInitialize (), if necessary
  NetDeviceContainer devices;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<NetDevice> device = InstallSingleInterRatHoCapableUeDevice (node);
      device->SetAddress (Mac64Address::Allocate ());
      devices.Add (device);
    }
  return devices;
}

NetDeviceContainer
newradioHelper::InstallEnbDevice (NodeContainer c)
{
  NS_LOG_FUNCTION (this);
  Initialize ();        // Run DoInitialize (), if necessary
  NetDeviceContainer devices;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<NetDevice> device = InstallSingleEnbDevice (node);
      device->SetAddress (Mac64Address::Allocate ());
      devices.Add (device);
    }
  return devices;
}

NetDeviceContainer
newradioHelper::InstallLteEnbDevice (NodeContainer c)
{
  NS_LOG_FUNCTION (this);
  Initialize ();        // Run DoInitialize (), if necessary
  NetDeviceContainer devices;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<NetDevice> device = InstallSingleLteEnbDevice (node);
      device->SetAddress (Mac64Address::Allocate ());
      devices.Add (device);
    }
  return devices;
}

Ptr<NetDevice>
newradioHelper::InstallSingleMcUeDevice (Ptr<Node> n)
{
  NS_LOG_FUNCTION (this);


  NS_ABORT_MSG_IF (m_imsiCounter >= 0xFFFFFFFF, "max num UEs exceeded");
  uint64_t imsi = ++m_imsiCounter;

  Ptr<McUeNetDevice> device = m_mcUeNetDeviceFactory.Create<McUeNetDevice> ();

  // newradio phy, mac and channel
  NS_ABORT_MSG_IF (m_componentCarrierPhyParams.size () == 0 && m_useCa, "If CA is enabled, before call this method you need to install Enbs --> InstallEnbDevice()");
  std::map<uint8_t, Ptr<newradioComponentCarrierUe> > newradioUeCcMap;

  for (std::map< uint8_t, newradioComponentCarrier >::iterator it = m_componentCarrierPhyParams.begin (); it != m_componentCarrierPhyParams.end (); ++it)
    {
      Ptr <newradioComponentCarrierUe> cc =  CreateObject<newradioComponentCarrierUe> ();
      //cc->SetBandwidth ( it->second.GetBandwidth ());
      //cc->SetEarfcn ( it->second.GetUlEarfcn ());
      cc->SetConfigurationParameters (it->second.GetConfigurationParameters ());
      cc->SetAsPrimary (it->second.IsPrimary ());
      Ptr<newradioUeMac> mac = CreateObject<newradioUeMac> ();
      cc->SetMac (mac);
      // cc->GetPhy ()->Initialize (); // it is initialized within the LteUeNetDevice::DoInitialize ()
      newradioUeCcMap.insert (std::pair<uint8_t, Ptr<newradioComponentCarrierUe> > (it->first, cc));
    }

  for (std::map<uint8_t, Ptr<newradioComponentCarrierUe> >::iterator it = newradioUeCcMap.begin (); it != newradioUeCcMap.end (); ++it)
    {
      Ptr<newradioSpectrumPhy> ulPhy = CreateObject<newradioSpectrumPhy> ();
      Ptr<newradioSpectrumPhy> dlPhy = CreateObject<newradioSpectrumPhy> ();

      Ptr<newradioUePhy> phy = CreateObject<newradioUePhy> (dlPhy, ulPhy);

      Ptr<newradioHarqPhy> harq = Create<newradioHarqPhy> (it->second->GetConfigurationParameters ()->GetNumHarqProcess ());

      dlPhy->SetHarqPhyModule (harq);
      //ulPhy->SetHarqPhyModule (harq);
      phy->SetHarqPhyModule (harq);

      /* Do not do this here. Do it during registration with the BS
       * phy->SetConfigurationParameters(m_phyMacCommon);*/

      if (m_channelModelType == "ns3::newradioBeamforming")
        {
          phy->AddSpectrumPropagationLossModel (m_beamforming.at (it->first));
        }
      else if (m_channelModelType == "ns3::newradioChannelMatrix")
        {
          phy->AddSpectrumPropagationLossModel (m_channelMatrix.at (it->first));
        }
      else if (m_channelModelType == "ns3::newradioChannelRaytracing")
        {
          phy->AddSpectrumPropagationLossModel (m_raytracing.at (it->first));
        }
      if (!m_pathlossModelType.empty ())
        {
          Ptr<PropagationLossModel> splm = m_pathlossModel.at (it->first)->GetObject<PropagationLossModel> ();
          if ( splm )
            {
              phy->AddPropagationLossModel (splm);
            }
        }
      else
        {
          NS_LOG_UNCOND (this << " No PropagationLossModel!");
        }

      /*
Ptr<LteChunkProcessor> pRs = Create<LteChunkProcessor> ();
pRs->AddCallback (MakeCallback (&LteUePhy::ReportRsReceivedPower, phy));
dlPhy->AddRsPowerChunkProcessor (pRs);

Ptr<LteChunkProcessor> pInterf = Create<LteChunkProcessor> ();
pInterf->AddCallback (MakeCallback (&LteUePhy::ReportInterference, phy));
dlPhy->AddInterferenceCtrlChunkProcessor (pInterf);   // for RSRQ evaluation of UE Measurements

Ptr<LteChunkProcessor> pCtrl = Create<LteChunkProcessor> ();
pCtrl->AddCallback (MakeCallback (&LteSpectrumPhy::UpdateSinrPerceived, dlPhy));
dlPhy->AddCtrlSinrChunkProcessor (pCtrl);
      */

      Ptr<newradioChunkProcessor> pData = Create<newradioChunkProcessor> ();
      pData->AddCallback (MakeCallback (&newradioUePhy::GenerateDlCqiReport, phy));
      pData->AddCallback (MakeCallback (&newradioSpectrumPhy::UpdateSinrPerceived, dlPhy));
      dlPhy->AddDataSinrChunkProcessor (pData);
      if (m_harqEnabled)
        {
          //In lte-helper this is done in the last for cycle
          dlPhy->SetPhyDlHarqFeedbackCallback (MakeCallback (&newradioUePhy::ReceiveLteDlHarqFeedback, phy));
        }

      /*Check if this is supported in newradio
if (m_usePdschForCqiGeneration)
{
// CQI calculation based on PDCCH for signal and PDSCH for interference
pCtrl->AddCallback (MakeCallback (&LteUePhy::GenerateMixedCqiReport, phy));
Ptr<LteChunkProcessor> pDataInterf = Create<LteChunkProcessor> ();
pDataInterf->AddCallback (MakeCallback (&LteUePhy::ReportDataInterference, phy));
dlPhy->AddInterferenceDataChunkProcessor (pDataInterf);
}
else
{
// CQI calculation based on PDCCH for both signal and interference
pCtrl->AddCallback (MakeCallback (&LteUePhy::GenerateCtrlCqiReport, phy));
}*/

      ulPhy->SetChannel (m_channel.at (it->first));
      dlPhy->SetChannel (m_channel.at (it->first));

      Ptr<MobilityModel> mm = n->GetObject<MobilityModel> ();
      NS_ASSERT_MSG (mm, "MobilityModel needs to be set on node before calling LteHelper::InstallUeDevice ()");
      dlPhy->SetMobility (mm);
      ulPhy->SetMobility (mm);

      Ptr<AntennaModel> antenna = (m_ueAntennaModelFactory.Create ())->GetObject<AntennaModel> ();
      DynamicCast<AntennaArrayModel> (antenna)->SetPlanesNumber (m_noUePanels);
      DynamicCast<AntennaArrayModel> (antenna)->SetDeviceType (true);
      NS_LOG_INFO("MC device->GetAntennaNum() " << device->GetAntennaNum());
      DynamicCast<AntennaArrayModel> (antenna)->SetTotNoArrayElements (device->GetAntennaNum());
      NS_ASSERT_MSG (antenna, "error in creating the AntennaModel object");
      dlPhy->SetAntenna (antenna);
      ulPhy->SetAntenna (antenna);

      it->second->SetPhy (phy);
    }

  // LTE phy, mac and channel
  NS_ABORT_MSG_IF (m_lteComponentCarrierPhyParams.size () == 0 && m_lteUseCa, "If CA is enabled, before call this method you need to install Enbs --> InstallLteEnbDevice()");
  std::map<uint8_t, Ptr<ComponentCarrierUe> > lteUeCcMap;

  for (std::map< uint8_t, ComponentCarrier >::iterator it = m_lteComponentCarrierPhyParams.begin (); it != m_lteComponentCarrierPhyParams.end (); ++it)
    {
      Ptr <ComponentCarrierUe> cc =  CreateObject<ComponentCarrierUe> ();
      cc->SetUlBandwidth ( it->second.GetUlBandwidth ());
      cc->SetDlBandwidth ( it->second.GetDlBandwidth ());
      cc->SetDlEarfcn ( it->second.GetDlEarfcn ());
      cc->SetUlEarfcn ( it->second.GetUlEarfcn ());
      cc->SetAsPrimary (it->second.IsPrimary ());
      Ptr<LteUeMac> mac = CreateObject<LteUeMac> ();
      cc->SetMac (mac);
      // cc->GetPhy ()->Initialize (); // it is initialized within the LteUeNetDevice::DoInitialize ()
      lteUeCcMap.insert (std::pair<uint8_t, Ptr<ComponentCarrierUe> > (it->first, cc));
    }

  for (std::map<uint8_t, Ptr<ComponentCarrierUe> >::iterator it = lteUeCcMap.begin (); it != lteUeCcMap.end (); ++it)
    {
      Ptr<LteSpectrumPhy> dlPhy = CreateObject<LteSpectrumPhy> ();
      Ptr<LteSpectrumPhy> ulPhy = CreateObject<LteSpectrumPhy> ();

      Ptr<LteUePhy> phy = CreateObject<LteUePhy> (dlPhy, ulPhy);

      Ptr<LteHarqPhy> harq = Create<LteHarqPhy> ();
      dlPhy->SetHarqPhyModule (harq);
      ulPhy->SetHarqPhyModule (harq);
      phy->SetHarqPhyModule (harq);

      Ptr<LteChunkProcessor> pRs = Create<LteChunkProcessor> ();
      pRs->AddCallback (MakeCallback (&LteUePhy::ReportRsReceivedPower, phy));
      dlPhy->AddRsPowerChunkProcessor (pRs);

      Ptr<LteChunkProcessor> pInterf = Create<LteChunkProcessor> ();
      pInterf->AddCallback (MakeCallback (&LteUePhy::ReportInterference, phy));
      dlPhy->AddInterferenceCtrlChunkProcessor (pInterf);                     // for RSRQ evaluation of UE Measurements

      Ptr<LteChunkProcessor> pCtrl = Create<LteChunkProcessor> ();
      pCtrl->AddCallback (MakeCallback (&LteSpectrumPhy::UpdateSinrPerceived, dlPhy));
      dlPhy->AddCtrlSinrChunkProcessor (pCtrl);

      Ptr<LteChunkProcessor> pData = Create<LteChunkProcessor> ();
      pData->AddCallback (MakeCallback (&LteSpectrumPhy::UpdateSinrPerceived, dlPhy));
      dlPhy->AddDataSinrChunkProcessor (pData);

      if (m_usePdschForCqiGeneration)
        {
          // CQI calculation based on PDCCH for signal and PDSCH for interference
          pCtrl->AddCallback (MakeCallback (&LteUePhy::GenerateMixedCqiReport, phy));
          Ptr<LteChunkProcessor> pDataInterf = Create<LteChunkProcessor> ();
          pDataInterf->AddCallback (MakeCallback (&LteUePhy::ReportDataInterference, phy));
          dlPhy->AddInterferenceDataChunkProcessor (pDataInterf);
        }
      else
        {
          // CQI calculation based on PDCCH for both signal and interference
          pCtrl->AddCallback (MakeCallback (&LteUePhy::GenerateCtrlCqiReport, phy));
        }

      dlPhy->SetChannel (m_downlinkChannel);
      ulPhy->SetChannel (m_uplinkChannel);

      Ptr<MobilityModel> mm = n->GetObject<MobilityModel> ();
      NS_ASSERT_MSG (mm, "MobilityModel needs to be set on node before calling LteHelper::InstallUeDevice ()");
      dlPhy->SetMobility (mm);
      ulPhy->SetMobility (mm);

      Ptr<AntennaModel> antenna = (m_lteUeAntennaModelFactory.Create ())->GetObject<AntennaModel> ();
      NS_ASSERT_MSG (antenna, "error in creating the AntennaModel object");
      dlPhy->SetAntenna (antenna);
      ulPhy->SetAntenna (antenna);

      it->second->SetPhy (phy);
    }

  // newradio CCM and RRC
  Ptr<LteUeComponentCarrierManager> newradioCcmUe = m_ueComponentCarrierManagerFactory.Create<LteUeComponentCarrierManager> ();

  Ptr<LteUeRrc> newradioRrc = CreateObject<LteUeRrc> ();
  newradioRrc->SetAttribute ("SecondaryRRC", BooleanValue (true));

  newradioRrc->SetLteMacSapProvider (newradioCcmUe->GetLteMacSapProvider ());
  // setting ComponentCarrierManager SAP
  newradioRrc->SetLteCcmRrcSapProvider (newradioCcmUe->GetLteCcmRrcSapProvider ());
  newradioCcmUe->SetLteCcmRrcSapUser (newradioRrc->GetLteCcmRrcSapUser ());
  // Set number of component carriers. Note: UE CCM would also set the
  // number of component carriers in UE RRC
  newradioCcmUe->SetNumberOfComponentCarriers (m_noOfCcs);

  // run intializeSap to create the proper number of sap provider/users
  newradioRrc->InitializeSap ();

  if (m_useIdealRrc)
    {
      Ptr<newradioUeRrcProtocolIdeal> rrcProtocol = CreateObject<newradioUeRrcProtocolIdeal> ();
      rrcProtocol->SetUeRrc (newradioRrc);
      newradioRrc->AggregateObject (rrcProtocol);
      rrcProtocol->SetLteUeRrcSapProvider (newradioRrc->GetLteUeRrcSapProvider ());
      newradioRrc->SetLteUeRrcSapUser (rrcProtocol->GetLteUeRrcSapUser ());
    }
  else
    {
      Ptr<newradioLteUeRrcProtocolReal> rrcProtocol = CreateObject<newradioLteUeRrcProtocolReal> ();
      rrcProtocol->SetUeRrc (newradioRrc);
      newradioRrc->AggregateObject (rrcProtocol);
      rrcProtocol->SetLteUeRrcSapProvider (newradioRrc->GetLteUeRrcSapProvider ());
      newradioRrc->SetLteUeRrcSapUser (rrcProtocol->GetLteUeRrcSapUser ());
    }

  if (m_epcHelper != 0)
    {
      newradioRrc->SetUseRlcSm (false);
    }
  else
    {
      newradioRrc->SetUseRlcSm (true);
    }

  // newradio SAPs
  for (std::map<uint8_t, Ptr<newradioComponentCarrierUe> >::iterator it = newradioUeCcMap.begin (); it != newradioUeCcMap.end (); ++it)
    {
      newradioRrc->SetLteUeCmacSapProvider (it->second->GetMac ()->GetUeCmacSapProvider (), it->first);
      it->second->GetMac ()->SetUeCmacSapUser (newradioRrc->GetLteUeCmacSapUser (it->first));
      it->second->GetMac ()->SetComponentCarrierId (it->first);

      it->second->GetPhy ()->SetUeCphySapUser (newradioRrc->GetLteUeCphySapUser (it->first));
      newradioRrc->SetLteUeCphySapProvider (it->second->GetPhy ()->GetUeCphySapProvider (), it->first);
      it->second->GetPhy ()->SetComponentCarrierId (it->first);

      it->second->GetPhy ()->SetPhySapUser (it->second->GetMac ()->GetPhySapUser ());
      it->second->GetMac ()->SetPhySapProvider (it->second->GetPhy ()->GetPhySapProvider ());

      it->second->GetPhy ()->SetConfigurationParameters (it->second->GetConfigurationParameters ());
      it->second->GetMac ()->SetConfigurationParameters (it->second->GetConfigurationParameters ());

      bool ccmTest = newradioCcmUe->SetComponentCarrierMacSapProviders (it->first, it->second->GetMac ()->GetUeMacSapProvider ());

      if (ccmTest == false)
        {
          NS_FATAL_ERROR ("Error in SetComponentCarrierMacSapProviders");
        }
    }


  device->SetNode (n);
  device->SetAttribute ("Imsi", UintegerValue (imsi));
  //device->SetAttribute ("newradioUePhy", PointerValue(phy));
  //device->SetAttribute ("newradioUeMac", PointerValue(mac));
  device->SetnewradioCcMap (newradioUeCcMap);
  device->SetAttribute ("newradioUeRrc", PointerValue (newradioRrc));
  device->SetAttribute ("newradioUeComponentCarrierManager", PointerValue (newradioCcmUe));

  for (std::map<uint8_t, Ptr<newradioComponentCarrierUe> >::iterator it = newradioUeCcMap.begin (); it != newradioUeCcMap.end (); ++it)
    {
      Ptr<newradioUePhy> ccPhy = it->second->GetPhy ();
      ccPhy->SetDevice (device);
      ccPhy->SetImsi (imsi);
      ccPhy->GetUlSpectrumPhy ()->SetDevice (device);
      ccPhy->GetDlSpectrumPhy ()->SetDevice (device);
      ccPhy->GetDlSpectrumPhy ()->SetPhyRxDataEndOkCallback (MakeCallback (&newradioUePhy::PhyDataPacketReceived, ccPhy));
      ccPhy->GetDlSpectrumPhy ()->SetPhyRxCtrlEndOkCallback (MakeCallback (&newradioUePhy::ReceiveControlMessageList, ccPhy));
      //ccPhy->GetDlSpectrumPhy ()->SetLtePhyRxPssCallback (MakeCallback (&LteUePhy::ReceivePss, ccPhy));
      //ccPhy->GetDlSpectrumPhy ()->SetLtePhyDlHarqFeedbackCallback (MakeCallback (&LteUePhy::ReceiveLteDlHarqFeedback, ccPhy)); this is done before
    }


  // LTE CCM and RRC
  Ptr<LteUeComponentCarrierManager> lteCcmUe = m_ueComponentCarrierManagerFactory.Create<LteUeComponentCarrierManager> ();

  Ptr<LteUeRrc> lteRrc = CreateObject<LteUeRrc> ();
  lteRrc->m_numberOfnewradioComponentCarriers = m_noOfCcs;

  lteRrc->SetLteMacSapProvider (lteCcmUe->GetLteMacSapProvider ());
  lteRrc->SetnewradioMacSapProvider (newradioCcmUe->GetLteMacSapProvider ());
  // setting ComponentCarrierManager SAP
  lteRrc->SetLteCcmRrcSapProvider (lteCcmUe->GetLteCcmRrcSapProvider ());
  lteCcmUe->SetLteCcmRrcSapUser (lteRrc->GetLteCcmRrcSapUser ());
  // Set number of component carriers. Note: UE CCM would also set the
  // number of component carriers in UE RRC
  lteCcmUe->SetNumberOfComponentCarriers (m_noOfLteCcs);

  // run intializeSap to create the proper number of sap provider/users
  lteRrc->InitializeSap ();

  if (m_useIdealRrc)
    {
      Ptr<newradioUeRrcProtocolIdeal> rrcProtocol = CreateObject<newradioUeRrcProtocolIdeal> ();
      rrcProtocol->SetUeRrc (lteRrc);
      lteRrc->AggregateObject (rrcProtocol);
      rrcProtocol->SetLteUeRrcSapProvider (lteRrc->GetLteUeRrcSapProvider ());
      lteRrc->SetLteUeRrcSapUser (rrcProtocol->GetLteUeRrcSapUser ());
    }
  else
    {
      Ptr<newradioLteUeRrcProtocolReal> rrcProtocol = CreateObject<newradioLteUeRrcProtocolReal> ();
      rrcProtocol->SetUeRrc (lteRrc);
      lteRrc->AggregateObject (rrcProtocol);
      rrcProtocol->SetLteUeRrcSapProvider (lteRrc->GetLteUeRrcSapProvider ());
      lteRrc->SetLteUeRrcSapUser (rrcProtocol->GetLteUeRrcSapUser ());
    }

  if (m_epcHelper != 0)
    {
      lteRrc->SetUseRlcSm (false);
    }

  Ptr<EpcUeNas> lteNas = CreateObject<EpcUeNas> ();

  lteNas->SetAsSapProvider (lteRrc->GetAsSapProvider ());
  lteRrc->SetAsSapUser (lteNas->GetAsSapUser ());
  lteNas->SetnewradioAsSapProvider (newradioRrc->GetAsSapProvider ());
  newradioRrc->SetAsSapUser (lteNas->GetAsSapUser ());

  for (std::map<uint8_t, Ptr<ComponentCarrierUe> >::iterator it = lteUeCcMap.begin (); it != lteUeCcMap.end (); ++it)
    {
      lteRrc->SetLteUeCmacSapProvider (it->second->GetMac ()->GetLteUeCmacSapProvider (), it->first);
      it->second->GetMac ()->SetLteUeCmacSapUser (lteRrc->GetLteUeCmacSapUser (it->first));
      it->second->GetMac ()->SetComponentCarrierId (it->first);

      it->second->GetPhy ()->SetLteUeCphySapUser (lteRrc->GetLteUeCphySapUser (it->first));
      lteRrc->SetLteUeCphySapProvider (it->second->GetPhy ()->GetLteUeCphySapProvider (), it->first);
      it->second->GetPhy ()->SetComponentCarrierId (it->first);

      it->second->GetPhy ()->SetLteUePhySapUser (it->second->GetMac ()->GetLteUePhySapUser ());
      it->second->GetMac ()->SetLteUePhySapProvider (it->second->GetPhy ()->GetLteUePhySapProvider ());

      bool ccmTest = lteCcmUe->SetComponentCarrierMacSapProviders (it->first, it->second->GetMac ()->GetLteMacSapProvider ());

      if (ccmTest == false)
        {
          NS_FATAL_ERROR ("Error in SetComponentCarrierMacSapProviders");
        }
    }

  for (std::map<uint8_t, Ptr<newradioComponentCarrierUe> >::iterator it = newradioUeCcMap.begin (); it != newradioUeCcMap.end (); ++it)
    {
      lteRrc->SetnewradioUeCmacSapProvider (it->second->GetMac ()->GetUeCmacSapProvider (), it->first);
    }

  device->SetLteCcMap (lteUeCcMap);
  device->SetAttribute ("LteUeRrc", PointerValue (lteRrc));
  device->SetAttribute ("EpcUeNas", PointerValue (lteNas));
  device->SetAttribute ("LteUeComponentCarrierManager", PointerValue (lteCcmUe));
  device->SetAttribute ("Imsi", UintegerValue (imsi));

  for (std::map<uint8_t, Ptr<ComponentCarrierUe> >::iterator it = lteUeCcMap.begin (); it != lteUeCcMap.end (); ++it)
    {
      Ptr<LteUePhy> ccPhy = it->second->GetPhy ();
      ccPhy->SetDevice (device);
      ccPhy->GetUlSpectrumPhy ()->SetDevice (device);
      ccPhy->GetDlSpectrumPhy ()->SetDevice (device);
      ccPhy->GetDlSpectrumPhy ()->SetLtePhyRxDataEndOkCallback (MakeCallback (&LteUePhy::PhyPduReceived, ccPhy));
      ccPhy->GetDlSpectrumPhy ()->SetLtePhyRxCtrlEndOkCallback (MakeCallback (&LteUePhy::ReceiveLteControlMessageList, ccPhy));
      ccPhy->GetDlSpectrumPhy ()->SetLtePhyRxPssCallback (MakeCallback (&LteUePhy::ReceivePss, ccPhy));
      ccPhy->GetDlSpectrumPhy ()->SetLtePhyDlHarqFeedbackCallback (MakeCallback (&LteUePhy::ReceiveLteDlHarqFeedback, ccPhy));
    }

  lteNas->SetDevice (device);

  n->AddDevice (device);

  lteNas->SetForwardUpCallback (MakeCallback (&McUeNetDevice::Receive, device));

  if (m_epcHelper != 0)
    {
      m_epcHelper->AddUe (device, device->GetImsi ());
    }

  device->Initialize ();
  return device;
}

Ptr<NetDevice>
newradioHelper::InstallSingleInterRatHoCapableUeDevice (Ptr<Node> n)
{
  NS_LOG_FUNCTION (this);

  // Use a McUeNetDevice but install a single RRC
  Ptr<McUeNetDevice> device = m_mcUeNetDeviceFactory.Create<McUeNetDevice> ();
  NS_ABORT_MSG_IF (m_imsiCounter >= 0xFFFFFFFF, "max num UEs exceeded");
  /*
  uint64_t imsi = ++m_imsiCounter;

  // Phy part of newradio
  Ptr<newradioSpectrumPhy> newradioUlPhy = CreateObject<newradioSpectrumPhy> ();
  Ptr<newradioSpectrumPhy> newradioDlPhy = CreateObject<newradioSpectrumPhy> ();

  Ptr<newradioUePhy> newradioPhy = CreateObject<newradioUePhy> (newradioDlPhy, newradioUlPhy);

  Ptr<newradioHarqPhy> newradioHarq = Create<newradioHarqPhy> (m_phyMacCommon->GetNumHarqProcess ());

  newradioDlPhy->SetHarqPhyModule (newradioHarq);
  newradioPhy->SetHarqPhyModule (newradioHarq);

  Ptr<newradioChunkProcessor> newradiopData = Create<newradioChunkProcessor> ();
  newradiopData->AddCallback (MakeCallback (&newradioUePhy::GenerateDlCqiReport, newradioPhy));
  newradiopData->AddCallback (MakeCallback (&newradioSpectrumPhy::UpdateSinrPerceived, newradioDlPhy));
  newradioDlPhy->AddDataSinrChunkProcessor (newradiopData);
  if(m_harqEnabled)
  {
          newradioDlPhy->SetPhyDlHarqFeedbackCallback (MakeCallback (&newradioUePhy::ReceiveLteDlHarqFeedback, newradioPhy));
  }

  // hack to allow periodic computation of SINR at the eNB, without pilots
  if(m_channelModelType == "ns3::newradioBeamforming")
  {
          newradioPhy->AddSpectrumPropagationLossModel(m_beamforming);
  }
  else if(m_channelModelType == "ns3::newradioChannelMatrix")
  {
          newradioPhy->AddSpectrumPropagationLossModel(m_channelMatrix);
  }
  else if(m_channelModelType == "ns3::newradioChannelRaytracing")
  {
          newradioPhy->AddSpectrumPropagationLossModel(m_raytracing);
  }
  if (!m_pathlossModelType.empty ())
  {
          Ptr<PropagationLossModel> splm = m_pathlossModel->GetObject<PropagationLossModel> ();
          if( splm )
          {
                  newradioPhy->AddPropagationLossModel (splm);
          }
  }
  else
  {
          NS_LOG_UNCOND (this << " No PropagationLossModel!");
  }

  // Phy part of LTE
  Ptr<LteSpectrumPhy> lteDlPhy = CreateObject<LteSpectrumPhy> ();
  Ptr<LteSpectrumPhy> lteUlPhy = CreateObject<LteSpectrumPhy> ();

  Ptr<LteUePhy> ltePhy = CreateObject<LteUePhy> (lteDlPhy, lteUlPhy);

  Ptr<LteHarqPhy> lteHarq = Create<LteHarqPhy> ();
  lteDlPhy->SetHarqPhyModule (lteHarq);
  lteUlPhy->SetHarqPhyModule (lteHarq);
  ltePhy->SetHarqPhyModule (lteHarq);

  Ptr<LteChunkProcessor> pRs = Create<LteChunkProcessor> ();
  pRs->AddCallback (MakeCallback (&LteUePhy::ReportRsReceivedPower, ltePhy));
  lteDlPhy->AddRsPowerChunkProcessor (pRs);

  Ptr<LteChunkProcessor> pInterf = Create<LteChunkProcessor> ();
  pInterf->AddCallback (MakeCallback (&LteUePhy::ReportInterference, ltePhy));
  lteDlPhy->AddInterferenceCtrlChunkProcessor (pInterf); // for RSRQ evaluation of UE Measurements

  Ptr<LteChunkProcessor> pCtrl = Create<LteChunkProcessor> ();
  pCtrl->AddCallback (MakeCallback (&LteSpectrumPhy::UpdateSinrPerceived, lteDlPhy));
  lteDlPhy->AddCtrlSinrChunkProcessor (pCtrl);

  Ptr<LteChunkProcessor> pData = Create<LteChunkProcessor> ();
  pData->AddCallback (MakeCallback (&LteSpectrumPhy::UpdateSinrPerceived, lteDlPhy));
  lteDlPhy->AddDataSinrChunkProcessor (pData);

  if (m_usePdschForCqiGeneration)
  {
          // CQI calculation based on PDCCH for signal and PDSCH for interference
          pCtrl->AddCallback (MakeCallback (&LteUePhy::GenerateMixedCqiReport, ltePhy));
          Ptr<LteChunkProcessor> pDataInterf = Create<LteChunkProcessor> ();
          pDataInterf->AddCallback (MakeCallback (&LteUePhy::ReportDataInterference, ltePhy));
          lteDlPhy->AddInterferenceDataChunkProcessor (pDataInterf);
  }
  else
  {
          // CQI calculation based on PDCCH for both signal and interference
          pCtrl->AddCallback (MakeCallback (&LteUePhy::GenerateCtrlCqiReport, ltePhy));
  }

  // Set newradio channel
  newradioUlPhy->SetChannel(m_channel);
  newradioDlPhy->SetChannel(m_channel);
  // Set LTE channel
  lteUlPhy->SetChannel(m_uplinkChannel);
  lteDlPhy->SetChannel(m_downlinkChannel);

  Ptr<MobilityModel> mm = n->GetObject<MobilityModel> ();
  NS_ASSERT_MSG (mm, "MobilityModel needs to be set on node before calling newradioHelper::InstallUeDevice ()");
  newradioUlPhy->SetMobility(mm);
  newradioDlPhy->SetMobility(mm);
  lteUlPhy->SetMobility(mm);
  lteDlPhy->SetMobility(mm);

  // Antenna model for newradio and for LTE
  Ptr<AntennaModel> antenna = (m_ueAntennaModelFactory.Create ())->GetObject<AntennaModel> ();
  DynamicCast<AntennaArrayModel> (antenna)->SetPlanesNumber(m_noUePanels);
  DynamicCast<AntennaArrayModel> (antenna)->SetDeviceType(true);
  DynamicCast<AntennaArrayModel> (antenna)->SetTotNoArrayElements(m_noRxAntenna);
  NS_ASSERT_MSG (antenna, "error in creating the AntennaModel object");
  newradioUlPhy->SetAntenna (antenna);
  newradioDlPhy->SetAntenna (antenna);
  antenna = (m_lteUeAntennaModelFactory.Create ())->GetObject<AntennaModel> ();
  lteUlPhy->SetAntenna (antenna);
  lteDlPhy->SetAntenna (antenna);

  // ----------------------- newradio MAC and connections -------------
  Ptr<newradioUeMac> newradioMac = CreateObject<newradioUeMac> ();

  newradioPhy->SetConfigurationParameters (m_phyMacCommon);
  newradioMac->SetConfigurationParameters (m_phyMacCommon);
  newradioMac->SetAttribute("InterRatHoCapable", BooleanValue(true));

  newradioPhy->SetPhySapUser (newradioMac->GetPhySapUser());
  newradioMac->SetPhySapProvider (newradioPhy->GetPhySapProvider());

  device->SetNode(n);
  device->SetAttribute ("newradioUePhy", PointerValue(newradioPhy));
  device->SetAttribute ("newradioUeMac", PointerValue(newradioMac));

  newradioPhy->SetDevice (device);
  newradioPhy->SetImsi (imsi);
  //newradioPhy->SetForwardUpCallback (MakeCallback (&McUeNetDevice::Receive, device));
  newradioDlPhy->SetDevice(device);
  newradioUlPhy->SetDevice(device);

  newradioDlPhy->SetPhyRxDataEndOkCallback (MakeCallback (&newradioUePhy::PhyDataPacketReceived, newradioPhy));
  newradioDlPhy->SetPhyRxCtrlEndOkCallback (MakeCallback (&newradioUePhy::ReceiveControlMessageList, newradioPhy));

  // ----------------------- LTE stack ----------------------
  Ptr<LteUeMac> lteMac = CreateObject<LteUeMac> ();
  Ptr<LteUeRrc> rrc = CreateObject<LteUeRrc> (); //  single rrc

  if (m_useIdealRrc)
  {
          Ptr<newradioUeRrcProtocolIdeal> rrcProtocol = CreateObject<newradioUeRrcProtocolIdeal> ();
          rrcProtocol->SetUeRrc (rrc);
          rrc->AggregateObject (rrcProtocol);
          rrcProtocol->SetLteUeRrcSapProvider (rrc->GetLteUeRrcSapProvider ());
          rrc->SetLteUeRrcSapUser (rrcProtocol->GetLteUeRrcSapUser ());
  }
  else
  {
          Ptr<newradioLteUeRrcProtocolReal> rrcProtocol = CreateObject<newradioLteUeRrcProtocolReal> ();
          rrcProtocol->SetUeRrc (rrc);
          rrc->AggregateObject (rrcProtocol);
          rrcProtocol->SetLteUeRrcSapProvider (rrc->GetLteUeRrcSapProvider ());
          rrc->SetLteUeRrcSapUser (rrcProtocol->GetLteUeRrcSapUser ());
  }

  if (m_epcHelper != 0)
  {
          rrc->SetUseRlcSm (false);
  }

  Ptr<EpcUeNas> lteNas = CreateObject<EpcUeNas> ();

  lteNas->SetAsSapProvider (rrc->GetAsSapProvider ());
  rrc->SetAsSapUser (lteNas->GetAsSapUser ());

  // CMAC SAP
  lteMac->SetLteUeCmacSapUser (rrc->GetLteUeCmacSapUser ());
  newradioMac->SetUeCmacSapUser (rrc->GetLteUeCmacSapUser ());
  rrc->SetLteUeCmacSapProvider (lteMac->GetLteUeCmacSapProvider ());
  rrc->SetnewradioUeCmacSapProvider (newradioMac->GetUeCmacSapProvider());

  // CPHY SAP
  ltePhy->SetLteUeCphySapUser (rrc->GetLteUeCphySapUser ());
  newradioPhy->SetUeCphySapUser (rrc->GetLteUeCphySapUser ());
  rrc->SetLteUeCphySapProvider (ltePhy->GetLteUeCphySapProvider ());
  rrc->SetnewradioUeCphySapProvider (newradioPhy->GetUeCphySapProvider());

  // MAC SAP
  rrc->SetLteMacSapProvider (lteMac->GetLteMacSapProvider ());
  rrc->SetnewradioMacSapProvider (newradioMac->GetUeMacSapProvider());

  rrc->SetAttribute ("InterRatHoCapable", BooleanValue(true));

  ltePhy->SetLteUePhySapUser (lteMac->GetLteUePhySapUser ());
  lteMac->SetLteUePhySapProvider (ltePhy->GetLteUePhySapProvider ());

  device->SetAttribute ("LteUePhy", PointerValue (ltePhy));
  device->SetAttribute ("LteUeMac", PointerValue (lteMac));
  device->SetAttribute ("LteUeRrc", PointerValue (rrc));
  device->SetAttribute ("EpcUeNas", PointerValue (lteNas));
  device->SetAttribute ("Imsi", UintegerValue(imsi));

  ltePhy->SetDevice (device);
  lteDlPhy->SetDevice (device);
  lteUlPhy->SetDevice (device);
  lteNas->SetDevice (device);

  lteDlPhy->SetLtePhyRxDataEndOkCallback (MakeCallback (&LteUePhy::PhyPduReceived, ltePhy));
  lteDlPhy->SetLtePhyRxCtrlEndOkCallback (MakeCallback (&LteUePhy::ReceiveLteControlMessageList, ltePhy));
  lteDlPhy->SetLtePhyRxPssCallback (MakeCallback (&LteUePhy::ReceivePss, ltePhy));
  lteDlPhy->SetLtePhyDlHarqFeedbackCallback (MakeCallback (&LteUePhy::ReceiveLteDlHarqFeedback, ltePhy));
  lteNas->SetForwardUpCallback (MakeCallback (&McUeNetDevice::Receive, device));

  if (m_epcHelper != 0)
  {
          m_epcHelper->AddUe (device, device->GetImsi ());
  }

  n->AddDevice(device);
  device->Initialize();
  */

  return device;
}

Ptr<NetDevice>
newradioHelper::InstallSingleUeDevice (Ptr<Node> n)
{
  NS_LOG_FUNCTION (this);

  NS_ABORT_MSG_IF (m_componentCarrierPhyParams.size () == 0, "Before call this method you need to install Enbs --> InstallEnbDevice()");

  Ptr<newradioUeNetDevice> device = m_ueNetDeviceFactory.Create<newradioUeNetDevice> ();
  std::map<uint8_t, Ptr<newradioComponentCarrierUe> > ueCcMap;

  for (std::map< uint8_t, newradioComponentCarrier >::iterator it = m_componentCarrierPhyParams.begin (); it != m_componentCarrierPhyParams.end (); ++it)
    {
      Ptr <newradioComponentCarrierUe> cc =  CreateObject<newradioComponentCarrierUe> ();
      //cc->SetBandwidth ( it->second.GetBandwidth ());
      //cc->SetEarfcn ( it->second.GetUlEarfcn ());
      cc->SetConfigurationParameters (it->second.GetConfigurationParameters ());
      cc->SetAsPrimary (it->second.IsPrimary ());
      Ptr<newradioUeMac> mac = CreateObject<newradioUeMac> ();
      cc->SetMac (mac);
      // cc->GetPhy ()->Initialize (); // it is initialized within the LteUeNetDevice::DoInitialize ()
      ueCcMap.insert (std::pair<uint8_t, Ptr<newradioComponentCarrierUe> > (it->first, cc));
    }

  for (std::map<uint8_t, Ptr<newradioComponentCarrierUe> >::iterator it = ueCcMap.begin (); it != ueCcMap.end (); ++it)
    {
      Ptr<newradioSpectrumPhy> ulPhy = CreateObject<newradioSpectrumPhy> ();
      Ptr<newradioSpectrumPhy> dlPhy = CreateObject<newradioSpectrumPhy> ();

      Ptr<newradioUePhy> phy = CreateObject<newradioUePhy> (dlPhy, ulPhy);

      Ptr<newradioHarqPhy> harq = Create<newradioHarqPhy> (it->second->GetConfigurationParameters ()->GetNumHarqProcess ());

      dlPhy->SetHarqPhyModule (harq);
      //ulPhy->SetHarqPhyModule (harq);
      phy->SetHarqPhyModule (harq);

      /* Do not do this here. Do it during registration with the BS
       * phy->SetConfigurationParameters(m_phyMacCommon);*/

      if (m_channelModelType == "ns3::newradioBeamforming")
        {
          phy->AddSpectrumPropagationLossModel (m_beamforming.at (it->first));
        }
      else if (m_channelModelType == "ns3::newradioChannelMatrix")
        {
          phy->AddSpectrumPropagationLossModel (m_channelMatrix.at (it->first));
        }
      else if (m_channelModelType == "ns3::newradioChannelRaytracing")
        {
          phy->AddSpectrumPropagationLossModel (m_raytracing.at (it->first));
        }
      if (!m_pathlossModelType.empty ())
        {
          Ptr<PropagationLossModel> splm = m_pathlossModel.at (it->first)->GetObject<PropagationLossModel> ();
          if ( splm )
            {
              phy->AddPropagationLossModel (splm);
            }
        }
      else
        {
          NS_LOG_UNCOND (this << " No PropagationLossModel!");
        }

      /*
Ptr<LteChunkProcessor> pRs = Create<LteChunkProcessor> ();
pRs->AddCallback (MakeCallback (&LteUePhy::ReportRsReceivedPower, phy));
dlPhy->AddRsPowerChunkProcessor (pRs);

Ptr<LteChunkProcessor> pInterf = Create<LteChunkProcessor> ();
pInterf->AddCallback (MakeCallback (&LteUePhy::ReportInterference, phy));
dlPhy->AddInterferenceCtrlChunkProcessor (pInterf);   // for RSRQ evaluation of UE Measurements

Ptr<LteChunkProcessor> pCtrl = Create<LteChunkProcessor> ();
pCtrl->AddCallback (MakeCallback (&LteSpectrumPhy::UpdateSinrPerceived, dlPhy));
dlPhy->AddCtrlSinrChunkProcessor (pCtrl);
      */

      Ptr<newradioChunkProcessor> pData = Create<newradioChunkProcessor> ();
      pData->AddCallback (MakeCallback (&newradioUePhy::GenerateDlCqiReport, phy));
      pData->AddCallback (MakeCallback (&newradioSpectrumPhy::UpdateSinrPerceived, dlPhy));
      dlPhy->AddDataSinrChunkProcessor (pData);
      if (m_harqEnabled)
        {
          //In lte-helper this is done in the last for cycle
          dlPhy->SetPhyDlHarqFeedbackCallback (MakeCallback (&newradioUePhy::ReceiveLteDlHarqFeedback, phy));
        }

      /*Check if this is supported in newradio
if (m_usePdschForCqiGeneration)
{
// CQI calculation based on PDCCH for signal and PDSCH for interference
pCtrl->AddCallback (MakeCallback (&LteUePhy::GenerateMixedCqiReport, phy));
Ptr<LteChunkProcessor> pDataInterf = Create<LteChunkProcessor> ();
pDataInterf->AddCallback (MakeCallback (&LteUePhy::ReportDataInterference, phy));
dlPhy->AddInterferenceDataChunkProcessor (pDataInterf);
}
else
{
// CQI calculation based on PDCCH for both signal and interference
pCtrl->AddCallback (MakeCallback (&LteUePhy::GenerateCtrlCqiReport, phy));
}*/

      ulPhy->SetChannel (m_channel.at (it->first));
      dlPhy->SetChannel (m_channel.at (it->first));

      Ptr<MobilityModel> mm = n->GetObject<MobilityModel> ();
      NS_ASSERT_MSG (mm, "MobilityModel needs to be set on node before calling LteHelper::InstallUeDevice ()");
      dlPhy->SetMobility (mm);
      ulPhy->SetMobility (mm);

      Ptr<AntennaModel> antenna = (m_ueAntennaModelFactory.Create ())->GetObject<AntennaModel> ();
      DynamicCast<AntennaArrayModel> (antenna)->SetPlanesNumber (m_noUePanels);
      DynamicCast<AntennaArrayModel> (antenna)->SetDeviceType (true);
      NS_LOG_INFO("UE device->GetAntennaNum() " << device->GetAntennaNum());
      DynamicCast<AntennaArrayModel> (antenna)->SetTotNoArrayElements (device->GetAntennaNum());
      NS_ASSERT_MSG (antenna, "error in creating the AntennaModel object");
      dlPhy->SetAntenna (antenna);
      ulPhy->SetAntenna (antenna);

      it->second->SetPhy (phy);
    }

  Ptr<LteUeComponentCarrierManager> ccmUe = m_ueComponentCarrierManagerFactory.Create<LteUeComponentCarrierManager> ();

  Ptr<LteUeRrc> rrc = CreateObject<LteUeRrc> ();
  rrc->SetLteMacSapProvider (ccmUe->GetLteMacSapProvider ());
  // setting ComponentCarrierManager SAP
  rrc->SetLteCcmRrcSapProvider (ccmUe->GetLteCcmRrcSapProvider ());
  ccmUe->SetLteCcmRrcSapUser (rrc->GetLteCcmRrcSapUser ());
  // Set number of component carriers. Note: UE CCM would also set the
  // number of component carriers in UE RRC
  ccmUe->SetNumberOfComponentCarriers (m_noOfCcs);

  // run intializeSap to create the proper number of sap provider/users
  rrc->InitializeSap ();

  if (m_useIdealRrc)
    {
      Ptr<newradioUeRrcProtocolIdeal> rrcProtocol = CreateObject<newradioUeRrcProtocolIdeal> ();
      rrcProtocol->SetUeRrc (rrc);
      rrc->AggregateObject (rrcProtocol);
      rrcProtocol->SetLteUeRrcSapProvider (rrc->GetLteUeRrcSapProvider ());
      rrc->SetLteUeRrcSapUser (rrcProtocol->GetLteUeRrcSapUser ());
    }
  else
    {
      Ptr<newradioLteUeRrcProtocolReal> rrcProtocol = CreateObject<newradioLteUeRrcProtocolReal> ();
      rrcProtocol->SetUeRrc (rrc);
      rrc->AggregateObject (rrcProtocol);
      rrcProtocol->SetLteUeRrcSapProvider (rrc->GetLteUeRrcSapProvider ());
      rrc->SetLteUeRrcSapUser (rrcProtocol->GetLteUeRrcSapUser ());
    }

  if (m_epcHelper != 0)
    {
      rrc->SetUseRlcSm (false);
    }
  else
    {
      rrc->SetUseRlcSm (true);
    }

  Ptr<EpcUeNas> nas = CreateObject<EpcUeNas> ();

  nas->SetAsSapProvider (rrc->GetAsSapProvider ());
  rrc->SetAsSapUser (nas->GetAsSapUser ());

  for (std::map<uint8_t, Ptr<newradioComponentCarrierUe> >::iterator it = ueCcMap.begin (); it != ueCcMap.end (); ++it)
    {
      rrc->SetLteUeCmacSapProvider (it->second->GetMac ()->GetUeCmacSapProvider (), it->first);
      it->second->GetMac ()->SetUeCmacSapUser (rrc->GetLteUeCmacSapUser (it->first));
      it->second->GetMac ()->SetComponentCarrierId (it->first);

      it->second->GetPhy ()->SetUeCphySapUser (rrc->GetLteUeCphySapUser (it->first));
      rrc->SetLteUeCphySapProvider (it->second->GetPhy ()->GetUeCphySapProvider (), it->first);
      it->second->GetPhy ()->SetComponentCarrierId (it->first);

      it->second->GetPhy ()->SetPhySapUser (it->second->GetMac ()->GetPhySapUser ());
      it->second->GetMac ()->SetPhySapProvider (it->second->GetPhy ()->GetPhySapProvider ());

      it->second->GetPhy ()->SetConfigurationParameters (it->second->GetConfigurationParameters ());
      it->second->GetMac ()->SetConfigurationParameters (it->second->GetConfigurationParameters ());

      bool ccmTest = ccmUe->SetComponentCarrierMacSapProviders (it->first, it->second->GetMac ()->GetUeMacSapProvider ());

      if (ccmTest == false)
        {
          NS_FATAL_ERROR ("Error in SetComponentCarrierMacSapProviders");
        }
    }

  NS_ABORT_MSG_IF (m_imsiCounter >= 0xFFFFFFFF, "max num UEs exceeded");
  uint64_t imsi = ++m_imsiCounter;

  device->SetNode (n);
  device->SetAttribute ("Imsi", UintegerValue (imsi));
  //device->SetAttribute ("newradioUePhy", PointerValue(phy));
  //device->SetAttribute ("newradioUeMac", PointerValue(mac));
  device->SetCcMap (ueCcMap);
  device->SetAttribute ("EpcUeNas", PointerValue (nas));
  device->SetAttribute ("newradioUeRrc", PointerValue (rrc));
  device->SetAttribute ("LteUeComponentCarrierManager", PointerValue (ccmUe));

  for (std::map<uint8_t, Ptr<newradioComponentCarrierUe> >::iterator it = ueCcMap.begin (); it != ueCcMap.end (); ++it)
    {
      Ptr<newradioUePhy> ccPhy = it->second->GetPhy ();
      ccPhy->SetDevice (device);
      ccPhy->SetImsi (imsi);
      ccPhy->GetUlSpectrumPhy ()->SetDevice (device);
      ccPhy->GetDlSpectrumPhy ()->SetDevice (device);
      ccPhy->GetDlSpectrumPhy ()->SetPhyRxDataEndOkCallback (MakeCallback (&newradioUePhy::PhyDataPacketReceived, ccPhy));
      ccPhy->GetDlSpectrumPhy ()->SetPhyRxCtrlEndOkCallback (MakeCallback (&newradioUePhy::ReceiveControlMessageList, ccPhy));
      //ccPhy->GetDlSpectrumPhy ()->SetLtePhyRxPssCallback (MakeCallback (&LteUePhy::ReceivePss, ccPhy));
      //ccPhy->GetDlSpectrumPhy ()->SetLtePhyDlHarqFeedbackCallback (MakeCallback (&LteUePhy::ReceiveLteDlHarqFeedback, ccPhy)); this is done before
    }

  nas->SetDevice (device);

  n->AddDevice (device);

  nas->SetForwardUpCallback (MakeCallback (&newradioUeNetDevice::Receive, device));

  if (m_epcHelper != 0)
    {
      m_epcHelper->AddUe (device, device->GetImsi ());
    }

  device->Initialize ();

  return device;
}

Ptr<NetDevice>
newradioHelper::InstallSingleEnbDevice (Ptr<Node> n)
{
  //NS_ABORT_MSG_IF (m_cellIdCounter == 65535, "max num eNBs exceeded");
  uint16_t cellId = m_cellIdCounter;       //TODO remove, eNB has no cellId

  //Before calling InstallEnbDevice:
  //1) create a std::map where the key is index of
  //component carrier starting from 0, where 0 refers to PCC. The value is
  //an instance of newradioComponentCarrier which contains the attribute members for the
  //configuration of the phy paramenters
  //2) call SetCcPhyParams
  NS_ASSERT_MSG (m_componentCarrierPhyParams.size () != 0, "Cannot create enb ccm map. Call SetCcPhyParams first.");
  Ptr<newradioEnbNetDevice> device = m_enbNetDeviceFactory.Create<newradioEnbNetDevice> ();

  // create component carrier map for this eNb device
  std::map<uint8_t,Ptr<newradioComponentCarrierEnb> > ccMap;
  for (std::map<uint8_t, newradioComponentCarrier >::iterator it = m_componentCarrierPhyParams.begin (); it != m_componentCarrierPhyParams.end (); ++it)
    {
      Ptr <newradioComponentCarrierEnb> cc =  CreateObject<newradioComponentCarrierEnb> ();
      //cc->SetBandwidth(it->second.GetBandwidth());
      //cc->SetEarfcn(it->second.GetEarfcn());
      cc->SetConfigurationParameters (it->second.GetConfigurationParameters ());
      cc->SetAsPrimary (it->second.IsPrimary ());
      NS_ABORT_MSG_IF (m_cellIdCounter == 65535, "max num cells exceeded");
      cc->SetCellId (m_cellIdCounter++);
      ccMap [it->first] =  cc;
    }
  NS_ABORT_MSG_IF (m_useCa && ccMap.size () < 2, "You have to either specify carriers or disable carrier aggregation");
  NS_ASSERT (ccMap.size () == m_noOfCcs);

  for (std::map<uint8_t,Ptr<newradioComponentCarrierEnb> >::iterator it = ccMap.begin (); it != ccMap.end (); ++it)
    {
      NS_LOG_DEBUG (this << "component carrier map size " << (uint16_t) ccMap.size ());
      Ptr<newradioSpectrumPhy> ulPhy = CreateObject<newradioSpectrumPhy> ();
      Ptr<newradioSpectrumPhy> dlPhy = CreateObject<newradioSpectrumPhy> ();

      Ptr<newradioEnbPhy> phy = CreateObject<newradioEnbPhy> (dlPhy, ulPhy);
      //NS_LOG_UNCOND("CC " << cellId << " newradioSpectrumPhy " << dlPhy);

      Ptr<newradioHarqPhy> harq = Create<newradioHarqPhy> (it->second->GetConfigurationParameters ()->GetNumHarqProcess ());
      dlPhy->SetHarqPhyModule (harq);
      //	ulPhy->SetHarqPhyModule (harq);
      phy->SetHarqPhyModule (harq);

      Ptr<newradioChunkProcessor> pData = Create<newradioChunkProcessor> ();
      if (!m_snrTest)
        {
          pData->AddCallback (MakeCallback (&newradioEnbPhy::GenerateDataCqiReport, phy));
          pData->AddCallback (MakeCallback (&newradioSpectrumPhy::UpdateSinrPerceived, dlPhy));
        }
      dlPhy->AddDataSinrChunkProcessor (pData);

      phy->SetConfigurationParameters (it->second->GetConfigurationParameters ());

      ulPhy->SetChannel (m_channel.at (it->first));
      dlPhy->SetChannel (m_channel.at (it->first));

      Ptr<MobilityModel> mm = n->GetObject<MobilityModel> ();
      NS_ASSERT_MSG (mm, "MobilityModel needs to be set on node before calling newradioHelper::InstallEnbDevice ()");
      ulPhy->SetMobility (mm);
      dlPhy->SetMobility (mm);

      // hack to allow periodic computation of SINR at the eNB, without pilots
      if (m_channelModelType == "ns3::newradioBeamforming")
        {
          phy->AddSpectrumPropagationLossModel (m_beamforming.at (it->first));
        }
      else if (m_channelModelType == "ns3::newradioChannelMatrix")
        {
          phy->AddSpectrumPropagationLossModel (m_channelMatrix.at (it->first));
        }
      else if (m_channelModelType == "ns3::newradioChannelRaytracing")
        {
          phy->AddSpectrumPropagationLossModel (m_raytracing.at (it->first));
        }
      else if (m_channelModelType == "ns3::newradio3gppChannel")
        {
          phy->AddSpectrumPropagationLossModel (m_3gppChannel.at (it->first));
        }
      if (!m_pathlossModelType.empty ())
        {
          Ptr<PropagationLossModel> splm = m_pathlossModel.at (it->first)->GetObject<PropagationLossModel> ();
          if ( splm )
            {
              phy->AddPropagationLossModel (splm);
              if (m_losTracker.find (it->first) != m_losTracker.end ())
                {
                  phy->AddLosTracker (m_losTracker.at (it->first));                             // use m_losTracker in phy (and in particular in enbPhy)
                }
            }
        }
      else
        {
          NS_LOG_UNCOND (this << " No PropagationLossModel!");
        }

      /* Antenna model */
      Ptr<AntennaModel> antenna = (m_enbAntennaModelFactory.Create ())->GetObject<AntennaModel> ();
      DynamicCast<AntennaArrayModel> (antenna)->SetPlanesNumber (m_noEnbPanels);
      DynamicCast<AntennaArrayModel> (antenna)->SetDeviceType (false);
      NS_LOG_INFO("eNB device->GetAntennaNum() " << device->GetAntennaNum());
      DynamicCast<AntennaArrayModel> (antenna)->SetTotNoArrayElements (device->GetAntennaNum());      
      NS_ASSERT_MSG (antenna, "error in creating the AntennaModel object");
      dlPhy->SetAntenna (antenna);
      ulPhy->SetAntenna (antenna);

      Ptr<newradioEnbMac> mac = CreateObject<newradioEnbMac> ();
      mac->SetConfigurationParameters (it->second->GetConfigurationParameters ());
      Ptr<newradioMacScheduler> sched = m_schedulerFactory.Create<newradioMacScheduler> ();

      /*to use the dummy ffrAlgorithm, I changed the bandwidth to 25 in EnbNetDevice
      m_ffrAlgorithmFactory = ObjectFactory ();
      m_ffrAlgorithmFactory.SetTypeId ("ns3::LteFrNoOpAlgorithm");
      Ptr<LteFfrAlgorithm> ffrAlgorithm = m_ffrAlgorithmFactory.Create<LteFfrAlgorithm> ();
      */
      sched->ConfigureCommonParameters (it->second->GetConfigurationParameters ());

      /**********************************************************
      //To do later?
      *mac->SetnewradioMacSchedSapProvider(sched->GetMacSchedSapProvider());
      *sched->SetMacSchedSapUser (mac->GetnewradioMacSchedSapUser());
      *mac->SetnewradioMacCschedSapProvider(sched->GetMacCschedSapProvider());
      *sched->SetMacCschedSapUser (mac->GetnewradioMacCschedSapUser());

      *phy->SetPhySapUser (mac->GetPhySapUser());
      *mac->SetPhySapProvider (phy->GetPhySapProvider());
      *************************************************************/

      it->second->SetMac (mac);
      it->second->SetMacScheduler (sched);

      it->second->SetPhy (phy);
    }

  Ptr<LteEnbRrc> rrc = CreateObject<LteEnbRrc> ();
  Ptr<LteEnbComponentCarrierManager> ccmEnbManager = m_enbComponentCarrierManagerFactory.Create<LteEnbComponentCarrierManager> ();

  //ComponentCarrierManager SAP
  rrc->SetLteCcmRrcSapProvider (ccmEnbManager->GetLteCcmRrcSapProvider ());
  ccmEnbManager->SetLteCcmRrcSapUser (rrc->GetLteCcmRrcSapUser ());
  // Set number of component carriers. Note: eNB CCM would also set the
  // number of component carriers in eNB RRC
  ccmEnbManager->SetNumberOfComponentCarriers (m_noOfCcs);

  // create the newradioComponentCarrierConf map used for the RRC setup
  std::map<uint8_t, LteEnbRrc::newradioComponentCarrierConf> ccConfMap;
  for (std::map<uint8_t,Ptr<newradioComponentCarrierEnb> >::iterator it = ccMap.begin (); it != ccMap.end (); ++it)
    {
      LteEnbRrc::newradioComponentCarrierConf ccConf;
      ccConf.m_ccId = it->second->GetConfigurationParameters ()->GetCcId ();
      ccConf.m_cellId = it->second->GetCellId ();
      ccConf.m_bandwidth = it->second->GetBandwidth ();

      ccConfMap[it->first] = ccConf;
    }
  rrc->ConfigurenewradioCarriers (ccConfMap);

  std::map<uint8_t, double> bandwidthMap;
  for (std::map<uint8_t,Ptr<newradioComponentCarrierEnb> >::iterator it = ccMap.begin (); it != ccMap.end (); ++it)
    {
      Ptr<newradioPhyMacCommon> phyMacConfig = it->second->GetConfigurationParameters ();
      bandwidthMap[it->first] = phyMacConfig->GetNumRb () * phyMacConfig->GetChunkWidth () * phyMacConfig->GetNumChunkPerRb ();
      NS_LOG_UNCOND ("bandwidth " << (uint32_t)it->first << " = " << bandwidthMap[it->first]);
    }

  ccmEnbManager->SetBandwidthMap (bandwidthMap);

  if (m_useIdealRrc)
    {
      Ptr<newradioEnbRrcProtocolIdeal> rrcProtocol = CreateObject<newradioEnbRrcProtocolIdeal> ();
      rrcProtocol->SetLteEnbRrcSapProvider (rrc->GetLteEnbRrcSapProvider ());
      rrc->SetLteEnbRrcSapUser (rrcProtocol->GetLteEnbRrcSapUser ());
      rrc->AggregateObject (rrcProtocol);
      rrcProtocol->SetCellId (cellId);
    }
  else
    {
      Ptr<newradioLteEnbRrcProtocolReal> rrcProtocol = CreateObject<newradioLteEnbRrcProtocolReal> ();
      rrcProtocol->SetLteEnbRrcSapProvider (rrc->GetLteEnbRrcSapProvider ());
      rrc->SetLteEnbRrcSapUser (rrcProtocol->GetLteEnbRrcSapUser ());
      rrc->AggregateObject (rrcProtocol);
      rrcProtocol->SetCellId (cellId);
    }

  if (m_epcHelper != 0)
    {
      EnumValue epsBearerToRlcMapping;
      rrc->GetAttribute ("EpsBearerToRlcMapping", epsBearerToRlcMapping);
      // it does not make sense to use RLC/SM when also using the EPC
      if (epsBearerToRlcMapping.Get () == LteEnbRrc::RLC_SM_ALWAYS)
        {
          if (m_rlcAmEnabled)
            {
              rrc->SetAttribute ("EpsBearerToRlcMapping", EnumValue (LteEnbRrc::RLC_AM_ALWAYS));
            }
          else
            {
              rrc->SetAttribute ("EpsBearerToRlcMapping", EnumValue (LteEnbRrc::RLC_UM_LOWLAT_ALWAYS));
            }
        }
    }

  rrc->SetAttribute ("newradioDevice", BooleanValue (true));

  // This RRC attribute is used to connect each new RLC instance with the MAC layer
  // (for function such as TransmitPdu, ReportBufferStatusReport).
  // Since in this new architecture, the component carrier manager acts a proxy, it
  // will have its own LteMacSapProvider interface, RLC will see it as through original MAC
  // interface LteMacSapProvider, but the function call will go now through LteEnbComponentCarrierManager
  // instance that needs to implement functions of this interface, and its task will be to
  // forward these calls to the specific MAC of some of the instances of component carriers. This
  // decision will depend on the specific implementation of the component carrier manager.
  rrc->SetLteMacSapProvider (ccmEnbManager->GetLteMacSapProvider ());

  bool ccmTest;
  for (std::map<uint8_t,Ptr<newradioComponentCarrierEnb> >::iterator it = ccMap.begin (); it != ccMap.end (); ++it)
    {
      it->second->GetPhy ()->SetnewradioEnbCphySapUser (rrc->GetLteEnbCphySapUser (it->first));
      rrc->SetLteEnbCphySapProvider (it->second->GetPhy ()->GetnewradioEnbCphySapProvider (), it->first);

      rrc->SetLteEnbCmacSapProvider (it->second->GetMac ()->GetEnbCmacSapProvider (),it->first );
      it->second->GetMac ()->SetEnbCmacSapUser (rrc->GetLteEnbCmacSapUser (it->first));

      it->second->GetPhy ()->SetComponentCarrierId (it->first);
      it->second->GetMac ()->SetComponentCarrierId (it->first);
      //FFR SAP
      /* not used in newradio
it->second->GetFfMacScheduler ()->SetLteFfrSapProvider (it->second->GetFfrAlgorithm ()->GetLteFfrSapProvider ());
it->second->GetFfrAlgorithm ()->SetLteFfrSapUser (it->second->GetFfMacScheduler ()->GetLteFfrSapUser ());
rrc->SetLteFfrRrcSapProvider (it->second->GetFfrAlgorithm ()->GetLteFfrRrcSapProvider (), it->first);
it->second->GetFfrAlgorithm ()->SetLteFfrRrcSapUser (rrc->GetLteFfrRrcSapUser (it->first));
//FFR SAP END*/

      // PHY <--> MAC SAP
      it->second->GetPhy ()->SetPhySapUser (it->second->GetMac ()->GetPhySapUser ());
      it->second->GetMac ()->SetPhySapProvider (it->second->GetPhy ()->GetPhySapProvider ());
      // PHY <--> MAC SAP END

      //Scheduler SAP
      it->second->GetMac ()->SetnewradioMacSchedSapProvider (it->second->GetMacScheduler ()->GetMacSchedSapProvider ());
      it->second->GetMac ()->SetnewradioMacCschedSapProvider (it->second->GetMacScheduler ()->GetMacCschedSapProvider ());

      it->second->GetMacScheduler ()->SetMacSchedSapUser (it->second->GetMac ()->GetnewradioMacSchedSapUser ());
      it->second->GetMacScheduler ()->SetMacCschedSapUser (it->second->GetMac ()->GetnewradioMacCschedSapUser ());
      // Scheduler SAP END

      it->second->GetMac ()->SetLteCcmMacSapUser (ccmEnbManager->GetLteCcmMacSapUser ());
      ccmEnbManager->SetCcmMacSapProviders (it->first, it->second->GetMac ()->GetLteCcmMacSapProvider ());

      // insert the pointer to the LteMacSapProvider interface of the MAC layer of the specific component carrier
      ccmTest = ccmEnbManager->SetMacSapProvider (it->first, it->second->GetMac ()->GetUeMacSapProvider ());

      if (ccmTest == false)
        {
          NS_FATAL_ERROR ("Error in SetComponentCarrierMacSapProviders");
        }
    }

  device->SetNode (n);
  device->SetAttribute ("CellId", UintegerValue (cellId));
  device->SetAttribute ("LteEnbComponentCarrierManager", PointerValue (ccmEnbManager));
  device->SetCcMap (ccMap);

  //device->SetAttribute ("newradioEnbPhy", PointerValue (phy));
  //device->SetAttribute ("newradioEnbMac", PointerValue (mac));
  //device->SetAttribute ("newradioScheduler", PointerValue(sched));
  device->SetAttribute ("LteEnbRrc", PointerValue (rrc));

  /*to do for each cc
        *phy->SetDevice (device);
        *dlPhy->SetDevice (device);
        dlPhy->SetCellId (cellId);
        *ulPhy->SetDevice (device);
        *n->AddDevice (device);

        mac->SetCellId(cellId);
        *dlPhy->SetPhyRxDataEndOkCallback (MakeCallback (&newradioEnbPhy::PhyDataPacketReceived, phy));
        *dlPhy->SetPhyRxCtrlEndOkCallback (MakeCallback (&newradioEnbPhy::PhyCtrlMessagesReceived, phy));
  *	dlPhy->SetPhyUlHarqFeedbackCallback (MakeCallback (&newradioEnbPhy::ReceiveUlHarqFeedback, phy));
*/
  for (std::map<uint8_t,Ptr<newradioComponentCarrierEnb> >::iterator it = ccMap.begin (); it != ccMap.end (); ++it)
    {
      Ptr<newradioEnbPhy> ccPhy = it->second->GetPhy ();
      ccPhy->SetDevice (device);
      ccPhy->GetUlSpectrumPhy ()->SetDevice (device);
      ccPhy->GetDlSpectrumPhy ()->SetDevice (device);
      ccPhy->GetDlSpectrumPhy ()->SetPhyRxDataEndOkCallback (MakeCallback (&newradioEnbPhy::PhyDataPacketReceived, ccPhy));
      ccPhy->GetDlSpectrumPhy ()->SetPhyRxCtrlEndOkCallback (MakeCallback (&newradioEnbPhy::PhyCtrlMessagesReceived, ccPhy));
      ccPhy->GetDlSpectrumPhy ()->SetPhyUlHarqFeedbackCallback (MakeCallback (&newradioEnbPhy::ReceiveUlHarqFeedback, ccPhy));
      NS_LOG_LOGIC ("set the propagation model frequencies");
      //double freq = LteSpectrumValueHelper::GetCarrierFrequency (it->second->m_dlEarfcn);
      double freq = it->second->GetCenterFrequency ();

      NS_LOG_LOGIC ("Channel Frequency: " << freq);
      if (!m_pathlossModelType.empty ())
        {
          bool freqOk = m_pathlossModel.at (it->first)->SetAttributeFailSafe ("Frequency", DoubleValue (freq));
          if (!freqOk)
            {
              NS_LOG_WARN ("Propagation model does not have a Frequency attribute");
            }
        }
    }              //end for

  //mac->SetForwardUpCallback (MakeCallback (&newradioEnbNetDevice::Receive, device));
  rrc->SetForwardUpCallback (MakeCallback (&newradioEnbNetDevice::Receive, device));

  /* to do for each cc (see for above)
  NS_LOG_LOGIC ("set the propagation model frequencies");
  double freq = m_phyMacCommon->GetCenterFrequency ();
  NS_LOG_LOGIC ("Channel Frequency: " << freq);
  if (!m_pathlossModelType.empty ())
  {
          bool freqOk = m_pathlossModel->SetAttributeFailSafe ("Frequency", DoubleValue (freq));
          if (!freqOk)
          {
                  NS_LOG_WARN ("Propagation model does not have a Frequency attribute");
          }
  }*/

  device->Initialize ();
  n->AddDevice (device);

  for (std::map<uint8_t,Ptr<newradioComponentCarrierEnb> >::iterator it = ccMap.begin (); it != ccMap.end (); ++it)
    {
      //m_channel->AddRx (dlPhy); substitute
      m_channel.at (it->first)->AddRx (it->second->GetPhy ()->GetDlSpectrumPhy ()); //TODO check if Dl and Ul are the same
    }


  if (m_epcHelper != 0)
    {
      NS_LOG_INFO ("adding this eNB to the EPC");
      m_epcHelper->AddEnb (n, device, device->GetCellId ());
      Ptr<EpcEnbApplication> enbApp = n->GetApplication (0)->GetObject<EpcEnbApplication> ();
      NS_ASSERT_MSG (enbApp != 0, "cannot retrieve EpcEnbApplication");

      // S1 SAPs
      rrc->SetS1SapProvider (enbApp->GetS1SapProvider ());
      enbApp->SetS1SapUser (rrc->GetS1SapUser ());

      // X2 SAPs
      Ptr<EpcX2> x2 = n->GetObject<EpcX2> ();
      x2->SetEpcX2SapUser (rrc->GetEpcX2SapUser ());
      rrc->SetEpcX2SapProvider (x2->GetEpcX2SapProvider ());
      rrc->SetEpcX2RlcProvider (x2->GetEpcX2RlcProvider ());

    }

  return device;
}


Ptr<NetDevice>
newradioHelper::InstallSingleLteEnbDevice (Ptr<Node> n)
{
  uint16_t cellId = m_cellIdCounter;       // \todo Remove, eNB has no cell ID

  Ptr<LteEnbNetDevice> dev = m_lteEnbNetDeviceFactory.Create<LteEnbNetDevice> ();
  Ptr<LteHandoverAlgorithm> handoverAlgorithm = m_lteHandoverAlgorithmFactory.Create<LteHandoverAlgorithm> ();

  NS_ASSERT_MSG (m_lteComponentCarrierPhyParams.size () != 0, "Cannot create enb ccm map.");
  // create component carrier map for this eNb device
  std::map<uint8_t,Ptr<ComponentCarrierEnb> > ccMap;
  for (std::map<uint8_t, ComponentCarrier >::iterator it = m_lteComponentCarrierPhyParams.begin (); it != m_lteComponentCarrierPhyParams.end (); ++it)
    {
      Ptr <ComponentCarrierEnb> cc =  CreateObject<ComponentCarrierEnb> ();
      cc->SetUlBandwidth (it->second.GetUlBandwidth ());
      cc->SetDlBandwidth (it->second.GetDlBandwidth ());
      cc->SetDlEarfcn (it->second.GetDlEarfcn ());
      cc->SetUlEarfcn (it->second.GetUlEarfcn ());
      cc->SetAsPrimary (it->second.IsPrimary ());
      NS_ABORT_MSG_IF (m_cellIdCounter == 65535, "max num cells exceeded");
      cc->SetCellId (m_cellIdCounter++);
      ccMap [it->first] =  cc;
    }
  NS_ABORT_MSG_IF (m_lteUseCa && ccMap.size () < 2, "You have to either specify carriers or disable carrier aggregation");
  NS_ASSERT (ccMap.size () == m_noOfLteCcs);

  for (std::map<uint8_t,Ptr<ComponentCarrierEnb> >::iterator it = ccMap.begin (); it != ccMap.end (); ++it)
    {
      NS_LOG_DEBUG (this << "component carrier map size " << (uint16_t) ccMap.size ());
      Ptr<LteSpectrumPhy> dlPhy = CreateObject<LteSpectrumPhy> ();
      Ptr<LteSpectrumPhy> ulPhy = CreateObject<LteSpectrumPhy> ();
      Ptr<LteEnbPhy> phy = CreateObject<LteEnbPhy> (dlPhy, ulPhy);

      Ptr<LteHarqPhy> harq = Create<LteHarqPhy> ();
      dlPhy->SetHarqPhyModule (harq);
      ulPhy->SetHarqPhyModule (harq);
      phy->SetHarqPhyModule (harq);

      Ptr<LteChunkProcessor> pCtrl = Create<LteChunkProcessor> ();
      pCtrl->AddCallback (MakeCallback (&LteEnbPhy::GenerateCtrlCqiReport, phy));
      ulPhy->AddCtrlSinrChunkProcessor (pCtrl);   // for evaluating SRS UL-CQI

      Ptr<LteChunkProcessor> pData = Create<LteChunkProcessor> ();
      pData->AddCallback (MakeCallback (&LteEnbPhy::GenerateDataCqiReport, phy));
      pData->AddCallback (MakeCallback (&LteSpectrumPhy::UpdateSinrPerceived, ulPhy));
      ulPhy->AddDataSinrChunkProcessor (pData);   // for evaluating PUSCH UL-CQI

      Ptr<LteChunkProcessor> pInterf = Create<LteChunkProcessor> ();
      pInterf->AddCallback (MakeCallback (&LteEnbPhy::ReportInterference, phy));
      ulPhy->AddInterferenceDataChunkProcessor (pInterf);   // for interference power tracing

      dlPhy->SetChannel (m_downlinkChannel);
      ulPhy->SetChannel (m_uplinkChannel);

      Ptr<MobilityModel> mm = n->GetObject<MobilityModel> ();
      NS_ASSERT_MSG (mm, "MobilityModel needs to be set on node before calling LteHelper::InstallEnbDevice ()");
      dlPhy->SetMobility (mm);
      ulPhy->SetMobility (mm);

      Ptr<AntennaModel> antenna = (m_lteEnbAntennaModelFactory.Create ())->GetObject<AntennaModel> ();
      NS_ASSERT_MSG (antenna, "error in creating the AntennaModel object");
      dlPhy->SetAntenna (antenna);
      ulPhy->SetAntenna (antenna);

      Ptr<LteEnbMac> mac = CreateObject<LteEnbMac> ();
      Ptr<FfMacScheduler> sched = m_lteSchedulerFactory.Create<FfMacScheduler> ();
      Ptr<LteFfrAlgorithm> ffrAlgorithm = m_lteFfrAlgorithmFactory.Create<LteFfrAlgorithm> ();
      it->second->SetMac (mac);
      it->second->SetFfMacScheduler (sched);
      it->second->SetFfrAlgorithm (ffrAlgorithm);

      it->second->SetPhy (phy);

    }

  Ptr<LteEnbRrc> rrc = CreateObject<LteEnbRrc> ();
  Ptr<LteEnbComponentCarrierManager> ccmEnbManager = m_lteEnbComponentCarrierManagerFactory.Create<LteEnbComponentCarrierManager> ();

  //ComponentCarrierManager SAP
  rrc->SetLteCcmRrcSapProvider (ccmEnbManager->GetLteCcmRrcSapProvider ());
  ccmEnbManager->SetLteCcmRrcSapUser (rrc->GetLteCcmRrcSapUser ());
  // Set number of component carriers. Note: eNB CCM would also set the
  // number of component carriers in eNB RRC
  ccmEnbManager->SetNumberOfComponentCarriers (m_noOfLteCcs);

  rrc->ConfigureCarriers (ccMap);

  if (m_useIdealRrc)
    {
      Ptr<newradioEnbRrcProtocolIdeal> rrcProtocol = CreateObject<newradioEnbRrcProtocolIdeal> ();
      rrcProtocol->SetLteEnbRrcSapProvider (rrc->GetLteEnbRrcSapProvider ());
      rrc->SetLteEnbRrcSapUser (rrcProtocol->GetLteEnbRrcSapUser ());
      rrc->AggregateObject (rrcProtocol);
      rrcProtocol->SetCellId (cellId);
    }
  else
    {
      Ptr<newradioLteEnbRrcProtocolReal> rrcProtocol = CreateObject<newradioLteEnbRrcProtocolReal> ();
      rrcProtocol->SetLteEnbRrcSapProvider (rrc->GetLteEnbRrcSapProvider ());
      rrc->SetLteEnbRrcSapUser (rrcProtocol->GetLteEnbRrcSapUser ());
      rrc->AggregateObject (rrcProtocol);
      rrcProtocol->SetCellId (cellId);
    }

  if (m_epcHelper != 0)
    {
      EnumValue epsBearerToRlcMapping;
      rrc->GetAttribute ("EpsBearerToRlcMapping", epsBearerToRlcMapping);
      // it does not make sense to use RLC/SM when also using the EPC

// ***************** RDF EDIT 6/9/2016 ***************** //
//	  if (epsBearerToRlcMapping.Get () == LteEnbRrc::RLC_SM_ALWAYS)
//	    {
//	      rrc->SetAttribute ("EpsBearerToRlcMapping", EnumValue (LteEnbRrc::RLC_UM_ALWAYS));
//	    }

      if (m_rlcAmEnabled)
        {
          rrc->SetAttribute ("EpsBearerToRlcMapping", EnumValue (LteEnbRrc::RLC_AM_ALWAYS));
        }
      else
        {
          rrc->SetAttribute ("EpsBearerToRlcMapping", EnumValue (LteEnbRrc::RLC_UM_LOWLAT_ALWAYS));
        }
    }

  rrc->SetLteHandoverManagementSapProvider (handoverAlgorithm->GetLteHandoverManagementSapProvider ());
  handoverAlgorithm->SetLteHandoverManagementSapUser (rrc->GetLteHandoverManagementSapUser ());

  // This RRC attribute is used to connect each new RLC instance with the MAC layer
  // (for function such as TransmitPdu, ReportBufferStatusReport).
  // Since in this new architecture, the component carrier manager acts a proxy, it
  // will have its own LteMacSapProvider interface, RLC will see it as through original MAC
  // interface LteMacSapProvider, but the function call will go now through LteEnbComponentCarrierManager
  // instance that needs to implement functions of this interface, and its task will be to
  // forward these calls to the specific MAC of some of the instances of component carriers. This
  // decision will depend on the specific implementation of the component carrier manager.
  rrc->SetLteMacSapProvider (ccmEnbManager->GetLteMacSapProvider ());

  bool ccmTest;
  for (std::map<uint8_t,Ptr<ComponentCarrierEnb> >::iterator it = ccMap.begin (); it != ccMap.end (); ++it)
    {
      it->second->GetPhy ()->SetLteEnbCphySapUser (rrc->GetLteEnbCphySapUser (it->first));
      rrc->SetLteEnbCphySapProvider (it->second->GetPhy ()->GetLteEnbCphySapProvider (), it->first);

      rrc->SetLteEnbCmacSapProvider (it->second->GetMac ()->GetLteEnbCmacSapProvider (),it->first );
      it->second->GetMac ()->SetLteEnbCmacSapUser (rrc->GetLteEnbCmacSapUser (it->first));

      it->second->GetPhy ()->SetComponentCarrierId (it->first);
      it->second->GetMac ()->SetComponentCarrierId (it->first);
      //FFR SAP
      it->second->GetFfMacScheduler ()->SetLteFfrSapProvider (it->second->GetFfrAlgorithm ()->GetLteFfrSapProvider ());
      it->second->GetFfrAlgorithm ()->SetLteFfrSapUser (it->second->GetFfMacScheduler ()->GetLteFfrSapUser ());
      rrc->SetLteFfrRrcSapProvider (it->second->GetFfrAlgorithm ()->GetLteFfrRrcSapProvider (), it->first);
      it->second->GetFfrAlgorithm ()->SetLteFfrRrcSapUser (rrc->GetLteFfrRrcSapUser (it->first));
      //FFR SAP END

      // PHY <--> MAC SAP
      it->second->GetPhy ()->SetLteEnbPhySapUser (it->second->GetMac ()->GetLteEnbPhySapUser ());
      it->second->GetMac ()->SetLteEnbPhySapProvider (it->second->GetPhy ()->GetLteEnbPhySapProvider ());
      // PHY <--> MAC SAP END

      //Scheduler SAP
      it->second->GetMac ()->SetFfMacSchedSapProvider (it->second->GetFfMacScheduler ()->GetFfMacSchedSapProvider ());
      it->second->GetMac ()->SetFfMacCschedSapProvider (it->second->GetFfMacScheduler ()->GetFfMacCschedSapProvider ());

      it->second->GetFfMacScheduler ()->SetFfMacSchedSapUser (it->second->GetMac ()->GetFfMacSchedSapUser ());
      it->second->GetFfMacScheduler ()->SetFfMacCschedSapUser (it->second->GetMac ()->GetFfMacCschedSapUser ());
      // Scheduler SAP END

      it->second->GetMac ()->SetLteCcmMacSapUser (ccmEnbManager->GetLteCcmMacSapUser ());
      ccmEnbManager->SetCcmMacSapProviders (it->first, it->second->GetMac ()->GetLteCcmMacSapProvider ());

      // insert the pointer to the LteMacSapProvider interface of the MAC layer of the specific component carrier
      ccmTest = ccmEnbManager->SetMacSapProvider (it->first, it->second->GetMac ()->GetLteMacSapProvider ());

      if (ccmTest == false)
        {
          NS_FATAL_ERROR ("Error in SetComponentCarrierMacSapProviders");
        }
    }



  dev->SetNode (n);
  dev->SetAttribute ("CellId", UintegerValue (cellId));
  dev->SetAttribute ("LteEnbComponentCarrierManager", PointerValue (ccmEnbManager));
  dev->SetCcMap (ccMap);
  std::map<uint8_t,Ptr<ComponentCarrierEnb> >::iterator it = ccMap.begin ();
  dev->SetAttribute ("LteEnbRrc", PointerValue (rrc));
  dev->SetAttribute ("LteHandoverAlgorithm", PointerValue (handoverAlgorithm));
  dev->SetAttribute ("LteFfrAlgorithm", PointerValue (it->second->GetFfrAlgorithm ()));

  if (m_isAnrEnabled)
    {
      Ptr<LteAnr> anr = CreateObject<LteAnr> (cellId);
      rrc->SetLteAnrSapProvider (anr->GetLteAnrSapProvider ());
      anr->SetLteAnrSapUser (rrc->GetLteAnrSapUser ());
      dev->SetAttribute ("LteAnr", PointerValue (anr));
    }

  for (it = ccMap.begin (); it != ccMap.end (); ++it)
    {
      Ptr<LteEnbPhy> ccPhy = it->second->GetPhy ();
      ccPhy->SetDevice (dev);
      ccPhy->GetUlSpectrumPhy ()->SetDevice (dev);
      ccPhy->GetDlSpectrumPhy ()->SetDevice (dev);
      ccPhy->GetUlSpectrumPhy ()->SetLtePhyRxDataEndOkCallback (MakeCallback (&LteEnbPhy::PhyPduReceived, ccPhy));
      ccPhy->GetUlSpectrumPhy ()->SetLtePhyRxCtrlEndOkCallback (MakeCallback (&LteEnbPhy::ReceiveLteControlMessageList, ccPhy));
      ccPhy->GetUlSpectrumPhy ()->SetLtePhyUlHarqFeedbackCallback (MakeCallback (&LteEnbPhy::ReceiveLteUlHarqFeedback, ccPhy));
      NS_LOG_LOGIC ("set the propagation model frequencies");
      double dlFreq = LteSpectrumValueHelper::GetCarrierFrequency (it->second->m_dlEarfcn);
      NS_LOG_LOGIC ("DL freq: " << dlFreq);
      bool dlFreqOk = m_downlinkPathlossModel->SetAttributeFailSafe ("Frequency", DoubleValue (dlFreq));
      if (!dlFreqOk)
        {
          NS_LOG_WARN ("DL propagation model does not have a Frequency attribute");
        }

      double ulFreq = LteSpectrumValueHelper::GetCarrierFrequency (it->second->m_ulEarfcn);

      NS_LOG_LOGIC ("UL freq: " << ulFreq);
      bool ulFreqOk = m_uplinkPathlossModel->SetAttributeFailSafe ("Frequency", DoubleValue (ulFreq));
      if (!ulFreqOk)
        {
          NS_LOG_WARN ("UL propagation model does not have a Frequency attribute");
        }
    }  //end for
  rrc->SetForwardUpCallback (MakeCallback (&LteEnbNetDevice::Receive, dev));
  dev->Initialize ();
  n->AddDevice (dev);

  for (it = ccMap.begin (); it != ccMap.end (); ++it)
    {
      m_uplinkChannel->AddRx (it->second->GetPhy ()->GetUlSpectrumPhy ());
    }

  if (m_epcHelper != 0)
    {
      NS_LOG_INFO ("adding this eNB to the EPC");
      m_epcHelper->AddEnb (n, dev, dev->GetCellId ());
      Ptr<EpcEnbApplication> enbApp = n->GetApplication (0)->GetObject<EpcEnbApplication> ();
      NS_ASSERT_MSG (enbApp != 0, "cannot retrieve EpcEnbApplication");

      // S1 SAPs
      rrc->SetS1SapProvider (enbApp->GetS1SapProvider ());
      enbApp->SetS1SapUser (rrc->GetS1SapUser ());

      // X2 SAPs
      Ptr<EpcX2> x2 = n->GetObject<EpcX2> ();
      x2->SetEpcX2SapUser (rrc->GetEpcX2SapUser ());
      rrc->SetEpcX2SapProvider (x2->GetEpcX2SapProvider ());
      rrc->SetEpcX2PdcpProvider (x2->GetEpcX2PdcpProvider ());
    }

  return dev;
}

// only for newradio-only devices
void
newradioHelper::AttachToClosestEnb (NetDeviceContainer ueDevices, NetDeviceContainer enbDevices)
{
  NS_LOG_FUNCTION (this);

  for (std::map<uint8_t, newradioComponentCarrier >::iterator it = m_componentCarrierPhyParams.begin (); it != m_componentCarrierPhyParams.end (); ++it)
    {

      // initialize the beamforming vectors
      for (NetDeviceContainer::Iterator i = ueDevices.Begin (); i != ueDevices.End (); i++)
        {
          for (NetDeviceContainer::Iterator enbIter = enbDevices.Begin ();
               enbIter != enbDevices.End (); ++enbIter)
            {
              Ptr<AntennaArrayModel> ueAntennaArray = DynamicCast<AntennaArrayModel> (
                  (DynamicCast<newradioUeNetDevice> (*i))->GetPhy (it->first)->GetDlSpectrumPhy ()->GetRxAntenna ());
              complexVector_t dummy;
              ueAntennaArray->SetBeamformingVectorPanel (dummy,*enbIter);
            }
        }

      if (m_channelModelType == "ns3::newradioBeamforming")
        {
          m_beamforming.at (it->first)->Initial (ueDevices,enbDevices);
        }
      else if (m_channelModelType == "ns3::newradioChannelMatrix")
        {
          m_channelMatrix.at (it->first)->Initial (ueDevices,enbDevices);
        }
      else if (m_channelModelType == "ns3::newradioChannelRaytracing")
        {
          m_raytracing.at (it->first)->Initial (ueDevices,enbDevices);
        }
      else if (m_channelModelType == "ns3::newradio3gppChannel")
        {
          m_3gppChannel.at (it->first)->Initial (ueDevices,enbDevices);
        }
    }

  for (NetDeviceContainer::Iterator i = ueDevices.Begin (); i != ueDevices.End (); i++)
    {
      AttachToClosestEnb (*i, enbDevices);
    }
}

// for MC devices
void
newradioHelper::AttachToClosestEnb (NetDeviceContainer ueDevices, NetDeviceContainer newradioEnbDevices, NetDeviceContainer lteEnbDevices)
{
  NS_LOG_FUNCTION (this);

  for (NetDeviceContainer::Iterator i = ueDevices.Begin (); i != ueDevices.End (); i++)
    {
      AttachMcToClosestEnb (*i, newradioEnbDevices, lteEnbDevices);
    }

  for (std::map<uint8_t, newradioComponentCarrier >::iterator it = m_componentCarrierPhyParams.begin (); it != m_componentCarrierPhyParams.end (); ++it)
    {
      if (m_channelModelType == "ns3::newradioBeamforming")
        {
          m_beamforming.at (it->first)->Initial (ueDevices,newradioEnbDevices);
        }
      else if (m_channelModelType == "ns3::newradioChannelMatrix")
        {
          m_channelMatrix.at (it->first)->Initial (ueDevices,newradioEnbDevices);
        }
      else if (m_channelModelType == "ns3::newradioChannelRaytracing")
        {
          m_raytracing.at (it->first)->Initial (ueDevices,newradioEnbDevices);
        }
      else if (m_channelModelType == "ns3::newradio3gppChannel")
        {
          m_3gppChannel.at (it->first)->Initial (ueDevices,newradioEnbDevices);
        }
    }
}

// for InterRatHoCapable devices
void
newradioHelper::AttachIrToClosestEnb (NetDeviceContainer ueDevices, NetDeviceContainer newradioEnbDevices, NetDeviceContainer lteEnbDevices)
{
  NS_LOG_FUNCTION (this);

  /*
  // set initial conditions on beamforming before attaching the UE to the eNBs
  if(m_channelModelType == "ns3::newradioBeamforming")
  {
          m_beamforming->Initial(ueDevices,newradioEnbDevices);
  }
  else if(m_channelModelType == "ns3::newradioChannelMatrix")
  {
          m_channelMatrix->Initial(ueDevices,newradioEnbDevices);
  }
  else if(m_channelModelType == "ns3::newradioChannelRaytracing")
  {
          m_raytracing->Initial(ueDevices,newradioEnbDevices);
  }
  else if(m_channelModelType == "ns3::newradio3gppChannel")
  {
          m_3gppChannel->Initial(ueDevices,newradioEnbDevices);
  }

  for (NetDeviceContainer::Iterator i = ueDevices.Begin(); i != ueDevices.End(); i++)
  {
          AttachIrToClosestEnb(*i, newradioEnbDevices, lteEnbDevices);
  }
  */
}

void
newradioHelper::AttachToClosestEnb (Ptr<NetDevice> ueDevice, NetDeviceContainer enbDevices)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT_MSG (enbDevices.GetN () > 0, "empty enb device container");
  Vector uepos = ueDevice->GetNode ()->GetObject<MobilityModel> ()->GetPosition ();
  double minDistance = std::numeric_limits<double>::infinity ();
  Ptr<NetDevice> closestEnbDevice;
  for (NetDeviceContainer::Iterator i = enbDevices.Begin (); i != enbDevices.End (); ++i)
    {
      Vector enbpos = (*i)->GetNode ()->GetObject<MobilityModel> ()->GetPosition ();
      double distance = CalculateDistance (uepos, enbpos);
      if (distance < minDistance)
        {
          minDistance = distance;
          closestEnbDevice = *i;
        }
    }
  NS_ASSERT (closestEnbDevice != 0);

  Ptr<newradioUeNetDevice> newradioUe = ueDevice->GetObject<newradioUeNetDevice> ();

  // Necessary operation to connect newradio UE to eNB at lower layers
  for (NetDeviceContainer::Iterator i = enbDevices.Begin (); i != enbDevices.End (); ++i)
    {
      Ptr<newradioEnbNetDevice> newradioEnb = (*i)->GetObject<newradioEnbNetDevice> ();

      std::map<uint8_t, Ptr<newradioComponentCarrierEnb> > enbCcMap = newradioEnb->GetCcMap ();
      for (std::map<uint8_t, Ptr<newradioComponentCarrierEnb> >::iterator itEnb = enbCcMap.begin (); itEnb != enbCcMap.end (); ++itEnb)
        {
          uint16_t newradioCellId = itEnb->second->GetCellId ();
          Ptr<newradioPhyMacCommon> configParams = itEnb->second->GetPhy ()->GetConfigurationParameters ();
          itEnb->second->GetPhy ()->AddUePhy (newradioUe->GetImsi (), ueDevice);
          // register newradio eNBs informations in the newradioUePhy

          std::map<uint8_t, Ptr<newradioComponentCarrierUe> > ueCcMap = newradioUe->GetCcMap ();
          for (std::map<uint8_t, Ptr<newradioComponentCarrierUe> >::iterator itUe = ueCcMap.begin (); itUe != ueCcMap.end (); ++itUe)
            {
              itUe->second->GetPhy ()->RegisterOtherEnb (newradioCellId, configParams, newradioEnb);
            }
          //closestnewradio->GetMac ()->AssociateUeMAC (mcDevice->GetImsi ()); //TODO this does not do anything
          NS_LOG_INFO ("newradioCellId " << newradioCellId);
        }
    }

  uint16_t cellId = closestEnbDevice->GetObject<newradioEnbNetDevice> ()->GetCellId ();
  Ptr<newradioPhyMacCommon> configParams = closestEnbDevice->GetObject<newradioEnbNetDevice> ()->GetPhy ()->GetConfigurationParameters ();

  closestEnbDevice->GetObject<newradioEnbNetDevice> ()->GetPhy ()->AddUePhy (ueDevice->GetObject<newradioUeNetDevice> ()->GetImsi (), ueDevice);
  ueDevice->GetObject<newradioUeNetDevice> ()->GetPhy ()->RegisterToEnb (cellId, configParams);
  closestEnbDevice->GetObject<newradioEnbNetDevice> ()->GetMac ()->AssociateUeMAC (ueDevice->GetObject<newradioUeNetDevice> ()->GetImsi ());

  // connect to the closest one
  Ptr<EpcUeNas> ueNas = ueDevice->GetObject<newradioUeNetDevice> ()->GetNas ();
  ueNas->Connect (closestEnbDevice->GetObject<newradioEnbNetDevice> ()->GetCellId (),
                  closestEnbDevice->GetObject<newradioEnbNetDevice> ()->GetEarfcn ());

  if (m_epcHelper != 0)
    {
      // activate default EPS bearer
      m_epcHelper->ActivateEpsBearer (ueDevice, ueDevice->GetObject<newradioUeNetDevice> ()->GetImsi (), EpcTft::Default (), EpsBearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT));
    }

  // tricks needed for the simplified LTE-only simulations
  //if (m_epcHelper == 0)
  //{
  ueDevice->GetObject<newradioUeNetDevice> ()->SetTargetEnb (closestEnbDevice->GetObject<newradioEnbNetDevice> ());
  //}

}

void
newradioHelper::AttachMcToClosestEnb (Ptr<NetDevice> ueDevice, NetDeviceContainer newradioEnbDevices, NetDeviceContainer lteEnbDevices)
{
  NS_LOG_FUNCTION (this);
  Ptr<McUeNetDevice> mcDevice = ueDevice->GetObject<McUeNetDevice> ();

  NS_ASSERT_MSG (newradioEnbDevices.GetN () > 0 && lteEnbDevices.GetN () > 0,
                 "empty lte or newradio enb device container");

  // Find the closest LTE station
  Vector uepos = ueDevice->GetNode ()->GetObject<MobilityModel> ()->GetPosition ();
  double minDistance = std::numeric_limits<double>::infinity ();
  Ptr<NetDevice> lteClosestEnbDevice;
  for (NetDeviceContainer::Iterator i = lteEnbDevices.Begin (); i != lteEnbDevices.End (); ++i)
    {
      Vector enbpos = (*i)->GetNode ()->GetObject<MobilityModel> ()->GetPosition ();
      double distance = CalculateDistance (uepos, enbpos);
      if (distance < minDistance)
        {
          minDistance = distance;
          lteClosestEnbDevice = *i;
        }
    }
  NS_ASSERT (lteClosestEnbDevice != 0);
  NS_ASSERT (lteClosestEnbDevice->GetObject<LteEnbNetDevice> () != 0);       // stop if it is not an LTE eNB

  // Necessary operation to connect newradio UE to eNB at lower layers
  for (NetDeviceContainer::Iterator i = newradioEnbDevices.Begin (); i != newradioEnbDevices.End (); ++i)
    {
      Ptr<newradioEnbNetDevice> newradioEnb = (*i)->GetObject<newradioEnbNetDevice> ();
      std::map<uint8_t, Ptr<newradioComponentCarrierEnb> > newradioEnbCcMap = newradioEnb->GetCcMap ();

      for (std::map<uint8_t, Ptr<newradioComponentCarrierEnb> >::iterator itEnb = newradioEnbCcMap.begin (); itEnb != newradioEnbCcMap.end (); ++itEnb)
        {
          uint16_t newradioCellId = itEnb->second->GetCellId ();
          Ptr<newradioPhyMacCommon> configParams = itEnb->second->GetPhy ()->GetConfigurationParameters ();
          itEnb->second->GetPhy ()->AddUePhy (mcDevice->GetImsi (), ueDevice);
          // register newradio eNBs informations in the newradioUePhy

          std::map<uint8_t, Ptr<newradioComponentCarrierUe> > ueCcMap = mcDevice->GetnewradioCcMap ();
          for (std::map<uint8_t, Ptr<newradioComponentCarrierUe> >::iterator itUe = ueCcMap.begin (); itUe != ueCcMap.end (); ++itUe)
            {
              itUe->second->GetPhy ()->RegisterOtherEnb (newradioCellId, configParams, newradioEnb);
            }
          //closestnewradio->GetMac ()->AssociateUeMAC (mcDevice->GetImsi ()); //TODO this does not do anything
          NS_LOG_INFO ("newradioCellId " << newradioCellId);
        }
    }

  // Attach the MC device the LTE eNB, the best newradio eNB will be selected automatically
  Ptr<LteEnbNetDevice> enbLteDevice = lteClosestEnbDevice->GetObject<LteEnbNetDevice> ();
  Ptr<EpcUeNas> lteUeNas = mcDevice->GetNas ();
  lteUeNas->Connect (enbLteDevice->GetCellId (), enbLteDevice->GetDlEarfcn ());       // the newradioCell will be automatically selected

  if (m_epcHelper != 0)
    {
      // activate default EPS bearer
      m_epcHelper->ActivateEpsBearer (ueDevice, lteUeNas, mcDevice->GetImsi (), EpcTft::Default (), EpsBearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT));
    }

  mcDevice->SetLteTargetEnb (enbLteDevice);
}

void
newradioHelper::AttachIrToClosestEnb (Ptr<NetDevice> ueDevice, NetDeviceContainer newradioEnbDevices, NetDeviceContainer lteEnbDevices)
{
  NS_LOG_FUNCTION (this);
  Ptr<McUeNetDevice> mcDevice = ueDevice->GetObject<McUeNetDevice> ();
  Ptr<LteUeRrc> ueRrc = mcDevice->GetLteRrc ();

  NS_ASSERT_MSG (ueRrc != 0, "McUeDevice with undefined rrc");

  NS_ASSERT_MSG (newradioEnbDevices.GetN () > 0 && lteEnbDevices.GetN () > 0,
                 "empty lte or newradio enb device container");

  // Find the closest LTE station
  Vector uepos = ueDevice->GetNode ()->GetObject<MobilityModel> ()->GetPosition ();
  double minDistance = std::numeric_limits<double>::infinity ();
  Ptr<NetDevice> lteClosestEnbDevice;
  for (NetDeviceContainer::Iterator i = lteEnbDevices.Begin (); i != lteEnbDevices.End (); ++i)
    {
      Ptr<LteEnbNetDevice> lteEnb = (*i)->GetObject<LteEnbNetDevice> ();
      uint16_t cellId = lteEnb->GetCellId ();
      ueRrc->AddLteCellId (cellId);
      // Let the RRC know that the UE in this simulation is InterRatHoCapable
      Ptr<LteEnbRrc> enbRrc = lteEnb->GetRrc ();
      enbRrc->SetInterRatHoMode ();
      Vector enbpos = (*i)->GetNode ()->GetObject<MobilityModel> ()->GetPosition ();
      double distance = CalculateDistance (uepos, enbpos);
      if (distance < minDistance)
        {
          minDistance = distance;
          lteClosestEnbDevice = *i;
        }
    }
  NS_ASSERT (lteClosestEnbDevice != 0);

  // Necessary operation to connect newradio UE to eNB at lower layers
  minDistance = std::numeric_limits<double>::infinity ();
  Ptr<NetDevice> closestEnbDevice;
  for (NetDeviceContainer::Iterator i = newradioEnbDevices.Begin (); i != newradioEnbDevices.End (); ++i)
    {
      Ptr<newradioEnbNetDevice> newradioEnb = (*i)->GetObject<newradioEnbNetDevice> ();
      uint16_t newradioCellId = newradioEnb->GetCellId ();
      ueRrc->AddnewradioCellId (newradioCellId);
      Ptr<newradioPhyMacCommon> configParams = newradioEnb->GetPhy ()->GetConfigurationParameters ();
      newradioEnb->GetPhy ()->AddUePhy (mcDevice->GetImsi (), ueDevice);
      // register newradio eNBs informations in the newradioUePhy
      mcDevice->GetnewradioPhy ()->RegisterOtherEnb (newradioCellId, configParams, newradioEnb);
      //closestnewradio->GetMac ()->AssociateUeMAC (mcDevice->GetImsi ()); //TODO this does not do anything
      NS_LOG_INFO ("newradioCellId " << newradioCellId);

      // Let the RRC know that the UE in this simulation is InterRatHoCapable
      Ptr<LteEnbRrc> enbRrc = newradioEnb->GetRrc ();
      enbRrc->SetInterRatHoMode ();
      Vector enbpos = (*i)->GetNode ()->GetObject<MobilityModel> ()->GetPosition ();
      double distance = CalculateDistance (uepos, enbpos);
      if (distance < minDistance)
        {
          minDistance = distance;
          closestEnbDevice = *i;
        }
    }

  // Attach the MC device the Closest LTE eNB
  Ptr<LteEnbNetDevice> enbLteDevice = lteClosestEnbDevice->GetObject<LteEnbNetDevice> ();
  Ptr<EpcUeNas> lteUeNas = mcDevice->GetNas ();
  lteUeNas->Connect (enbLteDevice->GetCellId (), enbLteDevice->GetDlEarfcn ());       // force connection to the LTE eNB

  if (m_epcHelper != 0)
    {
      // activate default EPS bearer
      m_epcHelper->ActivateEpsBearer (ueDevice, lteUeNas, mcDevice->GetImsi (), EpcTft::Default (), EpsBearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT));
    }

  // set initial targets
  Ptr<newradioEnbNetDevice> enbDevice = closestEnbDevice->GetObject<newradioEnbNetDevice> ();
  mcDevice->SetLteTargetEnb (enbLteDevice);
  mcDevice->SetnewradioTargetEnb (enbDevice);
}

void
newradioHelper::AddX2Interface (NodeContainer enbNodes)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT_MSG (m_epcHelper != 0, "X2 interfaces cannot be set up when the EPC is not used");

  for (NodeContainer::Iterator i = enbNodes.Begin (); i != enbNodes.End (); ++i)
    {
      for (NodeContainer::Iterator j = i + 1; j != enbNodes.End (); ++j)
        {
          AddX2Interface (*i, *j);
        }
    }

  // print stats
  m_cnStats = CreateObject<CoreNetworkStatsCalculator> ();

  // add traces
  Config::Connect ("/NodeList/*/$ns3::EpcX2/RxPDU",
                   MakeCallback (&CoreNetworkStatsCalculator::LogX2Packet, m_cnStats));
}

void
newradioHelper::AddX2Interface (Ptr<Node> enbNode1, Ptr<Node> enbNode2)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_INFO ("setting up the X2 interface");

  m_epcHelper->AddX2Interface (enbNode1, enbNode2);
}

void
newradioHelper::AddX2Interface (NodeContainer lteEnbNodes, NodeContainer newradioEnbNodes)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT_MSG (m_epcHelper != 0, "X2 interfaces cannot be set up when the EPC is not used");

  for (NodeContainer::Iterator i = newradioEnbNodes.Begin (); i != newradioEnbNodes.End (); ++i)
    {
      for (NodeContainer::Iterator j = i + 1; j != newradioEnbNodes.End (); ++j)
        {
          AddX2Interface (*i, *j);
        }
    }
  for (NodeContainer::Iterator i = newradioEnbNodes.Begin (); i != newradioEnbNodes.End (); ++i)
    {
      // get the position of the newradio eNB
      Vector newradioPos = (*i)->GetObject<MobilityModel> ()->GetPosition ();
      double minDistance = std::numeric_limits<double>::infinity ();
      Ptr<Node> closestLteNode;
      for (NodeContainer::Iterator j = lteEnbNodes.Begin (); j != lteEnbNodes.End (); ++j)
        {
          AddX2Interface (*i, *j);
          Vector ltePos = (*j)->GetObject<MobilityModel> ()->GetPosition ();
          double distance = CalculateDistance (newradioPos, ltePos);
          if (distance < minDistance)
            {
              minDistance = distance;
              closestLteNode = *j;
            }
        }

      // get closestLteNode cellId and store it in the newradioEnb RRC
      Ptr<LteEnbNetDevice> closestEnbDevice = closestLteNode->GetDevice (0)->GetObject <LteEnbNetDevice> ();
      if (closestEnbDevice != 0)
        {
          uint16_t lteCellId = closestEnbDevice->GetRrc ()->GetCellId ();
          NS_LOG_LOGIC ("ClosestLteCellId " << lteCellId);
          (*i)->GetDevice (0)->GetObject <newradioEnbNetDevice> ()->GetRrc ()->SetClosestLteCellId (lteCellId);
        }
      else
        {
          NS_FATAL_ERROR ("LteDevice not retrieved");
        }

    }
  for (NodeContainer::Iterator i = lteEnbNodes.Begin (); i != lteEnbNodes.End (); ++i)
    {
      for (NodeContainer::Iterator j = i + 1; j != lteEnbNodes.End (); ++j)
        {
          AddX2Interface (*i, *j);
        }
    }
  // print stats
  if (m_cnStats == 0)
    {
      m_cnStats = CreateObject<CoreNetworkStatsCalculator> ();
    }

  // add traces
  Config::Connect ("/NodeList/*/$ns3::EpcX2/RxPDU",
                   MakeCallback (&CoreNetworkStatsCalculator::LogX2Packet, m_cnStats));
}

void
newradioHelper::SetEpcHelper (Ptr<EpcHelper> epcHelper)
{
  m_epcHelper = epcHelper;
}

class newradioDrbActivator : public SimpleRefCount<newradioDrbActivator>
{
public:
  newradioDrbActivator (Ptr<NetDevice> ueDevice, EpsBearer bearer);
  static void ActivateCallback (Ptr<newradioDrbActivator> a, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti);
  void ActivateDrb (uint64_t imsi, uint16_t cellId, uint16_t rnti);
private:
  bool m_active;
  Ptr<NetDevice> m_ueDevice;
  EpsBearer m_bearer;
  uint64_t m_imsi;
};

newradioDrbActivator::newradioDrbActivator (Ptr<NetDevice> ueDevice, EpsBearer bearer)
  : m_active (false),
    m_ueDevice (ueDevice),
    m_bearer (bearer)
{
  if (m_ueDevice->GetObject< newradioUeNetDevice> ())
    {           // newradio
      m_imsi = m_ueDevice->GetObject< newradioUeNetDevice> ()->GetImsi ();
    }
  else if (m_ueDevice->GetObject< McUeNetDevice> ())
    {
      m_imsi = m_ueDevice->GetObject< McUeNetDevice> ()->GetImsi ();           // TODO support for LTE part
    }
}

void
newradioDrbActivator::ActivateCallback (Ptr<newradioDrbActivator> a, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (a << context << imsi << cellId << rnti);
  a->ActivateDrb (imsi, cellId, rnti);
}

void
newradioDrbActivator::ActivateDrb (uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this << imsi << cellId << rnti << m_active);
  if ((!m_active) && (imsi == m_imsi))
    {
      Ptr<LteUeRrc> ueRrc = m_ueDevice->GetObject<newradioUeNetDevice> ()->GetRrc ();
      NS_ASSERT (ueRrc->GetState () == LteUeRrc::CONNECTED_NORMALLY);
      uint16_t rnti = ueRrc->GetRnti ();
      Ptr<newradioEnbNetDevice> enbLteDevice = m_ueDevice->GetObject<newradioUeNetDevice> ()->GetTargetEnb ();
      Ptr<LteEnbRrc> enbRrc = enbLteDevice->GetObject<newradioEnbNetDevice> ()->GetRrc ();
      NS_ASSERT (ueRrc->GetCellId () == enbLteDevice->GetCellId ());
      Ptr<UeManager> ueManager = enbRrc->GetUeManager (rnti);
      NS_ASSERT (ueManager->GetState () == UeManager::CONNECTED_NORMALLY
                 || ueManager->GetState () == UeManager::CONNECTION_RECONFIGURATION);
      EpcEnbS1SapUser::DataRadioBearerSetupRequestParameters params;
      params.rnti = rnti;
      params.bearer = m_bearer;
      params.bearerId = 0;
      params.gtpTeid = 0; // don't care
      enbRrc->GetS1SapUser ()->DataRadioBearerSetupRequest (params);
      m_active = true;
    }
}

// TODO this does not support yet Mc devices
void
newradioHelper::ActivateDataRadioBearer (NetDeviceContainer ueDevices, EpsBearer bearer)
{
  NS_LOG_FUNCTION (this);
  for (NetDeviceContainer::Iterator i = ueDevices.Begin (); i != ueDevices.End (); ++i)
    {
      ActivateDataRadioBearer (*i, bearer);
    }
}
void
newradioHelper::ActivateDataRadioBearer (Ptr<NetDevice> ueDevice, EpsBearer bearer)
{
  NS_LOG_FUNCTION (this << ueDevice);
  //NS_ASSERT_MSG (m_epcHelper == 0, "this method must not be used when the EPC is being used");

  // Normally it is the EPC that takes care of activating DRBs
  // when the UE gets connected. When the EPC is not used, we achieve
  // the same behavior by hooking a dedicated DRB activation function
  // to the Enb RRC Connection Established trace source


  Ptr<newradioEnbNetDevice> enbnewradioDevice = ueDevice->GetObject<newradioUeNetDevice> ()->GetTargetEnb ();

  std::ostringstream path;
  path << "/NodeList/" << enbnewradioDevice->GetNode ()->GetId ()
       << "/DeviceList/" << enbnewradioDevice->GetIfIndex ()
       << "/LteEnbRrc/ConnectionEstablished";
  Ptr<newradioDrbActivator> arg = Create<newradioDrbActivator> (ueDevice, bearer);
  Config::Connect (path.str (), MakeBoundCallback (&newradioDrbActivator::ActivateCallback, arg));
}


void
newradioHelper::EnableTraces (void)
{
  EnableDlPhyTrace ();
  EnableUlPhyTrace ();
  //EnableEnbPacketCountTrace (); //the trace source does not exist
  //EnableUePacketCountTrace (); //the trace source does not exist
  //EnableTransportBlockTrace (); //the callback does nothing
  EnableRlcTraces ();
  EnablePdcpTraces ();
  EnableMcTraces ();
}


// TODO traces for MC
void
newradioHelper::EnableDlPhyTrace (void)
{
  //NS_LOG_FUNCTION_NOARGS ();
  //Config::Connect ("/NodeList/*/DeviceList/*/newradioUePhy/ReportCurrentCellRsrpSinr",
  //		MakeBoundCallback (&newradioPhyRxTrace::ReportCurrentCellRsrpSinrCallback, m_phyStats));

  // regulare newradio UE device
  Config::Connect ("/NodeList/*/DeviceList/*/ComponentCarrierMapUe/*/newradioUePhy/DlSpectrumPhy/RxPacketTraceUe",
                   MakeBoundCallback (&newradioPhyRxTrace::RxPacketTraceUeCallback, m_phyStats));

  // MC ue device
  Config::Connect ("/NodeList/*/DeviceList/*/newradioComponentCarrierMapUe/*/newradioUePhy/DlSpectrumPhy/RxPacketTraceUe",
                   MakeBoundCallback (&newradioPhyRxTrace::RxPacketTraceUeCallback, m_phyStats));
}

void
newradioHelper::EnableUlPhyTrace (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Config::Connect ("/NodeList/*/DeviceList/*/ComponentCarrierMap/*/newradioEnbPhy/DlSpectrumPhy/RxPacketTraceEnb",
                   MakeBoundCallback (&newradioPhyRxTrace::RxPacketTraceEnbCallback, m_phyStats));
}

void
newradioHelper::EnableEnbPacketCountTrace ()
{
  NS_LOG_FUNCTION_NOARGS ();
  Config::Connect ("/NodeList/*/DeviceList/*/ComponentCarrierMap/*/newradioEnbPhy/DlSpectrumPhy/ReportEnbTxRxPacketCount",
                   MakeBoundCallback (&newradioPhyRxTrace::ReportPacketCountEnbCallback, m_phyStats));

}

void
newradioHelper::EnableUePacketCountTrace ()
{
  NS_LOG_FUNCTION_NOARGS ();
  Config::Connect ("/NodeList/*/DeviceList/*/ComponentCarrierMapUe/*/newradioUePhy/DlSpectrumPhy/ReportUeTxRxPacketCount",
                   MakeBoundCallback (&newradioPhyRxTrace::ReportPacketCountUeCallback, m_phyStats));

}

void
newradioHelper::EnableTransportBlockTrace ()
{
  NS_LOG_FUNCTION_NOARGS ();
  Config::Connect ("/NodeList/*/DeviceList/*/ComponentCarrierMapUe/*/newradioUePhy/ReportDownlinkTbSize",
                   MakeBoundCallback (&newradioPhyRxTrace::ReportDownLinkTBSize, m_phyStats));
}


void
newradioHelper::EnableRlcTraces (void)
{
  NS_ASSERT_MSG (m_rlcStats == 0, "please make sure that newradioHelper::EnableRlcTraces is called at most once");
  m_rlcStats = CreateObject<newradioBearerStatsCalculator> ("RLC");
  m_radioBearerStatsConnector->EnableRlcStats (m_rlcStats);
}

Ptr<newradioBearerStatsCalculator>
newradioHelper::GetRlcStats (void)
{
  return m_rlcStats;
}

void
newradioHelper::EnablePdcpTraces (void)
{
  NS_ASSERT_MSG (m_pdcpStats == 0, "please make sure that newradioHelper::EnablePdcpTraces is called at most once");
  m_pdcpStats = CreateObject<newradioBearerStatsCalculator> ("PDCP");
  m_radioBearerStatsConnector->EnablePdcpStats (m_pdcpStats);
}

Ptr<newradioBearerStatsCalculator>
newradioHelper::GetPdcpStats (void)
{
  return m_pdcpStats;
}

void
newradioHelper::EnableMcTraces (void)
{
  NS_ASSERT_MSG (m_mcStats == 0, "please make sure that newradioHelper::EnableMcTraces is called at most once");
  m_mcStats = CreateObject<McStatsCalculator> ();
  m_radioBearerStatsConnector->EnableMcStats (m_mcStats);
}

Ptr<McStatsCalculator>
newradioHelper::GetMcStats (void)
{
  return m_mcStats;
}

std::string
newradioHelper::GetEnbComponentCarrierManagerType () const
{
  return m_enbComponentCarrierManagerFactory.GetTypeId ().GetName ();
}

void
newradioHelper::SetEnbComponentCarrierManagerType (std::string type)
{
  NS_LOG_FUNCTION (this << type);
  m_enbComponentCarrierManagerFactory = ObjectFactory ();
  m_enbComponentCarrierManagerFactory.SetTypeId (type);
}

std::string
newradioHelper::GetLteEnbComponentCarrierManagerType () const
{
  return m_lteEnbComponentCarrierManagerFactory.GetTypeId ().GetName ();
}

void
newradioHelper::SetLteEnbComponentCarrierManagerType (std::string type)
{
  NS_LOG_FUNCTION (this << type);
  m_lteEnbComponentCarrierManagerFactory = ObjectFactory ();
  m_lteEnbComponentCarrierManagerFactory.SetTypeId (type);
}

std::string
newradioHelper::GetUeComponentCarrierManagerType () const
{
  return m_ueComponentCarrierManagerFactory.GetTypeId ().GetName ();
}

void
newradioHelper::SetUeComponentCarrierManagerType (std::string type)
{
  NS_LOG_FUNCTION (this << type);
  m_ueComponentCarrierManagerFactory = ObjectFactory ();
  m_ueComponentCarrierManagerFactory.SetTypeId (type);
}

}
}
