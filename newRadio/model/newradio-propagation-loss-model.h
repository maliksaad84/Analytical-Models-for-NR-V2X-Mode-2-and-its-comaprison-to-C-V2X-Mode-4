#ifndef SRC_newradio_MODEL_newradio_PROPAGATION_LOSS_MODEL_H_
#define SRC_newradio_MODEL_newradio_PROPAGATION_LOSS_MODEL_H_


#include <ns3/propagation-loss-model.h>
#include <ns3/newradio-phy-mac-common.h>
#include "ns3/object.h"
#include "ns3/random-variable-stream.h"
#include <map>

namespace ns3 {

namespace newradio {

struct channelScenario
{
  char m_channelScenario;
  double m_shadowing;
};

// map store the path loss scenario(LOS,NLOS,OUTAGE) of each propapgation channel
typedef std::map< std::pair< Ptr<MobilityModel>, Ptr<MobilityModel> >, channelScenario> channelScenarioMap_t;

class newradioPropagationLossModel : public PropagationLossModel
{
public:
  static TypeId GetTypeId (void);
  newradioPropagationLossModel ();

  /**
   * Set the configuration parameters which are common in the whole simulation
   * \param a pointer to the newradioPhyMacCommon object
   */
  void SetConfigurationParameters (Ptr<newradioPhyMacCommon> ptrConfig);

  /**
   * \param minLoss the minimum loss (dB)
   *
   * no matter how short the distance, the total propagation loss (in
   * dB) will always be greater or equal than this value
   */
  void SetMinLoss (double minLoss);

  /**
   * \return the minimum loss.
   */
  double GetMinLoss (void) const;

  /**
   * \returns the current frequency (Hz)
   */
  double GetFrequency (void) const;

  void SetLossFixedDb (double loss);

private:
  newradioPropagationLossModel (const newradioPropagationLossModel &o);
  newradioPropagationLossModel & operator = (const newradioPropagationLossModel &o);
  virtual double DoCalcRxPower (double txPowerDbm,
                                Ptr<MobilityModel> a,
                                Ptr<MobilityModel> b) const;
  virtual int64_t DoAssignStreams (int64_t stream);
  void UpDataScenarioMap ();

  double m_lambda;
  mutable double m_frequency;
  double m_minLoss;
  mutable channelScenarioMap_t m_channelScenarioMap;
  std::string m_channelStates;
  double m_lossFixedDb;
  bool  m_fixedLossTst;
  Ptr<newradioPhyMacCommon> m_phyMacConfig;
};

} // namespace newradio

} // namespace ns3

#endif
