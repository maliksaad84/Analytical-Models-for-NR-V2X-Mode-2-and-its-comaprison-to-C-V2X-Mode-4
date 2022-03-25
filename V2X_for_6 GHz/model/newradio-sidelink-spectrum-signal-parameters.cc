#include <ns3/log.h>
#include <ns3/packet-burst.h>
#include <ns3/ptr.h>
#include "newradio-sidelink-spectrum-signal-parameters.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("newradioSidelinkSpectrumSignalParameters");

namespace millicar {

newradioSidelinkSpectrumSignalParameters::newradioSidelinkSpectrumSignalParameters ()
{
  NS_LOG_FUNCTION (this);
}

newradioSidelinkSpectrumSignalParameters::newradioSidelinkSpectrumSignalParameters (const newradioSidelinkSpectrumSignalParameters& p)
  : SpectrumSignalParameters (p)
{
  NS_LOG_FUNCTION (this << &p);
  if (p.packetBurst)
    {
      packetBurst = p.packetBurst->Copy ();
    }
  //ctrlMsgList = p.ctrlMsgList;
  slotInd = p.slotInd;
  mcs = p.mcs;
  size = p.size;
  rbBitmap = p.rbBitmap;
  pss = p.pss;
  numSym = p.numSym;
  senderRnti = p.senderRnti;
  destinationRnti = p.destinationRnti;
}

Ptr<SpectrumSignalParameters>
newradioSidelinkSpectrumSignalParameters::Copy ()
{
  NS_LOG_FUNCTION (this);
  // Ideally we would use:
  //   return Copy<newradioSpectrumSignalParameters> (*this);
  // but for some reason it doesn't work. Another alternative is
  //   return Copy<newradioSpectrumSignalParameters> (this);
  // but it causes a double creation of the object, hence it is less efficient.
  // The solution below is copied from the implementation of Copy<> (Ptr<>) in ptr.h
  Ptr<newradioSidelinkSpectrumSignalParameters> lssp (new newradioSidelinkSpectrumSignalParameters (*this), false);
  return lssp;
}

}

}
