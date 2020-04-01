/*---------------------------------------------------------------------------

BSD 3-Clause License

Copyright (c) 2018, feelkit
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


---------------------------------------------------------------------------*/
/* Include files                                                             */
/*---------------------------------------------------------------------------*/
#include <utility.h>
#include <cvirte.h>
#include <userint.h>
#include <ansi_c.h>
#include <progressbar.h>
#include <toolbox.h>

#include "bio_common.h"

#include "bio_demo.h"
#include "serial.h"
#include "mmd_comm.h"
#include "fwup_cb.h"

#define PAGESIZE MAX_TDATA_LENGTH
#define PAGECNT 232

#define OTA_PAGECNT 8


extern h_comm_handle_t h_comm_handle;

unsigned char Wrbuf[MAX_TDATA_LENGTH+MAX_TMISC_LENGTH];
int FWUpgrade_handle;
unsigned char *binBuf = NULL;
char bin_path[MAX_PATHNAME_LEN];
unsigned char fw_type,check_cnt;
unsigned int program_cnt,cur_prog;
FILE * pfile;
unsigned int filelen;


static CmtTSQCallbackID fwupPlotcallbackID = 0;


int CVICALLBACK fwupTimerCallback (int panel, int control, int event,
								   void *callbackData, int eventData1,
								   int eventData2)
{
	switch (event)
	{
		case EVENT_TIMER_TICK:
			MessagePopup ("Error:","rsp timeout !!!!");
			break;
	}
	return 0;
}




int CVICALLBACK FW_SWCb (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		{
			GetCtrlAttribute(panel, PANEL_FWUP_FW_SW,ATTR_CTRL_VAL,&fw_type);
			if(fw_type)
			{
				CmtUninstallTSQCallback (h_comm_handle.queueHandle, fwupPlotcallbackID);

				CmtInstallTSQCallback (h_comm_handle.queueHandle, EVENT_TSQ_ITEMS_IN_QUEUE, 1, sensorFWUPFromQueueCallback, NULL,CmtGetCurrentThreadID(), &fwupPlotcallbackID);

				MessagePopup ("Tips:","Firmware Upgraded for Sensor Module?");
			}
			else
			{
				CmtUninstallTSQCallback (h_comm_handle.queueHandle, fwupPlotcallbackID);

				CmtInstallTSQCallback (h_comm_handle.queueHandle, EVENT_TSQ_ITEMS_IN_QUEUE, 1, otaFWUPFromQueueCallback, NULL,CmtGetCurrentThreadID(), &fwupPlotcallbackID);

				MessagePopup ("Tips:","Ota for wireless MCU?");
			}
			memset( bin_path,0, MAX_PATHNAME_LEN);
			SetCtrlVal (panel, PANEL_FWUP_BIN_PATH, "<Path displayed here>");
			SetCtrlAttribute(panel,PANEL_FWUP_FW_UPGRADE,ATTR_DIMMED,1);

		}
		break;
	}

	return 0;
}

int CVICALLBACK LoadBinFileCb (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		{
			if(pfile != NULL)
			{
				fclose(pfile);
				pfile = NULL;
			}
			GetCtrlAttribute(panel, PANEL_FWUP_FW_SW,ATTR_CTRL_VAL,&fw_type);

			if (FileSelectPopupEx ("", "*.bin", "", "Select File to load",VAL_OK_BUTTON, 0, 0, bin_path) != VAL_NO_FILE_SELECTED)
			{
				if(fw_type)
				{
					if(strstr(bin_path,"sensor") == NULL)
					{
						MessagePopup ("Error:","choice bin file error!!");
						return 0;
					}
					InstallCtrlCallback(panel,PANEL_FWUP_FW_UPGRADE,sensorFWUpgradeCb,NULL);


				}
				else
				{
					if(strstr(bin_path,"main_ota") == NULL)
					{
						MessagePopup ("Error:","choice bin file error!!");
						return 0;
					}
					InstallCtrlCallback(panel,PANEL_FWUP_FW_UPGRADE,mainOtaFWUpgradeCb,NULL);

				}

				SetCtrlVal (panel, PANEL_FWUP_BIN_PATH, bin_path);
				SetCtrlAttribute(panel,PANEL_FWUP_FW_UPGRADE,ATTR_DIMMED,0);


			}
		}
		break;
	}
	return 0;
}


