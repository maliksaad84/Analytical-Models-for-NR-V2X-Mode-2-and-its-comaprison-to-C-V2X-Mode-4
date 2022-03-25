#include "newradio-vehicular-helper.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/newradio-vehicular-net-device.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/single-model-spectrum-channel.h"
#include "ns3/newradio-vehicular-antenna-array-model.h"
#include "ns3/newradio-vehicular-spectrum-propagation-loss-model.h"
#include "ns3/pointer.h"
#include "ns3/config.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("newradioVehicularHelper"); // TODO check if this has to be defined here

namespace millicar {

NS_OBJECT_ENSURE_REGISTERED (newradioVehicularHelper); // TODO check if this has to be defined here

newradioVehicularHelper::newradioVehicularHelper ()
: m_phyTraceHelper{CreateObject<newradioVehicularTracesHelper>("sinr-mcs.txt")} // TODO name as attribute
{
  NS_LOG_FUNCTION (this);
}

newradioVehicularHelper::~newradioVehicularHelper ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
newradioVehicularHelper::GetTypeId ()
{
  static TypeId tid =
  TypeId ("ns3::newradioVehicularHelper")
  .SetParent<Object> ()
  .AddConstructor<newradioVehicularHelper> ()
  .AddAttribute ("PropagationLossModel",
                 "The type of path-loss model to be used. "
                 "The allowed values for this attributes are the type names "
                 "of any class inheriting from ns3::PropagationLossModel.",
                 StringValue (""),
                 MakeStringAccessor (&newradioVehicularHelper::SetPropagationLossModelType),
                 MakeStringChecker ())
  .AddAttribute ("SpectrumPropagationLossModel",
                 "The type of fast fading model to be used. "
                 "The allowed values for this attributes are the type names "
                 "of any class inheriting from ns3::SpectrumPropagationLossModel.",
                 StringValue (""),
                 MakeStringAccessor (&newradioVehicularHelper::SetSpectrumPropagationLossModelType),
                 MakeStringChecker ())
  .AddAttribute ("PropagationDelayModel",
                 "The type of propagation delay model to be used. "
                 "The allowed values for this attributes are the type names "
                 "of any class inheriting from ns3::PropagationDelayModel.",
                 StringValue (""),
                 MakeStringAccessor (&newradioVehicularHelper::SetPropagationDelayModelType),
                 MakeStringChecker ())
  .AddAttribute ("Numerology",
                 "Numerology to use for the definition of the frame structure."
		 "0 : subcarrier spacing will be set to 15 KHz"
		 "1 : subcarrier spacing will be set to 30 KHz"
                 "2 : subcarrier spacing will be set to 60 KHz",
                 UintegerValue (0),
                 MakeUintegerAccessor (&newradioVehicularHelper::SetNumerology),
                 MakeUintegerChecker<uint8_t> ())
  .AddAttribute ("Bandwidth",
                 "Bandwidth in Hz",
                 DoubleValue (1e8),
                 MakeDoubleAccessor (&newradioVehicularHelper::m_bandwidth),
                 MakeDoubleChecker<double> ())
  .AddAttribute ("SchedulingPatternOption",
                 "The type of scheduling pattern option to be used for resources assignation."
                 "Default   : one single slot per subframe for each device"
                 "Optimized : each slot of the subframe is used",
                 EnumValue(DEFAULT),
                 MakeEnumAccessor (&newradioVehicularHelper::SetSchedulingPatternOptionType,
                                   &newradioVehicularHelper::GetSchedulingPatternOptionType),
                 MakeEnumChecker(DEFAULT, "Default",
                                 OPTIMIZED, "Optimized"))
  ;

  return tid;
}

void
newradioVehicularHelper::DoInitialize ()
{
  NS_LOG_FUNCTION (this);

  // intialize the RNTI counter
  m_rntiCounter = 0;

  // create the channel
  m_channel = CreateObject<SingleModelSpectrumChannel> ();
  if (!m_propagationLossModelType.empty ())
  {
    ObjectFactory factory (m_propagationLossModelType);
    m_channel->AddPropagationLossModel (factory.Create<PropagationLossModel> ());
  }
  if (!m_spectrumPropagationLossModelType.empty ())
  {
    ObjectFactory factory (m_spectrumPropagationLossModelType);
    m_channel->AddSpectrumPropagationLossModel (factory.Create<SpectrumPropagationLossModel> ());
  }
  if (!m_propagationDelayModelType.empty ())
  {
    ObjectFactory factory (m_propagationDelayModelType);
    m_channel->SetPropagationDelayModel (factory.Create<PropagationDelayModel> ());
  }

  // 3GPP vehicular channel needs proper configuration
  if (m_spectrumPropagationLossModelType == "ns3::newradioVehicularSpectrumPropagationLossModel")
  {
    Ptr<newradioVehicularSpectrumPropagationLossModel> threeGppSplm = DynamicCast<newradioVehicularSpectrumPropagationLossModel> (m_channel->GetSpectrumPropagationLossModel ());
    PointerValue plm;
    m_channel->GetAttribute ("PropagationLossModel", plm);
    threeGppSplm->SetPathlossModel (plm.Get<PropagationLossModel> ()); // associate pathloss and fast fading models
  }
}

void
newradioVehicularHelper::SetConfigurationParameters (Ptr<newradio::newradioPhyMacCommon> conf)
{
  NS_LOG_FUNCTION (this);
  m_phyMacConfig = conf;
}

Ptr<newradio::newradioPhyMacCommon>
newradioVehicularHelper::GetConfigurationParameters () const
{
  NS_LOG_FUNCTION (this);
  return m_phyMacConfig;
}


void
newradioVehicularHelper::SetNumerology (uint8_t index)
{

  NS_LOG_FUNCTION (this);

  NS_ASSERT_MSG ( (index == 2) || (index == 3), "Numerology index is not valid.");

  m_numerologyIndex = index;

  m_phyMacConfig = CreateObject<newradio::newradioPhyMacCommon> ();

  double subcarrierSpacing = 15 * std::pow (2, m_numerologyIndex) * 1000; // subcarrier spacing based on the numerology. Only 60KHz and 120KHz is supported in NR V2X.

  m_phyMacConfig->SetSymbPerSlot(14); // TR 38.802 Section 5.3: each slot must have 14 symbols < Symbol duration is dependant on the numerology
  m_phyMacConfig->SetSlotPerSubframe(std::pow (2, m_numerologyIndex)); // flexible number of slots per subframe - depends on numerology
  m_phyMacConfig->SetSubframePeriod (1000); // TR 38.802 Section 5.3: the subframe duration is 1ms, i.e., 1000us, and the frame length is 10ms.
  m_phyMacConfig->SetSymbolPeriod ( m_phyMacConfig->GetSubframePeriod () / m_phyMacConfig->GetSlotsPerSubframe () / m_phyMacConfig->GetSymbPerSlot ()); // symbol period is required in microseconds

  double subCarriersPerRB = 12;

  m_phyMacConfig->SetNumChunkPerRB(1); // each resource block contains 1 chunk
  m_phyMacConfig->SetNumRb ( uint32_t( m_bandwidth / (subcarrierSpacing * subCarriersPerRB) ) );

  m_phyMacConfig->SetChunkWidth (subCarriersPerRB*subcarrierSpacing);

  //TODO: How many reference subcarriers per symbols should we consider to be compliant with NR-V2X? 
  m_phyMacConfig->SetNumRefScPerSym(0);

}

NetDeviceContainer
newradioVehicularHelper::InstallnewradioVehicularNetDevices (NodeContainer nodes)
{
  NS_LOG_FUNCTION (this);

  Initialize (); // run DoInitialize if necessary

  NetDeviceContainer devices;
  for (NodeContainer::Iterator i = nodes.Begin (); i != nodes.End (); ++i)
    {
      Ptr<Node> node = *i;

      // create the device
      Ptr<newradioVehicularNetDevice> device = InstallSinglenewradioVehicularNetDevice (node, ++m_rntiCounter);

      // assign an address
      device->SetAddress (Mac64Address::Allocate ());

      devices.Add (device);
    }

  return devices;
}

Ptr<newradioVehicularNetDevice>
newradioVehicularHelper::InstallSinglenewradioVehicularNetDevice (Ptr<Node> node, uint16_t rnti)
{
  NS_LOG_FUNCTION (this);

  // create the antenna
  Ptr<newradioVehicularAntennaArrayModel> aam = CreateObject<newradioVehicularAntennaArrayModel> ();

  // create and configure the tx spectrum phy
  Ptr<newradioSidelinkSpectrumPhy> ssp = CreateObject<newradioSidelinkSpectrumPhy> ();
  NS_ASSERT_MSG (node->GetObject<MobilityModel> (), "Missing mobility model");
  ssp->SetMobility (node->GetObject<MobilityModel> ());
  ssp->SetAntenna (aam);
  NS_ASSERT_MSG (m_channel, "First create the channel");
  ssp->SetChannel (m_channel);

  // add the spectrum phy to the spectrum channel
  m_channel->AddRx (ssp);

  // create and configure the chunk processor
  Ptr<newradio::newradioChunkProcessor> pData = Create<newradio::newradioChunkProcessor> ();
  pData->AddCallback (MakeCallback (&newradioSidelinkSpectrumPhy::UpdateSinrPerceived, ssp));
  ssp->AddDataSinrChunkProcessor (pData);

  // create the phy
  NS_ASSERT_MSG (m_phyMacConfig, "First set the configuration parameters");
  Ptr<newradioSidelinkPhy> phy = CreateObject<newradioSidelinkPhy> (ssp, m_phyMacConfig);

  // connect the rx callback of the spectrum object to the sink
  ssp->SetPhyRxDataEndOkCallback (MakeCallback (&newradioSidelinkPhy::Receive, phy));

  // connect the callback to report the SINR
  ssp->SetSidelinkSinrReportCallback (MakeCallback (&newradioSidelinkPhy::GenerateSinrReport, phy));

  if(m_phyTraceHelper != 0)
  {
    ssp->SetSidelinkSinrReportCallback (MakeCallback (&newradioVehicularTracesHelper::McsSinrCallback, m_phyTraceHelper));
  }

  // create the mac
  Ptr<newradioSidelinkMac> mac = CreateObject<newradioSidelinkMac> (m_phyMacConfig);
  mac->SetRnti (rnti);

  // connect phy and mac
  phy->SetPhySapUser (mac->GetPhySapUser ());
  mac->SetPhySapProvider (phy->GetPhySapProvider ());

  // create and configure the device
  Ptr<newradioVehicularNetDevice> device = CreateObject<newradioVehicularNetDevice> (phy, mac);
  node->AddDevice (device);
  device->SetNode (node);
  ssp->SetDevice (device);

  // connect the rx callback of the mac object to the rx method of the NetDevice
  mac->SetForwardUpCallback(MakeCallback(&newradioVehicularNetDevice::Receive, device));

  // initialize the channel (if needed)
  Ptr<newradioVehicularSpectrumPropagationLossModel> splm = DynamicCast<newradioVehicularSpectrumPropagationLossModel> (m_channel->GetSpectrumPropagationLossModel ());
  if (splm)
    splm->AddDevice (device, aam);

  return device;
}

void
newradioVehicularHelper::PairDevices (NetDeviceContainer devices)
{
  NS_LOG_FUNCTION (this);

  // TODO update this part to enable a more flexible configuration of the
  // scheduling pattern

  std::vector<uint16_t> pattern = CreateSchedulingPattern(devices);

  uint8_t bearerId = 1;

  for (NetDeviceContainer::Iterator i = devices.Begin (); i != devices.End (); ++i)
    {

      Ptr<newradioVehicularNetDevice> di = DynamicCast<newradioVehicularNetDevice> (*i);
      Ptr<Node> iNode = di->GetNode ();
      Ptr<Ipv4> iNodeIpv4 = iNode->GetObject<Ipv4> ();
      NS_ASSERT_MSG (iNodeIpv4 != 0, "Nodes need to have IPv4 installed before pairing can be activated");

      di->GetMac ()->SetSfAllocationInfo (pattern); // this is called ONCE for each NetDevice

      for (NetDeviceContainer::Iterator j = i + 1; j != devices.End (); ++j)
      {
        Ptr<newradioVehicularNetDevice> dj = DynamicCast<newradioVehicularNetDevice> (*j);
        Ptr<Node> jNode = dj->GetNode ();
        Ptr<Ipv4> jNodeIpv4 = jNode->GetObject<Ipv4> ();
        NS_ASSERT_MSG (jNodeIpv4 != 0, "Nodes need to have IPv4 installed before pairing can be activated");

        // initialize the <IP address, RNTI> map of the devices
        int32_t interface =  jNodeIpv4->GetInterfaceForDevice (dj);
        Ipv4Address diAddr = iNodeIpv4->GetAddress (interface, 0).GetLocal ();
        Ipv4Address djAddr = jNodeIpv4->GetAddress (interface, 0).GetLocal ();

        // register the associated devices in the PHY
        di->GetPhy ()->AddDevice (dj->GetMac ()->GetRnti (), dj);
        dj->GetPhy ()->AddDevice (di->GetMac ()->GetRnti (), di);

        // bearer activation by creating a logical channel between the two devices
        NS_LOG_DEBUG("Activation of bearer between " << diAddr << " and " << djAddr);
        NS_LOG_DEBUG("Bearer ID: " << uint32_t(bearerId) << " - Associate RNTI " << di->GetMac ()->GetRnti () << " to " << dj->GetMac ()->GetRnti ());

        di->ActivateBearer(bearerId, dj->GetMac ()->GetRnti (), djAddr);
        dj->ActivateBearer(bearerId, di->GetMac ()->GetRnti (), diAddr);
        bearerId++;
      }


    }
}

std::vector<uint16_t>
newradioVehicularHelper::CreateSchedulingPattern (NetDeviceContainer devices)
{
  NS_ABORT_MSG_IF (devices.GetN () > m_phyMacConfig->GetSlotsPerSubframe (), "Too many devices");

  // NOTE fixed scheduling pattern, set in configuration time and assumed the same for each subframe

  uint8_t slotPerSf = m_phyMacConfig->GetSlotsPerSubframe ();
  std::vector<uint16_t> pattern;

  switch (m_schedulingOpt)
  {
    case DEFAULT:
    {
      // Each slot in the subframe is assigned to a different user.
      // If (numDevices < numSlots), the remaining available slots are unused
      pattern = std::vector<uint16_t> (slotPerSf);
      for (uint16_t i = 0; i < devices.GetN (); i++)
      {
        Ptr<newradioVehicularNetDevice> di = DynamicCast<newradioVehicularNetDevice> (devices.Get (i));
        pattern.at(i) = di->GetMac ()->GetRnti ();
        NS_LOG_DEBUG ("slot " << i << " assigned to rnti " << di->GetMac ()->GetRnti ());
      }
      break;
    }
    case OPTIMIZED:
    {
      // Each slot in the subframe is used
      uint8_t slotPerDev = std::floor ( slotPerSf / devices.GetN ());
      uint8_t remainingSlots = slotPerSf % devices.GetN ();

      NS_LOG_DEBUG("Minimum number of slots per device = " << (uint16_t)slotPerDev);
      NS_LOG_DEBUG("Available slots = " << (uint16_t)slotPerSf);

      uint8_t slotCnt = 0;

      for (uint16_t i = 0; i < devices.GetN (); i++)
      {
        Ptr<newradioVehicularNetDevice> di = DynamicCast<newradioVehicularNetDevice> (devices.Get (i));

        for (uint8_t j = 0; j < slotPerDev; j++)
        {
          pattern.push_back(di->GetMac ()->GetRnti ());
          NS_LOG_DEBUG ("slot " << uint16_t(slotCnt) << " assigned to rnti " << di->GetMac ()->GetRnti ());
          slotCnt++;
        }
      }

      NS_LOG_DEBUG("Remaining slots = " << (uint16_t)remainingSlots);
      for (uint16_t i = 0; i < remainingSlots; i++)
      {
        Ptr<newradioVehicularNetDevice> di = DynamicCast<newradioVehicularNetDevice> (devices.Get (i));
        pattern.push_back(di->GetMac ()->GetRnti ());
        NS_LOG_DEBUG ("slot " << uint16_t(slotCnt) << " assigned to rnti " << di->GetMac ()->GetRnti ());
        slotCnt++;
      }
      break;
    }
    default:
    {
      NS_FATAL_ERROR("Programming Error.");
    }
  }

  return pattern;
}

void
newradioVehicularHelper::SetPropagationLossModelType (std::string plm)
{
  NS_LOG_FUNCTION (this);
  m_propagationLossModelType = plm;
}

void
newradioVehicularHelper::SetSpectrumPropagationLossModelType (std::string splm)
{
  NS_LOG_FUNCTION (this);
  m_spectrumPropagationLossModelType = splm;
}

void
newradioVehicularHelper::SetPropagationDelayModelType (std::string pdm)
{
  NS_LOG_FUNCTION (this);
  m_propagationDelayModelType = pdm;
}

void
newradioVehicularHelper::SetSchedulingPatternOptionType (SchedulingPatternOption_t spo)
{
  NS_LOG_FUNCTION (this);
  m_schedulingOpt = spo;
}

newradioVehicularHelper::SchedulingPatternOption_t
newradioVehicularHelper::GetSchedulingPatternOptionType () const
{
  NS_LOG_FUNCTION (this);
  return m_schedulingOpt;
}

} // namespace millicar
} // namespace ns3
