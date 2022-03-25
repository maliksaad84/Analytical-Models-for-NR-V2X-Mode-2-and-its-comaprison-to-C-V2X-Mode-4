#ifndef newradio_VEHICULAR_ANTENNA_ARRAY_MODEL_H_
#define newradio_VEHICULAR_ANTENNA_ARRAY_MODEL_H_
#include <ns3/antenna-model.h>
#include <complex>
#include <ns3/net-device.h>
#include <map>
#include <ns3/nstime.h>
#include <ns3/node.h>
#include <ns3/mobility-model.h>

namespace ns3 {

namespace millicar {

typedef std::vector< std::complex<double> > complexVector_t;

class newradioVehicularAntennaArrayModel : public AntennaModel
{
public:
  newradioVehicularAntennaArrayModel ();
  virtual ~newradioVehicularAntennaArrayModel ();
  static TypeId GetTypeId ();
  virtual double GetGainDb (Angles a);

  // to store dummy info
  void SetBeamformingVectorPanel (complexVector_t antennaWeights, Ptr<NetDevice> device = 0);
  void SetBeamformingVectorWithDelay (complexVector_t antennaWeights, Ptr<NetDevice> device = 0);

  void SetBeamformingVectorPanelDevices (Ptr<NetDevice> thisDevice = 0, Ptr<NetDevice> otherDevice = 0);
  void ChangeBeamformingVectorPanel (Ptr<NetDevice> device);
  complexVector_t GetBeamformingVectorPanel ();
  complexVector_t GetBeamformingVectorPanel (Ptr<NetDevice> device);

  void ChangeToOmniTx ();
  bool IsOmniTx ();
  double GetRadiationPattern (double vangle, double hangle = 0);
  Vector GetAntennaLocation (uint16_t index, uint16_t* antennaNum);
  void SetSector (uint8_t sector, uint16_t *antennaNum, double elevation = 90);

  void SetPlanesNumber (uint8_t planesNumber);
  double GetPlanesId (void);
  void SetDeviceType (bool isUe);
  void SetTotNoArrayElements (uint64_t arrayElements);
  uint64_t GetTotNoArrayElements () const;
  double GetOffset ();

  Ptr<NetDevice> GetCurrentDevice ();
  Time GetLastUpdate (Ptr<NetDevice> device);

private:
  bool m_omniTx;
  // double m_minAngle;
  // double m_maxAngle;
  complexVector_t m_beamformingVector;
  int m_currentPanelId;
  // std::map<Ptr<NetDevice>, complexVector_t> m_beamformingVectorMap;
  std::map<Ptr<NetDevice>, std::pair<complexVector_t,int> > m_beamformingVectorPanelMap;

  double m_disV;       //antenna spacing in the vertical direction in terms of wave length.
  double m_disH;       //antenna spacing in the horizontal direction in terms of wave length.

  uint8_t m_noPlane;
  bool m_isUe;
  uint64_t m_totNoArrayElements;
  double m_hpbw;
  double m_gMax;

  Ptr<NetDevice> m_currentDev;

  std::map<Ptr<NetDevice>, Time> m_lastUpdateMap;
  std::map<Ptr<NetDevice>, Time> m_lastUpdatePairMap;

  bool m_isotropicElement;

  std::string m_antennaElementPattern; // configuration of antenna parameters based on different 3GPP technical reports (38.901, 37.885)
};

} /* namespace millicar */

} /* namespace ns3 */

#endif /* newradio_VEHICULAR_ANTENNA_ARRAY_MODEL_H_ */
