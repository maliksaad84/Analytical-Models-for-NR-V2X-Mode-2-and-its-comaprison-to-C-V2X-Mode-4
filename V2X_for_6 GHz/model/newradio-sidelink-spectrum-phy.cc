#include <ns3/object-factory.h>
#include <ns3/log.h>
#include <ns3/ptr.h>
#include <ns3/boolean.h>
#include <cmath>
#include <ns3/simulator.h>
#include <ns3/antenna-model.h>
#include "newradio-sidelink-spectrum-phy.h"
#include "newradio-sidelink-spectrum-signal-parameters.h"
#include <stdio.h>
#include <ns3/double.h>
#include <ns3/newradio-mi-error-model.h>
#include <ns3/newradio-vehicular-net-device.h>
#include <ns3/newradio-vehicular-antenna-array-model.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("newradioSidelinkSpectrumPhy");

namespace millicar {

NS_OBJECT_ENSURE_REGISTERED (newradioSidelinkSpectrumPhy);

static const double EffectiveCodingRate[29] = {
  0.08,
  0.1,
  0.11,
  0.15,
  0.19,
  0.24,
  0.3,
  0.37,
  0.44,
  0.51,
  0.3,
  0.33,
  0.37,
  0.42,
  0.48,
  0.54,
  0.6,
  0.43,
  0.45,
  0.5,
  0.55,
  0.6,
  0.65,
  0.7,
  0.75,
  0.8,
  0.85,
  0.89,
  0.92
};

newradioSidelinkSpectrumPhy::newradioSidelinkSpectrumPhy ()
  : m_state (IDLE),
    m_componentCarrierId (0)
{
  m_interferenceData = CreateObject<newradio::newradioInterference> ();
  m_random = CreateObject<UniformRandomVariable> ();
  m_random->SetAttribute ("Min", DoubleValue (0.0));
  m_random->SetAttribute ("Max", DoubleValue (1.0));
}

newradioSidelinkSpectrumPhy::~newradioSidelinkSpectrumPhy ()
{

}

TypeId
newradioSidelinkSpectrumPhy::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::newradioSidelinkSpectrumPhy")
    .SetParent<NetDevice> ()
    .AddAttribute ("DataErrorModelEnabled",
                   "Activate/Deactivate the error model of data [by default is active].",
                   BooleanValue (true),
                   MakeBooleanAccessor (&newradioSidelinkSpectrumPhy::m_dataErrorModelEnabled),
                   MakeBooleanChecker ())
  ;

  return tid;
}
void
newradioSidelinkSpectrumPhy::DoDispose ()
{

}

void
newradioSidelinkSpectrumPhy::Reset ()
{
  NS_LOG_FUNCTION (this);
  m_state = IDLE;
  m_endTxEvent.Cancel ();
  m_endRxDataEvent.Cancel ();
  //m_endRxCtrlEvent.Cancel ();
  //m_rxControlMessageList.clear ();
  m_rxTransportBlock.clear ();
}

void
newradioSidelinkSpectrumPhy::ResetSpectrumModel ()
{
  m_rxSpectrumModel = 0;
}

void
newradioSidelinkSpectrumPhy::SetDevice (Ptr<NetDevice> d)
{
  NS_ABORT_MSG_IF(DynamicCast<newradioVehicularNetDevice>(d) == 0,
    "The newradioSidelinkSpectrumPhy only works with newradioVehicularNetDevices");
  m_device = d;
}

Ptr<NetDevice>
newradioSidelinkSpectrumPhy::GetDevice () const
{
  return m_device;
}

void
newradioSidelinkSpectrumPhy::SetMobility (Ptr<MobilityModel> m)
{
  m_mobility = m;
}

Ptr<MobilityModel>
newradioSidelinkSpectrumPhy::GetMobility ()
{
  return m_mobility;
}

void
newradioSidelinkSpectrumPhy::SetChannel (Ptr<SpectrumChannel> c)
{
  m_channel = c;
}

