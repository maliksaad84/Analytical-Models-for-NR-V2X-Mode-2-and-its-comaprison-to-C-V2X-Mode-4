#include <ns3/log.h>
#include "newradio-phy-rx-trace.h"
#include <ns3/simulator.h>
#include <stdio.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("newradioPhyRxTrace");

namespace newradio {

NS_OBJECT_ENSURE_REGISTERED (newradioPhyRxTrace);

std::ofstream newradioPhyRxTrace::m_rxPacketTraceFile;
std::string newradioPhyRxTrace::m_rxPacketTraceFilename;

newradioPhyRxTrace::newradioPhyRxTrace ()
{
}

newradioPhyRxTrace::~newradioPhyRxTrace ()
{
  if (m_rxPacketTraceFile.is_open ())
    {
      m_rxPacketTraceFile.close ();
    }
}

TypeId
newradioPhyRxTrace::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::newradioPhyRxTrace")
    .SetParent<Object> ()
    .AddConstructor<newradioPhyRxTrace> ()
    .AddAttribute ("OutputFilename",
                   "Name of the file where the uplink results will be saved.",
                   StringValue ("RxPacketTrace.txt"),
                   MakeStringAccessor (&newradioPhyRxTrace::SetOutputFilename),
                   MakeStringChecker ())
  ;
  return tid;
}

void
newradioPhyRxTrace::SetOutputFilename ( std::string fileName)
{
  NS_LOG_INFO ("Filename: " << fileName);
  m_rxPacketTraceFilename = fileName;
}

void
newradioPhyRxTrace::ReportCurrentCellRsrpSinrCallback (Ptr<newradioPhyRxTrace> phyStats, std::string path,
                                                     uint64_t imsi, SpectrumValue& sinr, SpectrumValue& power)
{
  NS_LOG_INFO ("UE" << imsi << "->Generate RsrpSinrTrace");
  //phyStats->ReportInterferenceTrace (imsi, sinr);
}



/*void
newradioPhyRxTrace::ReportInterferenceTrace (uint64_t imsi, SpectrumValue& sinr)
{
        uint64_t slot_count = Now().GetMicroSeconds ()/125;
        uint32_t rb_count = 1;
        FILE* log_file;
        char fname[255];
        sprintf(fname, "UE_%llu_SINR_dB.txt", (long long unsigned ) imsi);
        log_file = fopen(fname, "a");
        Values::iterator it = sinr.ValuesBegin();
        while(it!=sinr.ValuesEnd())
        {
                //fprintf(log_file, "%d\t%d\t%f\t \n", slot_count/2, rb_count, 10*log10(*it));
                fprintf(log_file, "%llu\t%llu\t%d\t%f\t \n",(long long unsigned) slot_count/8+1, (long long unsigned) slot_count%8+1, rb_count, 10*log10(*it));
                rb_count++;
                it++;
        }
        fflush(log_file);
        fclose(log_file);
}*/



void
newradioPhyRxTrace::ReportPacketCountUeCallback (Ptr<newradioPhyRxTrace> phyStats, std::string path,
                                               UePhyPacketCountParameter param)
{
  //phyStats->ReportPacketCountUe (param);
}
void
newradioPhyRxTrace::ReportPacketCountEnbCallback (Ptr<newradioPhyRxTrace> phyStats, std::string path,
                                                EnbPhyPacketCountParameter param)
{
  //phyStats->ReportPacketCountEnb (param);
}

void
newradioPhyRxTrace::ReportDownLinkTBSize (Ptr<newradioPhyRxTrace> phyStats, std::string path,
                                        uint64_t imsi, uint64_t tbSize)
{
  //phyStats->ReportDLTbSize (imsi, tbSize);
}


