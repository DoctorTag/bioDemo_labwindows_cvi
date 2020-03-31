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

/*---------------------------------------------------------------------------*/
/* Include files                                                             */
/*---------------------------------------------------------------------------*/
#include <tcpsupp.h>
#include <analysis.h>
#include <utility.h>
#include <cvirte.h>
#include <userint.h>
#include <ansi_c.h>
#include "bio_demo.h"
#include "serial.h"

#include "regmap_cb.h"
#include "fwup_cb.h"

#include "timer_cb.h"
#include "ppg_cb.h"
//#include "ppg_r_cb.h"

#include "ecg_d_cb.h"
#include "mmd_comm.h"
#include "local_analysis_cb.h"

#define     QUEUE_LENGTH  5000

extern int PPG_handle,ECG_D_handle;
extern int FWUpgrade_handle;
extern int l_analysis_handle;

int hpanel,cfg_handle;


static      CmtThreadFunctionID tcpThreadFunctionID;
static      CmtThreadFunctionID serialThreadFunctionID;
//static      CmtThreadFunctionID plotThreadFunctionID;


unsigned char  funSelectionValue = 0;

h_comm_handle_t  h_comm_handle;



/*---------------------------------------------------------------------------*/
/* This is the application's entry-point.                                    */
/*---------------------------------------------------------------------------*/
int main (int argc, char *argv[])
{
	h_comm_handle.com_ok =0 ;
	h_comm_handle.com_port = 35;
	h_comm_handle.s_receiving = 0;

	h_comm_handle.tcp_port = 3338;
	h_comm_handle.tcp_rdata_sig =0 ;
	h_comm_handle.tcp_handle = 0;
	h_comm_handle.tcp_ok = 0;
	h_comm_handle.t_receiving = 0;

	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;


	if ((hpanel = LoadPanel (0, "bio_demo.uir", PANEL)) < 0)
		return -1;

	SetCtrlIndex (hpanel, PANEL_LISTBOX_FUNCTION, 0);
	SetActiveCtrl (hpanel, PANEL_LISTBOX_FUNCTION);

	/* Display the panel and run the UI */
	DisplayPanel (hpanel);

	/* Create Thread-safe Queue */
//	if(CmtNewTSQ (QUEUE_LENGTH, sizeof(h_comm_rdata_t), OPT_TSQ_DYNAMIC_SIZE, &h_comm_handle.queueHandle) < 0)
				if(CmtNewTSQ (QUEUE_LENGTH, sizeof(new_tsq_t), OPT_TSQ_DYNAMIC_SIZE, &h_comm_handle.queueHandle) < 0)

		return -1;

	installDefaultTSQCb() ;

	RunUserInterface ();

	/* Free resources and return */
	h_comm_handle.s_receiving = 0;
//	h_comm_handle.receiving = 0;
	if(	h_comm_handle.tcp_ok )
		DisconnectFromTCPServer (h_comm_handle.tcp_handle);

	/* Destroy Thread-safe Queue */
	CmtDiscardTSQ (h_comm_handle.queueHandle);

	DiscardPanel (hpanel);
	CloseCVIRTE ();
	return 0;
}

int CVICALLBACK cfgQuitCb (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		{

			DiscardPanel (panel);
			break;
		}
	}
	return 0;
}


int CVICALLBACK cfgOkCallback (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	int ret;
	unsigned char port;
	switch (event)
	{
		case EVENT_COMMIT:
		{
			GetCtrlAttribute(panel, PANEL_CFG_UART_CHKBOX,ATTR_CTRL_VAL,&ret);
			if(ret == 1)
			{
				GetCtrlAttribute(panel, PANEL_CFG_COM,ATTR_CTRL_VAL,&port);

				ret = Init_ComPort(port);
				if( ret != 0)
				{
					char msg[120];

					h_comm_handle.com_ok = 0 ;
					h_comm_handle.com_port = 0;
					sprintf (msg, "Config COM port error: %d, please reconfig!!!",ret);
					MessagePopup ("This is a Message Popup",msg);
				}
				else
				{
					h_comm_handle.com_port = port;
					h_comm_handle.com_ok = 1 ;
					h_comm_handle.s_receiving = 1;

					CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, serial_comm_recv_Thread,(void *)&h_comm_handle, &serialThreadFunctionID);
				}
			}

			GetCtrlAttribute(panel, PANEL_CFG_TCP_CHKBOX,ATTR_CTRL_VAL,&ret);
			if(ret == 1)
			{
				GetCtrlAttribute(panel, PANEL_CFG_TCP_PORT,ATTR_CTRL_VAL,&h_comm_handle.tcp_port);
				GetCtrlAttribute(panel, PANEL_CFG_IP,ATTR_CTRL_VAL,h_comm_handle.serverip);
				//	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, tcp_comm_recv_Thread,(void *)&h_comm_handle, &tcpThreadFunctionID);
				h_comm_connectTcpServer(&h_comm_handle) ;
			}

			DiscardPanel (panel);
			break;
		}
	}
	return 0;
}


