
/******************************************************************************
*
*   FILE
*   ----
*   TicketAuthenticator.c
*
*   History
*   -------
*   2013-01-31   File created
*
*******************************************************************************
*
*   This file is generated by the 'acplt_builder' command
*
******************************************************************************/


#ifndef OV_COMPILE_LIBRARY_ksbase
#define OV_COMPILE_LIBRARY_ksbase
#endif


#include "ksbase.h"
#include "libov/ov_macros.h"
#include "ks_logfile.h"


OV_DLLFNCEXPORT OV_RESULT ksbase_TicketAuthenticator_constructor(
	OV_INSTPTR_ov_object 	pobj
) {
    /*    
    *   local variables
    */
    OV_INSTPTR_ov_class	pclass = NULL;

    OV_RESULT    result;

    /* do what the base class does first */
    result = ov_object_constructor(pobj);
    if(Ov_Fail(result))
         return result;

    /* do what */

     /*	check if there is already an authenticator of the same type     */
    pclass = Ov_GetParent(ov_instantiation, pobj);
    if(Ov_GetFirstChild(ov_instantiation, pclass) != pobj)
    	return OV_ERR_ALREADYEXISTS;
    if(Ov_GetNextChild(ov_instantiation, pobj))
    	return OV_ERR_ALREADYEXISTS;
    if(Ov_GetPreviousChild(ov_instantiation, pobj))
    	return OV_ERR_ALREADYEXISTS;

    Ov_StaticPtrCast(ksbase_TicketAuthenticator, pobj)->v_ticket.vtbl = NULL;


    return OV_ERR_OK;
}

/**
 * This method is called on startup.
 * It allocates memory for the ticket vtable and set the functions pointers to the object's functions
 */
OV_DLLFNCEXPORT void ksbase_TicketAuthenticator_startup(
		OV_INSTPTR_ov_object 	pobj
) {
	OV_INSTPTR_ksbase_TicketAuthenticator this = Ov_StaticPtrCast(ksbase_TicketAuthenticator, pobj);
	OV_VTBLPTR_ksbase_TicketAuthenticator pVTBLTicketAuthenticator = NULL;

	ov_object_startup(pobj);

	/*	link authenticator functions to ticketvtable	*/
	Ov_GetVTablePtr(ksbase_TicketAuthenticator, pVTBLTicketAuthenticator, this);
	if(!pVTBLTicketAuthenticator)
		return;
	if(!this->v_ticket.vtbl)
		this->v_ticket.vtbl = ov_malloc(sizeof(OV_TICKET_VTBL));

	if(!this->v_ticket.vtbl)
		return;
	KS_logfile_debug(("%s: startup", this->v_identifier));
	this->v_ticket.vtbl->createticket = pVTBLTicketAuthenticator->m_createticket;
	this->v_ticket.vtbl->deleteticket = pVTBLTicketAuthenticator->m_deleteticket;
	this->v_ticket.vtbl->encodereply = pVTBLTicketAuthenticator->m_encodereply;
	this->v_ticket.vtbl->getaccess = pVTBLTicketAuthenticator->m_TicketGetaccess;

	this->v_ticket.type = this->v_TicketType;
	return;
}

/**
 * Procedure called on object shutdown.
 * It frees the ticket's vtable pointer
 */
OV_DLLFNCEXPORT void ksbase_TicketAuthenticator_shutdown(
		OV_INSTPTR_ov_object 	pobj
) {
	OV_INSTPTR_ksbase_TicketAuthenticator this = Ov_StaticPtrCast(ksbase_TicketAuthenticator, pobj);

	KS_logfile_debug(("%s: shutdown - database started = %x", this->v_identifier, pdb->started));
	if(pdb->started){
		//prevent a crash if the database did not shutdown clean and the pointer points to foreign data
		ov_free(this->v_ticket.vtbl);
	}
	this->v_ticket.vtbl = NULL;

	ov_object_shutdown(pobj);
	return;
}


OV_DLLFNCEXPORT OV_BOOL ksbase_TicketAuthenticator_ACRead_get(
    OV_INSTPTR_ksbase_TicketAuthenticator          pobj
) {
	if(pobj->v_TicketAccess & OV_AC_READ)
		return TRUE;
	else
		return FALSE;
}

OV_DLLFNCEXPORT OV_RESULT ksbase_TicketAuthenticator_ACRead_set(
    OV_INSTPTR_ksbase_TicketAuthenticator          pobj,
    const OV_BOOL  value
) {
    if(value)
    	pobj->v_TicketAccess |= OV_AC_READ;
    else
    	pobj->v_TicketAccess &= ~(OV_AC_READ);

	return OV_ERR_OK;
}

OV_DLLFNCEXPORT OV_BOOL ksbase_TicketAuthenticator_ACWrite_get(
    OV_INSTPTR_ksbase_TicketAuthenticator          pobj
) {
	if(pobj->v_TicketAccess & OV_AC_WRITE)
		return TRUE;
	else
		return FALSE;
}