Ptr<const SpectrumModel>
newradioSidelinkSpectrumPhy::GetRxSpectrumModel () const
{
  return m_rxSpectrumModel;
}

Ptr<AntennaModel>
newradioSidelinkSpectrumPhy::GetRxAntenna ()
{
  return m_antenna;
}

void
newradioSidelinkSpectrumPhy::SetAntenna (Ptr<AntennaModel> a)
{
  m_antenna = a;
}

void
newradioSidelinkSpectrumPhy::ChangeState (State newState)
{
  NS_LOG_LOGIC (this << " state: " << m_state << " -> " << newState);
  m_state = newState;
}

void
newradioSidelinkSpectrumPhy::SetNoisePowerSpectralDensity (Ptr<const SpectrumValue> noisePsd)
{
  NS_LOG_FUNCTION (this << noisePsd);
  NS_ASSERT (noisePsd);
  m_rxSpectrumModel = noisePsd->GetSpectrumModel ();
  m_interferenceData->SetNoisePowerSpectralDensity (noisePsd);
}

void
newradioSidelinkSpectrumPhy::SetTxPowerSpectralDensity (Ptr<SpectrumValue> TxPsd)
{
  m_txPsd = TxPsd;
}

void
newradioSidelinkSpectrumPhy::SetPhyRxDataEndOkCallback (newradioPhyRxDataEndOkCallback c)
{
  m_phyRxDataEndOkCallback = c;
}

// void
// newradioSidelinkSpectrumPhy::SetPhyRxCtrlEndOkCallback (newradioPhyRxCtrlEndOkCallback c)
// {
//   m_phyRxCtrlEndOkCallback = c;
// }

void
newradioSidelinkSpectrumPhy::SetSidelinkSinrReportCallback (newradioSidelinkSinrReportCallback c)
{
  NS_LOG_FUNCTION (this);
  m_slSinrReportCallback.push_back(c);
}

void
newradioSidelinkSpectrumPhy::StartRx (Ptr<SpectrumSignalParameters> params)
{

  NS_LOG_FUNCTION (this);

  Ptr<newradioSidelinkSpectrumSignalParameters> newradioSidelinkParams =
    DynamicCast<newradioSidelinkSpectrumSignalParameters> (params);

  if (newradioSidelinkParams != 0)
    {
      // TODO remove dead code after testing that the following code can be
      // handled by the newradioSidelinkSpectrumPhy state machine
      //bool isAllocated = true;

      // TODO this could be handled by a switch state machine implemented in this class
      // Ptr<newradio::newradioUeNetDevice> ueRx = 0;
      // ueRx = DynamicCast<newradio::newradioUeNetDevice> (GetDevice ());
      //
      // if ((ueRx != 0) && (ueRx->GetPhy (m_componentCarrierId)->IsReceptionEnabled () == false))
      //   {               // if the first cast is 0 (the device is MC) then this if will not be executed
      //     isAllocated = false;
      //   }

      StartRxData (newradioSidelinkParams);

    }
    else
    {
      // other type of signal that needs to be counted as interference
      m_interferenceData->AddSignal (params->psd, params->duration);
    }
}

