#ifndef newradio_BEARER_STATS_CONNECTOR_H
#define newradio_BEARER_STATS_CONNECTOR_H


#include <ns3/traced-callback.h>
#include <ns3/config.h>
#include <ns3/simple-ref-count.h>
#include <ns3/ptr.h>
#include "mc-stats-calculator.h"
#include <fstream>
#include "ns3/object.h"
#include <string>

#include <set>
#include <map>

namespace ns3 {

namespace newradio {

class newradioBearerStatsCalculator;
//class McStatsCalculator;

/**
 * \ingroup lte
 *
 * This class is very useful when user needs to collect
 * statistics from PDCD and RLC. It automatically connects
 * newradioBearerStatsCalculator to appropriate trace sinks.
 * Usually user do not use this class. All he/she needs to
 * to do is to call: LteHelper::EnablePdcpTraces() and/or
 * LteHelper::EnableRlcTraces().
 */

class newradioBearerStatsConnector : public Object
{
public:
  /// Constructor
  newradioBearerStatsConnector ();

  /**
   * Class destructor
   */
  virtual
  ~newradioBearerStatsConnector ();

  // Inherited from ns3::Object
  /**
   *  Register this type.
   *  \return The object TypeId.
   */
  static TypeId GetTypeId (void);
  void DoDispose ();

  /**
   * Enables trace sinks for RLC layer. Usually, this function
   * is called by LteHelper::EnableRlcTraces().
   * \param rlcStats statistics calculator for RLC layer
   */
  void EnableRlcStats (Ptr<newradioBearerStatsCalculator> rlcStats);

  /**
   * Enables trace sinks for PDCP layer. Usually, this function
   * is called by LteHelper::EnablePdcpTraces().
   * \param pdcpStats statistics calculator for PDCP layer
   */
  void EnablePdcpStats (Ptr<newradioBearerStatsCalculator> pdcpStats);

  void EnableMcStats  (Ptr<McStatsCalculator> mcStats);

  /**
   * Connects trace sinks to appropriate trace sources
   */
  void EnsureConnected ();

  // trace sinks, to be used with MakeBoundCallback

  /**
   * Function hooked to RandomAccessSuccessful trace source at UE RRC,
   * which is fired upon successful completion of the random access procedure
   * \param c
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  static void NotifyRandomAccessSuccessfulUe (newradioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Sink connected source of UE Connection Setup trace. Not used.
   * \param c
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  static void NotifyConnectionSetupUe (newradioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Function hooked to ConnectionReconfiguration trace source at UE RRC,
   * which is fired upon RRC connection reconfiguration
   * \param c
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  static void NotifyConnectionReconfigurationUe (newradioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Function hooked to HandoverStart trace source at UE RRC,
   * which is fired upon start of a handover procedure
   * \param c
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   * \param targetCellId
   */
  static void NotifyHandoverStartUe (newradioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti, uint16_t targetCellId);

  /**
   * Function hooked to HandoverStart trace source at UE RRC,
   * which is fired upon successful termination of a handover procedure
   * \param c
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  static void NotifyHandoverEndOkUe (newradioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Function hooked to NewUeContext trace source at eNB RRC,
   * which is fired upon creation of a new UE context
   * \param c
   * \param context
   * \param cellid
   * \param rnti
   */
  static void NotifyNewUeContextEnb (newradioBearerStatsConnector* c, std::string context, uint16_t cellid, uint16_t rnti);

  /**
   * Function hooked to ConnectionReconfiguration trace source at eNB RRC,
   * which is fired upon RRC connection reconfiguration
   * \param c
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  static void NotifyConnectionReconfigurationEnb (newradioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Function hooked to HandoverStart trace source at eNB RRC,
   * which is fired upon start of a handover procedure
   * \param c
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   * \param targetCellId
   */
  static void NotifyHandoverStartEnb (newradioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti, uint16_t targetCellId);

  /**
   * Function hooked to HandoverEndOk trace source at eNB RRC,
   * which is fired upon successful termination of a handover procedure
   * \param c
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  static void NotifyHandoverEndOkEnb (newradioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  // TODO doc
  static void NotifySwitchTonewradioUe (newradioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti);

  static void NotifySecondarynewradioEnbAvailable (newradioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti);

  static void NotifynewradioSinr (newradioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellId, long double sinr);
  void PrintnewradioSinr (uint64_t imsi, uint16_t cellId, long double sinr);
  static void NotifyLteSinr (newradioBearerStatsConnector* c, std::string context, uint16_t cellId, uint16_t rnti, double rsrp, double sinr, uint8_t cc);
  void PrintLteSinr (uint16_t rnti, uint16_t cellId, double sinr);

  std::string GetEnbHandoverStartOutputFilename (void);
  std::string  GetUeHandoverStartOutputFilename (void);
  std::string GetEnbHandoverEndOutputFilename (void);
  std::string  GetUeHandoverEndOutputFilename (void);
  std::string GetCellIdStatsOutputFilename (void);
  std::string GetnewradioSinrOutputFilename (void);
  std::string GetLteSinrOutputFilename (void);

  void SetEnbHandoverStartOutputFilename (std::string outputFilename);
  void  SetUeHandoverStartOutputFilename (std::string outputFilename);
  void SetEnbHandoverEndOutputFilename (std::string outputFilename);
  void  SetUeHandoverEndOutputFilename (std::string outputFilename);
  void SetCellIdStatsOutputFilename (std::string outputFilename);
  void SetnewradioSinrOutputFilename (std::string outputFilename);
  void SetLteSinrOutputFilename (std::string outputFilename);

private:
  /**
   * Creates UE Manager path and stores it in m_ueManagerPathByCellIdRnti
   * \param ueManagerPath
   * \param cellId
   * \param rnti
   */
  void StoreUeManagerPath (std::string ueManagerPath, uint16_t cellId, uint16_t rnti);

