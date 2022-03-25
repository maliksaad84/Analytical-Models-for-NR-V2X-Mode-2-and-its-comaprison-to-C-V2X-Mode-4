#include "newradio-vehicular-traces-helper.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("newradioVehicularTracesHelper");

namespace millicar {

NS_OBJECT_ENSURE_REGISTERED (newradioVehicularTracesHelper);

newradioVehicularTracesHelper::newradioVehicularTracesHelper (std::string filename)
: m_filename(filename)
{
  NS_LOG_FUNCTION (this);

  if(!m_outputFile.is_open())
  {
    m_outputFile.open(m_filename.c_str());
    if (!m_outputFile.is_open ())
    {
      NS_FATAL_ERROR ("Could not open tracefile");
    }
  }
}

newradioVehicularTracesHelper::~newradioVehicularTracesHelper ()
{
  NS_LOG_FUNCTION (this);
}

void
newradioVehicularTracesHelper::McsSinrCallback(const SpectrumValue& sinr, uint16_t rnti, uint8_t numSym, uint32_t tbSize, uint8_t mcs)
{
  double sinrAvg = Sum (sinr) / (sinr.GetSpectrumModel ()->GetNumBands ());
  m_outputFile << Simulator::Now().GetSeconds() << "\t" << rnti << "\t" << 10 * std::log10 (sinrAvg) << "\t" << (uint32_t)numSym << "\t" << tbSize << "\t" << (uint32_t)mcs << std::endl;
}

}

}