void
newradioSidelinkSpectrumPhy::StartRxData (Ptr<newradioSidelinkSpectrumSignalParameters> params)
{
  NS_LOG_FUNCTION (this);

  switch (m_state)
    {
    case TX:
      // If there are other intereferent devices that transmit in the same slot, the current
      // device simply does not consider the signal and goes on with the transmission. The code does not raise any errors since
      // otherwise we are not able to study scenarios where interference could be an issue.
      break;
    case RX_CTRL:
      NS_FATAL_ERROR ("Cannot receive control in data period");
      break;
    case RX_DATA:
      // If this device is in the RX_DATA state and another call to StartRx is
      // triggered, it means that multiple concurrent signals are being received.
      // In this case, we assume that the device will synchronize with the first
      // received signal, while the other will act as interferers
      m_interferenceData->AddSignal (params->psd, params->duration);
      break;
    case IDLE:
      {
        // check if the packet is for this device, otherwise
        // consider it only for the interference
        m_interferenceData->AddSignal (params->psd, params->duration);
        uint16_t thisDeviceRnti =
          DynamicCast<newradioVehicularNetDevice>(m_device)->GetMac()->GetRnti();
        if(thisDeviceRnti == params->destinationRnti)
        {
          // this is a useful signal
          m_interferenceData->StartRx (params->psd);

          if (m_rxTransportBlock.empty ())
            {
              NS_ASSERT (m_state == IDLE);
              // first transmission, i.e., we're IDLE and we start RX
              m_firstRxStart = Simulator::Now ();
              m_firstRxDuration = params->duration;
              NS_LOG_LOGIC (this << " scheduling EndRx with delay " << params->duration.GetSeconds () << "s");

              m_endRxDataEvent = Simulator::Schedule (params->duration, &newradioSidelinkSpectrumPhy::EndRxData, this);
            }
          else
            {
              NS_ASSERT (m_state == RX_DATA);
              // sanity check: if there are multiple RX events, they
              // should occur at the same time and have the same
              // duration, otherwise the interference calculation
              // won't be correct
              NS_ASSERT ((m_firstRxStart == Simulator::Now ()) && (m_firstRxDuration == params->duration));
            }

          ChangeState (RX_DATA);
          if (params->packetBurst && !params->packetBurst->GetPackets ().empty ())
            {
              TbInfo_t tbInfo = {params->packetBurst, params->size, params->mcs, params->numSym, params->senderRnti, params->rbBitmap};
              m_rxTransportBlock.push_back (tbInfo);
            }
        }
        else
        {
          NS_LOG_LOGIC (this << " not in sync with this signal (rnti="
              << params->destinationRnti  << ", rnti of the device="
              << thisDeviceRnti << ")");
        }
        //m_rxControlMessageList.insert (m_rxControlMessageList.end (), params->ctrlMsgList.begin (), params->ctrlMsgList.end ());
      }
      break;
    default:
      NS_FATAL_ERROR ("Programming Error: Unknown State");
    }
}

// void
// newradioSidelinkSpectrumPhy::StartRxCtrl (Ptr<SpectrumSignalParameters> params)
// {
//   NS_LOG_FUNCTION (this);
//   switch (m_state)
//     {
//     case TX:
//       NS_FATAL_ERROR ("Cannot RX while TX");
//       break;
//     case RX_DATA:
//       NS_FATAL_ERROR ("Cannot RX data while receiving control");
//       break;
//     case RX_CTRL:
//       {
//         Ptr<newradioSidelinkSpectrumSignalParameters> sidelinkParams = DynamicCast<newradioSidelinkSpectrumSignalParameters> (params);
//         m_rxControlMessageList.insert (m_rxControlMessageList.end (), sidelinkParams->ctrlMsgList.begin (), sidelinkParams->ctrlMsgList.end ());
//         break;
//       }
//     case IDLE:
//       {
//         // first transmission, i.e., we're IDLE and we start RX
//         NS_ASSERT (m_rxControlMessageList.empty ());
//         m_firstRxStart = Simulator::Now ();
//         m_firstRxDuration = params->duration;
//         NS_LOG_LOGIC (this << " scheduling EndRx with delay " << params->duration);
//
//         // store the DCIs
//         m_rxControlMessageList = sidelinkParams->ctrlMsgList;
//         m_endRxDlCtrlEvent = Simulator::Schedule (params->duration, &newradioSidelinkSpectrumPhy::EndRxCtrl, this);
//         ChangeState (RX_CTRL);
//         break;
//       }
//     default:
//       NS_FATAL_ERROR ("Programming Error: Unknown State");
//     }
// }

