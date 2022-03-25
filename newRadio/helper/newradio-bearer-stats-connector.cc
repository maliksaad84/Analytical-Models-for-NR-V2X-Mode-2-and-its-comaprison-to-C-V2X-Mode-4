#include "newradio-bearer-stats-calculator.h"
#include "newradio-bearer-stats-connector.h"

#include <ns3/log.h>
#include <ns3/config.h>

#include "ns3/string.h"
#include "ns3/nstime.h"

#include <ns3/lte-enb-rrc.h>
#include <ns3/lte-enb-net-device.h>
#include <ns3/lte-ue-rrc.h>
#include <ns3/lte-ue-net-device.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("newradioBearerStatsConnector");

namespace newradio {

NS_OBJECT_ENSURE_REGISTERED (newradioBearerStatsConnector);

/**
  * Less than operator for CellIdRnti, because it is used as key in map
  */
bool
operator < (const newradioBearerStatsConnector::CellIdRnti& a, const newradioBearerStatsConnector::CellIdRnti& b)
{
  return ( (a.cellId < b.cellId) || ( (a.cellId == b.cellId) && (a.rnti < b.rnti) ) );
}

/**
 * This structure is used as interface between trace
 * sources and newradioBearerStatsCalculator. It stores
 * and provides calculators with cellId and IMSI,
 * because most trace sources do not provide it.
 */
struct newradioBoundCallbackArgument : public SimpleRefCount<newradioBoundCallbackArgument>
{
public:
  Ptr<newradioBearerStatsCalculator> stats;  //!< statistics calculator
  uint64_t imsi; //!< imsi
  uint16_t cellId; //!< cellId
};

struct McnewradioBoundCallbackArgument : public SimpleRefCount<McnewradioBoundCallbackArgument>
{
public:
  Ptr<McStatsCalculator> stats;
};

/**
 * Callback function for DL TX statistics for both RLC and PDCP
 * /param arg
 * /param path
 * /param rnti
 * /param lcid
 * /param packetSize
 */
void
DlTxPduCallback (Ptr<newradioBoundCallbackArgument> arg, std::string path,
                 uint16_t rnti, uint8_t lcid, uint32_t packetSize)
{
  NS_LOG_FUNCTION (path << rnti << (uint16_t)lcid << packetSize);
  arg->stats->DlTxPdu (arg->cellId, arg->imsi, rnti, lcid, packetSize);
}

/**
 * Callback function for DL RX statistics for both RLC and PDCP
 * /param arg
 * /param path
 * /param rnti
 * /param lcid
 * /param packetSize
 * /param delay
 */
void
DlRxPduCallback (Ptr<newradioBoundCallbackArgument> arg, std::string path,
                 uint16_t rnti, uint8_t lcid, uint32_t packetSize, uint64_t delay)
{
  NS_LOG_FUNCTION (path << rnti << (uint16_t)lcid << packetSize << delay);
  arg->stats->DlRxPdu (arg->cellId, arg->imsi, rnti, lcid, packetSize, delay);
}

/**
 * Callback function for UL TX statistics for both RLC and PDCP
 * /param arg
 * /param path
 * /param rnti
 * /param lcid
 * /param packetSize
 */
void
UlTxPduCallback (Ptr<newradioBoundCallbackArgument> arg, std::string path,
                 uint16_t rnti, uint8_t lcid, uint32_t packetSize)
{
  NS_LOG_FUNCTION (path << rnti << (uint16_t)lcid << packetSize);

  arg->stats->UlTxPdu (arg->cellId, arg->imsi, rnti, lcid, packetSize);
}

/**
 * Callback function for UL RX statistics for both RLC and PDCP
 * /param arg
 * /param path
 * /param rnti
 * /param lcid
 * /param packetSize
 * /param delay
 */
void
UlRxPduCallback (Ptr<newradioBoundCallbackArgument> arg, std::string path,
                 uint16_t rnti, uint8_t lcid, uint32_t packetSize, uint64_t delay)
{
  NS_LOG_FUNCTION (path << rnti << (uint16_t)lcid << packetSize << delay);

  arg->stats->UlRxPdu (arg->cellId, arg->imsi, rnti, lcid, packetSize, delay);
}

void
SwitchToLteCallback (Ptr<McnewradioBoundCallbackArgument> arg, std::string path, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (path << rnti << cellId << imsi);

  arg->stats->SwitchToLte (imsi, cellId, rnti);
}

void
SwitchTonewradioCallback (Ptr<McnewradioBoundCallbackArgument> arg, std::string path, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (path << rnti << cellId << imsi);

  arg->stats->SwitchTonewradio (imsi, cellId, rnti);
}


newradioBearerStatsConnector::newradioBearerStatsConnector ()
  : m_connected (false),
    m_enbHandoverStartFilename ("EnbHandoverStartStats.txt"),
    m_enbHandoverEndFilename ("EnbHandoverEndStats.txt"),
    m_ueHandoverStartFilename ("UeHandoverStartStats.txt"),
    m_ueHandoverEndFilename ("UeHandoverEndStats.txt"),
    m_cellIdInTimeHandoverFilename ("CellIdStatsHandover.txt")
{

}

newradioBearerStatsConnector::~newradioBearerStatsConnector ()
{
  NS_LOG_FUNCTION (this);
  if (m_enbHandoverStartOutFile.is_open ())
    {
      m_enbHandoverStartOutFile.close ();
    }
  if (m_ueHandoverStartOutFile.is_open ())
    {
      m_ueHandoverStartOutFile.close ();
    }
  if (m_enbHandoverEndOutFile.is_open ())
    {
      m_enbHandoverEndOutFile.close ();
    }
  if (m_ueHandoverEndOutFile.is_open ())
    {
      m_ueHandoverEndOutFile.close ();
    }

  if (m_cellIdInTimeHandoverOutFile.is_open ())
    {
      m_cellIdInTimeHandoverOutFile.close ();
    }
  if (m_newradioSinrOutFile.is_open ())
    {
      m_newradioSinrOutFile.close ();
    }
  if (m_lteSinrOutFile.is_open ())
    {
      m_lteSinrOutFile.close ();
    }
}

TypeId
newradioBearerStatsConnector::GetTypeId (void)
{
  static TypeId tid =
    TypeId ("ns3::newradioBearerStatsConnector")
    .SetParent<Object> ()
    .AddConstructor<newradioBearerStatsConnector> ()
    .SetGroupName ("Lte")
    .AddAttribute ("EnbHandoverStartOutputFilename",
                   "Name of the file where the eNB handover start traces will be saved.",
                   StringValue ("EnbHandoverStartStats.txt"),
                   MakeStringAccessor (&newradioBearerStatsConnector::SetEnbHandoverStartOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("EnbHandoverEndOutputFilename",
                   "Name of the file where the eNB handover end traces will be saved.",
                   StringValue ("EnbHandoverEndStats.txt"),
                   MakeStringAccessor (&newradioBearerStatsConnector::SetEnbHandoverEndOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("newradioSinrOutputFilename",
                   "Name of the file where the newradio eNB sinr will be saved.",
                   StringValue ("newradioSinrTime.txt"),
                   MakeStringAccessor (&newradioBearerStatsConnector::SetnewradioSinrOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("LteSinrOutputFilename",
                   "Name of the file where the LTE eNB sinr will be saved.",
                   StringValue ("LteSinrTime.txt"),
                   MakeStringAccessor (&newradioBearerStatsConnector::SetLteSinrOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("UeHandoverStartOutputFilename",
                   "Name of the file where the UE handover start events will be saved.",
                   StringValue ("UeHandoverStartStats.txt"),
                   MakeStringAccessor (&newradioBearerStatsConnector::SetUeHandoverStartOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("UeHandoverEndOutputFilename",
                   "Name of the file where the UE handover end events will be saved.",
                   StringValue ("UeHandoverEndStats.txt"),
                   MakeStringAccessor (&newradioBearerStatsConnector::SetUeHandoverEndOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("CellIdStatsHandoverOutputFilename",
                   "Name of the file where the current cellId for the UE will be stored.",
                   StringValue ("CellIdStatsHandover.txt"),
                   MakeStringAccessor (&newradioBearerStatsConnector::SetCellIdStatsOutputFilename),
                   MakeStringChecker ())
  ;
  return tid;
}

void
newradioBearerStatsConnector::DoDispose ()
{
  NS_LOG_FUNCTION (this);
}

void
newradioBearerStatsConnector::EnableRlcStats (Ptr<newradioBearerStatsCalculator> rlcStats)
{
  m_rlcStats = rlcStats;
  EnsureConnected ();
}

void
newradioBearerStatsConnector::EnablePdcpStats (Ptr<newradioBearerStatsCalculator> pdcpStats)
{
  m_pdcpStats = pdcpStats;
  EnsureConnected ();
}

void
newradioBearerStatsConnector::EnableMcStats (Ptr<McStatsCalculator> mcStats)
{
  m_mcStats = mcStats;
  EnsureConnected ();
}

void
newradioBearerStatsConnector::EnsureConnected ()
{
  NS_LOG_FUNCTION (this);
  if (!m_connected)
    {
      Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/NewUeContext",
                       MakeBoundCallback (&newradioBearerStatsConnector::NotifyNewUeContextEnb, this));
      Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/RandomAccessSuccessful",
                       MakeBoundCallback (&newradioBearerStatsConnector::NotifyRandomAccessSuccessfulUe, this));
      Config::Connect ("/NodeList/*/DeviceList/*/newradioUeRrc/RandomAccessSuccessful",
                       MakeBoundCallback (&newradioBearerStatsConnector::NotifyRandomAccessSuccessfulUe, this));
      Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/ConnectionReconfiguration",
                       MakeBoundCallback (&newradioBearerStatsConnector::NotifyConnectionReconfigurationEnb, this));
      Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/ConnectionReconfiguration",
                       MakeBoundCallback (&newradioBearerStatsConnector::NotifyConnectionReconfigurationUe, this));
      Config::Connect ("/NodeList/*/DeviceList/*/newradioUeRrc/ConnectionReconfiguration",
                       MakeBoundCallback (&newradioBearerStatsConnector::NotifyConnectionReconfigurationUe, this));
      Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverStart",
                       MakeBoundCallback (&newradioBearerStatsConnector::NotifyHandoverStartEnb, this));
      Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/HandoverStart",
                       MakeBoundCallback (&newradioBearerStatsConnector::NotifyHandoverStartUe, this));
      Config::Connect ("/NodeList/*/DeviceList/*/newradioUeRrc/HandoverStart",
                       MakeBoundCallback (&newradioBearerStatsConnector::NotifyHandoverStartUe, this));
      Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverEndOk",
                       MakeBoundCallback (&newradioBearerStatsConnector::NotifyHandoverEndOkEnb, this));
      Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/HandoverEndOk",
                       MakeBoundCallback (&newradioBearerStatsConnector::NotifyHandoverEndOkUe, this));
      Config::Connect ("/NodeList/*/DeviceList/*/newradioUeRrc/HandoverEndOk",
                       MakeBoundCallback (&newradioBearerStatsConnector::NotifyHandoverEndOkUe, this));
      Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/SwitchTonewradio",
                       MakeBoundCallback (&newradioBearerStatsConnector::NotifySwitchTonewradioUe, this));
      // newradio SINR from RT, LTE SINR from the PHY callbacks
      Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/NotifynewradioSinr",
                       MakeBoundCallback (&newradioBearerStatsConnector::NotifynewradioSinr, this));
      Config::Connect ("/NodeList/*/DeviceList/*/LteUePhy/ReportCurrentCellRsrpSinr",
                       MakeBoundCallback (&newradioBearerStatsConnector::NotifyLteSinr, this));
      m_connected = true;
    }
}

void
newradioBearerStatsConnector::NotifyRandomAccessSuccessfulUe (newradioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  c->ConnectSrb0Traces (context, imsi, cellId, rnti);
}

void
newradioBearerStatsConnector::NotifyConnectionSetupUe (newradioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  c->ConnectSrb1TracesUe (context, imsi, cellId, rnti);
}

void
newradioBearerStatsConnector::NotifyConnectionReconfigurationUe (newradioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  c->ConnectTracesUeIfFirstTime (context, imsi, cellId, rnti);
}

void
newradioBearerStatsConnector::NotifyHandoverStartUe (newradioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti, uint16_t targetCellId)
{
  c->PrintUeStartHandover (imsi, cellId, targetCellId, rnti);
  c->DisconnectTracesUe (context, imsi, cellId, rnti);
}

void
newradioBearerStatsConnector::NotifyHandoverEndOkUe (newradioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  c->PrintUeEndHandover (imsi, cellId, rnti);
  c->ConnectSrb1TracesUe (context, imsi, cellId, rnti);
  c->ConnectDrbTracesUe (context, imsi, cellId, rnti);
}

void
newradioBearerStatsConnector::NotifyNewUeContextEnb (newradioBearerStatsConnector* c, std::string context, uint16_t cellId, uint16_t rnti)
{
  c->StoreUeManagerPath (context, cellId, rnti);
}

void
newradioBearerStatsConnector::NotifyConnectionReconfigurationEnb (newradioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  c->ConnectTracesEnbIfFirstTime (context, imsi, cellId, rnti);
}

void
newradioBearerStatsConnector::NotifyHandoverStartEnb (newradioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti, uint16_t targetCellId)
{
  c->PrintEnbStartHandover (imsi, cellId, targetCellId, rnti);
  c->DisconnectTracesEnb (context, imsi, cellId, rnti);
}

void
newradioBearerStatsConnector::NotifyHandoverEndOkEnb (newradioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  c->PrintEnbEndHandover (imsi, cellId, rnti);
  c->ConnectSrb1TracesEnb (context, imsi, cellId, rnti);
  c->ConnectDrbTracesEnb (context, imsi, cellId, rnti);
}

void
newradioBearerStatsConnector::NotifySwitchTonewradioUe (newradioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  c->ConnectSecondaryTracesUe (context, imsi, cellId, rnti);
}

void
newradioBearerStatsConnector::NotifySecondarynewradioEnbAvailable (newradioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  c->ConnectSecondaryTracesEnb (context, imsi, cellId, rnti);
}

void
newradioBearerStatsConnector::NotifynewradioSinr (newradioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellId, long double sinr)
{
  c->PrintnewradioSinr (imsi, cellId, sinr);
}

void
newradioBearerStatsConnector::PrintnewradioSinr (uint64_t imsi, uint16_t cellId, long double sinr)
{
  NS_LOG_FUNCTION (this << " PrintnewradioSinr " << Simulator::Now ().GetSeconds ());
  if (!m_newradioSinrOutFile.is_open ())
    {
      m_newradioSinrOutFile.open (GetnewradioSinrOutputFilename ().c_str ());
    }
  m_newradioSinrOutFile << Simulator::Now ().GetNanoSeconds () / 1.0e9 << " " << imsi << " " << cellId << " " << 10 * std::log10 (sinr) << std::endl;
}

void
newradioBearerStatsConnector::NotifyLteSinr (newradioBearerStatsConnector* c, std::string context, uint16_t cellId, uint16_t rnti, double rsrp, double sinr, uint8_t cc)
{
  c->PrintLteSinr (rnti, cellId, sinr);
}

void
newradioBearerStatsConnector::PrintLteSinr (uint16_t rnti, uint16_t cellId, double sinr)
{
  NS_LOG_FUNCTION (this << " PrintLteSinr " << Simulator::Now ().GetSeconds ());
  if (!m_lteSinrOutFile.is_open ())
    {
      m_lteSinrOutFile.open (GetLteSinrOutputFilename ().c_str ());
    }
  m_lteSinrOutFile << Simulator::Now ().GetNanoSeconds () / 1.0e9 << " " << rnti << " " << cellId << " " << sinr << std::endl;
}

std::string
newradioBearerStatsConnector::GetEnbHandoverStartOutputFilename (void)
{
  return m_enbHandoverStartFilename;
}

std::string
newradioBearerStatsConnector::GetEnbHandoverEndOutputFilename (void)
{
  return m_enbHandoverEndFilename;
}

std::string
newradioBearerStatsConnector::GetnewradioSinrOutputFilename (void)
{
  return m_newradioSinrOutputFilename;
}

std::string
newradioBearerStatsConnector::GetLteSinrOutputFilename (void)
{
  return m_lteSinrOutputFilename;
}

std::string
newradioBearerStatsConnector::GetUeHandoverStartOutputFilename (void)
{
  return m_ueHandoverStartFilename;
}

std::string
newradioBearerStatsConnector::GetUeHandoverEndOutputFilename (void)
{
  return m_ueHandoverEndFilename;
}

std::string
newradioBearerStatsConnector::GetCellIdStatsOutputFilename (void)
{
  return m_cellIdInTimeHandoverFilename;
}

void newradioBearerStatsConnector::SetEnbHandoverEndOutputFilename (std::string outputFilename)
{
  m_enbHandoverEndFilename = outputFilename;
}

void newradioBearerStatsConnector::SetEnbHandoverStartOutputFilename (std::string outputFilename)
{
  m_enbHandoverStartFilename = outputFilename;
}

void
newradioBearerStatsConnector::SetUeHandoverStartOutputFilename (std::string outputFilename)
{
  m_ueHandoverStartFilename = outputFilename;
}

void
newradioBearerStatsConnector::SetUeHandoverEndOutputFilename (std::string outputFilename)
{
  m_ueHandoverEndFilename = outputFilename;
}

void
newradioBearerStatsConnector::SetCellIdStatsOutputFilename (std::string outputFilename)
{
  m_cellIdInTimeHandoverFilename = outputFilename;
}

void
newradioBearerStatsConnector::SetnewradioSinrOutputFilename (std::string outputFilename)
{
  m_newradioSinrOutputFilename = outputFilename;
}

void
newradioBearerStatsConnector::SetLteSinrOutputFilename (std::string outputFilename)
{
  m_lteSinrOutputFilename = outputFilename;
}

void
newradioBearerStatsConnector::PrintEnbStartHandover (uint64_t imsi, uint16_t sourceCellid, uint16_t targetCellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this << " NotifyHandoverStartEnb " << Simulator::Now ().GetSeconds ());
  if (!m_enbHandoverStartOutFile.is_open ())
    {
      m_enbHandoverStartOutFile.open (GetEnbHandoverStartOutputFilename ().c_str ());
    }
  m_enbHandoverStartOutFile << Simulator::Now ().GetNanoSeconds () / 1.0e9 << " " << imsi << " " << rnti << " " << sourceCellid << " " << targetCellId << std::endl;
}

void
newradioBearerStatsConnector::PrintEnbEndHandover (uint64_t imsi, uint16_t targetCellId, uint16_t rnti)
{
  NS_LOG_FUNCTION ("NotifyHandoverOkEnb " << Simulator::Now ().GetSeconds ());
  if (!m_enbHandoverEndOutFile.is_open ())
    {
      m_enbHandoverEndOutFile.open (GetEnbHandoverEndOutputFilename ().c_str ());
    }
  m_enbHandoverEndOutFile << Simulator::Now ().GetNanoSeconds () / 1.0e9 << " " << imsi << " " << rnti << " " << targetCellId << std::endl;
}

void
newradioBearerStatsConnector::PrintUeStartHandover (uint64_t imsi, uint16_t sourceCellid, uint16_t targetCellId, uint16_t rnti)
{
  NS_LOG_FUNCTION ("NotifyHandoverStartUe " << Simulator::Now ().GetSeconds ());
  if (!m_ueHandoverStartOutFile.is_open ())
    {
      m_ueHandoverStartOutFile.open (GetUeHandoverStartOutputFilename ().c_str ());
    }
  m_ueHandoverStartOutFile << Simulator::Now ().GetNanoSeconds () / 1.0e9 << " " << imsi << " " << rnti << " " << sourceCellid << " " << targetCellId << std::endl;
}

void
newradioBearerStatsConnector::PrintUeEndHandover (uint64_t imsi, uint16_t targetCellId, uint16_t rnti)
{
  NS_LOG_FUNCTION ("NotifyHandoverOkUe " << Simulator::Now ().GetSeconds ());
  if (!m_ueHandoverEndOutFile.is_open ())
    {
      m_ueHandoverEndOutFile.open (GetUeHandoverEndOutputFilename ().c_str ());
    }
  m_ueHandoverEndOutFile << Simulator::Now ().GetNanoSeconds () / 1.0e9 << " " << imsi << " " << rnti << " " << targetCellId << std::endl;

  if (!m_cellIdInTimeHandoverOutFile.is_open ())
    {
      m_cellIdInTimeHandoverOutFile.open (GetCellIdStatsOutputFilename ().c_str ());
    }
  m_cellIdInTimeHandoverOutFile << Simulator::Now ().GetNanoSeconds () / 1.0e9 << " " << imsi << " " << rnti << " " << targetCellId << std::endl;
}

void
newradioBearerStatsConnector::StoreUeManagerPath (std::string context, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this << context << cellId << rnti);
  std::ostringstream ueManagerPath;
  ueManagerPath <<  context.substr (0, context.rfind ("/")) << "/UeMap/" << (uint32_t) rnti;
  CellIdRnti key;
  key.cellId = cellId;
  key.rnti = rnti;
  m_ueManagerPathByCellIdRnti[key] = ueManagerPath.str ();

  if(m_rlcStats)
  {
      Config::Connect (ueManagerPath.str () + "/SecondaryRlcCreated",
                 MakeBoundCallback (&NotifySecondarynewradioEnbAvailable, this));
  }
}

void
newradioBearerStatsConnector::ConnectSrb0Traces (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this << imsi << cellId << rnti);
  std::string ueRrcPath =  context.substr (0, context.rfind ("/"));
  CellIdRnti key;
  key.cellId = cellId;
  key.rnti = rnti;
  std::map<CellIdRnti, std::string>::iterator it = m_ueManagerPathByCellIdRnti.find (key);
  NS_ASSERT (it != m_ueManagerPathByCellIdRnti.end ());
  std::string ueManagerPath = it->second;
  NS_LOG_LOGIC (this << " ueManagerPath: " << ueManagerPath);
  m_ueManagerPathByCellIdRnti.erase (it);