int CVICALLBACK FWUP_QuitCb (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		{
			if(binBuf != NULL)
			{
				free(binBuf);
				binBuf = NULL;
			}

			if(pfile != NULL)
			{
				fclose(pfile);
				pfile = NULL;
			}
			CmtUninstallTSQCallback (h_comm_handle.queueHandle, fwupPlotcallbackID);

			DiscardPanel (FWUpgrade_handle);
			break;
		}
	}
	return 0;
}

static void FWUpgradeReturnInd(int panel,char *tip,char *msg )
{


	SetCtrlAttribute(panel,PANEL_FWUP_PROGRESSBAR,ATTR_DIMMED,1);
	SetCtrlAttribute(panel,PANEL_FWUP_FWUP_QUIT,ATTR_DIMMED,0);
	SetCtrlAttribute(panel,PANEL_FWUP_FW_UPGRADE,ATTR_DIMMED,0);
	SetCtrlAttribute(panel,PANEL_FWUP_LOAD_FILE,ATTR_DIMMED,0);
	ProgressBar_Revert (panel, PANEL_FWUP_PROGRESSBAR);

	if((tip != NULL)&&(msg != NULL))
		MessagePopup (tip,msg);

}


void CVICALLBACK sensorFWUPFromQueueCallback (CmtTSQHandle queueHandle, unsigned int event,int value, void *callbackData)
{
	new_tsq_t rtsq[1];
	h_comm_rdata_t *rdata;
	char message[64];

	switch (event)
	{
		case EVENT_TSQ_ITEMS_IN_QUEUE:

			CmtReadTSQData (queueHandle, rtsq, 1, TSQ_INFINITE_TIMEOUT, 0);
			rdata = rtsq[0].p_tsq;
			switch(rdata->dframe[1])
			{
				case STATUS_INFO_REQ:
				{
					//	SetCtrlAttribute (FWUpgrade_handle, PANEL_FWUP_TIMER, ATTR_ENABLED, 0);


					switch(rdata->dframe[3])
					{

						case  BIO_NORMAL:   //is AP?
						{
							if(check_cnt == 1)
							{
								FWUpgradeReturnInd(FWUpgrade_handle,"BioSensor :","Fail to  restart for bootloader !!!");
								//goto sensor_up_done;

							}
							else
							{
								check_cnt++;
								h_comm_sendCMD(&h_comm_handle,RESTART_COMMAND,FWUPGRADE_RST,0,0);

								//	SetCtrlAttribute (FWUpgrade_handle, PANEL_FWUP_TIMER, ATTR_ENABLED, 1);

							}

						}
						break;

						case   BIO_BOOTL:   //sensor is bootloader status !
						{
							h_comm_sendCMD(&h_comm_handle,FIRMWARE_UPGRADING_COMMAND,BL_CMD_UPGRADE_REQ,FWUP_PWD,0);

							//	SetCtrlAttribute (FWUpgrade_handle, PANEL_FWUP_TIMER, ATTR_ENABLED, 1);

						}
						break;

						case BIO_LOSE :
						{
							if(check_cnt == 1)
							{
								FWUpgradeReturnInd(FWUpgrade_handle,"BioSensor :","Sensor module lost !!!");
								//goto sensor_up_done;

							}
							else
							{
								check_cnt++;
								h_comm_sendCMD(&h_comm_handle,RESTART_COMMAND,FWUPGRADE_RST,0,0);

								//	SetCtrlAttribute (FWUpgrade_handle, PANEL_FWUP_TIMER, ATTR_ENABLED, 1);

							}

						}
						break;

						default:

							FWUpgradeReturnInd(FWUpgrade_handle,"BioSensor :","Sensor module unknow !!!");
							//	goto sensor_up_done;

							break;


					}

				}
				break;

				case RESTART_COMMAND:
				{
					//	SetCtrlAttribute (FWUpgrade_handle, PANEL_FWUP_TIMER, ATTR_ENABLED, 0);


					switch(rdata->dframe[3])
					{

						case  FWUPGRADE_RST:
						{

							Delay(3);
							h_comm_sendCMD(&h_comm_handle,STATUS_INFO_REQ,0,0,0);

						}
						break;

						case    NORMAL_RST:
						{

						}
						break;




					}

				}
				break;

				case FIRMWARE_UPGRADING_COMMAND:
				{
					//	SetCtrlAttribute (FWUpgrade_handle, PANEL_FWUP_TIMER, ATTR_ENABLED, 0);

					switch(rdata->dframe[3])
					{

						case  BL_CMD_UPGRADE_REQ :
						{
							SetCtrlAttribute(FWUpgrade_handle,PANEL_FWUP_PROGRESSBAR,ATTR_LABEL_TEXT,"Erasing...");
							DisplayPanel (FWUpgrade_handle);
							h_comm_sendCMD(&h_comm_handle,FIRMWARE_UPGRADING_COMMAND,BL_CMD_ERASEAPP,FWUP_PWD,0);
							Delay(1);
							//	SetCtrlAttribute (FWUpgrade_handle, PANEL_FWUP_TIMER, ATTR_ENABLED, 1);
						}
						break;

						case BL_CMD_ERASEAPP:
						{
							SetCtrlAttribute(FWUpgrade_handle,PANEL_FWUP_PROGRESSBAR,ATTR_LABEL_TEXT,"Programming...");
							DisplayPanel (FWUpgrade_handle);
							program_cnt = 1;
							if(rdata->dframe[4] == RSP_OK)
								h_comm_sendFWUpgrade(&h_comm_handle,BL_CMD_PROGRAM,Wrbuf,0,binBuf,MAX_TDATA_LENGTH) ;
							else
							{
								sprintf (message, "ERASE Error Upgrade rsp: 0x%x", rdata->dframe[4]);
								FWUpgradeReturnInd(FWUpgrade_handle,"Sensor Error:",message);

							}
							//	SetCtrlAttribute (FWUpgrade_handle, PANEL_FWUP_TIMER, ATTR_ENABLED, 1);

						}
						break;

						//	default:
						case BL_CMD_PROGRAM:
						{

							if(	program_cnt < PAGECNT)
							{
								//	h_comm_sendFWUpgrade(&h_comm_handle,BL_CMD_PROGRAM,Wrbuf,0,binBuf,MAX_TDATA_LENGTH) ;
								h_comm_sendFWUpgrade(&h_comm_handle,BL_CMD_PROGRAM,Wrbuf,program_cnt*(MAX_TDATA_LENGTH / 2),binBuf+program_cnt*MAX_TDATA_LENGTH,MAX_TDATA_LENGTH) ;
								//	Delay(0.5);
								ProgressBar_SetPercentage (FWUpgrade_handle, PANEL_FWUP_PROGRESSBAR, (100*program_cnt)/PAGECNT, NULL);
								program_cnt++;

							}
							else
								h_comm_sendCMD(&h_comm_handle,FIRMWARE_UPGRADING_COMMAND,BL_CMD_APRDY,FWUP_PWD,0);
							//	SetCtrlAttribute (FWUpgrade_handle, PANEL_FWUP_TIMER, ATTR_ENABLED, 1);
						}
						break;

						case BL_CMD_APRDY:
						{

							//	h_comm_sendCMD(&h_comm_handle,RESTART_COMMAND,NORMAL_RST,0);
							SetCtrlAttribute(FWUpgrade_handle,PANEL_FWUP_PROGRESSBAR,ATTR_LABEL_TEXT,"Program OK");
							FWUpgradeReturnInd(FWUpgrade_handle,"Firmware upgrade:","Program OK!!");

						}
						break;

						default:
						{

							sprintf (message, "Error Upgrade rsp: 0x%x , 0x%x", rdata->dframe[3],rdata->dframe[4]);
							FWUpgradeReturnInd(FWUpgrade_handle,"Sensor Error:",message);
							//	goto sensor_up_done;
						}
						break;


					}

				}
				break;
			}

			free(rdata);

			break;
	}
}


