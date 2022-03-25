#include "newradio-chunk-processor.h"

#include <ns3/log.h>
#include <ns3/spectrum-value.h>

NS_LOG_COMPONENT_DEFINE ("newradioChunkProcessor");

namespace ns3 {

namespace newradio {

newradioChunkProcessor::newradioChunkProcessor ()
{
  NS_LOG_FUNCTION (this);
}

newradioChunkProcessor::~newradioChunkProcessor ()
{
  NS_LOG_FUNCTION (this);
}

void
newradioChunkProcessor::AddCallback (newradioChunkProcessorCallback c)
{
  NS_LOG_FUNCTION (this);
  m_newradioChunkProcessorCallbacks.push_back (c);
}

void
newradioChunkProcessor::Start ()
{
  NS_LOG_FUNCTION (this);
  m_sumValues = 0;
  m_totDuration = MicroSeconds (0);
}


void
newradioChunkProcessor::EvaluateChunk (const SpectrumValue& sinr, Time duration)
{
  NS_LOG_FUNCTION (this << sinr << duration);
  if (m_sumValues == 0)
    {
      m_sumValues = Create<SpectrumValue> (sinr.GetSpectrumModel ());
    }
  (*m_sumValues) += sinr * duration.GetSeconds ();
  m_totDuration += duration;
}

void
newradioChunkProcessor::End ()
{
  NS_LOG_FUNCTION (this);
  if (m_totDuration.GetSeconds () > 0)
    {
      std::vector<newradioChunkProcessorCallback>::iterator it;
      for (it = m_newradioChunkProcessorCallbacks.begin (); it != m_newradioChunkProcessorCallbacks.end (); it++)
        {
          (*it)((*m_sumValues) / m_totDuration.GetSeconds ());
        }
    }
  else
    {
      NS_LOG_WARN ("m_numSinr == 0");
    }
}

} // namespace newradio

} // namespace ns3