  if (m_rlcStats)
    {
      Ptr<newradioBoundCallbackArgument> arg = Create<newradioBoundCallbackArgument> ();
      arg->imsi = imsi;
      arg->cellId = cellId;
      arg->stats = m_rlcStats;

      // diconnect eventually previously connected SRB0 both at UE and eNB
      Config::Disconnect (ueRrcPath + "/Srb0/LteRlc/TxPDU",
                          MakeBoundCallback (&UlTxPduCallback, arg));
      Config::Disconnect (ueRrcPath + "/Srb0/LteRlc/RxPDU",
                          MakeBoundCallback (&DlRxPduCallback, arg));
      Config::Disconnect (ueManagerPath + "/Srb0/LteRlc/TxPDU",
                          MakeBoundCallback (&DlTxPduCallback, arg));
      Config::Disconnect (ueManagerPath + "/Srb0/LteRlc/RxPDU",
                          MakeBoundCallback (&UlRxPduCallback, arg));

      // connect SRB0 both at UE and eNB
      Config::Connect (ueRrcPath + "/Srb0/LteRlc/TxPDU",
                       MakeBoundCallback (&UlTxPduCallback, arg));
      Config::Connect (ueRrcPath + "/Srb0/LteRlc/RxPDU",
                       MakeBoundCallback (&DlRxPduCallback, arg));
      Config::Connect (ueManagerPath + "/Srb0/LteRlc/TxPDU",
                       MakeBoundCallback (&DlTxPduCallback, arg));
      Config::Connect (ueManagerPath + "/Srb0/LteRlc/RxPDU",
                       MakeBoundCallback (&UlRxPduCallback, arg));

      // connect SRB1 at eNB only (at UE SRB1 will be setup later)
      Config::Connect (ueManagerPath + "/Srb1/LteRlc/TxPDU",
                       MakeBoundCallback (&DlTxPduCallback, arg));
      Config::Connect (ueManagerPath + "/Srb1/LteRlc/RxPDU",
                       MakeBoundCallback (&UlRxPduCallback, arg));

    }
  if (m_pdcpStats)
    {
      Ptr<newradioBoundCallbackArgument> arg = Create<newradioBoundCallbackArgument> ();
      arg->imsi = imsi;
      arg->cellId = cellId;
      arg->stats = m_pdcpStats;

      // connect SRB1 at eNB only (at UE SRB1 will be setup later)
      Config::Connect (ueManagerPath + "/Srb1/LtePdcp/RxPDU",
                       MakeBoundCallback (&UlRxPduCallback, arg));
      Config::Connect (ueManagerPath + "/Srb1/LtePdcp/TxPDU",
                       MakeBoundCallback (&DlTxPduCallback, arg));
    }
}