void
newradioSidelinkSpectrumPhy::EndRxData ()
{
  NS_LOG_FUNCTION (this);
  m_interferenceData->EndRx ();

  double sinrAvg = Sum (m_sinrPerceived) / (m_sinrPerceived.GetSpectrumModel ()->GetNumBands ());

  NS_ASSERT (m_state = RX_DATA);

  if ((m_dataErrorModelEnabled)&&(m_rxTransportBlock.size () > 0))
  {
    for (std::list<TbInfo_t>::const_iterator i = m_rxTransportBlock.begin ();
         i != m_rxTransportBlock.end (); ++i)
     {
       // Here we need to initialize an empty harqInfoList since it is mandatory input
       // for the method. Since the vector is empty, no harq procedures are triggeres (as we want)
       std::vector <newradio::newradioHarqProcessInfoElement_t> harqInfoList;

       NS_LOG_DEBUG ("average sinr " << 10*log10 (sinrAvg) << " MCS " <<  (uint16_t)(*i).mcs);
       newradio::newradioTbStats_t tbStats = newradio::newradioMiErrorModel::GetTbDecodificationStats (m_sinrPerceived, (*i).rbBitmap, (*i).size, (*i).mcs, harqInfoList);

       // trigger callbacks
       for (auto& it : m_slSinrReportCallback)
        {
          it(m_sinrPerceived, (*i).rnti, (*i).numSym, (*i).size, (*i).mcs); // TODO also export corrupt and tbler)
        }

       bool corrupt = m_random->GetValue () > tbStats.tbler ? false : true;
       if(!corrupt)
       {
         Ptr<PacketBurst> burst = (*i).packetBurst;
         for (std::list<Ptr<Packet> >::const_iterator j = (*burst).Begin (); j != (*burst).End (); ++j)
         {
           if ((*j)->GetSize () == 0)
           {
             continue;
           }

           // Do we need the LteRadioBearerTag also here to check the rnti? I don't think so.
           NS_ASSERT_MSG (!m_phyRxDataEndOkCallback.IsNull (), "First set the rx callback");
           m_phyRxDataEndOkCallback (*j);
         }
       }
       else
       {
         NS_LOG_INFO ("TB failed");
       }
     }
  }

  // forward control messages of this frame to newradioPhy

  // if (!m_rxControlMessageList.empty () && !m_phyRxCtrlEndOkCallback.IsNull ())
  // {
  //   m_phyRxCtrlEndOkCallback (m_rxControlMessageList);
  // }

  m_state = IDLE;
  m_rxTransportBlock.clear ();
  //m_rxControlMessageList.clear ();
}

// void
// newradioSidelinkSpectrumPhy::EndRxCtrl ()
// {
//   NS_ASSERT (m_state = RX_CTRL);
//
//   // control error model not supported
//   // forward control messages of this frame to LtePhy
//   if (!m_rxControlMessageList.empty ())
//     {
//       if (!m_phyRxCtrlEndOkCallback.IsNull ())
//         {
//           m_phyRxCtrlEndOkCallback (m_rxControlMessageList);
//         }
//     }
//
//   m_state = IDLE;
//   m_rxControlMessageList.clear ();
// }

bool
newradioSidelinkSpectrumPhy::StartTxDataFrames (Ptr<PacketBurst> pb,
  Time duration,
  uint8_t slotInd,
  uint8_t mcs,
  uint32_t size,
  uint8_t numSym,
  uint16_t senderRnti,
  uint16_t destinationRnti,
  std::vector<int> rbBitmap)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT_MSG (m_channel, "First configure the SpectrumChannel");

  switch (m_state)
    {
    case RX_DATA:
    case RX_CTRL:
      NS_FATAL_ERROR ("Cannot transmit while receiving");
      break;
    case TX:
      NS_FATAL_ERROR ("Cannot transmit while a different transmission is still on");
      break;
    case IDLE:
      {
        NS_ASSERT (m_txPsd);

        m_state = TX;
        Ptr<newradioSidelinkSpectrumSignalParameters> txParams = Create<newradioSidelinkSpectrumSignalParameters> ();
        txParams->duration = duration;
        txParams->txPhy = this->GetObject<SpectrumPhy> ();
        txParams->psd = m_txPsd;
        txParams->packetBurst = pb;
        //txParams->ctrlMsgList = ctrlMsgList;
        txParams->slotInd = slotInd;
        txParams->txAntenna = m_antenna;
        txParams->mcs = mcs;
        txParams->numSym = numSym;
        txParams->destinationRnti = destinationRnti;
        txParams->senderRnti = senderRnti;
        txParams->size = size;
        txParams->rbBitmap = rbBitmap;

        m_channel->StartTx (txParams);

        // The end of the tranmission is reduced by 1 ns to avoid collision in case of a consecutive tranmission in the same slot.
        m_endTxEvent = Simulator::Schedule (duration - NanoSeconds(1.0), &newradioSidelinkSpectrumPhy::EndTx, this);
      }
      break;
    default:
      NS_LOG_FUNCTION (this << "Programming Error. Code should not reach this point");
    }
  return true;
}

