#ifndef BUILDINGS_OBSTACLE_PROPAGATION_LOSS_MODEL_H_
#define BUILDINGS_OBSTACLE_PROPAGATION_LOSS_MODEL_H_

#include <ns3/buildings-propagation-loss-model.h>
#include "newradio-los-tracker.h"
#include <ns3/simulator.h>
#include "newradio-phy-mac-common.h"



namespace ns3 {

namespace newradio {

class newradioBeamforming;

typedef std::pair<Ptr<MobilityModel>, Ptr<MobilityModel> > keyMob_t;

class BuildingsObstaclePropagationLossModel : public BuildingsPropagationLossModel
{

public:
  static TypeId GetTypeId (void);
  BuildingsObstaclePropagationLossModel ();
  ~BuildingsObstaclePropagationLossModel ();
  virtual double GetLoss (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;
  // inherited from PropagationLossModel
  virtual double DoCalcRxPower (double txPowerDbm, Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;

  /**
  * Set the configuration parameters which are common in the whole simulation
  * \param a pointer to the newradioPhyMacCommon object
  */
  void SetConfigurationParameters (Ptr<newradioPhyMacCommon> ptrConfig);

  void SetBeamforming (Ptr<newradioBeamforming> beamforming);
  void SetLosTracker (Ptr<newradioLosTracker> losTracker);

private:
  double newradioLosLoss (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;
  double newradioNlosLoss (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;
  double m_frequency;
  double m_lambda;
  Ptr<newradioBeamforming> m_beamforming;
  Ptr<newradioLosTracker> m_losTracker;
  Ptr<newradioPhyMacCommon> m_phyMacConfig;
};

}

}

#endif /* BUILDINGS_OBSTACLE_PROPAGATION_LOSS_MODEL_H_ */