OV_DLLFNCEXPORT OV_RESULT ksbase_TicketAuthenticator_ACWrite_set(
    OV_INSTPTR_ksbase_TicketAuthenticator          pobj,
    const OV_BOOL  value
) {
	 if(value)
		 pobj->v_TicketAccess |= OV_AC_WRITE;
	 else
		 pobj->v_TicketAccess &= ~(OV_AC_WRITE);
	 return OV_ERR_OK;
}

OV_DLLFNCEXPORT OV_BOOL ksbase_TicketAuthenticator_ACInstantiate_get(
    OV_INSTPTR_ksbase_TicketAuthenticator          pobj
) {
	if(pobj->v_TicketAccess & OV_AC_INSTANTIABLE)
		return TRUE;
	else
		return FALSE;
}

OV_DLLFNCEXPORT OV_RESULT ksbase_TicketAuthenticator_ACInstantiate_set(
    OV_INSTPTR_ksbase_TicketAuthenticator          pobj,
    const OV_BOOL  value
) {
	 if(value)
		 pobj->v_TicketAccess |= OV_AC_INSTANTIABLE;
	 else
		 pobj->v_TicketAccess &= ~(OV_AC_INSTANTIABLE);
	 return OV_ERR_OK;
}

OV_DLLFNCEXPORT OV_BOOL ksbase_TicketAuthenticator_ACDelete_get(
    OV_INSTPTR_ksbase_TicketAuthenticator          pobj
) {
	if(pobj->v_TicketAccess & OV_AC_DELETEABLE)
		return TRUE;
	else
		return FALSE;
}

OV_DLLFNCEXPORT OV_RESULT ksbase_TicketAuthenticator_ACDelete_set(
    OV_INSTPTR_ksbase_TicketAuthenticator          pobj,
    const OV_BOOL  value
) {
	 if(value)
		 pobj->v_TicketAccess |= OV_AC_DELETEABLE;
	 else
		 pobj->v_TicketAccess &= ~(OV_AC_DELETEABLE);
	 return OV_ERR_OK;
}

OV_DLLFNCEXPORT OV_BOOL ksbase_TicketAuthenticator_ACRename_get(
    OV_INSTPTR_ksbase_TicketAuthenticator          pobj
) {
	if(pobj->v_TicketAccess & OV_AC_RENAMEABLE)
		return TRUE;
	else
		return FALSE;
}

OV_DLLFNCEXPORT OV_RESULT ksbase_TicketAuthenticator_ACRename_set(
    OV_INSTPTR_ksbase_TicketAuthenticator          pobj,
    const OV_BOOL  value
) {
	 if(value)
		 pobj->v_TicketAccess |= OV_AC_RENAMEABLE;
	 else
		 pobj->v_TicketAccess &= ~(OV_AC_RENAMEABLE);
	 return OV_ERR_OK;
}

OV_DLLFNCEXPORT OV_BOOL ksbase_TicketAuthenticator_ACLink_get(
    OV_INSTPTR_ksbase_TicketAuthenticator          pobj
) {
	if(pobj->v_TicketAccess & OV_AC_LINKABLE)
		return TRUE;
	else
		return FALSE;
}

OV_DLLFNCEXPORT OV_RESULT ksbase_TicketAuthenticator_ACLink_set(
    OV_INSTPTR_ksbase_TicketAuthenticator          pobj,
    const OV_BOOL  value
) {
	 if(value)
		 pobj->v_TicketAccess |= OV_AC_LINKABLE;
	 else
		 pobj->v_TicketAccess &= ~(OV_AC_LINKABLE);
	 return OV_ERR_OK;
}

OV_DLLFNCEXPORT OV_BOOL ksbase_TicketAuthenticator_ACUnlink_get(
    OV_INSTPTR_ksbase_TicketAuthenticator          pobj
) {
	if(pobj->v_TicketAccess & OV_AC_UNLINKABLE)
		return TRUE;
	else
		return FALSE;
}

OV_DLLFNCEXPORT OV_RESULT ksbase_TicketAuthenticator_ACUnlink_set(
    OV_INSTPTR_ksbase_TicketAuthenticator          pobj,
    const OV_BOOL  value
) {
	 if(value)
		 pobj->v_TicketAccess |= OV_AC_UNLINKABLE;
	 else
		 pobj->v_TicketAccess &= ~(OV_AC_UNLINKABLE);
	 return OV_ERR_OK;
}

OV_DLLFNCEXPORT OV_TICKET *ksbase_TicketAuthenticator_createticket(
	void *data,
	OV_TICKET_TYPE type
) {
    return NULL;
}

OV_DLLFNCEXPORT void ksbase_TicketAuthenticator_deleteticket(
	OV_TICKET *pticket
) {
    return;
}

OV_DLLFNCEXPORT OV_BOOL ksbase_TicketAuthenticator_encodereply(
	void *data,
	OV_TICKET *pticket
) {
    return FALSE;
}

OV_DLLFNCEXPORT OV_ACCESS ksbase_TicketAuthenticator_TicketGetaccess(
	const OV_TICKET *pticket
) {
    return OV_AC_NONE;
}