/*---------------------------------------------------------------------------*/
/* This function is called whenever any item in the Configuration or Test    */
/* menus is selected.  Notice that we are passed the menubar handle and item */
/* ID of the selected item.                                                  */
/*---------------------------------------------------------------------------*/

void CVICALLBACK MenuConfigPortCb (int menubar, int menuItem,void *callbackData, int panel)
{


	switch (menuItem)
	{


		case MENUBAR_MENU_COM :

			cfg_handle = LoadPanel (hpanel, "bio_demo.uir", PANEL_CFG);
			if(h_comm_handle.com_port > 0)
			{
				SetCtrlAttribute(cfg_handle,PANEL_CFG_COM,ATTR_DIMMED,0);
				SetCtrlAttribute(cfg_handle,PANEL_CFG_COM,ATTR_CTRL_VAL,h_comm_handle.com_port);
				SetCtrlAttribute(cfg_handle,PANEL_CFG_UART_CHKBOX,ATTR_CTRL_VAL,1);
			}

			if(h_comm_handle.tcp_port > 0)
			{
				SetCtrlAttribute(cfg_handle,PANEL_CFG_TCP_PORT,ATTR_DIMMED,0);
				SetCtrlAttribute(cfg_handle,PANEL_CFG_TCP_PORT,ATTR_CTRL_VAL,h_comm_handle.tcp_port);
				SetCtrlAttribute(cfg_handle,PANEL_CFG_IP,ATTR_DIMMED,0);
				//	SetCtrlAttribute(cfg_handle,PANEL_CFG_IP,ATTR_CTRL_VAL,h_comm_handle.serverip);

				SetCtrlAttribute(cfg_handle,PANEL_CFG_TCP_CHKBOX,ATTR_CTRL_VAL,1);


			}
			InstallPopup (cfg_handle);
			break;

	}
}


void CVICALLBACK MenuFirmUpgradeCb (int menubar, int menuItem,
									void *callbackData, int panel)
{
	unsigned char forSensor;
	switch (menuItem)
	{
		case MENUBAR_MENU_FM_UPGRADE :

			if(fwfile_malloc())
			{
				FWUpgrade_handle = LoadPanel (hpanel, "bio_demo.uir", PANEL_FWUP);
				GetCtrlAttribute(FWUpgrade_handle, PANEL_FWUP_FW_SW,ATTR_CTRL_VAL,&forSensor);
				uninstallDefaultTSQCb() ;
				initFwupCb(forSensor);

				InstallPopup (FWUpgrade_handle);
			}
			else
				MessagePopup ("Error !!!","malloc fail!!!!");
			break;

	}
}

void CVICALLBACK MenuLocalAnalysisCb (int menubar, int menuItem,
									  void *callbackData, int panel)
{
	switch (menuItem)
	{
		case MENUBAR_MENU_LOCAL_ANALYSIS :

			l_analysis_handle = LoadPanel (hpanel, "bio_demo.uir", PANEL_LA);

			uninstallDefaultTSQCb() ;
			InstallPopup (l_analysis_handle);
			break;

	}
}

int CVICALLBACK FunctionTestBeginCb(int panel, int control, int event,
									void *callbackData, int eventData1, int eventData2)
{

	switch (event)
	{
		case EVENT_COMMIT:
		{
			if(GetCtrlVal(panel, PANEL_LISTBOX_FUNCTION, &funSelectionValue) == 0)
			{
				switch(funSelectionValue)
				{
					case 0:
					case 1:
					case 2:
					case 9:
					case 5:
						PPG_handle = LoadPanel (hpanel, "bio_demo.uir", PANEL_PPG);
						uninstallDefaultTSQCb() ;
						InstallPopup (PPG_handle);

						break;

					case 3:
					case 4:
						ECG_D_handle = LoadPanel (hpanel, "bio_demo.uir", PANEL_ECGD);
						uninstallDefaultTSQCb() ;
						InstallPopup (ECG_D_handle);
						break;

				}


			}

		}
		break;
	}
	return 0;
}


/*---------------------------------------------------------------------------*/
/* Quit the UI loop.                                                         */
/*---------------------------------------------------------------------------*/
int CVICALLBACK Shutdown (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_COMMIT)
	{
//	h_comm_handle.receiving = 0;
		h_comm_handle.s_receiving = 0;
	//	uninstallDefaultTSQCb() ;
		QuitUserInterface (0);
	}
	return 0;
}



int CVICALLBACK PanelCB (int panel, int event, void *callbackData,
						 int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_CLOSE:
//	tcp_comm_handle.receiving = 0;
			h_comm_handle.s_receiving = 0;
		//	uninstallDefaultTSQCb() ;
			QuitUserInterface (0);
			break;


	}
	return 0;
}