void
newradioBearerStatsConnector::ConnectTracesUeIfFirstTime (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this << context);

  //Connect PDCP and RLC traces for SRB1
  if (m_imsiSeenUeSrb.find (imsi) == m_imsiSeenUeSrb.end ())
    {
      m_imsiSeenUeSrb.insert (imsi);
      ConnectSrb1TracesUe (context, imsi, cellId, rnti);
    }

  std::string basePath = context.substr (0, context.rfind ("/"));
  Config::MatchContainer rlc_container = Config::LookupMatches (basePath +  "/DataRadioBearerMap/*/LteRlc/");
  uint16_t numberOfRlc = rlc_container.GetN ();

  //Connect PDCP and RLC for data radio bearers
  std::map<uint64_t,uint16_t>::iterator it = m_imsiSeenUeDrb.find (imsi);
  if (m_imsiSeenUeDrb.find (imsi) == m_imsiSeenUeDrb.end () && rlc_container.GetN () > 0)
    {
      //If it is the first time for this imsi
      NS_LOG_DEBUG ("Insert imsi " + std::to_string (imsi));
      m_imsiSeenUeDrb.insert (m_imsiSeenUeDrb.end (), std::pair<uint64_t,uint16_t> (imsi, 1));
      ConnectDrbTracesUe (context, imsi, cellId, rnti);
    }
  else
    {
      if (it->second < numberOfRlc)
        {
          //If this imsi has already been connected but a new DRB is established
          NS_LOG_DEBUG ("There is a new RLC. Call ConnectDrbTracesUe to connect the traces.");
          it->second++; //TODO Check if there could be more than one RLC to connect
          DisconnectDrbTracesUe (context, imsi, cellId, rnti);
          ConnectDrbTracesUe (context, imsi, cellId, rnti);
        }
      else
        {
          //it->second = numberOfRlc; //One or more DRBs could have been removed
          NS_LOG_DEBUG ("All RLCs traces are already connected. No need for a call to ConnectDrbTracesUe.");
        }
    }
}


