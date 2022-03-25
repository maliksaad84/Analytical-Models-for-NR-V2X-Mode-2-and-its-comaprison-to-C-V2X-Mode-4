#include "newradio-vehicular-antenna-array-model.h"
#include "ns3/string.h"
#include <ns3/log.h>
#include <ns3/math.h>
#include <ns3/simulator.h>
#include "ns3/double.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"


NS_LOG_COMPONENT_DEFINE ("newradioVehicularAntennaArrayModel");

namespace ns3 {

namespace millicar {

NS_OBJECT_ENSURE_REGISTERED (newradioVehicularAntennaArrayModel);

newradioVehicularAntennaArrayModel::newradioVehicularAntennaArrayModel () :
m_omniTx {false},
m_currentPanelId {0},
m_noPlane {0},
m_isUe {false},
m_totNoArrayElements {0},
m_hpbw {0},       //HPBW value of each antenna element
m_gMax {0}       //directivity value expressed in dBi and valid only for TRP (see table A.1.6-3 in 38.802)
// :m_minAngle (0),m_maxAngle(2*M_PI)
{
  m_lastUpdateMap.clear ();
  m_lastUpdatePairMap.clear ();
}

newradioVehicularAntennaArrayModel::~newradioVehicularAntennaArrayModel ()
{

}

TypeId
newradioVehicularAntennaArrayModel::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::newradioVehicularAntennaArrayModel")
    .SetParent<Object> ()
    .AddConstructor<newradioVehicularAntennaArrayModel> ()
    .AddAttribute ("AntennaHorizontalSpacing",
                   "Horizontal spacing between antenna elements, in multiples of lambda",
                   DoubleValue (0.5),
                   MakeDoubleAccessor (&newradioVehicularAntennaArrayModel::m_disH),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("AntennaVerticalSpacing",
                   "Vertical spacing between antenna elements, in multiples of lambda",
                   DoubleValue (0.5),
                   MakeDoubleAccessor (&newradioVehicularAntennaArrayModel::m_disV),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("IsotropicAntennaElements",
                   "If true, the antenna elements are isotropic. If false, they follow the 3GPP spec on element radiation pattern",
                   BooleanValue (true),
                   MakeBooleanAccessor (&newradioVehicularAntennaArrayModel::m_isotropicElement),
                   MakeBooleanChecker ())
    .AddAttribute ("AntennaElementPattern",
                   "The available antenna element patterns refer to '3GPP-newradio', '3GPP-V2V'",
                   StringValue ("3GPP-newradio"),
                   MakeStringAccessor (&newradioVehicularAntennaArrayModel ::m_antennaElementPattern),
                   MakeStringChecker ())
    .AddAttribute ("AntennaElements",
                   "The number of antenna elements",
                   UintegerValue (4),
                   MakeUintegerAccessor (&newradioVehicularAntennaArrayModel::SetTotNoArrayElements,
                                         &newradioVehicularAntennaArrayModel::GetTotNoArrayElements),
                   MakeUintegerChecker<uint64_t> ())
    .AddAttribute ("NumSectors",
                   "The number of antenna sectors",
                   UintegerValue (2),
                   MakeUintegerAccessor (&newradioVehicularAntennaArrayModel::SetPlanesNumber),
                   MakeUintegerChecker<uint8_t> ())
  ;
  return tid;
}

double
newradioVehicularAntennaArrayModel::GetGainDb (Angles a)
{
  return 0;
}
void
newradioVehicularAntennaArrayModel::SetBeamformingVectorWithDelay (complexVector_t antennaWeights, Ptr<NetDevice> device)
{
  Simulator::Schedule (MilliSeconds (8), &newradioVehicularAntennaArrayModel::SetBeamformingVectorPanel,this,antennaWeights,device);
}

void
newradioVehicularAntennaArrayModel::SetPlanesNumber (uint8_t planesNumber)
{
  m_noPlane = planesNumber;
}

double
newradioVehicularAntennaArrayModel::GetPlanesId (void)
{
  return m_currentPanelId;
}


void
newradioVehicularAntennaArrayModel::SetTotNoArrayElements (uint64_t arrayElements)
{
  m_totNoArrayElements = arrayElements;
}

uint64_t
newradioVehicularAntennaArrayModel::GetTotNoArrayElements () const
{
  return m_totNoArrayElements;
}

void
newradioVehicularAntennaArrayModel::SetDeviceType (bool isUe)
{
  m_isUe = isUe;
  if (m_antennaElementPattern == "3GPP-newradio")
  {
    if (isUe)
    {
      m_hpbw = 90;           //HPBW value of each antenna element
      m_gMax = 5;           //directivity value expressed in dBi and valid only for TRP (see table A.1.6-3 in 38.802
    }
  else
    {
      m_hpbw = 65;           //HPBW value of each antenna element
      m_gMax = 8;           //directivity value expressed in dBi and valid only for TRP (see table A.1.6-3 in 38.802
    }
  }
  else if (m_antennaElementPattern == "3GPP-V2V")
  {
      m_hpbw = 90;           //HPBW value of each antenna element
      m_gMax = 5;           //directivity value expressed in dBi and valid only in the case of V2V communication (see table 6.1.4-4 in TR 37.885)
  }
  else
  {
    NS_FATAL_ERROR("Unknown antenna element pattern");
  }
}

double
newradioVehicularAntennaArrayModel::GetOffset ()
{
  NS_LOG_DEBUG ("GetOffset " << m_currentPanelId);
  NS_LOG_DEBUG ("Offset " << m_currentPanelId * 2 * M_PI / m_noPlane);
  return m_currentPanelId * 2 * M_PI / m_noPlane;
}

void
newradioVehicularAntennaArrayModel::SetBeamformingVectorPanelDevices (Ptr<NetDevice> thisDevice, Ptr<NetDevice> otherDevice)
{
  NS_LOG_FUNCTION (this << otherDevice << Simulator::Now ());
  m_omniTx = false;
  complexVector_t antennaWeights;
  int panelId = 0;       // initialize all the variables
  if (thisDevice != 0 && otherDevice != 0)
    {
      Vector aPos = thisDevice->GetNode ()->GetObject<MobilityModel> ()->GetPosition ();
      NS_LOG_INFO ("aPos: " << aPos);
      Vector bPos = otherDevice->GetNode ()->GetObject<MobilityModel> ()->GetPosition ();
      NS_LOG_INFO ("bPos: " << bPos);

      Angles completeAngle (bPos,aPos);

      double posX = bPos.x - aPos.x;
      double phiAngle = atan ((bPos.y - aPos.y) / posX);          //horizontal angle only

      if (posX < 0)
        {
          phiAngle = phiAngle + M_PI;
        }
      if (phiAngle < 0)
        {
          phiAngle = phiAngle + 2 * M_PI;
        }

      // if (m_isUe)
      //        phiAngle = M_PI;
      // else
      //        phiAngle=0; // cast BF vectors fixed to zero [DEBUG PROCEDURE TEST LINE]

      panelId = floor (fmod (phiAngle + M_PI / m_noPlane,2 * M_PI) * m_noPlane / (2 * M_PI));         // panel id into the interval [0,N-1]

      double hAngleRadian = fmod ((phiAngle + (M_PI / m_noPlane)),2 * M_PI / m_noPlane) - (M_PI / m_noPlane);
      double vAngleRadian = completeAngle.theta;
      double power = 1 / sqrt (m_totNoArrayElements);
      uint16_t antennaNum [2];
      antennaNum[0] = sqrt (m_totNoArrayElements);
      antennaNum[1] = sqrt (m_totNoArrayElements);
      NS_LOG_INFO ("hAngleRadian: " << hAngleRadian);

      for (uint64_t ind = 0; ind < m_totNoArrayElements; ind++)
        {
          Vector loc = GetAntennaLocation (ind, antennaNum);
          double phase = -2 * M_PI * (sin (vAngleRadian) * cos (hAngleRadian) * loc.x
                                      + sin (vAngleRadian) * sin (hAngleRadian) * loc.y
                                      + cos (vAngleRadian) * loc.z);
          antennaWeights.push_back (exp (std::complex<double> (0, phase)) * power);
        }

      std::map< Ptr<NetDevice>, std::pair<complexVector_t,int> >::iterator iter = m_beamformingVectorPanelMap.find (otherDevice);
      if (iter != m_beamformingVectorPanelMap.end ())
        {
          (*iter).second = std::make_pair (antennaWeights, panelId);
          m_lastUpdatePairMap[otherDevice] = Simulator::Now ();
        }
      else
        {
          m_beamformingVectorPanelMap.insert (std::make_pair (otherDevice, std::make_pair (antennaWeights, panelId) ));
          m_lastUpdatePairMap.insert (std::make_pair (otherDevice, Simulator::Now ()));

          NS_LOG_INFO ("m_lastUpdatePairMap.size " << m_lastUpdatePairMap.size ());
        }
    }
  m_beamformingVector = antennaWeights;
  m_currentPanelId = panelId;
  m_currentDev = otherDevice;
  NS_LOG_INFO ("panelId: " << panelId);
  NS_LOG_INFO ("otherDevice: " << otherDevice);
}

// to store dummy info
void
newradioVehicularAntennaArrayModel::SetBeamformingVectorPanel (complexVector_t antennaWeights, Ptr<NetDevice> device)
{
  NS_LOG_FUNCTION (this << device << Simulator::Now ());
  m_omniTx = false;
  if (device != 0)
    {
      auto iter = m_beamformingVectorPanelMap.find (device);
      if (iter != m_beamformingVectorPanelMap.end ())
        {
          (*iter).second = std::make_pair (antennaWeights, 0);
          m_lastUpdatePairMap[device] = Simulator::Now ();
        }
      else
        {
          m_beamformingVectorPanelMap.insert (std::make_pair (device, std::make_pair (antennaWeights, 0)) );
          m_lastUpdatePairMap.insert (std::make_pair (device, Simulator::Now ()));

          NS_LOG_INFO ("m_lastUpdatePairMap.size " << m_lastUpdatePairMap.size ());
        }
    }
  // following lines are commented to store dummy info; call ChangeBeamformingVectorPanel (device) to set the antennaWeights
  // m_beamformingVector = antennaWeights;
  // m_currentDev = device;
}

void
newradioVehicularAntennaArrayModel::ChangeBeamformingVectorPanel (Ptr<NetDevice> device)
{
  NS_LOG_FUNCTION (this << device << Simulator::Now ());
  m_omniTx = false;
  std::map< Ptr<NetDevice>, std::pair<complexVector_t,int> >::iterator it = m_beamformingVectorPanelMap.find (device);
  NS_ASSERT_MSG (it != m_beamformingVectorPanelMap.end (), "could not find");
  NS_LOG_DEBUG ("ChangeBeamformingVectorPanel towards dev " << device << " prev panel " << m_currentPanelId << " updated to " << it->second.second);
  m_beamformingVector = it->second.first;
  m_currentPanelId = it->second.second;
  m_currentDev = device;
}

complexVector_t
newradioVehicularAntennaArrayModel::GetBeamformingVectorPanel ()
{
  NS_LOG_FUNCTION (this << Simulator::Now ());
  if (m_omniTx)
    {
      NS_FATAL_ERROR ("Omni transmission do not need beamforming vector");
    }
  return m_beamformingVector;
}

void
newradioVehicularAntennaArrayModel::ChangeToOmniTx ()
{
  m_omniTx = true;
}

bool
newradioVehicularAntennaArrayModel::IsOmniTx ()
{
  return m_omniTx;
}

complexVector_t
newradioVehicularAntennaArrayModel::GetBeamformingVectorPanel (Ptr<NetDevice> device)
{
  NS_LOG_FUNCTION (this << device << Simulator::Now ());
  complexVector_t weights;
  std::map< Ptr<NetDevice>, std::pair<complexVector_t,int> >::iterator it = m_beamformingVectorPanelMap.find (device);
  if (it != m_beamformingVectorPanelMap.end ())
    {
      weights = it->second.first;
    }
  else
    {
      weights = m_beamformingVector;
    }
  return weights;
}

Ptr<NetDevice>
newradioVehicularAntennaArrayModel::GetCurrentDevice ()
{
  return m_currentDev;
}

double
newradioVehicularAntennaArrayModel::GetRadiationPattern (double vAngleRadian, double hAngleRadian)
{
  if (m_isotropicElement)
    {
      return 1;
    }

  while (hAngleRadian >= M_PI)
    {
      hAngleRadian -= 2 * M_PI;
    }
  while (hAngleRadian < -M_PI)
    {
      hAngleRadian += 2 * M_PI;
    }

  double vAngle = vAngleRadian * 180 / M_PI;
  double hAngle = hAngleRadian * 180 / M_PI;
  //NS_LOG_INFO(" it is " << vAngle);
  NS_ASSERT_MSG (vAngle >= 0&&vAngle <= 180, "the vertical angle should be the range of [0,180]");
  //NS_LOG_INFO(" it is " << hAngle);
  NS_ASSERT_MSG (hAngle >= -180&&hAngle <= 180, "the horizontal angle should be the range of [-180,180]");

  double A_M = 0;       //front-back ratio expressed in dB
  double SLA = 0;       //side-lobe level limit expressed in dB

  if (m_antennaElementPattern == "3GPP-newradio") //front-back ratio and side-lobe level in case of standard newradio antenna configuration
  {
    A_M = 30;
    SLA = 30;
  }
  else if (m_antennaElementPattern == "3GPP-V2V") //front-back ratio and side-lobe level values in case of V2V antenna configuration
  {
    A_M = 25;
    SLA = 25;
  }
  else
  {
    NS_FATAL_ERROR("Unknown antenna element pattern");
  }

  double A_v = -1 * std::min (SLA,12 * pow ((vAngle - 90) / m_hpbw,2));      //TODO: check position of z-axis zero
  double A_h = -1 * std::min (A_M,12 * pow (hAngle / m_hpbw,2));
  double A = m_gMax - 1 * std::min (A_M,-1 * A_v - 1 * A_h);

  return sqrt (pow (10,A / 10));     //filed factor term converted to linear;
}

Vector
newradioVehicularAntennaArrayModel::GetAntennaLocation (uint16_t index, uint16_t* antennaNum)
{
  //assume the left bottom corner is (0,0,0), and the rectangular antenna array is on the y-z plane.
  Vector loc;
  loc.x = 0;
  loc.y = m_disH * (index % antennaNum[0]);
  loc.z = m_disV * floor (index / antennaNum[0]);
  return loc;
}

void
newradioVehicularAntennaArrayModel::SetSector (uint8_t sector, uint16_t *antennaNum, double elevation)
{
  complexVector_t tempVector;
  double hAngle_radian = M_PI * (double)sector / (double)antennaNum[1] - 0.5 * M_PI;
  double vAngle_radian = elevation * M_PI / 180;
  uint64_t size = antennaNum[0] * antennaNum[1];
  double power = 1 / sqrt (size);
  for (uint64_t ind = 0; ind < size; ind++)
    {
      Vector loc = GetAntennaLocation (ind, antennaNum);
      double phase = -2 * M_PI * (sin (vAngle_radian) * cos (hAngle_radian) * loc.x
                                  + sin (vAngle_radian) * sin (hAngle_radian) * loc.y
                                  + cos (vAngle_radian) * loc.z);
      tempVector.push_back (exp (std::complex<double> (0, phase)) * power);
    }
  m_beamformingVector = tempVector;
}

Time
newradioVehicularAntennaArrayModel::GetLastUpdate (Ptr<NetDevice> device)
{
  NS_LOG_FUNCTION (this << device << Simulator::Now ());
  auto lastUpdate = m_lastUpdateMap.find (device);
  NS_ASSERT_MSG (lastUpdate != m_lastUpdateMap.end (), "Device never updated!");
  NS_LOG_INFO ("Last update for device " << device << " at time " << lastUpdate->second.GetSeconds ());
  return lastUpdate->second;
}

} /* namespace millicar */

} /* namespace ns3 */
