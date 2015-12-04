
/******************************************************************************
*
*   FILE
*   ----
*   WagoManager.c
*
*   History
*   -------
*   2011-01-25   File created
*
*******************************************************************************
*
*   This file is generated by the 'fb_builder' command
*
******************************************************************************/
/*****************************************************************************
*
* This file won't compile on Windows due to a type-check in the Ov_CreateObject
* macro. This is, however, no problem because the IPC is a Linux machine :-)
*
*
*******************************************************************************/

#ifndef OV_COMPILE_LIBRARY_wagoIPClib
#define OV_COMPILE_LIBRARY_wagoIPClib
#endif


#include "kbusl.h"
#include "wagoIPClib.h"
#include "wagoIPClib_macros.h"
#include "libov/ov_macros.h"
#include "libov/ov_logfile.h"
#include "kbus.h"
#include "misc.h"




OV_DLLFNCEXPORT void wagoIPClib_WagoIPCManager_startup(
	OV_INSTPTR_ov_object 	pobj
) {
	int res;
    	OV_INSTPTR_wagoIPClib_WagoIPCManager pinst = Ov_StaticPtrCast(wagoIPClib_WagoIPCManager, pobj);
    	
	res = kbus_init();
	if(res)
	{
		ov_logfile_error("%s: error while initializing kbus.\n", pinst->v_identifier);
		pinst->v_Error = FALSE;
		switch(res)
		{
			case KBUS_ERROR_NODEV:
				pinst->v_ErrorCode = KBUS_ERROR_NODEV;
				ov_string_setvalue(&pinst->v_ErrorString, "No device found");
				break;
			case KBUS_ERROR_IO:
				pinst->v_ErrorCode = KBUS_ERROR_IO;
				ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_IO);
				break;
			default:	
				pinst->v_ErrorCode = KBUS_ERROR_INIT;
				ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_INIT);
		}
		return;
		
	}
	
	res = kbus_reset_wdi_cfg();
	kbus_checkerr(res)
	
	res = misc_init();
	if(res)
	{
		ov_logfile_error("%s: error while initializing misc functionality.\n", pinst->v_identifier);
		pinst->v_Error = FALSE;
		switch(res)
		{
			case KBUS_ERROR_NODEV:
				pinst->v_ErrorCode = KBUS_ERROR_NODEV;
				ov_string_setvalue(&pinst->v_ErrorString, "mapping not supported");
				break;
			case KBUS_ERROR_IO:
				pinst->v_ErrorCode = KBUS_ERROR_IO;
				ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_IO);
				break;
			case KBUS_ERROR_FAULT:
				pinst->v_ErrorCode = KBUS_ERROR_FAULT;
				ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_FAULT);
				break;
			case KBUS_ERROR_PERM:
				pinst->v_ErrorCode = KBUS_ERROR_PERM;
				ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_PERM);
				break;
			default:	
				pinst->v_ErrorCode = KBUS_ERROR_INIT;
				ov_string_setvalue(&pinst->v_ErrorString, MISC_ERRORSTR_INIT);
		}
		return;
		
	}
	
	misc_set_led_ctrl(0, 1);
	misc_set_led_off(0);
	misc_set_led_ctrl(1, 1);
	misc_set_led_off(1);
	misc_set_led_ctrl(2, 1);
	misc_set_led_off(2);
	misc_set_led_ctrl(3, 1);
	misc_set_led_off(3);

    return;
}


OV_DLLFNCEXPORT void wagoIPClib_WagoIPCManager_shutdown(
	OV_INSTPTR_ov_object 	pobj
) {
	misc_set_led_off(0);
	misc_set_led_off(1);
	misc_set_led_off(2);
	misc_set_led_off(3);
	
	kbus_shutdown();
	misc_close();
	return;
}


