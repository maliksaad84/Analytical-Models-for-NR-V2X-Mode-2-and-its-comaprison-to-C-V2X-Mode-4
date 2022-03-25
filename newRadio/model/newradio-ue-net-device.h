#ifndef SRC_newradio_MODEL_newradio_UE_NET_DEVICE_H_
#define SRC_newradio_MODEL_newradio_UE_NET_DEVICE_H_


#include "newradio-net-device.h"
#include "newradio-enb-net-device.h"
#include "ns3/event-id.h"
#include "ns3/traced-callback.h"
#include "ns3/nstime.h"
#include "newradio-phy.h"
#include "newradio-ue-mac.h"
#include <ns3/lte-ue-rrc.h>
#include <ns3/epc-ue-nas.h>
#include "ns3/newradio-component-carrier-ue.h"
#include <map>

namespace ns3 {

class Packet;
class PacketBurst;
class Node;
class LteUeComponentCarrierManager;
//class newradioPhy;

namespace newradio {
class newradioUePhy;
class newradioUeMac;
class newradioEnbNetDevice;

class newradioUeNetDevice : public newradioNetDevice
{
public:
  static TypeId GetTypeId (void);

  newradioUeNetDevice (void);
  virtual ~newradioUeNetDevice (void);
  virtual void DoDispose ();

  uint32_t GetCsgId () const;
  void SetCsgId (uint32_t csgId);

  void UpdateConfig (void);


  virtual bool DoSend (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);

  Ptr<newradioUePhy> GetPhy (void) const;

  Ptr<newradioUePhy> GetPhy (uint8_t index) const;

  Ptr<newradioUeMac> GetMac (void) const;

  uint64_t GetImsi () const;

  uint16_t GetEarfcn () const;

  Ptr<EpcUeNas> GetNas (void) const;

  Ptr<LteUeComponentCarrierManager> GetComponentCarrierManager (void) const;

  Ptr<LteUeRrc> GetRrc () const;

  void SetEarfcn (uint16_t earfcn);

  void SetTargetEnb (Ptr<newradioEnbNetDevice> enb);

  Ptr<newradioEnbNetDevice> GetTargetEnb (void);

  void SetAntennaNum (uint16_t antennaNum);

  uint16_t GetAntennaNum () const;

  std::map < uint8_t, Ptr<newradioComponentCarrierUe> > GetCcMap ();

  void SetCcMap (std::map< uint8_t, Ptr<newradioComponentCarrierUe> > ccm);

protected:
  // inherited from Object
  virtual void DoInitialize (void);

private:
  newradioUeNetDevice (const newradioUeNetDevice &);

  bool m_isConstructed;

  Ptr<newradioEnbNetDevice> m_targetEnb;

  Ptr<LteUeRrc> m_rrc;
  Ptr<EpcUeNas> m_nas;
  Ptr<LteUeComponentCarrierManager> m_componentCarrierManager;       ///< the component carrier manager


  uint64_t m_imsi;

  uint16_t m_earfcn;

  uint32_t m_csgId;

  std::map < uint8_t, Ptr<newradioComponentCarrierUe> > m_ccMap;       ///< CC map

  uint16_t m_antennaNum;


};

}
}
#endif /* SRC_newradio_MODEL_newradio_UE_NET_DEVICE_H_ */