void
newradioBearerStatsConnector::ConnectTracesEnbIfFirstTime (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this << context);

  //NB SRB1 traces are already connected

  //Connect PDCP and RLC for data radio bearers
  //Look for the RLCs
  std::string basePath = context.substr (0, context.rfind ("/")) + "/UeMap/" + std::to_string ((uint32_t) rnti);
  Config::MatchContainer rlc_container = Config::LookupMatches (basePath +  "/DataRadioBearerMap/*/LteRlc/");

  if (m_imsiSeenEnbDrb.find (imsi) == m_imsiSeenEnbDrb.end () && rlc_container.GetN () > 0)
    {
      //it is executed only if there exist at least one rlc layer
      m_imsiSeenEnbDrb.insert (imsi);
      ConnectDrbTracesEnb (context, imsi, cellId, rnti);
    }
}



void
newradioBearerStatsConnector::ConnectDrbTracesUe (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this << context);
  NS_LOG_LOGIC (this << "expected context should match /NodeList/*/DeviceList/*/LteUeRrc/");
  std::string basePath = context.substr (0, context.rfind ("/"));
  if (m_rlcStats)
    {
      Ptr<newradioBoundCallbackArgument> arg = Create<newradioBoundCallbackArgument> ();
      arg->imsi = imsi;
      arg->cellId = cellId;
      arg->stats = m_rlcStats;

      m_rlcDrbDlRxCb[imsi] = MakeBoundCallback (&DlRxPduCallback, arg);
      m_rlcDrbUlTxCb[imsi] = MakeBoundCallback (&UlTxPduCallback, arg);

      Config::Connect (basePath + "/DataRadioBearerMap/*/LteRlc/TxPDU",
                       m_rlcDrbUlTxCb.at (imsi));
      Config::Connect (basePath + "/DataRadioBearerMap/*/LteRlc/RxPDU",
                       m_rlcDrbDlRxCb.at (imsi));

    }
  if (m_pdcpStats)
    {
      Ptr<newradioBoundCallbackArgument> arg = Create<newradioBoundCallbackArgument> ();
      arg->imsi = imsi;
      arg->cellId = cellId;
      arg->stats = m_pdcpStats;

      m_pdcpDrbDlRxCb[imsi] = MakeBoundCallback (&DlRxPduCallback, arg);
      m_pdcpDrbUlTxCb[imsi] = MakeBoundCallback (&UlTxPduCallback, arg);

      Config::Connect (basePath + "/DataRadioBearerMap/*/LtePdcp/RxPDU",
                       m_pdcpDrbDlRxCb.at (imsi));
      Config::Connect (basePath + "/DataRadioBearerMap/*/LtePdcp/TxPDU",
                       m_pdcpDrbUlTxCb.at (imsi));
    }
}

