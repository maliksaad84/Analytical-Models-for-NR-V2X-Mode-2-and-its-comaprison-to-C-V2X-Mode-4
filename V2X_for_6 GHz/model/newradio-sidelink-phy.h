#ifndef SRC_newradio_SIDELINK_PHY_H_
#define SRC_newradio_SIDELINK_PHY_H_

#include "newradio-sidelink-spectrum-phy.h"
#include "newradio-sidelink-sap.h"

namespace ns3 {

namespace millicar {

class newradioSidelinkPhy : public Object
{

public:

  /**
   * Dummy constructor, it is not used
   */
  newradioSidelinkPhy ();

  /**
   * newradioSidelinkPhy real constructor
   * \param channelPhy spectrum phy
   * \param confParams instance of newradio::newradioPhyMacCommon containing the
   *        configuration parameters
   *
   * Usually called by the helper. It starts the event loop for the device.
   */
  newradioSidelinkPhy (Ptr<newradioSidelinkSpectrumPhy> spectrumPhy, Ptr<newradio::newradioPhyMacCommon> confParams);

  /**
   * Desctructor
   */
  virtual ~newradioSidelinkPhy ();

  // inherited from Object
  static TypeId GetTypeId (void);
  virtual void DoInitialize (void);
  virtual void DoDispose (void);

  /**
   * Set the tx power
   * \param the tx power in dBm
   */
  void SetTxPower (double power);

  /**
   * Returns the tx power
   * \return the tx power in dBm
   */
  double GetTxPower () const;

  /**
   * Set the noise figure
   * \param the noise figure in dB
   */
  void SetNoiseFigure (double pf);

  /**
   * Returns the noise figure
   * \return the noise figure in dB
   */
  double GetNoiseFigure () const;

  /**
   * Returns the newradio::newradioPhyMacCommon instance associated with this phy containing
   * the configuration parameters
   * \return the newradio::newradioPhyMacCommon instance
   */
  Ptr<newradio::newradioPhyMacCommon> GetConfigurationParameters (void) const;

  /**
   * Returns the SpectrumPhy instance associated with this phy
   * \return the SpectrumPhy instance
   */
  Ptr<newradioSidelinkSpectrumPhy> GetSpectrumPhy () const;

  /**
  * Get the PHY SAP provider
  * \return a pointer to the SAP provider to the MAC
  */
  newradioSidelinkPhySapProvider* GetPhySapProvider () const;

  /**
  * Set the PHY SAP user
  * \param sap the PHY SAP user
  */
  void SetPhySapUser (newradioSidelinkPhySapUser* sap);

  /**
   * Add a <rnti, device> pair to m_deviceMap. All the devices we want to
   * communicate with must be inserted in this map, otherwise we would not
   * be able to correctly configure the beamforming.
   * \param rnti the RNTI identifier
   * \param dev pointer to the NetDevice object
   */
  void AddDevice (uint64_t rnti, Ptr<NetDevice> dev);

  /**
   * Add a transport block to the transmission buffer, which will be sent in the
   * current slot.
   * \param pb the packet burst containing the packets to be sent
   * \param info the newradio::SlotAllocInfo instance containg the transmission information
   */
  void DoAddTransportBlock (Ptr<PacketBurst> pb, newradio::SlotAllocInfo info);

  /**
   * Prepare for the reception from another device by properly configuring
   * the beamforming vector
   * \param rnti the RNTI of the transmitting device
   */
  void DoPrepareForReceptionFrom (uint16_t rnti);

  /**
  * Receive the packet from SpectrumPhy and forward it up to the MAC
  * \param p received packet
  */
  void Receive (Ptr<Packet> p);

  /**
  * \brief This method generates a new SINR report and sends it to the MAC layer.
           It is hooked to the callback newradioSidelinkSpectrumPhy::m_slSinrReportCallback
  * \param sinr pointer to the SpectrumValue instance representing the SINR
            measured on all the spectrum chunks
  * \param rnti the RNTI of the tranmitting device
  * \param numSym size of the transport block that generated the report in
            number of OFDM symbols
  * \param tbSize size of the transport block that generated the report in bytes
  * \param mcs the MCS of the transmission
  */
  void GenerateSinrReport (const SpectrumValue& sinr, uint16_t rnti, uint8_t numSym, uint32_t tbSize, uint8_t mcs);

private:

  /**
   * Start a slot. Send all the transport blocks in the buffer.
   * \param timingInfo the structure containing the timing information
   */
  void StartSlot (newradio::SfnSf timingInfo);

  /**
   * Transmit a transport block
   * \param pb the packet burst containing the packets to be sent
   * \param info the newradio::SlotAllocInfo instance containg the transmission information
   * \return the number of symbols used to send this TB
   */
  uint8_t SlData (Ptr<PacketBurst> pb, newradio::SlotAllocInfo info);

  /**
   * Set the transmission mask and creates the power spectral density for the
   * transmission
   * \return mask indicating the suchannels used for the transmission
   */
  std::vector<int> SetSubChannelsForTransmission ();

  /**
   * Send the packet burts
   * \param pb the packet burst
   * \param duration the duration of the transmissin
   * \param info the newradio::SlotAllocInfo instance containg the transmission information
   * \param rbBitmap the mask indicating the suchannels to be used for the
            transmission
   */
  void SendDataChannels (Ptr<PacketBurst> pb, Time duration, newradio::SlotAllocInfo info, std::vector<int> rbBitmap);

  /**
   * TODO: this can be done by overloading the operator ++ of the newradio::SfnSf struct
   * Update the newradio::SfnSf structure to point to the next slot. If the current slot
   * the last slot of the subframe, the next slot index will be 0 and the
   * subframe index will be incremented. If the current subframe is the last
   * subframe of the frame, the next subframe index will be 0 and the frame
   * frame index will be incremented.
   * \param info the newradio::SfnSf structure containg frame, subframe and slot indeces
   * \return the updated SnfSn structure pointing to the next slot
   */
  newradio::SfnSf UpdateTimingInfo (newradio::SfnSf info) const;

  newradioSidelinkPhySapUser* m_phySapUser; //!< Sidelink PHY SAP user
  newradioSidelinkPhySapProvider* m_phySapProvider; //!< Sidelink PHY SAP provider
  double m_txPower; //!< the transmission power in dBm
  double m_noiseFigure; //!< the noise figure in dB
  Ptr<newradioSidelinkSpectrumPhy> m_sidelinkSpectrumPhy; //!< the SpectrumPhy instance associated with this PHY
  Ptr<newradio::newradioPhyMacCommon> m_phyMacConfig; //!< the configuration parameters
  typedef std::pair<Ptr<PacketBurst>, newradio::SlotAllocInfo> PhyBufferEntry; //!< type of the phy buffer entries
  std::list<PhyBufferEntry> m_phyBuffer; //!< buffer of transport blocks to send in the current slot
  std::map<uint64_t, Ptr<NetDevice>> m_deviceMap; //!< map containing the <rnti, device> pairs of the nodes we want to communicate with
};

class MacSidelinkMemberPhySapProvider : public newradioSidelinkPhySapProvider
{

public:
  MacSidelinkMemberPhySapProvider (Ptr<newradioSidelinkPhy> phy);

  void AddTransportBlock (Ptr<PacketBurst> pb, newradio::SlotAllocInfo info) override;

  void PrepareForReception (uint16_t rnti) override;

private:
  Ptr<newradioSidelinkPhy> m_phy;

};

} // namespace millicar
} // namespace ns3

#endif /* SRC_newradio_SIDELINK_PHY_H_ */
