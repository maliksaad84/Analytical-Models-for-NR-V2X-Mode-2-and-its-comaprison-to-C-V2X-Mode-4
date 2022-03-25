#ifndef SRC_newradio_HELPER_newradio_PHY_RX_TRACE_H_
#define SRC_newradio_HELPER_newradio_PHY_RX_TRACE_H_
#include <ns3/object.h>
#include <ns3/spectrum-value.h>
#include <ns3/newradio-phy-mac-common.h>
#include <fstream>
#include <iostream>

namespace ns3 {

namespace newradio {

class newradioPhyRxTrace : public Object
{
public:
  newradioPhyRxTrace ();
  virtual ~newradioPhyRxTrace ();
  static TypeId GetTypeId (void);
  static void ReportCurrentCellRsrpSinrCallback (Ptr<newradioPhyRxTrace> phyStats, std::string path,
                                                 uint64_t imsi, SpectrumValue& sinr, SpectrumValue& power);
  static void ReportPacketCountUeCallback (Ptr<newradioPhyRxTrace> phyStats, std::string path,
                                           UePhyPacketCountParameter param);
  static void ReportPacketCountEnbCallback (Ptr<newradioPhyRxTrace> phyStats, std::string path,
                                            EnbPhyPacketCountParameter param);
  static void ReportDownLinkTBSize (Ptr<newradioPhyRxTrace> phyStats, std::string path,
                                    uint64_t imsi, uint64_t tbSize);
  static void RxPacketTraceUeCallback (Ptr<newradioPhyRxTrace> phyStats, std::string path, RxPacketTraceParams param);
  static void RxPacketTraceEnbCallback (Ptr<newradioPhyRxTrace> phyStats, std::string path, RxPacketTraceParams param);
  void SetOutputFilename ( std::string fileName);

private:
  //void ReportInterferenceTrace (uint64_t imsi, SpectrumValue& sinr);
  //void ReportPacketCountUe (UePhyPacketCountParameter param);
  //void ReportPacketCountEnb (EnbPhyPacketCountParameter param);
  //void ReportDLTbSize (uint64_t imsi, uint64_t tbSize);

  static std::ofstream m_rxPacketTraceFile;
  static std::string m_rxPacketTraceFilename;
};

} // namespace newradio

} /* namespace ns3 */

#endif /* SRC_newradio_HELPER_newradio_PHY_RX_TRACE_H_ */