void
newradioBearerStatsConnector::ConnectSrb1TracesUe (std::string ueRrcPath, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this << ueRrcPath);
  NS_LOG_LOGIC (this << "expected context should match /NodeList/*/DeviceList/*/LteUeRrc/");
  std::string basePath = ueRrcPath.substr (0, ueRrcPath.rfind ("/"));
  NS_LOG_FUNCTION (this << imsi << cellId << rnti);
  if (m_rlcStats)
    {
      Ptr<newradioBoundCallbackArgument> arg = Create<newradioBoundCallbackArgument> ();
      arg->imsi = imsi;
      arg->cellId = cellId;
      arg->stats = m_rlcStats;
      Config::Connect (basePath + "/Srb1/LteRlc/TxPDU",
                       MakeBoundCallback (&UlTxPduCallback, arg));
      Config::Connect (basePath + "/Srb1/LteRlc/RxPDU",
                       MakeBoundCallback (&DlRxPduCallback, arg));
    }
  if (m_pdcpStats)
    {
      Ptr<newradioBoundCallbackArgument> arg = Create<newradioBoundCallbackArgument> ();
      arg->imsi = imsi;
      arg->cellId = cellId;
      arg->stats = m_pdcpStats;
      Config::Connect (basePath + "/Srb1/LtePdcp/RxPDU",
                       MakeBoundCallback (&DlRxPduCallback, arg));
      Config::Connect (basePath + "/Srb1/LtePdcp/TxPDU",
                       MakeBoundCallback (&UlTxPduCallback, arg));
    }
  if (m_mcStats)
    {
      Ptr<McnewradioBoundCallbackArgument> arg = Create<McnewradioBoundCallbackArgument> ();
      arg->stats = m_mcStats;
      Config::Connect (basePath + "/SwitchToLte",
                       MakeBoundCallback (&SwitchToLteCallback, arg));
      Config::Connect (basePath + "/SwitchTonewradio",
                       MakeBoundCallback (&SwitchTonewradioCallback, arg));
    }
}