  /**
   * Connects Srb0 trace sources at UE and eNB to RLC and PDCP calculators,
   * and Srb1 trace sources at eNB to RLC and PDCP calculators,
   * \param ueRrcPath
   * \param imsi
   * \param cellId
   * \param rnti
   */
  void ConnectSrb0Traces (std::string ueRrcPath, uint64_t imsi, uint16_t cellId, uint16_t rnti);

  /**
   * Connects all trace sources at UE to RLC and PDCP calculators.
   * This function can connect traces only once for UE.
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  void ConnectTracesUeIfFirstTime (std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Connects all trace sources at eNB to RLC and PDCP calculators.
   * This function can connect traces only once for eNB.
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  void ConnectTracesEnbIfFirstTime (std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Connects DRBs trace sources at UE to RLC and PDCP calculators.
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  void ConnectDrbTracesUe (std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Connects SRB1 trace sources at UE to RLC and PDCP calculators
   * \param ueRrcPath
   * \param imsi
   * \param cellId
   * \param rnti
   */
  void ConnectSrb1TracesUe (std::string ueRrcPath, uint64_t imsi, uint16_t cellId, uint16_t rnti);

  /**
   * Disconnects all trace sources at UE to RLC and PDCP calculators.
   * Function is not implemented.
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  void DisconnectTracesUe (std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Disconnects DRB trace sources at UE to RLC and PDCP calculators.
   * Function is not implemented.
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  void DisconnectDrbTracesUe (std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Connects SRB1 trace sources at eNB to RLC and PDCP calculators
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  void ConnectSrb1TracesEnb (std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Connects DRBs trace sources at eNB to RLC and PDCP calculators
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  void ConnectDrbTracesEnb (std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Disconnects all trace sources at eNB to RLC and PDCP calculators.
   * Function is not implemented.
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  void DisconnectTracesEnb (std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  void ConnectSecondaryTracesUe (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti);
  void ConnectSecondaryTracesEnb (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti);

  void PrintEnbStartHandover (uint64_t imsi, uint16_t sourceCellid, uint16_t targetCellId, uint16_t rnti);
  void PrintEnbEndHandover (uint64_t imsi, uint16_t targetCellId, uint16_t rnti);
  void PrintUeStartHandover (uint64_t imsi, uint16_t sourceCellid, uint16_t targetCellId, uint16_t rnti);
  void PrintUeEndHandover (uint64_t imsi, uint16_t targetCellId, uint16_t rnti);


  Ptr<newradioBearerStatsCalculator> m_rlcStats; //!< Calculator for RLC Statistics
  Ptr<newradioBearerStatsCalculator> m_pdcpStats; //!< Calculator for PDCP Statistics
  Ptr<McStatsCalculator> m_mcStats; //!< Calculator for multi-connectivity Statistics

  std::map<uint64_t, CallbackBase> m_rlcDrbDlRxCb; //!< Sinks for the received DL RLC data packets
  std::map<uint64_t, CallbackBase> m_rlcDrbUlTxCb; //!< Sinks for the sent UL RLC data packets

  std::map<uint64_t, CallbackBase> m_pdcpDrbDlRxCb; //!< Sinks for the received DL PDCP data packets
  std::map<uint64_t, CallbackBase> m_pdcpDrbUlTxCb; //!< Sinks for the sent UL PDCP data packets

  bool m_connected; //!< true if traces are connected to sinks, initially set to false
  std::set<uint64_t> m_imsiSeenUeSrb; //!< stores all UEs for which RLC and PDCP for SRB1 traces were connected
  std::map<uint64_t,uint16_t> m_imsiSeenUeDrb; //!< stores all UEs for which RLC and PDCP traces for DRBs were connected
  std::set<uint64_t> m_imsiSeenEnbDrb; //!< stores all eNBs for which RLC and PDCP traces for DRBs were connected

  /**
   * Struct used as key in m_ueManagerPathByCellIdRnti map
   */
  struct CellIdRnti
  {
    uint16_t cellId; //!< cellId
    uint16_t rnti; //!< rnti
  };

  /**
   * Less than operator for CellIdRnti, because it is used as key in map
   */
  friend bool operator < (const CellIdRnti &a, const CellIdRnti &b);

  /**
   * List UE Manager Paths by CellIdRnti
   */
  std::map<CellIdRnti, std::string> m_ueManagerPathByCellIdRnti;

  std::string m_enbHandoverStartFilename;
  std::string m_enbHandoverEndFilename;
  std::string  m_ueHandoverStartFilename;
  std::string  m_ueHandoverEndFilename;
  std::string m_cellIdInTimeHandoverFilename;
  std::string m_newradioSinrOutputFilename;
  std::string m_lteSinrOutputFilename;

  std::ofstream m_enbHandoverStartOutFile;
  std::ofstream  m_ueHandoverStartOutFile;
  std::ofstream m_enbHandoverEndOutFile;
  std::ofstream  m_ueHandoverEndOutFile;
  std::ofstream m_cellIdInTimeHandoverOutFile;
  std::ofstream m_newradioSinrOutFile;
  std::ofstream m_lteSinrOutFile;
};

} // namespace newradio

} // namespace ns3


#endif // newradio_BEARER_STATS_CONNECTOR_H
