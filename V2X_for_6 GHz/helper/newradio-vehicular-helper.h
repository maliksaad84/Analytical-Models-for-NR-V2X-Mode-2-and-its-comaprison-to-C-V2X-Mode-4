#ifndef newradio_VEHICULAR_HELPER_H
#define newradio_VEHICULAR_HELPER_H

#include "ns3/newradio-vehicular.h"
#include "ns3/node-container.h"
#include "ns3/net-device-container.h"
#include "ns3/spectrum-channel.h"
#include "ns3/newradio-phy-mac-common.h"
#include "ns3/newradio-vehicular-traces-helper.h"

namespace ns3 {

namespace millicar {

class newradioVehicularNetDevice;

/**
 * This class is used for the creation of newradioVehicularNetDevices and
 * their configuration
 */

class newradioVehicularHelper : public Object
{
public:
  /**
   * Constructor
   */
  newradioVehicularHelper (void);

  /**
   * Destructor
   */
  virtual ~newradioVehicularHelper (void);

  // inherited from Object
  static TypeId GetTypeId (void);

  /**
   * Install a newradioVehicularNetDevice on each node in the container
   * \param nodes the node container
   * \return a NetDeviceContainer containing the installed devices
   */
  NetDeviceContainer InstallnewradioVehicularNetDevices (NodeContainer nodes);

  /**
   * Set the configuration parameters
   * \param conf pointer to newradio::newradioPhyMacCommon
   */
  void SetConfigurationParameters (Ptr<newradio::newradioPhyMacCommon> conf);

  /**
   * Retrieve pointer to the object that lists all the configuration parameters
   * \return a pointer to a newradioPhyMacCommon object
   */
  Ptr<newradio::newradioPhyMacCommon> GetConfigurationParameters () const;

  /**
   * Set the propagation loss model type
   * \param plm the type id of the propagation loss model to use
   */
  void SetPropagationLossModelType (std::string plm);

  /**
   * Set the spectrum propagation loss model type
   * \param splm the type id of the spectrum propagation loss model to use
   */
  void SetSpectrumPropagationLossModelType (std::string splm);

  /**
   * Set the propagation delay model type
   * \param pdm the type id of the propagation delay model to use
   */
  void SetPropagationDelayModelType (std::string pdm);

  /**
   * Associate the devices in the container
   * \param devices the NetDeviceContainer with the devices
   */
  void PairDevices (NetDeviceContainer devices);

  /**
   * Configure the frame structure to be consistent with the NR V2X numerology
   * \param index numerology index, used to define the frame structure
   */
  void SetNumerology (uint8_t index);

  /**
   * Configure the scheduling pattern for a specific group of devices
   * \param devices the NetDeviceContainer with the devices
   * \return a vector of integers representing the scheduling pattern
  */
  std::vector<uint16_t> CreateSchedulingPattern (NetDeviceContainer devices);

  /**
   * Identifies the supported scheduling pattern policies
   */
  enum SchedulingPatternOption_t {DEFAULT = 1,
                                   OPTIMIZED = 2};

  /**
  * Set the scheduling pattern option type
  * \param spo the enum representing the scheduling pattern policy to be adopted
  */
  void SetSchedulingPatternOptionType (SchedulingPatternOption_t spo);

  /**
  * Returns the adopted scheduling pattern policy
  * \return the adopted scheduling pattern policy
  */
  SchedulingPatternOption_t GetSchedulingPatternOptionType () const;

protected:
  // inherited from Object
  virtual void DoInitialize (void) override;

private:
  /**
   * Install a newradioVehicularNetDevice on the node
   * \param n the node
   * \param rnti the RNTI
   * \return pointer to the installed NetDevice
   */
  Ptr<newradioVehicularNetDevice> InstallSinglenewradioVehicularNetDevice (Ptr<Node> n, uint16_t rnti);

  Ptr<SpectrumChannel> m_channel; //!< the SpectrumChannel
  Ptr<newradio::newradioPhyMacCommon> m_phyMacConfig; //!< the configuration parameters
  uint16_t m_rntiCounter; //!< a counter to set the RNTIs
  uint8_t m_numerologyIndex; //!< numerology index
  double m_bandwidth; //!< system bandwidth
  std::string m_propagationLossModelType; //!< the type id of the propagation loss model to be used
  std::string m_spectrumPropagationLossModelType; //!< the type id of the spectrum propagation loss model to be used
  std::string m_propagationDelayModelType; //!< the type id of the delay model to be used
  SchedulingPatternOption_t m_schedulingOpt; //!< the type of scheduling pattern policy to be adopted

  Ptr<newradioVehicularTracesHelper> m_phyTraceHelper; //!< Ptr to an helper for the physical layer traces

};

} // namespace millicar
} // namespace ns3

#endif /* newradio_VEHICULAR_HELPER_H */
