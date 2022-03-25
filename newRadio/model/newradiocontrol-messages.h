#ifndef SRC_newradio_MODEL_newradio_CONTROL_MESSAGES_H_
#define SRC_newradio_MODEL_newradio_CONTROL_MESSAGES_H_

#include <ns3/ptr.h>
#include <ns3/simple-ref-count.h>
#include <ns3/lte-rrc-sap.h>
#include <ns3/ff-mac-common.h>
#include "newradio-phy-mac-common.h"
#include <list>

namespace ns3 {

namespace newradio {

class newradioControlMessage : public SimpleRefCount<newradioControlMessage>
{
public:
  enum messageType
  {
    DCI,             // The resources allocation map from the BS to the attached UEs
    DCI_TDMA,
    DL_CQI,
    MIB,             // Master Information Block
    SIB1,             // System Information Block Type 1
    RACH_PREAMBLE,             // Random Access Preamble
    RAR,             // Random Access Response
    BSR,             // Buffer Status Report
    DL_HARQ             // DL HARQ feedback
  };

  newradioControlMessage (void);
  virtual ~newradioControlMessage (void);

  void SetMessageType (messageType type);

  messageType GetMessageType (void);

private:
  messageType m_messageType;
};

/************************************************************
 * Defines the time and frequency based resource allocation *
 * for the UEs attached to a given BS.                      *
 ************************************************************/

class newradioDciMessage : public newradioControlMessage
{
public:
  newradioDciMessage (void);
  virtual ~newradioDciMessage (void);

//	void SetRbAllocationMap (SfAllocInfo allocMap);
//	SfAllocInfo GetRbAllocationMap (void);

  void SetDciInfoElement (DciInfoElement dci);
  DciInfoElement GetDciInfoElement (void);

  void SetSfnSf (uint32_t sfn);
  uint32_t GetSfnSf (void);

private:
  uint32_t m_sfnSf;        // frame num and sf num for debugging
//	SfAllocInfo m_rscAllocationMap;
  DciInfoElement m_dciInfoElement;
};

class newradioTdmaDciMessage : public newradioControlMessage
{
public:
  newradioTdmaDciMessage (void);
  virtual ~newradioTdmaDciMessage (void);

//	void SetRbAllocationMap (SfAllocInfo allocMap);
//	SfAllocInfo GetRbAllocationMap (void);

  void SetDciInfoElement (DciInfoElementTdma dci);
  DciInfoElementTdma GetDciInfoElement (void);

  void SetSfnSf (SfnSf sfn);
  SfnSf GetSfnSf (void);

private:
  SfnSf m_sfnSf;        // frame num and sf num for debugging
  //bool	m_ulGrant;	// is ul grant
//	SfAllocInfo m_rscAllocationMap;
  DciInfoElementTdma m_dciInfoElement;
};

class newradioDlCqiMessage : public newradioControlMessage
{
public:
  newradioDlCqiMessage (void);
  virtual ~newradioDlCqiMessage (void);

  void SetDlCqi (DlCqiInfo cqi);
  DlCqiInfo GetDlCqi ();

private:
  DlCqiInfo m_cqi;
};


/**
 * \ingroup newradio
 * The uplink BsrLteControlMessage defines the specific
 * extension of the CE element for reporting the buffer status report
 */
class newradioBsrMessage : public newradioControlMessage
{
public:
  newradioBsrMessage (void);
  virtual ~newradioBsrMessage (void);

  /**
  * \brief add a BSR feedback record into the message.
  * \param bsr the BSR feedback
  */
  void SetBsr (MacCeElement bsr);

  /**
  * \brief Get BSR informations
  * \return BSR message
  */
  MacCeElement GetBsr (void);

private:
  MacCeElement m_bsr;

};


// ---------------------------------------------------------------------------

/**
 * \ingroup newradio
 * \brief Abstract model for broadcasting the Master Information Block (MIB)
 *        within the control channel (BCCH).
 *
 */
class newradioMibMessage : public newradioControlMessage
{
public:
  /**
   * \brief Create a new instance of MIB control message.
   */
  newradioMibMessage (void);

