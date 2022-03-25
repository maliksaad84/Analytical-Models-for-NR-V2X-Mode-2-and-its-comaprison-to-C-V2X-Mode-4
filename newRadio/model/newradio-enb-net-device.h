#ifndef SRC_newradio_MODEL_newradio_ENB_NET_DEVICE_H_
#define SRC_newradio_MODEL_newradio_ENB_NET_DEVICE_H_


#include "newradio-net-device.h"
#include "ns3/event-id.h"
#include "ns3/traced-callback.h"
#include "ns3/nstime.h"
#include "newradio-phy.h"
#include "newradio-enb-phy.h"
#include "newradio-enb-mac.h"
#include "newradio-mac-scheduler.h"
#include "ns3/newradio-component-carrier-enb.h"
#include <vector>
#include <map>
#include <ns3/lte-enb-rrc.h>


namespace ns3 {
/* Add forward declarations here */
class Packet;
class PacketBurst;
class Node;
class LteEnbComponentCarrierManager;

namespace newradio {
//class newradioPhy;
class newradioEnbPhy;
class newradioEnbMac;

class newradioEnbNetDevice : public newradioNetDevice
{
public:
  static TypeId GetTypeId (void);

  newradioEnbNetDevice ();

  virtual ~newradioEnbNetDevice (void);
  virtual void DoDispose (void);
  virtual bool DoSend (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);

  Ptr<newradioEnbPhy> GetPhy (void) const;

  Ptr<newradioEnbPhy> GetPhy (uint8_t index);

  uint16_t GetCellId () const;

  bool HasCellId (uint16_t cellId) const;

  uint8_t GetBandwidth () const;

  void SetBandwidth (uint8_t bw);

  void SetEarfcn (uint16_t earfcn);

  uint16_t GetEarfcn () const;

  Ptr<newradioEnbMac> GetMac (void);

  Ptr<newradioEnbMac> GetMac (uint8_t index);

  void SetRrc (Ptr<LteEnbRrc> rrc);

  Ptr<LteEnbRrc> GetRrc (void);

  void SetAntennaNum (uint16_t antennaNum);

  uint16_t GetAntennaNum () const;

  std::map < uint8_t, Ptr<newradioComponentCarrierEnb> > GetCcMap ();

  void SetCcMap (std::map< uint8_t, Ptr<newradioComponentCarrierEnb> > ccm);

protected:
  virtual void DoInitialize (void);
  void UpdateConfig ();


private:
  Ptr<newradioMacScheduler> m_scheduler;

  Ptr<LteEnbRrc> m_rrc;

  uint16_t m_cellId;       /* Cell Identifer. To uniquely identify an E-nodeB  */

  uint8_t m_Bandwidth;       /* bandwidth in RBs (?) */

  uint16_t m_Earfcn;        /* carrier frequency */

  uint16_t m_antennaNum;

  std::map < uint8_t, Ptr<newradioComponentCarrierEnb> > m_ccMap;       /**< ComponentCarrier map */

  Ptr<LteEnbComponentCarrierManager> m_componentCarrierManager; ///< the component carrier manager of this eNb

  bool m_isConstructed;
  bool m_isConfigured;

};
}
}


#endif /* SRC_newradio_MODEL_newradio_ENB_NET_DEVICE_H_ */