OV_DLLFNCEXPORT void wagoIPClib_WagoIPCManager_typemethod(
	OV_INSTPTR_fb_functionblock	pfb,
	OV_TIME						*pltc
) {
	OV_INSTPTR_wagoIPClib_WagoIPCManager pinst = Ov_StaticPtrCast(wagoIPClib_WagoIPCManager, pfb);
	struct kbus_info* pinfo;
	OV_INSTPTR_ov_object pdatablock;
	OV_INSTPTR_kbuslib_Clamp pnewclamp = NULL;
	OV_INSTPTR_ov_object pcClampobj;
	OV_INSTPTR_kbuslib_Clamp pcClamp;
	
	OV_UINT tmp_number;
	OV_BOOL write = FALSE;
	
	struct kbus_add_pi_info* ppi_info;
	unsigned short dig_input_offset;
	unsigned short dig_output_offset;
	unsigned int i;
	unsigned int j;
	OV_STRING clamp_name = NULL;
	
	int res;
	double dtemp;
	uint8_t btemp;
	uint16_t value;
	
	pinfo = kbus_get_info();		//get information
	
	res = kbus_read_bus_status();		//get status information
	kbus_checkerr(res)
	
        
        pinst->v_ClampErrorType = pinfo->status.error_type;
	pinst->v_ErrorArgument = pinfo->status.error_arg;
	pinst->v_ClampWithError = pinfo->status.clamp_with_error;
	pinst->v_LEDErrorCode = pinfo->status.led_error_code;
	pinst->v_LEDErrorArgument = pinfo->status.led_error_arg;
	
	res = kbus_read_bitlength();	//get data information
	kbus_checkerr(res)
	
	pinst->v_nrAO = pinfo->bitlength_descr.nr_analog_out / 16;	//each clamp has 16 bits of data
	pinst->v_nrAI = pinfo->bitlength_descr.nr_analog_in / 16;
	pinst->v_nrDO = pinfo->bitlength_descr.nr_digital_out;
	pinst->v_nrDI = pinfo->bitlength_descr.nr_digital_in;
	
	
	res = kbus_read_digio_offsets();		//get offsets of digital ios in process images
	kbus_checkerr(res)
        
        dig_input_offset = pinfo->offsets.input_offset;
        dig_output_offset = pinfo->offsets.output_offset;
        
        
        /***************************************************
        	Reset Errors
        ****************************************************/
        
        pinst->v_Error = TRUE;
        pinst->v_ErrorCode = KBUS_ERROR_NOERROR;
        ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_NOERROR);
        misc_set_led_off(3);
        
        ov_string_setvalue(&pinst->v_RangeErrorIdent, "<none>");
        
        
        /****************************************************
        	Auto Detection of Clamps
        *****************************************************/
        
        
	if(pinst->v_autodetect == TRUE)
	{
		pdatablock = Ov_GetFirstChild(ov_containment, Ov_PtrUpCast(ov_domain, pfb));
		
		while(pdatablock)
		{
									
			if(Ov_CanCastTo(kbuslib_Clamp, pdatablock))	//if contained object is a Clamp, delete it an get the next one
			{
				if(Ov_Fail(Ov_DeleteObject(pdatablock)))
				{
					ov_logfile_error("%s: deletion of datablock %s failed\n", 
						pinst->v_identifier, Ov_StaticPtrCast(kbuslib_Clamp, pdatablock)->v_identifier);
					pinst->v_autodetect = FALSE;
					
					pinst->v_Error = FALSE;
					pinst->v_ErrorCode = KBUS_ERROR_AUTODETECT;
					ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_AUTODETECT);
					misc_set_led_on(3);
					misc_set_led_off(2);
					return;
				}
			
				pdatablock = Ov_GetFirstChild(ov_containment, Ov_PtrUpCast(ov_domain, pfb));
			}
			else	//if not, get next contained object
				pdatablock = Ov_GetNextChild(ov_containment, Ov_PtrUpCast(ov_object, pdatablock));
		}
		
		
		/* get information about installed terminals */
		ppi_info = kbus_get_add_pi_info();
		i=0;
		do 
		{
		
			res = kbus_read_add_pi_info(i);
			kbus_checkerr(res)
			
			/* check terminalname to identify which blocks must be created */
			switch(ppi_info[i].terminalname)
			{
				case 209:		/*digital in*/
					for(j = 0; j < ppi_info[i].sizePAE; j++)
					{
						/*create new object with name "Terminal$terminalnumber:$channelnumber-DI"*/
						ov_string_print(&clamp_name, "Terminal%03hhu:%02u-DI", ppi_info[i].position, j + 1);
						if(Ov_Fail(Ov_CreateObject(kbuslib_DigitalIN, pnewclamp, pinst, clamp_name)))
						{
							ov_logfile_error("%s: creation of DigitalIN %s failed\n", 
								pinst->v_identifier, clamp_name);
							pinst->v_autodetect = FALSE;
					
							pinst->v_Error = FALSE;
							pinst->v_ErrorCode = KBUS_ERROR_AUTODETECT;
							ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_AUTODETECT);
							misc_set_led_on(3);
							misc_set_led_off(2);
							return;
						}
						
						if(Ov_Fail(Ov_Link(fb_tasklist, pinst, pnewclamp)))
						{
							ov_logfile_warning("%s: DigitalIN %s could not be linked into Tasklist",
								pinst->v_identifier, clamp_name);
						}

						/*set byte Address*/
						pnewclamp->v_ByteAddress = ppi_info[i].bitposPAE / 8;
						/*determine BitOffset and adjust ByteAddress if necessary*/
						if((ppi_info[i].bitposPAE % 8 + j) > 7)
						{
							pnewclamp->v_ByteAddress += (ppi_info[i].bitposPAE % 8 + j) / 8;
							pnewclamp->v_BitOffset = (ppi_info[i].bitposPAE % 8 + j) % 8;
						}
						else
						{
							pnewclamp->v_BitOffset = ppi_info[i].bitposPAE % 8 + j;
						}
						/*set byteWidth*/
						pnewclamp->v_ByteWidth = 0;
						pnewclamp->v_Enabled = TRUE;
						
					}
				break;
				
				case 210:		/*digital out */
					for(j = 0; j < ppi_info[i].sizePAA; j++)
					{
						/*create new object with name "Terminal$terminalnumber:$channelnumber-DO"*/
						ov_string_print(&clamp_name, "Terminal%03hhu:%02u-DO", ppi_info[i].position, j + 1);
						if(Ov_Fail(Ov_CreateObject(kbuslib_DigitalOUT, pnewclamp, pinst, clamp_name)))
						{
							ov_logfile_error("%s: creation of DigitalOUT %s failed\n", 
								pinst->v_identifier, clamp_name);
							pinst->v_autodetect = FALSE;
					
							pinst->v_Error = FALSE;
							pinst->v_ErrorCode = KBUS_ERROR_AUTODETECT;
							ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_AUTODETECT);
							misc_set_led_on(3);
							misc_set_led_off(2);
							return;
						}
						if(Ov_Fail(Ov_Link(fb_tasklist, pinst, pnewclamp)))
						{
							ov_logfile_warning("%s: DigitalOUT %s could not be linked into Tasklist",
								pinst->v_identifier, clamp_name);
						}
						
						/*set byte Address*/
						pnewclamp->v_ByteAddress = ppi_info[i].bitposPAA / 8;
						/*determine BitOffset and adjust ByteAddress if necessary*/
						if((ppi_info[i].bitposPAA % 8 + j) > 7)
						{
							pnewclamp->v_ByteAddress += (ppi_info[i].bitposPAA % 8 + j) / 8;
							pnewclamp->v_BitOffset = (ppi_info[i].bitposPAA % 8 + j) % 8;
						}
						else
						{
							pnewclamp->v_BitOffset = ppi_info[i].bitposPAA % 8 + j;
						}
						/*set byteWidth*/
						pnewclamp->v_ByteWidth = 0;
						pnewclamp->v_Enabled = TRUE;
					}	
				break;
				
				case 211:		/*digital in/out */
							/*first create clamp-objects for inputs, then for outputs*/
							
					for(j = 0; j < ppi_info[i].sizePAE; j++)
					{
						/*create new object with name "Terminal$terminalnumber:$channelnumber-MDI"*/
						ov_string_print(&clamp_name, "Terminal%03hhu:%02u-MDI", ppi_info[i].position, j + 1);
						if(Ov_Fail(Ov_CreateObject(kbuslib_DigitalIN, pnewclamp, pinst, clamp_name)))
						{
							ov_logfile_error("%s: creation of DigitalIN %s failed\n", 
								pinst->v_identifier, clamp_name);
							pinst->v_autodetect = FALSE;
					
							pinst->v_Error = FALSE;
							pinst->v_ErrorCode = KBUS_ERROR_AUTODETECT;
							ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_AUTODETECT);
							misc_set_led_on(3);
							misc_set_led_off(2);
							return;
						}
						if(Ov_Fail(Ov_Link(fb_tasklist, pinst, pnewclamp)))
						{
							ov_logfile_warning("%s: DigitalIN %s could not be linked into Tasklist",
								pinst->v_identifier, clamp_name);
						}
						
						/*set byte Address*/
						pnewclamp->v_ByteAddress = ppi_info[i].bitposPAE / 8;
						/*determine BitOffset and adjust ByteAddress if necessary*/
						if((ppi_info[i].bitposPAE % 8 + j) > 7)
						{
							pnewclamp->v_ByteAddress += (ppi_info[i].bitposPAE % 8 + j) / 8;
							pnewclamp->v_BitOffset = (ppi_info[i].bitposPAE % 8 + j) % 8;
						}
						else
						{
							pnewclamp->v_BitOffset = ppi_info[i].bitposPAE % 8 + j;
						}
						/*set byteWidth*/
						pnewclamp->v_ByteWidth = 0;
						pnewclamp->v_Enabled = TRUE;
												
					}
					
					for(j = 0; j < ppi_info[i].sizePAA; j++)
					{
						/*create new object with name "Terminal$terminalnumber:$channelnumber-MDO"*/
						ov_string_print(&clamp_name, "Terminal%03hhu:%02u-MDO", ppi_info[i].position, j + 1);
						if(Ov_Fail(Ov_CreateObject(kbuslib_DigitalOUT, pnewclamp, pinst, clamp_name)))
						{
							ov_logfile_error("%s: creation of DigitalOUT %s failed\n", 
								pinst->v_identifier, clamp_name);
							pinst->v_autodetect = FALSE;
					
							pinst->v_Error = FALSE;
							pinst->v_ErrorCode = KBUS_ERROR_AUTODETECT;
							ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_AUTODETECT);
							misc_set_led_on(3);
							misc_set_led_off(2);
							return;
						}
						
						if(Ov_Fail(Ov_Link(fb_tasklist, pinst, pnewclamp)))
						{
							ov_logfile_warning("%s: DigitalOUT %s could not be linked into Tasklist",
								pinst->v_identifier, clamp_name);
						}


						/*set byte Address*/
						pnewclamp->v_ByteAddress = ppi_info[i].bitposPAA / 8;
						/*determine BitOffset and adjust ByteAddress if necessary*/
						if((ppi_info[i].bitposPAA % 8 + j) > 7)
						{
							pnewclamp->v_ByteAddress += (ppi_info[i].bitposPAA % 8 + j) / 8;
							pnewclamp->v_BitOffset = (ppi_info[i].bitposPAA % 8 + j) % 8;
						}
						else
						{
							pnewclamp->v_BitOffset = ppi_info[i].bitposPAA % 8 + j;
						}
						/*set byteWidth*/
						pnewclamp->v_ByteWidth = 0;
						pnewclamp->v_Enabled = TRUE;
					}
				break;
				
				default:		/*analog and special clamps are identified by product-id*/
					if(((ppi_info[i].terminalname > 400) && (ppi_info[i].terminalname < 500))
							&& ((ppi_info[i].terminalname != 404) && (ppi_info[i].terminalname!= 511)))
					{		/*analog in*/
						for(j = 0; j < ppi_info[i].channels; j++)
						{
							/*create new object with name "Terminal$terminalnumber:$channelnumber-AI"*/
							ov_string_print(&clamp_name, "Terminal%03hhu:%02u-AI", ppi_info[i].position, j + 1);
							if(Ov_Fail(Ov_CreateObject(kbuslib_AnalogIN, pnewclamp, pinst, clamp_name)))
							{
								ov_logfile_error("%s: creation of AnalogIN %s failed\n", 
									pinst->v_identifier, clamp_name);
								pinst->v_autodetect = FALSE;
					
								pinst->v_Error = FALSE;
								pinst->v_ErrorCode = KBUS_ERROR_AUTODETECT;
								ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_AUTODETECT);
								misc_set_led_on(3);
								misc_set_led_off(2);
								return;
							}

							if(Ov_Fail(Ov_Link(fb_tasklist, pinst, pnewclamp)))
							{
								ov_logfile_warning("%s: AnalogIN %s could not be linked into Tasklist",
										pinst->v_identifier, clamp_name);
							}

							/*set byte Address*/
							pnewclamp->v_ByteAddress = ppi_info[i].bitposPAE / 8 + j * sizeof(uint16_t);
							/*set byteWidth*/
							pnewclamp->v_ByteWidth = 2;
							pnewclamp->v_Enabled = TRUE;						
							
						}
						break;
					}
					else
					if(((ppi_info[i].terminalname > 500) && (ppi_info[i].terminalname < 600))
							&& ((ppi_info[i].terminalname != 404) && (ppi_info[i].terminalname != 511)))
					{		/*analog out*/
						for(j = 0; j < ppi_info[i].channels; j++)
						{
							/*create new object with name "Terminal$terminalnumber:$channelnumber-AO"*/
							ov_string_print(&clamp_name, "Terminal%03hhu:%02u-AO", ppi_info[i].position, j + 1);
							if(Ov_Fail(Ov_CreateObject(kbuslib_AnalogOUT, pnewclamp, pinst, clamp_name)))
							{
								ov_logfile_error("%s: creation of AnalogOUT %s failed\n", 
									pinst->v_identifier, clamp_name);
								pinst->v_autodetect = FALSE;
					
								pinst->v_Error = FALSE;
								pinst->v_ErrorCode = KBUS_ERROR_AUTODETECT;
								ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_AUTODETECT);
								misc_set_led_on(3);
								misc_set_led_off(2);
								return;
							}

							if(Ov_Fail(Ov_Link(fb_tasklist, pinst, pnewclamp)))
							{
								ov_logfile_warning("%s: AnalogOUT %s could not be linked into Tasklist",
									pinst->v_identifier, clamp_name);
							}

							/*set byte Address*/
							pnewclamp->v_ByteAddress = ppi_info[i].bitposPAA / 8 + j * sizeof(uint16_t);
							/*set byteWidth*/
							pnewclamp->v_ByteWidth = 2;
							pnewclamp->v_Enabled = TRUE;
												
						}
						
						break;
					}
					else
					{/*special clamps; not tested yet*/
						j=0;	/*initialize j*/
						do		/*at least one input*/
						{
							if(ppi_info[i].sizePAE)
							{	/*input found*/
									/*create new object with name "Terminal$terminalnumber:$channelnumber-SI"*/
								ov_string_print(&clamp_name, "Terminal%03hhu:%02u-SI", ppi_info[i].position, j);
								if(Ov_Fail(Ov_CreateObject(kbuslib_SpecialIN, pnewclamp, pinst, clamp_name)))
								{
									ov_logfile_error("%s: creation of SpecialIN %s failed\n", 
									pinst->v_identifier, clamp_name);
									pinst->v_autodetect = FALSE;
					
									pinst->v_Error = FALSE;
									pinst->v_ErrorCode = KBUS_ERROR_AUTODETECT;
									ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_AUTODETECT);
									misc_set_led_on(3);
									misc_set_led_off(2);
									return;
								}
								
								if(Ov_Fail(Ov_Link(fb_tasklist, pinst, pnewclamp)))
								{
									ov_logfile_warning("%s: SpecialIN %s could not be linked into Tasklist",
										pinst->v_identifier, clamp_name);
								}

								/*assuming number of input channels is the same as number of output channels. assuming
									each channel has the same size*/
									
								pnewclamp->v_ByteAddress = ppi_info[i].bitposPAE / 8;
								if(ppi_info[i].channels)
									pnewclamp->v_ByteAddress += j * (ppi_info[i].sizePAE / 8 / ppi_info[i].channels);
								pnewclamp->v_ByteWidth = ppi_info[i].sizePAE / 8;
								if(ppi_info[i].channels)
									pnewclamp->v_ByteWidth /= ppi_info[i].channels;
									
								pnewclamp->v_Enabled = TRUE;
							}
							
							if(ppi_info[i].sizePAA)
							{	/*output found*/
									/*create new object with name "Terminal$terminalnumber:$channelnumber-SO"*/
								ov_string_print(&clamp_name, "Terminal%03hhu:%02u-SO", ppi_info[i].position, j);
								if(Ov_Fail(Ov_CreateObject(kbuslib_SpecialOUT, pnewclamp, pinst, clamp_name)))
								{
									ov_logfile_error("%s: creation of SpecialOUT %s failed\n", 
									pinst->v_identifier, clamp_name);
									pinst->v_autodetect = FALSE;
					
									pinst->v_Error = FALSE;
									pinst->v_ErrorCode = KBUS_ERROR_AUTODETECT;
									ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_AUTODETECT);
									misc_set_led_on(3);
									misc_set_led_off(2);
									return;
								}

								if(Ov_Fail(Ov_Link(fb_tasklist, pinst, pnewclamp)))
								{
									ov_logfile_warning("%s: AnalogIN %s could not be linked into Tasklist",
										pinst->v_identifier, clamp_name);
								}

								pnewclamp->v_ByteAddress = ppi_info[i].bitposPAA / 8;
								if(ppi_info[i].channels)
									pnewclamp->v_ByteAddress += j * (ppi_info[i].sizePAA / 8 / ppi_info[i].channels);
								pnewclamp->v_ByteWidth = ppi_info[i].sizePAA / 8;
								if(ppi_info[i].channels)
									pnewclamp->v_ByteWidth /= ppi_info[i].channels;
								pnewclamp->v_Enabled = TRUE;
							}
						
						j++;		/*next clamp*/ 
						
						} while(j < ppi_info[i].channels);
						
					}
					
				break;
			}			
			
			
		
			i++;
		}while(i<ppi_info[0].terminals);
	
		
		
	pinst->v_autodetect = FALSE;		/*autodetec only once*/
				
	}     
	
	/********************************************************
		End Auto Detection of Clamps
	********************************************************/
	
	/******************************************************
		LED and Switch Functions
	******************************************************/
	
	if(misc_get_run_stop_switch())
		pinst->v_RunSwitch = TRUE;
	else
		pinst->v_RunSwitch = FALSE;
		
	if(misc_get_leds_num() >= 4)
	{
		
		
		if(pinst->v_UserLED)
		{
			switch(pinst->v_UserLEDColor)
			{
				case 1: 	
					misc_set_led_on(1);
					misc_set_led_off(0);
					break;
				case 2:	
					misc_set_led_on(0);
					misc_set_led_on(1);
					break;
				case 0:
				default:
					misc_set_led_on(0);
					misc_set_led_off(1);
					break; 
			}
		}
		else
		{
			misc_set_led_off(0);
			misc_set_led_off(1);
		}		
		
		
		if(pinst->v_LEDOnlast)
		{
			misc_set_led_off(2);
			pinst->v_LEDOnlast = FALSE;
		}
		else
		{
			misc_set_led_on(2);
			pinst->v_LEDOnlast = TRUE;
		}
	}
	
	/******************************************************
		End LED and Switch functions
	*******************************************************/
	
	
	/*****************************************************
		Communication with clamps
	******************************************************/
	
	
	
	/*	get Process data	*/
	
	res = kbus_read_input_image();
	switch(res)
	{
		case 0:			/*no error*/
			ov_time_gettime(&pinst->v_LastRead);	/*set time of successfull read*/
			break;
			
		case KBUS_ERROR_BUSY:
			pinst->v_Error = FALSE;
			pinst->v_ErrorCode = KBUS_ERROR_BUSY;
			ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_BUSY);
			ov_logfile_error("%s: kbus busy while trying to read input image", pinst->v_identifier);
			misc_set_led_on(3);
			misc_set_led_off(2);
			pinst->v_actimode = 0;
			return;
		
		case KBUS_ERROR_NODEV:
			pinst->v_Error = FALSE;
			pinst->v_ErrorCode = KBUS_ERROR_NODEV;
			ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_NODEV);
			ov_logfile_error("%s: DR-ram not initialized when trying to read input image", pinst->v_identifier);
			misc_set_led_on(3);
			misc_set_led_off(2);
			pinst->v_actimode = 0;
			return;
		
		case KBUS_ERROR_IO:
			pinst->v_Error = FALSE;
			pinst->v_ErrorCode = KBUS_ERROR_IO;
			ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_IO);
			ov_logfile_error("%s: kbus IO-Error while trying to read input image", pinst->v_identifier);
			misc_set_led_on(3);
			misc_set_led_off(2);
			pinst->v_actimode = 0;
			return;
		
		case KBUS_ERROR_TIME:
			pinst->v_Error = FALSE;
			pinst->v_ErrorCode = KBUS_ERROR_TIME;
			ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_TIME);
			ov_logfile_error("%s: communication timed out while trying to read input image", pinst->v_identifier);
			misc_set_led_on(3);
			misc_set_led_off(2);
			pinst->v_actimode = 0;
			return;
			
	}
	
	res = kbus_read_output_image();
	switch(res)
	{
		case 0:			/*no error*/
			break;
			
		case KBUS_ERROR_BUSY:
			pinst->v_Error = FALSE;
			pinst->v_ErrorCode = KBUS_ERROR_BUSY;
			ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_BUSY);
			ov_logfile_error("%s: kbus busy while trying to read output image", pinst->v_identifier);
			misc_set_led_on(3);
			misc_set_led_off(2);
			pinst->v_actimode = 0;
			return;
		
		case KBUS_ERROR_NODEV:
			pinst->v_Error = FALSE;
			pinst->v_ErrorCode = KBUS_ERROR_NODEV;
			ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_NODEV);
			ov_logfile_error("%s: DR-ram not initialized when trying to read output image", pinst->v_identifier);
			misc_set_led_on(3);
			misc_set_led_off(2);
			pinst->v_actimode = 0;
			return;
		
		case KBUS_ERROR_IO:
			pinst->v_Error = FALSE;
			pinst->v_ErrorCode = KBUS_ERROR_IO;
			ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_IO);
			ov_logfile_error("%s: kbus IO-Error while trying to read output image", pinst->v_identifier);
			misc_set_led_on(3);
			misc_set_led_off(2);
			pinst->v_actimode = 0;
			return;
		
		case KBUS_ERROR_TIME:
			pinst->v_Error = FALSE;
			pinst->v_ErrorCode = KBUS_ERROR_TIME;
			ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_TIME);
			ov_logfile_error("%s: communication timed out while trying to read output image", pinst->v_identifier);
			misc_set_led_on(3);
			misc_set_led_off(2);
			pinst->v_actimode = 0;
			return;
			
	}
	
	/*	iterate over clamps	*/
	
	Ov_ForEachChild(ov_containment, pinst, pcClampobj)		/*iterate over all contained objects*/
	{
		/*	Analog Ins	*/
		
		if(Ov_CanCastTo(kbuslib_AnalogIN, pcClampobj))	/* if object is an AnalogIN, feed in value*/
		{
			pcClamp = Ov_StaticPtrCast(kbuslib_Clamp, pcClampobj);
			
			tmp_number = pcClamp->v_ByteAddress;
			
			if(pcClamp->v_Enabled)				/*check if clamp enabled*/
			{
				if(tmp_number <= dig_input_offset)
				{
					/*if there was an error, or the clamp was diasbled reset it*/
					if((!pcClamp->v_Error) || pcClamp->v_ErrorCode == KBUS_CLAMP_DISABLED)	
					{
						pcClamp->v_Error = TRUE;
						pcClamp->v_ErrorCode = KBUS_ERROR_NOERROR;
						ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOERROR); 
					}
					
					
					Ov_StaticPtrCast(kbuslib_AnalogIN, pcClamp)->v_Value = 
						(((uint16_t*) pinfo->image.inputs.data)[tmp_number / sizeof(uint16_t)] / 32767.00);
					Ov_StaticPtrCast(kbuslib_AnalogIN, pcClamp)->v_ValuePV.value =
							Ov_StaticPtrCast(kbuslib_AnalogIN, pcClamp)->v_Value;
					Ov_StaticPtrCast(kbuslib_AnalogIN, pcClamp)->v_ValuePV.time = pinst->v_LastRead;
					Ov_StaticPtrCast(kbuslib_AnalogIN, pcClamp)->v_ValuePV.state = OV_ST_GOOD;
					Ov_StaticPtrCast(kbuslib_AnalogIN, pcClamp)->v_TimeStamp = pinst->v_LastRead;
				}
				else
				{
					ov_logfile_error("%s: An analog input Byte Address lies within space of digital ins", pinst->v_identifier);
					pcClamp->v_Error = FALSE;
					pcClamp->v_ErrorCode = KBUS_ERROR_NOCLAMP;
					ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOCLAMP); 
					misc_set_led_on(3);
					misc_set_led_off(2);
					Ov_StaticPtrCast(kbuslib_AnalogIN, pcClamp)->v_ValuePV.state = OV_ST_BAD;
				}
				
			}
			else
			{
				pcClamp->v_Error = TRUE;
				pcClamp->v_ErrorCode = KBUS_CLAMP_DISABLED;
				ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_STR_CLAMP_DISABLED); 
				Ov_StaticPtrCast(kbuslib_AnalogIN, pcClamp)->v_ValuePV.state = OV_ST_BAD;
			}
			
			continue;		/*	continue to next object	*/
		}
		
		/*	Special Ins	*/
		
		if(Ov_CanCastTo(kbuslib_SpecialIN, pcClampobj))	/* if object is an SpecialIN, feed in value*/
		{
			pcClamp = Ov_StaticPtrCast(kbuslib_Clamp, pcClampobj);
			
			tmp_number = pcClamp->v_ByteAddress;
			
			if(pcClamp->v_Enabled)				/*check if clamp enabled*/
			{

				if(tmp_number <= dig_input_offset)
				{
					/*if there was an error, or the clamp was diasbled reset it*/
					if((!pcClamp->v_Error) || pcClamp->v_ErrorCode == KBUS_CLAMP_DISABLED)	
					{
						pcClamp->v_Error = TRUE;
						pcClamp->v_ErrorCode = KBUS_ERROR_NOERROR;
						ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOERROR); 
					}
	
					Ov_SetDynamicVectorValue(&(Ov_StaticPtrCast(kbuslib_SpecialIN, pcClamp)->v_Value.value.valueunion.val_byte_vec), &(pinfo->image.inputs.data[tmp_number]), pcClamp->v_ByteWidth, BYTE);
					Ov_StaticPtrCast(kbuslib_SpecialIN, pcClamp)->v_TimeStamp = pinst->v_LastRead;
						
				}
				else
				{
					ov_logfile_error("%s: special input Byte Address lies within space of digital ins", pinst->v_identifier);
					pcClamp->v_Error = FALSE;
					pcClamp->v_ErrorCode = KBUS_ERROR_NOCLAMP;
					ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOCLAMP); 
					misc_set_led_on(3);
					misc_set_led_off(2);
					
				}
				
			}
			else
			{
				pcClamp->v_Error = TRUE;
				pcClamp->v_ErrorCode = KBUS_CLAMP_DISABLED;
				ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_STR_CLAMP_DISABLED); 
				
			}
			
			continue;		/*	continue to next object	*/
		}
		
		/*	Digital ins	*/
		
		if(Ov_CanCastTo(kbuslib_DigitalIN, pcClampobj))	/* if object is an DigitalIN, feed in value*/
		{
			pcClamp = Ov_StaticPtrCast(kbuslib_Clamp, pcClampobj);
			tmp_number = pcClamp->v_ByteAddress;
			
			if(pcClamp->v_Enabled)				/*check if clamp is enabled*/
			{

				if(tmp_number >= dig_input_offset)
				{
					/*if there was an error, or the clamp was diasbled reset it*/
					if((!(pcClamp->v_Error))
						|| pcClamp->v_ErrorCode == KBUS_CLAMP_DISABLED)	
					{
						pcClamp->v_Error = TRUE;
						pcClamp->v_ErrorCode = KBUS_ERROR_NOERROR;
						ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOERROR); 
					}
					
					btemp = pinfo->image.inputs.data[tmp_number];
					btemp = (btemp >> pcClamp->v_BitOffset) & 1;
					if(btemp)
						Ov_StaticPtrCast(kbuslib_DigitalIN, pcClamp)->v_Value = TRUE;
					else
						Ov_StaticPtrCast(kbuslib_DigitalIN, pcClamp)->v_Value = FALSE;

					Ov_StaticPtrCast(kbuslib_DigitalIN, pcClamp)->v_ValuePV.value =
							Ov_StaticPtrCast(kbuslib_DigitalIN, pcClamp)->v_Value;
					Ov_StaticPtrCast(kbuslib_DigitalIN, pcClamp)->v_ValuePV.state = OV_ST_GOOD;
					Ov_StaticPtrCast(kbuslib_DigitalIN, pcClamp)->v_ValuePV.time = pinst->v_LastRead;
					Ov_StaticPtrCast(kbuslib_DigitalIN, pcClamp)->v_TimeStamp = pinst->v_LastRead;
				}
				else
				{
					ov_logfile_error("%s: specified address lies not within space of digital inputs", pinst->v_identifier);
					pcClamp->v_Error = FALSE;
					pcClamp->v_ErrorCode = KBUS_ERROR_NOCLAMP;
					ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOCLAMP); 
					misc_set_led_on(3);
					misc_set_led_off(2);
					Ov_StaticPtrCast(kbuslib_DigitalIN, pcClamp)->v_ValuePV.state = OV_ST_BAD;
					
				}

			}
			else
			{
				pcClamp->v_Error = TRUE;
				pcClamp->v_ErrorCode = KBUS_CLAMP_DISABLED;
				ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_STR_CLAMP_DISABLED); 
				Ov_StaticPtrCast(kbuslib_DigitalIN, pcClamp)->v_ValuePV.state = OV_ST_BAD;
			}
			
			continue;		/*	continue to next object	*/
		}
		
		/*	Analog Outs	*/
		
		if(Ov_CanCastTo(kbuslib_AnalogOUT, pcClampobj))	/* if object is an AnalogOUT*/
		{
			pcClamp = Ov_StaticPtrCast(kbuslib_Clamp, pcClampobj);
			tmp_number = pcClamp->v_ByteAddress;
			
			if(pcClamp->v_Enabled)				/*check if clamp is enabled*/
			{

				if(tmp_number <= dig_output_offset)
				{
					/*if there was an error, or the clamp was disabled reset it*/
					if((!(pcClamp->v_Error))
						|| pcClamp->v_ErrorCode == KBUS_CLAMP_DISABLED)	
					{
						pcClamp->v_Error = TRUE;
						pcClamp->v_ErrorCode = KBUS_ERROR_NOERROR;
						ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOERROR); 
					}
					
					Ov_StaticPtrCast(kbuslib_AnalogOUT, pcClamp)->v_RBValue = 
						(((uint16_t*) pinfo->image.outputs.data)[tmp_number / sizeof(uint16_t)] / 32767.00);
					
					if(!Ov_StaticPtrCast(kbuslib_AnalogOUT, pcClamp)->v_ReadBackOnly)
					{
						write = TRUE;
						dtemp = Ov_StaticPtrCast(kbuslib_AnalogOUT, pcClamp)->v_Value;
						
						if(dtemp >= 0.0	&& dtemp <= 1.0)
						{
							((uint16_t*) pinfo->image.outputs.data)[tmp_number / sizeof(uint16_t)] = (uint16_t) (dtemp * 32767);
						}
						else
						{
							ov_logfile_error("%s: value exceeds range (0.0 - 1.0)",
								Ov_StaticPtrCast(kbuslib_AnalogIN, pcClamp)->v_identifier);
							pcClamp->v_Error = FALSE;
							pcClamp->v_ErrorCode = KBUS_ERROR_OUT_OF_RANGE;
							ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_OUT_OF_RANGE);
							pinst->v_Error = FALSE;
							pinst->v_ErrorCode = KBUS_ERROR_OUT_OF_RANGE;
							ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_OUT_OF_RANGE);
							ov_string_setvalue(&pinst->v_RangeErrorIdent, pcClamp->v_identifier);
							misc_set_led_on(3);
							
						}
					}
					
				}
				else
				{
					ov_logfile_error("%s: specified address lies within range of digital outs", pinst->v_identifier);
					pcClamp->v_Error = FALSE;
					pcClamp->v_ErrorCode = KBUS_ERROR_NOCLAMP;
					ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOCLAMP); 
					misc_set_led_on(3);
					misc_set_led_off(2);
					
				}
				
			}
			else
			{
				pcClamp->v_Error = TRUE;
				pcClamp->v_ErrorCode = KBUS_CLAMP_DISABLED;
				ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_STR_CLAMP_DISABLED); 
				
			}
			
			continue;		/*	continue to next object	*/
		}
		
		/*	Special Outs	*/
		
		if(Ov_CanCastTo(kbuslib_SpecialOUT, pcClampobj))	/* if object is a SpecialOUT*/
		{
			pcClamp = Ov_StaticPtrCast(kbuslib_Clamp, pcClampobj);
			tmp_number = pcClamp->v_ByteAddress;
			
			if(pcClamp->v_Enabled)				/*check if clamp is enabled*/
			{

				if(tmp_number <= dig_output_offset)
				{
					/*if there was an error, or the clamp was diasbled reset it*/
					if((!(pcClamp->v_Error))
						|| pcClamp->v_ErrorCode == KBUS_CLAMP_DISABLED)	
					{
						pcClamp->v_Error = TRUE;
						pcClamp->v_ErrorCode = KBUS_ERROR_NOERROR;
						ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOERROR); 
					}
					
					Ov_SetDynamicVectorValue(&(Ov_StaticPtrCast(kbuslib_SpecialOUT, pcClamp)->v_RbValue.value.valueunion.val_byte_vec), &(pinfo->image.outputs.data[tmp_number]), pcClamp->v_ByteWidth, BYTE);
					
					if(!Ov_StaticPtrCast(kbuslib_SpecialOUT, pcClamp)->v_ReadBackOnly)
					{
						write = TRUE;
						if(Ov_StaticPtrCast(kbuslib_SpecialOUT, pcClamp)->v_Value.value.valueunion.val_byte_vec.value)
						{
							for(i=0; i < pcClamp->v_ByteWidth; i++)
								pinfo->image.outputs.data[i + tmp_number] = Ov_StaticPtrCast(kbuslib_SpecialOUT, pcClamp)->v_Value.value.valueunion.val_byte_vec.value[i];
						}
					}
					
				}
				else
				{
					ov_logfile_error("%s: specified address lies within range of digital outs", pinst->v_identifier);
					pcClamp->v_Error = FALSE;
					pcClamp->v_ErrorCode = KBUS_ERROR_NOCLAMP;
					ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOCLAMP); 
					misc_set_led_on(3);
					misc_set_led_off(2);
					
				}
				
			}
			else
			{
				pcClamp->v_Error = TRUE;
				pcClamp->v_ErrorCode = KBUS_CLAMP_DISABLED;
				ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_STR_CLAMP_DISABLED); 
				
			}
			
			continue;		/*	continue to next object	*/
		}
		
		/*	Digital outs	*/
		
		if(Ov_CanCastTo(kbuslib_DigitalOUT, pcClampobj))	/* if object is a DigitalOUT*/
		{
			pcClamp = Ov_StaticPtrCast(kbuslib_Clamp, pcClampobj);
			
			tmp_number = Ov_StaticPtrCast(kbuslib_DigitalOUT, pcClamp)->v_ByteAddress;
			
			if(pcClamp->v_Enabled)				/*check if clamp is enabled*/
			{

				if(tmp_number >= dig_output_offset)
				{
					/*if there was an error, or the clamp was diasbled reset it*/
					if((!(pcClamp->v_Error))
						|| pcClamp->v_ErrorCode == KBUS_CLAMP_DISABLED)	
					{
						pcClamp->v_Error = TRUE;
						pcClamp->v_ErrorCode = KBUS_ERROR_NOERROR;
						ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOERROR); 
					}
					
					/*	get byte with value of interest	*/
					
					value = pinfo->image.outputs.data[tmp_number];
					
					btemp = value;
					btemp = (btemp >> (pcClamp->v_BitOffset)) & 1;
					if(btemp)
						Ov_StaticPtrCast(kbuslib_DigitalOUT, pcClamp)->v_RBValue = TRUE;
					else
						Ov_StaticPtrCast(kbuslib_DigitalOUT, pcClamp)->v_RBValue = FALSE;	
					
					if(!Ov_StaticPtrCast(kbuslib_DigitalOUT, pcClamp)->v_ReadBackOnly)
					{
					
						write = TRUE;
						/*	alter bit for digital out	*/
						if(Ov_StaticPtrCast(kbuslib_DigitalOUT, pcClamp)->v_Value)
						{
							value |= 1 << (pcClamp->v_BitOffset);
						}
						else
						{
						
							value &= ~(1 << (pcClamp->v_BitOffset));
						}
					
		
						/*	set altered byte in process image	*/
						pinfo->image.outputs.data[tmp_number] = value;
					}
					
				}
				else
				{
					ov_logfile_error("%s: specified address lies not within space of digital outs", pinst->v_identifier);
					pcClamp->v_Error = FALSE;
					pcClamp->v_ErrorCode = KBUS_ERROR_NOCLAMP;
					ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOCLAMP); 
					misc_set_led_on(3);
					misc_set_led_off(2);
					
				}
				
			}
			else
			{
				pcClamp->v_Error = TRUE;
				pcClamp->v_ErrorCode = KBUS_CLAMP_DISABLED;
				ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_STR_CLAMP_DISABLED); 
				
			}

			continue;		/*	continue to next object	*/
		}
	
	}
	
	
			
	/*	write back process images	*/
	if(write)
	{
		res = kbus_write_output_image();
		switch(res)
		{
			case 0:			/*no error*/
				ov_time_gettime(&pinst->v_LastWrite);	/*set time of successfull write*/
				break;
		
			case KBUS_ERROR_BUSY:
				pinst->v_Error = FALSE;
				pinst->v_ErrorCode = KBUS_ERROR_BUSY;
				ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_BUSY);
				ov_logfile_error("%s: kbus busy while trying to write output image", pinst->v_identifier);
				misc_set_led_on(3);
				misc_set_led_off(2);
				pinst->v_actimode = 0;
				return;
	
			case KBUS_ERROR_NODEV:
				pinst->v_Error = FALSE;
				pinst->v_ErrorCode = KBUS_ERROR_NODEV;
				ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_NODEV);
				ov_logfile_error("%s: DR-ram not initialized when trying to write output image", pinst->v_identifier);
				misc_set_led_on(3);
				misc_set_led_off(2);
				pinst->v_actimode = 0;
				return;
	
			case KBUS_ERROR_IO:
				pinst->v_Error = FALSE;
				pinst->v_ErrorCode = KBUS_ERROR_IO;
				ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_IO);
				ov_logfile_error("%s: kbus IO-Error while trying to write output image", pinst->v_identifier);
				misc_set_led_on(3);
				misc_set_led_off(2);
				pinst->v_actimode = 0;
				return;
	
			case KBUS_ERROR_TIME:
				pinst->v_Error = FALSE;
				pinst->v_ErrorCode = KBUS_ERROR_TIME;
				ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_TIME);
				ov_logfile_error("%s: communication timed out while trying to write output image", pinst->v_identifier);
				misc_set_led_on(3);
				misc_set_led_off(2);
				pinst->v_actimode = 0;
				return;
		
		}
	}
	

		
	
	
	/*****************************************************
		End Communication with clamps
	******************************************************/

	return;
}