  /**
   * \brief Replace the MIB content of this control message.
   * \param mib the desired MIB content
   */
  void SetMib (LteRrcSap::MasterInformationBlock mib);

  /**
   * \brief Retrieve the MIB content from this control message.
   * \return the current MIB content that this control message holds
   */
  LteRrcSap::MasterInformationBlock GetMib () const;

private:
  LteRrcSap::MasterInformationBlock m_mib;

}; // end of class newradioMibMessage


// ---------------------------------------------------------------------------

/**
 * \ingroup newradio
 * \brief Abstract model for broadcasting the System Information Block Type 1
 *        (SIB1) within the control channel (BCCH).
 *
 */
class newradioSib1Message : public newradioControlMessage
{
public:
  /**
   * \brief Create a new instance of SIB1 control message.
   */
  newradioSib1Message (void);

  /**
   * \brief Replace the SIB1 content of this control message.
   * \param sib1 the desired SIB1 content
   */
  void SetSib1 (LteRrcSap::SystemInformationBlockType1 sib1);

  /**
   * \brief Retrieve the SIB1 content from this control message.
   * \return the current SIB1 content that this control message holds
   */
  LteRrcSap::SystemInformationBlockType1 GetSib1 () const;

private:
  LteRrcSap::SystemInformationBlockType1 m_sib1;

}; // end of class newradioSib1Message
// ---------------------------------------------------------------------------

/**
 * \ingroup newradio
 *
 * abstract model for the Random Access Preamble
 */
class newradioRachPreambleMessage : public newradioControlMessage
{
public:
  newradioRachPreambleMessage (void);

  /**
   * Set the Random Access Preamble Identifier (RAPID), see 3GPP TS 36.321 6.2.2
   *
   * \param rapid the RAPID
   */
  void SetRapId (uint32_t rapid);

  /**
   *
   * \return the RAPID
   */
  uint32_t GetRapId () const;

private:
  uint32_t m_rapId;

};
// ---------------------------------------------------------------------------

/**
 * \ingroup newradio
 *
 * abstract model for the MAC Random Access Response message
 */
class newradioRarMessage : public newradioControlMessage
{
public:
  newradioRarMessage (void);

  /**
   *
   * \param raRnti the RA-RNTI, see 3GPP TS 36.321 5.1.4
   */
  void SetRaRnti (uint16_t raRnti);

  /**
   *
   * \return  the RA-RNTI, see 3GPP TS 36.321 5.1.4
   */
  uint16_t GetRaRnti () const;

  /**
   * a MAC RAR and the corresponding RAPID subheader
   *
   */
  struct Rar
  {
    uint8_t rapId;
    BuildRarListElement_s rarPayload;
  };

  /**
   * add a RAR to the MAC PDU, see 3GPP TS 36.321 6.2.3
   *
   * \param rar the rar
   */
  void AddRar (Rar rar);

  /**
   *
   * \return a const iterator to the beginning of the RAR list
   */
  std::list<Rar>::const_iterator RarListBegin () const;

  /**
   *
   * \return a const iterator to the end of the RAR list
   */
  std::list<Rar>::const_iterator RarListEnd () const;

private:
  std::list<Rar> m_rarList;
  uint16_t m_raRnti;

};


/**
 * \ingroup mmEave
 * The downlink newradioDlHarqFeedbackMessage defines the specific
 * messages for transmitting the DL HARQ feedback through PUCCH
 */
class newradioDlHarqFeedbackMessage : public newradioControlMessage
{
public:
  newradioDlHarqFeedbackMessage (void);
  virtual ~newradioDlHarqFeedbackMessage (void);

  /**
  * \brief add a DL HARQ feedback record into the message.
  * \param m the DL HARQ feedback
  */
  void SetDlHarqFeedback (DlHarqInfo m);

  /**
  * \brief Get DL HARQ informations
  * \return DL HARQ message
  */
  DlHarqInfo GetDlHarqFeedback (void);

private:
  DlHarqInfo m_dlHarqInfo;

};

} // namespace newradio

} // namespace ns3

#endif /* SRC_newradio_MODEL_newradio_CONTROL_MESSAGES_H_ */