int CVICALLBACK sensorFWUpgradeCb (int panel, int control, int event,
								   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		{
			/* Open the file and write out the data */
			pfile = fopen (bin_path,"rb");
			if(pfile != NULL)
			{

				size_t rcnt;
				fseek(pfile,0,SEEK_SET);
				rcnt = fread((void*)binBuf,PAGESIZE,PAGECNT, pfile);
				if(rcnt != PAGECNT)
				{
					SetCtrlAttribute(panel,PANEL_FWUP_FW_UPGRADE,ATTR_DIMMED,1);
					MessagePopup ("Error:","Read bin file error!!");
					if(pfile)
					{
						fclose(pfile);
						pfile = NULL;
					};
					return 0 ;
				}
			}
			else
			{
				MessagePopup ("Error:","Open bin file error!!");
				return 0 ;
			}


			ProgressBar_ConvertFromSlide (panel,PANEL_FWUP_PROGRESSBAR);
			ProgressBar_SetAttribute (panel,PANEL_FWUP_PROGRESSBAR, ATTR_PROGRESSBAR_UPDATE_MODE, VAL_PROGRESSBAR_MANUAL_MODE);

			SetCtrlAttribute(panel,PANEL_FWUP_PROGRESSBAR,ATTR_DIMMED,0);
			SetCtrlAttribute(panel,PANEL_FWUP_FWUP_QUIT,ATTR_DIMMED,1);
			SetCtrlAttribute(panel,PANEL_FWUP_FW_UPGRADE,ATTR_DIMMED,1);
			SetCtrlAttribute(panel,PANEL_FWUP_LOAD_FILE,ATTR_DIMMED,1);

			SetCtrlAttribute(panel,PANEL_FWUP_PROGRESSBAR,ATTR_LABEL_TEXT,"Device status requesting...");
			DisplayPanel (panel);

			CmtFlushTSQ(h_comm_handle.queueHandle,TSQ_FLUSH_ALL ,NULL);
			/* Install a callback to read and plot the generated data. */
			//		CmtInstallTSQCallback (h_comm_handle.queueHandle, EVENT_TSQ_ITEMS_IN_QUEUE, 1, sensorFWUPFromQueueCallback, NULL,CmtGetCurrentThreadID(), &fwupPlotcallbackID);

			h_comm_sendCMD(&h_comm_handle,STATUS_INFO_REQ,0,0,0);
			check_cnt = 0;

			//	SetCtrlAttribute (FWUpgrade_handle, PANEL_FWUP_TIMER, ATTR_ENABLED, 1);





			break;
		}
	}


	return 0;
}