// bool
// newradioSidelinkSpectrumPhy::StartTxControlFrames (std::list<Ptr<newradioControlMessage> > ctrlMsgList, Time duration)
// {
//   NS_LOG_LOGIC (this << " state: " << m_state);
//
//   switch (m_state)
//     {
//     case RX_DATA:
//     case RX_CTRL:
//       NS_FATAL_ERROR ("Cannot transmit while receiving");
//       break;
//     case TX:
//       NS_FATAL_ERROR ("Cannot transmit while a different transmission is still on");
//       break;
//     case IDLE:
//       {
//         NS_ASSERT (m_txPsd);
//
//         m_state = TX;
//
//         Ptr<newradioSidelinkSpectrumSignalParameters> txParams = Create<newradioSidelinkSpectrumSignalParameters> ();
//         txParams->duration = duration;
//         txParams->txPhy = GetObject<SpectrumPhy> ();
//         txParams->psd = m_txPsd;
//         txParams->pss = true;
//         txParams->ctrlMsgList = ctrlMsgList;
//         txParams->txAntenna = m_antenna;
//
//         m_channel->StartTx (txParams);
//
//         m_endTxEvent = Simulator::Schedule (duration, &newradioSidelinkSpectrumPhy::EndTx, this);
//       }
//     }
//   return false;
// }

void
newradioSidelinkSpectrumPhy::EndTx ()
{
  NS_ASSERT (m_state == TX);

  m_state = IDLE;
}

Ptr<SpectrumChannel>
newradioSidelinkSpectrumPhy::GetSpectrumChannel ()
{
  return m_channel;
}

// TODO remove
// void
// newradioSidelinkSpectrumPhy::SetComponentCarrierId (uint8_t componentCarrierId)
// {
//   m_componentCarrierId = componentCarrierId;
// }

void
newradioSidelinkSpectrumPhy::AddDataPowerChunkProcessor (Ptr<newradio::newradioChunkProcessor> p)
{
  m_interferenceData->AddPowerChunkProcessor (p);
}

void
newradioSidelinkSpectrumPhy::AddDataSinrChunkProcessor (Ptr<newradio::newradioChunkProcessor> p)
{
  m_interferenceData->AddSinrChunkProcessor (p);
}

void
newradioSidelinkSpectrumPhy::UpdateSinrPerceived (const SpectrumValue& sinr)
{
  NS_LOG_FUNCTION (this << sinr);
  m_sinrPerceived = sinr;

  // TODO create trace callback to fire everytime the SINR is updated
  NS_LOG_DEBUG(this << " " << Sum(m_sinrPerceived)/m_sinrPerceived.GetSpectrumModel()->GetNumBands());
}

void
newradioSidelinkSpectrumPhy::ConfigureBeamforming (Ptr<NetDevice> dev)
{
  NS_LOG_FUNCTION (this);

  Ptr<newradioVehicularAntennaArrayModel> antennaArray = DynamicCast<newradioVehicularAntennaArrayModel> (m_antenna);
  if (antennaArray)
  {
    //TODO consider to update this in order to not recompute the beamforming
    // vectors each time
    antennaArray->SetBeamformingVectorPanelDevices (m_device, dev);
  }
}

}

}