void
newradioBearerStatsConnector::ConnectSrb1TracesEnb (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this << context);
  NS_LOG_LOGIC (this << "expected context  should match /NodeList/*/DeviceList/*/LteEnbRrc/");
  std::ostringstream basePath;
  basePath <<  context.substr (0, context.rfind ("/")) << "/UeMap/" << (uint32_t) rnti;
  if (m_rlcStats)
    {
      Ptr<newradioBoundCallbackArgument> arg = Create<newradioBoundCallbackArgument> ();
      arg->imsi = imsi;
      arg->cellId = cellId;
      arg->stats = m_rlcStats;
      Config::Connect (basePath.str () + "/Srb0/LteRlc/RxPDU",
                       MakeBoundCallback (&UlRxPduCallback, arg));
      Config::Connect (basePath.str () + "/Srb0/LteRlc/TxPDU",
                       MakeBoundCallback (&DlTxPduCallback, arg));
      Config::Connect (basePath.str () + "/Srb1/LteRlc/RxPDU",
                       MakeBoundCallback (&UlRxPduCallback, arg));
      Config::Connect (basePath.str () + "/Srb1/LteRlc/TxPDU",
                       MakeBoundCallback (&DlTxPduCallback, arg));
    }
  if (m_pdcpStats)
    {
      Ptr<newradioBoundCallbackArgument> arg = Create<newradioBoundCallbackArgument> ();
      arg->imsi = imsi;
      arg->cellId = cellId;
      arg->stats = m_pdcpStats;
      Config::Connect (basePath.str () + "/Srb1/LtePdcp/TxPDU",
                       MakeBoundCallback (&DlTxPduCallback, arg));
      Config::Connect (basePath.str () + "/Srb1/LtePdcp/RxPDU",
                       MakeBoundCallback (&UlRxPduCallback, arg));
    }
}

