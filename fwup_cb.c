/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FILE:    stripchart.c                                                     */
/*                                                                           */
/* PURPOSE: This example illustrates the basic use of a Stripchart control.  */
/*          A timer control callback plots data to a Stripchart at regular   */
/*          intervals without direct user action.  The Stripchart control is */
/*          well-designed for continuous data display.                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Include files                                                             */
/*---------------------------------------------------------------------------*/
#include <utility.h>
#include <cvirte.h>
#include <userint.h>
#include <ansi_c.h>
#include "bio_demo.h"
#include "serial.h"
#include "mmd_comm.h"


#define PAGESIZE MAX_TDATA_LENGTH
#define PAGECNT 232

extern int FWUpgrade_handle;
extern int comport;

unsigned char *binBuf;
unsigned char *rspFrame;
FILE * pfile;

// unsigned char tPageCnt;

int CVICALLBACK LoadBinFileCb (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		{
			char path[MAX_PATHNAME_LEN];
			binBuf = NULL;
			pfile = NULL;
			if (FileSelectPopupEx ("", "*.bin", "", "Select File to load",
								   VAL_OK_BUTTON, 0, 0, path) != VAL_NO_FILE_SELECTED)
			{

				/* Open the file and write out the data */
				pfile = fopen (path,"rb");
				if(pfile != NULL)
				{
					binBuf = (unsigned char*)malloc(MAX_TDATA_LENGTH*PAGECNT);
					if(binBuf != NULL)
					{
						size_t rcnt;
						fseek(pfile,0,SEEK_SET);
						rcnt = fread((void*)binBuf,PAGESIZE,PAGECNT, pfile);
						if(rcnt == PAGECNT)
						{

							SetCtrlVal (panel, PANEL_FWUP_BIN_PATH, path);
							//InsertListItem (panel, PANEL_ATTACHMENTS, -1, path, path);
							SetCtrlAttribute(panel,PANEL_FWUP_FW_UPGRADE,ATTR_DIMMED,0);
						}
						else
						{
							SetCtrlAttribute(panel,PANEL_FWUP_FW_UPGRADE,ATTR_DIMMED,1);
							MessagePopup ("Error:","Read bin file error!!");
						}
					}
					else
						MessagePopup ("Read Error:","Malloc error!!");
				}
				else
					MessagePopup ("Error:","Open bin file error!!");
			}


			break;
		}
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
			DiscardPanel (FWUpgrade_handle);
			break;
		}
	}
	return 0;
}


int CVICALLBACK FWUpgradeCb (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	unsigned char blver,apver,devid,result;
	char * testbuf[64];
	switch (event)
	{
		case EVENT_COMMIT:
		{
			SetCtrlAttribute(panel,PANEL_FWUP_PROGRESSBAR,ATTR_DIMMED,0);

			SetCtrlAttribute(panel,PANEL_FWUP_PROGRESSBAR,ATTR_LABEL_TEXT,"Device id requesting...");
			DisplayPanel (panel);
			rspFrame = sendSyncCMDWithRsp(comport,DEVICE_ID_REQ,0,0,1000,&result);
			if(result)
			{
				devid = rspFrame[2];

			}
			else
				return 0;

			SetCtrlAttribute(panel,PANEL_FWUP_PROGRESSBAR,ATTR_LABEL_TEXT,"Firmware version requesting...");
			DisplayPanel (panel);
			//	sendCMD(comport,FIRMWARE_VERSION_REQ,0,0);
			rspFrame = sendSyncCMDWithRsp(comport,FIRMWARE_VERSION_REQ,0,0,1000,&result);
			if(result)
			{
				blver = rspFrame[2];
				apver = rspFrame[3];

				sprintf (testbuf, "Tips: blver=0x%x apver=0x%x !!", blver,apver);
				MessagePopup ("Upgrade :",testbuf);
			}
			else
				return 0;



			if((devid & blver & AP_MASK) == AP_MASK)  //is AP?
			{
				sendSyncCMDWithRsp(comport,RESTART_COMMAND,FWUPGRADE_RST,FWUPGRADE_PWD,1000,&result);
				if(result)
				{
					MessagePopup ("Tips:","RESTART_COMMAND RSP OK");
					Delay (1);
				}
				else
					return 0;
				
			}

			rspFrame = sendSyncCMDWithRsp(comport,FIRMWARE_UPGRADE_REQ,FWUPGRADE_PWD,0,2000,&result);
			if(result == 0)
				return 0;
			
			SetCtrlAttribute(panel,PANEL_FWUP_PROGRESSBAR,ATTR_LABEL_TEXT,"Erasing...");
			DisplayPanel (panel);

			sendSyncCMDWithRsp(comport,ERASE_MEMORY_COMMAND,FWUPGRADE_PWD,0,3000,&result);

			if(result == 0)
			{
				return 0;
			}

			SetCtrlAttribute(panel,PANEL_FWUP_PROGRESSBAR,ATTR_LABEL_TEXT,"Programming...");
			DisplayPanel (panel);

			sendSyncCMDWithRsp(comport,FIRMWARE_UPGRADE_REQ,FWUPGRADE_PWD,0,3000,&result);

			if(result)
			{
				unsigned int checksum = 0;

				unsigned char *Wrbuf = (unsigned char*)malloc(MAX_TDATA_LENGTH+MAX_TMISC_LENGTH);

				if(Wrbuf == NULL)
				{
					MessagePopup ("Upgrade Error:","Malloc error!!");
					return 0;
				}

				for(unsigned char pcnt = 0; pcnt < PAGECNT; pcnt++)
				{
					sendSyncAPUpgradeWithRsp(comport,Wrbuf,binBuf+pcnt*MAX_TDATA_LENGTH,MAX_TDATA_LENGTH,5000,&result) ;
					if(result == 0)
					{
							free(Wrbuf); 
							return 0;
					}
				}

				free(Wrbuf);
				SetCtrlAttribute(panel,PANEL_FWUP_PROGRESSBAR,ATTR_LABEL_TEXT,"Program OK");

				MessagePopup ("Firmware upgrade:","Program OK!!");
			}
			else
				return 0;


			break;
		}
	}
	return 0;
}
