#include "querystudy.h"
#include <dcdeftag.h> //provide the information for the tags
#include "studylist.h"
#include "pacsconnection.h"
#ifndef CONST
#include "const.h"    
#endif

namespace udg{

/** This action sets the connection that we will use to connect to the pacs
  *              @param   connection [in] Openned connection to the pacs
  *              @param   study Mask
  */
QueryStudy::QueryStudy(PacsConnection connection,StudyMask study)
{
    m_assoc = connection.getPacsConnection();
    m_mask = study.getMask();
}

/** Sets the connection to the PACS
  *        @param PACS's connection
  */
void QueryStudy::setConnection(PacsConnection connection)
{
    m_assoc = connection.getPacsConnection();
    //gets a pointer to list study, where the result of the search will be insert
    m_studyListSingleton = StudyListSingleton::getStudyListSingleton();
}

/** This action sets the mask that we will use to search the studies in to the pacs. This mask is created by mask class
  *              @param  search Mask
  */
void QueryStudy:: setMask(StudyMask study)
{
    m_mask = study.getMask();
}

/** This action is called for every patient that returns the find action. This is a callback action and inserts the found studies
  * in the list study
  */

/*It's a callback function, can't own to the class, It's can be called if its belongs to the class, for this
  reason, it hasn't been declared in the class pacsfind  */
void progressCallbackStudy(
        void * /*callbackData*/ ,
        T_DIMSE_C_FindRQ * /*request*/ ,
        int /*responseCount*/,
        T_DIMSE_C_FindRSP */*rsp*/,
        DcmDataset *responseIdentifiers
        )
{        
    Study study;
    const char* text;
    StudyListSingleton *studyList; 
    
    //set the patient Name
    responseIdentifiers->findAndGetString(DCM_PatientsName,text,false);
    if (text != NULL) study.setPatientName(text);   

    //set Patient's Birth Date
    responseIdentifiers->findAndGetString(DCM_PatientsBirthDate,text,false);
    if (text != NULL) study.setPatientBirthDate(text);
    
    //set Patient's Id
    responseIdentifiers->findAndGetString(DCM_PatientID,text,false);
    if (text != NULL) study.setPatientId(text);
     
    //set Patient's Sex
    responseIdentifiers->findAndGetString(DCM_PatientsSex,text,false);
    if (text != NULL) study.setPatientSex(text);
    
    //set Patiens Age
    responseIdentifiers->findAndGetString(DCM_PatientsAge,text,false);
    if (text != NULL) study.setPatientAge(text);  
        
    //set Study ID
    responseIdentifiers->findAndGetString(DCM_StudyID,text,false);
    if (text != NULL) study.setStudyId(text);     
    
    //set Study Date
    responseIdentifiers->findAndGetString(DCM_StudyDate,text,false);
    if (text != NULL) study.setStudyDate(text); 
      
     //set Study Description     
    responseIdentifiers->findAndGetString(DCM_StudyDescription,text,false);
    if (text != NULL) study.setStudyDescription(text);  
     
    //set Study Time
    responseIdentifiers->findAndGetString(DCM_StudyTime,text,false);
    if (text != NULL) study.setStudyTime(text);
     
    //set Institution Name
    responseIdentifiers->findAndGetString(DCM_InstitutionName,text,false);
    if (text != NULL) study.setInstitutionName(text);

    //set StudyUID
    responseIdentifiers->findAndGetString(DCM_StudyInstanceUID,text,false);
    if (text != NULL) study.setStudyUID(text); 
     
    //set Accession Number
    responseIdentifiers->findAndGetString(DCM_AccessionNumber,text,false);
    if (text != NULL) study.setAccessionNumber(text);
         
    //set Study Modality
    responseIdentifiers->findAndGetString(DCM_Modality,text,false);
    if (text != NULL) study.setStudyModality(text);  
    
    //set PACS AE Title Called
    responseIdentifiers->findAndGetString(DCM_RetrieveAETitle,text,false);
    if (text != NULL) study.setPacsAETitle(text);  
     

     
    //gets the pointer to the study list and inserts the new study    
    studyList = StudyListSingleton::getStudyListSingleton();
    studyList->insert(study);
    
}


static const char *     opt_abstractSyntax = UID_FINDModalityWorklistInformationModel;

/** Query studies to the pacs
  *        @param status
  */
Status QueryStudy::find()

{

    DIC_US msgId = m_assoc->nextMsgID++;
    T_ASC_PresentationContextID presId;
    T_DIMSE_C_FindRQ req;
    T_DIMSE_C_FindRSP rsp;
    DcmDataset *statusDetail = NULL;
    Status state;

    //If not connection has been setted, return error because we need a PACS connection
    if (m_assoc == NULL) 
    {
        return state.setStatus(error_NoConnection);
    }
    
    //If not mask has been setted, return error, we need a search mask
    if (m_mask == NULL)
    {
        return state.setStatus(error_NoMask);
    }

    /* figure out which of the accepted presentation contexts should be used */
   presId = ASC_findAcceptedPresentationContextID(m_assoc, UID_FINDStudyRootQueryRetrieveInformationModel);
    if (presId == 0)
    {        
        return state.setStatus(DIMSE_NOVALIDPRESENTATIONCONTEXTID);
    }

    /* prepare the transmission of data */
    bzero((char*)&req, sizeof(req));
    req.MessageID = msgId;
    strcpy(req.AffectedSOPClassUID, opt_abstractSyntax);
    req.DataSetType = DIMSE_DATASET_PRESENT;
    req.Priority = DIMSE_PRIORITY_LOW;


    /* finally conduct transmission of data */
    OFCondition cond = DIMSE_findUser(m_assoc, presId, &req, m_mask,
                          progressCallbackStudy,NULL,
                          DIMSE_BLOCKING, 0,
                          &rsp, &statusDetail);
    
    /* dump status detail information if there is some */
    if (statusDetail != NULL) {
        delete statusDetail;
    }

    /* return */
    return state.setStatus(cond);
}


/** get the list study with the results of the query
  *        @return  A pointer to the ListStudy with the results of the query
 */
StudyListSingleton* QueryStudy::getStudyList()
{
    m_studyListSingleton->firstStudy();
    return m_studyListSingleton;
}

}

