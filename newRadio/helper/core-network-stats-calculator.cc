#include "core-network-stats-calculator.h"
#include "ns3/string.h"
#include "ns3/nstime.h"
#include <ns3/log.h>
#include <vector>
#include <algorithm>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CoreNetworkStatsCalculator");

namespace newradio {

NS_OBJECT_ENSURE_REGISTERED (CoreNetworkStatsCalculator);

CoreNetworkStatsCalculator::CoreNetworkStatsCalculator ()
{
  NS_LOG_FUNCTION (this);
}

CoreNetworkStatsCalculator::~CoreNetworkStatsCalculator ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
CoreNetworkStatsCalculator::GetTypeId (void)
{
  static TypeId tid =
    TypeId ("ns3::CoreNetworkStatsCalculator")
    .SetParent<Object> ().AddConstructor<CoreNetworkStatsCalculator> ()
    .SetGroupName ("Lte")
    .AddAttribute ("X2FileName",
                   "Name of the file where the packet rx on X2 will be logged.",
                   StringValue ("X2Stats.txt"),
                   MakeStringAccessor (&CoreNetworkStatsCalculator::SetX2OutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("S1MmeFileName",
                   "Name of the file where the packet rx on S1-MME at eNB or MME side will be logged.",
                   StringValue ("MmeStats.txt"),
                   MakeStringAccessor (&CoreNetworkStatsCalculator::SetMmeOutputFilename),
                   MakeStringChecker ())
  ;
  return tid;
}

void
CoreNetworkStatsCalculator::DoDispose ()
{
  NS_LOG_FUNCTION (this);
}

void
CoreNetworkStatsCalculator::LogX2Packet (std::string path, uint16_t sourceCellId, uint16_t targetCellId, uint32_t size, uint64_t delay, bool data)
{
  NS_LOG_FUNCTION (this << "LogX2Packet" << sourceCellId << targetCellId << size << delay);

  if (!m_x2OutFile.is_open ())
    {
      m_x2OutFile.open (GetX2OutputFilename ().c_str ());
    }

  m_x2OutFile << Simulator::Now ().GetNanoSeconds () / 1.0e9 << " " << sourceCellId << " " << targetCellId << " " << size << " " << delay << " " << data << std::endl;
}

void
CoreNetworkStatsCalculator::LogMmePacket (std::string path, uint16_t sourceCellId, uint16_t targetCellId, uint32_t size, uint64_t delay)
{
  NS_LOG_FUNCTION (this << "LogX2Packet" << sourceCellId << targetCellId << size << delay);

  if (!m_mmeOutFile.is_open ())
    {
      m_mmeOutFile.open (GetMmeOutputFilename ().c_str ());
    }

  m_mmeOutFile << Simulator::Now ().GetNanoSeconds () / 1.0e9 << " " << sourceCellId << " " << targetCellId << " " << size << " " << delay << std::endl;
}

std::string
CoreNetworkStatsCalculator::GetX2OutputFilename (void)
{
  return m_x2OutFileName;
}

std::string
CoreNetworkStatsCalculator::GetMmeOutputFilename (void)
{
  return m_mmeOutFileName;
}

void
CoreNetworkStatsCalculator::SetX2OutputFilename (std::string outputFilename)
{
  m_x2OutFileName = outputFilename;
}

void
CoreNetworkStatsCalculator::SetMmeOutputFilename (std::string outputFilename)
{
  m_mmeOutFileName = outputFilename;
}

} // namespace newradio

} // namespace ns3