void
newradioBearerStatsConnector::ConnectDrbTracesEnb (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this << context);
  NS_LOG_LOGIC (this << "expected context  should match /NodeList/*/DeviceList/*/LteEnbRrc/");
  std::ostringstream basePath;
  basePath <<  context.substr (0, context.rfind ("/")) << "/UeMap/" << (uint32_t) rnti;
  if (m_rlcStats)
    {
      Ptr<newradioBoundCallbackArgument> arg = Create<newradioBoundCallbackArgument> ();
      arg->imsi = imsi;
      arg->cellId = cellId;
      arg->stats = m_rlcStats;
      Config::Connect (basePath.str () + "/DataRadioBearerMap/*/LteRlc/RxPDU",
                       MakeBoundCallback (&UlRxPduCallback, arg));
      Config::Connect (basePath.str () + "/DataRadioBearerMap/*/LteRlc/TxPDU",
                       MakeBoundCallback (&DlTxPduCallback, arg));
    }
  if (m_pdcpStats)
    {
      Ptr<newradioBoundCallbackArgument> arg = Create<newradioBoundCallbackArgument> ();
      arg->imsi = imsi;
      arg->cellId = cellId;
      arg->stats = m_pdcpStats;
      Config::Connect (basePath.str () + "/DataRadioBearerMap/*/LtePdcp/TxPDU",
                       MakeBoundCallback (&DlTxPduCallback, arg));
      Config::Connect (basePath.str () + "/DataRadioBearerMap/*/LtePdcp/RxPDU",
                       MakeBoundCallback (&UlRxPduCallback, arg));
    }
}