/*
void
newradioPhyRxTrace::ReportPacketCountUe (UePhyPacketCountParameter param)
{
        FILE* log_file;
        char fname[255];
        sprintf (fname,"UE_%llu_Packet_Trace.txt", (long long unsigned) param.m_imsi);
        log_file = fopen (fname, "a");
        if (param.m_isTx)
        {
                fprintf (log_file, "%d\t%d\t%d\n", param.m_subframeno, param.m_noBytes, 0);
        }
        else
        {
                fprintf (log_file, "%d\t%d\t%d\n", param.m_subframeno, 0, param.m_noBytes);
        }

        fflush(log_file);
        fclose(log_file);

}

void
newradioPhyRxTrace::ReportPacketCountEnb (EnbPhyPacketCountParameter param)
{
        FILE* log_file;
        char fname[255];
        sprintf (fname,"BS_%llu_Packet_Trace.txt",(long long unsigned) param.m_cellId);
        log_file = fopen (fname, "a");
        if (param.m_isTx)
        {
                fprintf (log_file, "%d\t%d\t%d\n", param.m_subframeno, param.m_noBytes, 0);
        }
        else
        {
                fprintf (log_file, "%d\t%d\t%d\n", param.m_subframeno, 0, param.m_noBytes);
        }

        fflush(log_file);
        fclose(log_file);
}

void
newradioPhyRxTrace::ReportDLTbSize (uint64_t imsi, uint64_t tbSize)
{
        FILE* log_file;
        char fname[255];
        sprintf (fname,"UE_%llu_Tb_Size.txt", (long long unsigned) imsi);
        log_file = fopen (fname, "a");

        fprintf (log_file, "%llu \t %llu\n", Now().GetMicroSeconds (), tbSize);
        fprintf (log_file, "%lld \t %llu \n",(long long int) Now().GetMicroSeconds (), (long long unsigned) tbSize);
        fflush(log_file);
        fclose(log_file);
}
*/
void
newradioPhyRxTrace::RxPacketTraceUeCallback (Ptr<newradioPhyRxTrace> phyStats, std::string path, RxPacketTraceParams params)
{
  if (!m_rxPacketTraceFile.is_open ())
    {
      m_rxPacketTraceFile.open (m_rxPacketTraceFilename.c_str ());
      m_rxPacketTraceFile << "\ttime\tframe\tsubF\t1stSym\tsymbol#\tcellId\trnti\tccId\ttbSize\tmcs\trv\tSINR(dB)\tcorrupt\tTBler" << std::endl;
      if (!m_rxPacketTraceFile.is_open ())
        {
          NS_FATAL_ERROR ("Could not open tracefile");
        }
    }
  m_rxPacketTraceFile << "DL\t" << Simulator::Now ().GetSeconds () << "\t" << params.m_frameNum << "\t" << (unsigned)params.m_sfNum << "\t" << (unsigned)params.m_symStart
                      << "\t" << (unsigned)params.m_numSym << "\t" << params.m_cellId
                      << "\t" << params.m_rnti << "\t" << (unsigned)params.m_ccId << "\t" << params.m_tbSize << "\t" << (unsigned)params.m_mcs << "\t" << (unsigned)params.m_rv << "\t"
                      << 10 * std::log10 (params.m_sinr) << "\t" << " \t" << params.m_corrupt << "\t" <<  params.m_tbler << std::endl;

  if (params.m_corrupt)
    {
      NS_LOG_DEBUG ("DL TB error\t" << params.m_frameNum << "\t" << (unsigned)params.m_sfNum << "\t" << (unsigned)params.m_symStart
                                    << "\t" << (unsigned)params.m_numSym
                                    << "\t" << params.m_rnti << "\t" << (unsigned)params.m_ccId << "\t" << params.m_tbSize << "\t" << (unsigned)params.m_mcs << "\t" << (unsigned)params.m_rv << "\t"
                                    << 10 * std::log10 (params.m_sinr) << "\t" << params.m_tbler << "\t" << params.m_corrupt);
    }
}
void
newradioPhyRxTrace::RxPacketTraceEnbCallback (Ptr<newradioPhyRxTrace> phyStats, std::string path, RxPacketTraceParams params)
{
  if (!m_rxPacketTraceFile.is_open ())
    {
      m_rxPacketTraceFile.open (m_rxPacketTraceFilename.c_str ());
      m_rxPacketTraceFile << "\ttime\tframe\tsubF\t1stSym\tsymbol#\tcellId\trnti\tccId\ttbSize\tmcs\trv\tSINR(dB)\tcorrupt\tTBler" << std::endl;
      if (!m_rxPacketTraceFile.is_open ())
        {
          NS_FATAL_ERROR ("Could not open tracefile");
        }
    }
  m_rxPacketTraceFile << "UL\t" << Simulator::Now ().GetSeconds () << "\t" << params.m_frameNum << "\t" << (unsigned)params.m_sfNum << "\t" << (unsigned)params.m_symStart
                      << "\t" << (unsigned)params.m_numSym << "\t" << params.m_cellId
                      << "\t" << params.m_rnti << "\t" << (unsigned)params.m_ccId << "\t" << params.m_tbSize << "\t" << (unsigned)params.m_mcs << "\t" << (unsigned)params.m_rv << "\t"
                      << 10 * std::log10 (params.m_sinr) << " \t" << params.m_corrupt << "\t" << params.m_tbler << std::endl;

  if (params.m_corrupt)
    {
      NS_LOG_DEBUG ("UL TB error\t" << params.m_frameNum << "\t" << (unsigned)params.m_sfNum << "\t" << (unsigned)params.m_symStart
                                    << "\t" << (unsigned)params.m_numSym
                                    << "\t" << params.m_rnti << "\t" << (unsigned)params.m_ccId << "\t" << params.m_tbSize << "\t" << (unsigned)params.m_mcs << "\t" << (unsigned)params.m_rv << "\t"
                                    << 10 * std::log10 (params.m_sinr) << "\t" << params.m_tbler << "\t" << params.m_corrupt << "\t" << params.m_sinrMin);
    }
}

} // namespace newradio

} /* namespace ns3 */
