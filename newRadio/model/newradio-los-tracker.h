#ifndef newradio_LOS_TRACKER_H
#define newradio_LOS_TRACKER_H

#include <ns3/buildings-propagation-loss-model.h>
#include <ns3/simulator.h>

namespace ns3 {

namespace newradio {

typedef std::pair<Ptr<MobilityModel>, Ptr<MobilityModel> > keyMob_t;

class newradioLosTracker : public Object
{

public:
  static TypeId GetTypeId (void);
  newradioLosTracker ();
  ~newradioLosTracker ();
  /**
   *	Given the two mobility models, updates the m_mapNlos and m_mapLos
   */
  void UpdateLosNlosState (Ptr<MobilityModel> a, Ptr<MobilityModel> b);
  int GetNlosSamples (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;
  int GetLosSamples (Ptr<MobilityModel> a, Ptr<MobilityModel> b) const;

private:
  std::map< keyMob_t, int > m_mapNlos;       // map for counting number of slots in NLOS for 'drop phase'
  std::map< keyMob_t, int > m_mapLos;       // map for counting number of slots in LOS for 'raise phase'
};

}

}

#endif /* newradio_LOS_TRACKER_H */
