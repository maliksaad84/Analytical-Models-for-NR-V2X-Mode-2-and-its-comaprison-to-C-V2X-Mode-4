#ifndef newradio_3GPP_PROPAGATION_LOSS_MODEL_H_
#define newradio_3GPP_PROPAGATION_LOSS_MODEL_H_


#include <ns3/propagation-loss-model.h>
#include "ns3/object.h"
#include "ns3/random-variable-stream.h"
#include <ns3/vector.h>
#include <map>
#include "newradio-phy-mac-common.h"
/*
 * This 3GPP channel model is implemented base on the 3GPP TR 38.900 v14.1.0 (2016-09).
 *
 * 3rd Generation Partnership Project;
 * Technical Specification Group Radio Access Network;
 * Study on channel model for frequency spectrum above 6 GHz
 * (Release 14)
 *
 * */

namespace ns3 {

namespace newradio {

struct channelCondition
{
  char m_channelCondition;
  double m_shadowing;
  Vector m_position;
  double m_hE;         //the effective environment height mentioned in Table 7.4.1-1 Note 1.
  double m_carPenetrationLoss;         //car penetration loss in dB.
};

// map store the path loss scenario(LOS,NLOS,OUTAGE) of each propapgation channel
typedef std::map< std::pair< Ptr<MobilityModel>, Ptr<MobilityModel> >, channelCondition> channelConditionMap_t;

class newradio3gppPropagationLossModel : public PropagationLossModel
{
public:
  static TypeId GetTypeId (void);
  newradio3gppPropagationLossModel ();

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

  char GetChannelCondition (Ptr<MobilityModel> a, Ptr<MobilityModel> b);

  std::string GetScenario ();

  double GetLoss (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;

private:
  newradio3gppPropagationLossModel (const newradio3gppPropagationLossModel &o);
  newradio3gppPropagationLossModel & operator = (const newradio3gppPropagationLossModel &o);
  virtual double DoCalcRxPower (double txPowerDbm,
                                Ptr<MobilityModel> a,
                                Ptr<MobilityModel> b) const;
  virtual int64_t DoAssignStreams (int64_t stream);
  void UpdateConditionMap (Ptr<MobilityModel> a, Ptr<MobilityModel> b, channelCondition cond) const;

  double m_lambda;
  double m_frequency;
  double m_minLoss;
  mutable channelConditionMap_t m_channelConditionMap;
  std::string m_channelConditions; //limit the channel condition to be LoS/NLoS only.
  std::string m_scenario;
  bool m_optionNlosEnabled;
  Ptr<NormalRandomVariable> m_norVar;
  Ptr<UniformRandomVariable> m_uniformVar;
  bool m_shadowingEnabled;
  bool m_inCar;
  Ptr<newradioPhyMacCommon> m_phyMacConfig;
};

} // namespace newradio

} // namespace ns3

#endif