void CVICALLBACK otaFWUPFromQueueCallback (CmtTSQHandle queueHandle, unsigned int event,int value, void *callbackData)
{
	h_comm_rdata_t *rdata;

	new_tsq_t rtsq[1];
	switch (event)
	{
		case EVENT_TSQ_ITEMS_IN_QUEUE:

			CmtReadTSQData (queueHandle, rtsq, 1, TSQ_INFINITE_TIMEOUT, 0);
			rdata = rtsq[0].p_tsq;
			switch(rdata->dframe[1])
			{


				case FIRMWARE_UPGRADING_COMMAND:
				{
					//SetCtrlAttribute (FWUpgrade_handle, PANEL_FWUP_TIMER, ATTR_ENABLED, 0);

					switch(rdata->dframe[3])
					{

						case  BL_CMD_OTA_REQ :
						{
							SetCtrlAttribute(FWUpgrade_handle,PANEL_FWUP_PROGRESSBAR,ATTR_LABEL_TEXT,"Programming...");
							DisplayPanel (FWUpgrade_handle);
							h_comm_sendFWUpgrade(&h_comm_handle,BL_CMD_OTA_DSEG0,Wrbuf,0,binBuf,MAX_TDATA_LENGTH) ;
							cur_prog = 0;
							//SetCtrlAttribute (FWUpgrade_handle, PANEL_FWUP_TIMER, ATTR_ENABLED, 1);
						}
						break;

						case BL_CMD_OTA_DSEG0:
						case BL_CMD_OTA_DSEG1:
						case BL_CMD_OTA_DSEG2:
						case BL_CMD_OTA_DSEG3:
						case BL_CMD_OTA_DSEG4:
						case BL_CMD_OTA_DSEG5:
						case BL_CMD_OTA_DSEG6:
							//	case BL_CMD_OTA_DSEG7:
						{
							if(cur_prog < (program_cnt-1))
							{
								h_comm_sendFWUpgrade(&h_comm_handle,rdata->dframe[3]+1,Wrbuf,0,binBuf+(rdata->dframe[3] - BL_CMD_OTA_DSEG0 + 1)*MAX_TDATA_LENGTH,MAX_TDATA_LENGTH) ;

								//	SetCtrlAttribute (FWUpgrade_handle, PANEL_FWUP_TIMER, ATTR_ENABLED, 1);

							}
							else if(cur_prog == (program_cnt-1))
							{


								if((rdata->dframe[3]- BL_CMD_OTA_DSEG0)== (check_cnt -1 ))
								{


									SetCtrlAttribute(FWUpgrade_handle,PANEL_FWUP_PROGRESSBAR,ATTR_LABEL_TEXT,"Program OK");
									FWUpgradeReturnInd(FWUpgrade_handle,"Firmware upgrade:","Program OK!!");

									//complete
								}
								else if((rdata->dframe[3]- BL_CMD_OTA_DSEG0)== (check_cnt - 2))
								{
									if(filelen%MAX_TDATA_LENGTH)
										h_comm_sendFWUpgrade(&h_comm_handle,rdata->dframe[3]+1,Wrbuf,0x100,binBuf+(rdata->dframe[3] - BL_CMD_OTA_DSEG0 + 1)*MAX_TDATA_LENGTH,filelen%MAX_TDATA_LENGTH) ;
									else
										h_comm_sendFWUpgrade(&h_comm_handle,rdata->dframe[3]+1,Wrbuf,0x100,binBuf+(rdata->dframe[3] - BL_CMD_OTA_DSEG0 + 1)*MAX_TDATA_LENGTH,MAX_TDATA_LENGTH) ;
									//SetCtrlAttribute (FWUpgrade_handle, PANEL_FWUP_TIMER, ATTR_ENABLED, 1);

								}
								else if((rdata->dframe[3]- BL_CMD_OTA_DSEG0) < (check_cnt - 2))
								{
									h_comm_sendFWUpgrade(&h_comm_handle,rdata->dframe[3]+1,Wrbuf,0,binBuf+(rdata->dframe[3] - BL_CMD_OTA_DSEG0 + 1)*MAX_TDATA_LENGTH,MAX_TDATA_LENGTH) ;
									//	SetCtrlAttribute (FWUpgrade_handle, PANEL_FWUP_TIMER, ATTR_ENABLED, 1);

								}


							}
							else
								FWUpgradeReturnInd(FWUpgrade_handle,"Error :","Error Upgrade unknow 0 !!!");
						}
						break;

						case BL_CMD_OTA_DSEG7:
						{
							cur_prog++;
							if(cur_prog < (program_cnt-1))
							{
								fread((void*)binBuf,MAX_TDATA_LENGTH,OTA_PAGECNT, pfile);
								h_comm_sendFWUpgrade(&h_comm_handle,BL_CMD_OTA_DSEG0,Wrbuf,0,binBuf,MAX_TDATA_LENGTH) ;
								ProgressBar_SetPercentage (FWUpgrade_handle, PANEL_FWUP_PROGRESSBAR, (100*cur_prog)/program_cnt, NULL);

								//	SetCtrlAttribute (FWUpgrade_handle, PANEL_FWUP_TIMER, ATTR_ENABLED, 1);

							}
							else if(cur_prog == (program_cnt-1))
							{
								unsigned int tmplen = ftell(pfile) ;
								check_cnt = (filelen-tmplen)/MAX_TDATA_LENGTH;
								if((filelen-tmplen)%MAX_TDATA_LENGTH)
									check_cnt++;

								fread((void*)binBuf,(filelen-tmplen),1, pfile);
								if(check_cnt > 1)
									h_comm_sendFWUpgrade(&h_comm_handle,BL_CMD_OTA_DSEG0,Wrbuf,0,binBuf,MAX_TDATA_LENGTH) ;
								else
								{
									if(filelen%MAX_TDATA_LENGTH)
										h_comm_sendFWUpgrade(&h_comm_handle,BL_CMD_OTA_DSEG0,Wrbuf,0x100,binBuf,filelen%MAX_TDATA_LENGTH) ;
									else
										h_comm_sendFWUpgrade(&h_comm_handle,BL_CMD_OTA_DSEG0,Wrbuf,0x100,binBuf,MAX_TDATA_LENGTH) ;
								}

								//	SetCtrlAttribute (FWUpgrade_handle, PANEL_FWUP_TIMER, ATTR_ENABLED, 1);

							}
							else
							{
								if(check_cnt == OTA_PAGECNT)
								{
									//complete

									SetCtrlAttribute(FWUpgrade_handle,PANEL_FWUP_PROGRESSBAR,ATTR_LABEL_TEXT,"Program OK");
									FWUpgradeReturnInd(FWUpgrade_handle,"Firmware upgrade:","Program OK!!");

								}
								else

									FWUpgradeReturnInd(FWUpgrade_handle,"Error :","Error Upgrade unknow !!!");
							}

						}
						break;


						default:

							FWUpgradeReturnInd(FWUpgrade_handle,"Error :","Error Upgrade rsp from sensor !!!");
							//	goto sensor_up_done;

							break;

					}

				}
				break;
			}


			free(rdata);
			break;
	}
}

