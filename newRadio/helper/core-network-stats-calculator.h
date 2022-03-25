#ifndef CORE_NETWORK_STATS_CALCULATOR_H_
#define CORE_NETWORK_STATS_CALCULATOR_H_

#include "ns3/lte-common.h"
#include "ns3/uinteger.h"
#include "ns3/object.h"
#include "ns3/basic-data-calculators.h"
#include "ns3/lte-common.h"
#include <string>
#include <map>
#include <fstream>

namespace ns3 {

namespace newradio {

class CoreNetworkStatsCalculator : public Object
{
public:
  CoreNetworkStatsCalculator ();
  ~CoreNetworkStatsCalculator ();

  static TypeId GetTypeId (void);

  void DoDispose ();

  void LogX2Packet (std::string path, uint16_t sourceCellId, uint16_t targetCellId, uint32_t size, uint64_t delay, bool data);
  void LogMmePacket (std::string path, uint16_t sourceCellId, uint16_t targetCellId, uint32_t size, uint64_t delay);

  std::string GetX2OutputFilename (void);
  std::string GetMmeOutputFilename (void);
  void SetX2OutputFilename (std::string outputFilename);
  void SetMmeOutputFilename (std::string outputFilename);

private:
  std::string m_mmeOutFileName;
  std::string m_x2OutFileName;

  std::ofstream m_x2OutFile;
  std::ofstream m_mmeOutFile;

};

} // namespace newradio

} // namespace ns3

#endif
