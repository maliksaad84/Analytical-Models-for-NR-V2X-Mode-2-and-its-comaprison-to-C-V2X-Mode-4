#ifndef SRC_newradio_MODEL_newradio_AMC_H_
#define SRC_newradio_MODEL_newradio_AMC_H_
#include <ns3/object.h>
#include <ns3/spectrum-value.h>
#include <ns3/newradio-phy-mac-common.h>

namespace ns3 {

namespace newradio {

class newradioAmc : public Object
{
public:
  static TypeId GetTypeId (void);
  newradioAmc ();
  newradioAmc (Ptr<newradioPhyMacCommon> ConfigParams);
  virtual ~newradioAmc ();
  enum AmcModel
  {
    PiroEW2010,
    MiErrorModel             // model based on 10% of BER according to LteMiErrorModel
  };

  int GetMcsFromCqi (int cqi);
  int GetTbSizeFromMcs (unsigned mcs, unsigned nprb);
  int GetTbSizeFromMcsSymbols (unsigned mcs, unsigned nsym);        // for TDMA
  int GetNumSymbolsFromTbsMcs (unsigned tbSize, unsigned mcs);
  std::vector<int> CreateCqiFeedbacks (const SpectrumValue& sinr, uint8_t rbgSize);
  std::vector<int> CreateCqiFeedbacksTdma (const SpectrumValue& sinr, uint8_t numSym);
  int CreateCqiFeedbackWbTdma (const SpectrumValue& sinr, uint8_t numSym, uint32_t tbs, int &mcsWb);
  int GetCqiFromSpectralEfficiency (double s);
  int GetMcsFromSpectralEfficiency (double s);

  static const unsigned int m_crcLen = 24;

private:
  double m_ber;
  AmcModel m_amcModel;

  Ptr<newradioPhyMacCommon> m_phyMacConfig;
  Ptr<SpectrumModel> m_lteRbModel;
};

} // end namespace newradio

} // end namespace ns3

#endif /* SRC_newradio_MODEL_newradio_AMC_H_ */
