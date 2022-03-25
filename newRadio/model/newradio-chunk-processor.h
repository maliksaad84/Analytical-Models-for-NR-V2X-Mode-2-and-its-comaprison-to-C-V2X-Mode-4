#ifndef newradio_CHUNK_PROCESSOR_H
#define newradio_CHUNK_PROCESSOR_H

#include <ns3/ptr.h>
#include <ns3/nstime.h>
#include <ns3/object.h>

namespace ns3 {

class SpectrumValue;

namespace newradio {

typedef Callback< void, const SpectrumValue& > newradioChunkProcessorCallback;

class newradioChunkProcessor : public SimpleRefCount<newradioChunkProcessor>
{
public:
  newradioChunkProcessor ();
  virtual ~newradioChunkProcessor ();

  virtual void AddCallback (newradioChunkProcessorCallback c);

  virtual void Start ();

  virtual void EvaluateChunk (const SpectrumValue& sinr, Time duration);

  virtual void End ();

private:
  Ptr<SpectrumValue> m_sumValues;
  Time m_totDuration;

  std::vector<newradioChunkProcessorCallback> m_newradioChunkProcessorCallbacks;
};

} // namespace newradio

} // namespace ns3



#endif /* newradio_CHUNK_PROCESSOR_H */