void
newradioBearerStatsConnector::DisconnectTracesUe (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this << context);
  NS_LOG_LOGIC (this << "expected context should match /NodeList/*/DeviceList/*/LteUeRrc/");
  std::string basePath = context.substr (0, context.rfind ("/"));
  Config::MatchContainer objects = Config::LookupMatches (basePath + "/DataRadioBearerMap/*/LteRlc/");
  NS_LOG_LOGIC ("basePath " << basePath);

  if (m_mcStats)
    {
      Ptr<McnewradioBoundCallbackArgument> arg = Create<McnewradioBoundCallbackArgument> ();
      arg->stats = m_mcStats;
      Config::Disconnect (basePath + "/SwitchToLte",
                          MakeBoundCallback (&SwitchToLteCallback, arg));
      Config::Disconnect (basePath + "/SwitchTonewradio",
                          MakeBoundCallback (&SwitchTonewradioCallback, arg));
    }
}

void
newradioBearerStatsConnector::DisconnectDrbTracesUe (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this << context);
  NS_LOG_LOGIC (this << "expected context should match /NodeList/*/DeviceList/*/LteUeRrc/");
  std::string basePath = context.substr (0, context.rfind ("/"));
  NS_LOG_LOGIC ("basePath " << basePath);

  if (m_rlcStats)
    {
      Config::MatchContainer rlc_container = Config::LookupMatches (basePath +  "/DataRadioBearerMap/*/LteRlc/");
      NS_LOG_LOGIC ("Number of RLC to disconnect " << rlc_container.GetN ());

      rlc_container.Disconnect ("RxPDU",m_rlcDrbDlRxCb.at (imsi));
      rlc_container.Disconnect ("TxPDU",m_rlcDrbUlTxCb.at (imsi));
    }

  if (m_pdcpStats)
    {
      Config::MatchContainer pdcp_container = Config::LookupMatches (basePath +  "/DataRadioBearerMap/*/LtePdcp/");
      NS_LOG_LOGIC ("Number of PDCP to disconnect " << pdcp_container.GetN ());

      pdcp_container.Disconnect ("RxPDU",m_pdcpDrbDlRxCb.at (imsi));
      pdcp_container.Disconnect ("TxPDU",m_pdcpDrbUlTxCb.at (imsi));
    }
}


void
newradioBearerStatsConnector::DisconnectTracesEnb (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this);
}


void
newradioBearerStatsConnector::ConnectSecondaryTracesUe (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_LOGIC (this << context);
  NS_LOG_LOGIC (this << "expected context should match /NodeList/*/DeviceList/*/LteUeRrc/");
  std::string basePath = context.substr (0, context.rfind ("/"));
  Config::MatchContainer objects = Config::LookupMatches (basePath + "/DataRadioRlcMap/*");
  NS_LOG_LOGIC ("basePath " << basePath);

  if (m_rlcStats)
    {
      Ptr<newradioBoundCallbackArgument> arg = Create<newradioBoundCallbackArgument> ();
      arg->imsi = imsi;
      arg->cellId = cellId;
      arg->stats = m_rlcStats;
      // for MC devices
      Config::Connect (basePath + "/DataRadioRlcMap/*/LteRlc/TxPDU",
                       MakeBoundCallback (&UlTxPduCallback, arg));
      Config::Connect (basePath + "/DataRadioRlcMap/*/LteRlc/RxPDU",
                       MakeBoundCallback (&DlRxPduCallback, arg));
    }
}

void
newradioBearerStatsConnector::ConnectSecondaryTracesEnb (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this << context);
  NS_LOG_LOGIC (this << "expected context should match /NodeList/*/DeviceList/*/LteEnbRrc/UeMap/*");
  std::ostringstream basePath;
  basePath <<  context.substr (0, context.rfind ("/"));
  Config::MatchContainer objects = Config::LookupMatches (basePath.str () + "/DataRadioRlcMap/*/LteRlc/");
  NS_LOG_LOGIC ("basePath " << basePath.str ());

  if (m_rlcStats)
    {
      Ptr<newradioBoundCallbackArgument> arg = Create<newradioBoundCallbackArgument> ();
      arg->imsi = imsi;
      arg->cellId = cellId;
      arg->stats = m_rlcStats;
      // for MC devices
      Config::Connect (basePath.str () + "/DataRadioRlcMap/*/LteRlc/RxPDU",
                       MakeBoundCallback (&UlRxPduCallback, arg));
      Config::Connect (basePath.str () + "/DataRadioRlcMap/*/LteRlc/TxPDU",
                       MakeBoundCallback (&DlTxPduCallback, arg));
    }
}

} // namespace newradio

} // namespace ns3
