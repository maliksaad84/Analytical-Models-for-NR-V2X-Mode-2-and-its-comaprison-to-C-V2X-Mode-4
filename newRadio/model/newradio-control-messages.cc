#include <ns3/log.h>
#include "newradio-control-messages.h"

namespace ns3 {

namespace newradio {

NS_LOG_COMPONENT_DEFINE ("newradioControlMessage");

newradioControlMessage::newradioControlMessage (void)
{
  NS_LOG_INFO (this);
}

newradioControlMessage::~newradioControlMessage (void)
{
  NS_LOG_INFO (this);
}

void
newradioControlMessage::SetMessageType (messageType type)
{
  m_messageType = type;
}

newradioControlMessage::messageType
newradioControlMessage::GetMessageType (void)
{
  return m_messageType;
}

newradioTdmaDciMessage::newradioTdmaDciMessage (void)
{
  NS_LOG_INFO (this);
  SetMessageType (newradioControlMessage::DCI_TDMA);
}

newradioTdmaDciMessage::~newradioTdmaDciMessage (void)
{
  NS_LOG_INFO (this);
}


void
newradioTdmaDciMessage::SetDciInfoElement (DciInfoElementTdma dci)
{
  m_dciInfoElement = dci;
}

DciInfoElementTdma
newradioTdmaDciMessage::GetDciInfoElement (void)
{
  return m_dciInfoElement;
}

void
newradioTdmaDciMessage::SetSfnSf (SfnSf sfn)
{
  m_sfnSf = sfn;
}

SfnSf
newradioTdmaDciMessage::GetSfnSf (void)
{
  return m_sfnSf;
}

newradioDciMessage::newradioDciMessage (void)
{
  NS_LOG_INFO (this);
  SetMessageType (newradioControlMessage::DCI);
}

newradioDciMessage::~newradioDciMessage (void)
{
  NS_LOG_INFO (this);
}


void
newradioDciMessage::SetDciInfoElement (DciInfoElement dci)
{
  m_dciInfoElement = dci;
}

DciInfoElement
newradioDciMessage::GetDciInfoElement (void)
{
  return m_dciInfoElement;
}

void
newradioDciMessage::SetSfnSf (uint32_t sfn)
{
  m_sfnSf = sfn;
}

uint32_t
newradioDciMessage::GetSfnSf (void)
{
  return m_sfnSf;
}

newradioDlCqiMessage::newradioDlCqiMessage (void)
{
  SetMessageType (newradioControlMessage::DL_CQI);
  NS_LOG_INFO (this);
}
newradioDlCqiMessage::~newradioDlCqiMessage (void)
{
  NS_LOG_INFO (this);
}

void
newradioDlCqiMessage::SetDlCqi (DlCqiInfo cqi)
{
  m_cqi = cqi;
}

DlCqiInfo
newradioDlCqiMessage::GetDlCqi ()
{
  return m_cqi;
}

// ----------------------------------------------------------------------------------------------------------

newradioBsrMessage::newradioBsrMessage (void)
{
  SetMessageType (newradioControlMessage::BSR);
}


newradioBsrMessage::~newradioBsrMessage (void)
{

}

void
newradioBsrMessage::SetBsr (MacCeElement bsr)
{
  m_bsr = bsr;

}


MacCeElement
newradioBsrMessage::GetBsr (void)
{
  return m_bsr;
}

// ----------------------------------------------------------------------------------------------------------



newradioMibMessage::newradioMibMessage (void)
{
  SetMessageType (newradioControlMessage::MIB);
}


void
newradioMibMessage::SetMib (LteRrcSap::MasterInformationBlock  mib)
{
  m_mib = mib;
}

LteRrcSap::MasterInformationBlock
newradioMibMessage::GetMib () const
{
  return m_mib;
}


// ----------------------------------------------------------------------------------------------------------



newradioSib1Message::newradioSib1Message (void)
{
  SetMessageType (newradioControlMessage::SIB1);
}


void
newradioSib1Message::SetSib1 (LteRrcSap::SystemInformationBlockType1 sib1)
{
  m_sib1 = sib1;
}

LteRrcSap::SystemInformationBlockType1
newradioSib1Message::GetSib1 () const
{
  return m_sib1;
}



// ----------------------------------------------------------------------------------------------------------

newradioRachPreambleMessage::newradioRachPreambleMessage (void)
{
  SetMessageType (newradioControlMessage::RACH_PREAMBLE);
}

void
newradioRachPreambleMessage::SetRapId (uint32_t rapId)
{
  m_rapId = rapId;
}

uint32_t
newradioRachPreambleMessage::GetRapId () const
{
  return m_rapId;
}

// ----------------------------------------------------------------------------------------------------------


newradioRarMessage::newradioRarMessage (void)
{
  SetMessageType (newradioControlMessage::RAR);
}


void
newradioRarMessage::SetRaRnti (uint16_t raRnti)
{
  m_raRnti = raRnti;
}

uint16_t
newradioRarMessage::GetRaRnti () const
{
  return m_raRnti;
}


void
newradioRarMessage::AddRar (Rar rar)
{
  m_rarList.push_back (rar);
}

std::list<newradioRarMessage::Rar>::const_iterator
newradioRarMessage::RarListBegin () const
{
  return m_rarList.begin ();
}

std::list<newradioRarMessage::Rar>::const_iterator
newradioRarMessage::RarListEnd () const
{
  return m_rarList.end ();
}

newradioDlHarqFeedbackMessage::newradioDlHarqFeedbackMessage (void)
{
  SetMessageType (newradioControlMessage::DL_HARQ);
}


newradioDlHarqFeedbackMessage::~newradioDlHarqFeedbackMessage (void)
{

}

void
newradioDlHarqFeedbackMessage::SetDlHarqFeedback (DlHarqInfo m)
{
  m_dlHarqInfo = m;
}


DlHarqInfo
newradioDlHarqFeedbackMessage::GetDlHarqFeedback (void)
{
  return m_dlHarqInfo;
}

}
}