int CVICALLBACK mainOtaFWUpgradeCb (int panel, int control, int event,
									void *callbackData, int eventData1, int eventData2)
{


	switch (event)
	{
		case EVENT_COMMIT:
		{
			/* Open the file and write out the data */
			pfile = fopen (bin_path,"rb");
			if(pfile != NULL)
			{
				fseek(pfile,0,SEEK_END);
				filelen = ftell(pfile) ;
				program_cnt = filelen/(MAX_TDATA_LENGTH*OTA_PAGECNT);
				if(filelen%(MAX_TDATA_LENGTH*OTA_PAGECNT))
					program_cnt ++;
				fseek(pfile,0,SEEK_SET);

				if(fread((void*)binBuf,MAX_TDATA_LENGTH,OTA_PAGECNT, pfile) != OTA_PAGECNT)
				{
					SetCtrlAttribute(panel,PANEL_FWUP_FW_UPGRADE,ATTR_DIMMED,1);
					MessagePopup ("Error:","Read bin file error!!");
					if(pfile)
					{
						fclose(pfile);
						pfile = NULL;
					};
					return 0 ;
				}

			}
			else
			{
				MessagePopup ("Error:","Open bin file error!!");
				return 0;
			}

			ProgressBar_ConvertFromSlide (panel,PANEL_FWUP_PROGRESSBAR);
			ProgressBar_SetAttribute (panel,PANEL_FWUP_PROGRESSBAR, ATTR_PROGRESSBAR_UPDATE_MODE, VAL_PROGRESSBAR_MANUAL_MODE);

			SetCtrlAttribute(panel,PANEL_FWUP_PROGRESSBAR,ATTR_DIMMED,0);
			SetCtrlAttribute(panel,PANEL_FWUP_FWUP_QUIT,ATTR_DIMMED,1);
			SetCtrlAttribute(panel,PANEL_FWUP_FW_UPGRADE,ATTR_DIMMED,1);
			SetCtrlAttribute(panel,PANEL_FWUP_LOAD_FILE,ATTR_DIMMED,1);

			SetCtrlAttribute(panel,PANEL_FWUP_PROGRESSBAR,ATTR_LABEL_TEXT,"OTA requesting...");
			DisplayPanel (panel);
			CmtFlushTSQ(h_comm_handle.queueHandle,TSQ_FLUSH_ALL ,NULL);


			h_comm_sendCMD(&h_comm_handle,FIRMWARE_UPGRADING_COMMAND,BL_CMD_OTA_REQ,0,0);

			//	SetCtrlAttribute (FWUpgrade_handle, PANEL_FWUP_TIMER, ATTR_ENABLED, 1);



		}
		break;
	}



	return 0;
}


void initFwupCb(unsigned char forSensor)
{

	if(fwupPlotcallbackID)
		CmtUninstallTSQCallback (h_comm_handle.queueHandle, fwupPlotcallbackID);
	if(forSensor)

		CmtInstallTSQCallback (h_comm_handle.queueHandle, EVENT_TSQ_ITEMS_IN_QUEUE, 1, sensorFWUPFromQueueCallback, NULL,CmtGetCurrentThreadID(), &fwupPlotcallbackID);

	else

		CmtInstallTSQCallback (h_comm_handle.queueHandle, EVENT_TSQ_ITEMS_IN_QUEUE, 1, otaFWUPFromQueueCallback, NULL,CmtGetCurrentThreadID(), &fwupPlotcallbackID);



}

void * fwfile_malloc(void)

{

	binBuf = (unsigned char *)malloc(MAX_TDATA_LENGTH*PAGECNT);
	return (void*)binBuf;

}
