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
#include <formatio.h>
#include "asynctmr.h"
#include <analysis.h>
#include <utility.h>
#include <cvirte.h>
#include <userint.h>
#include <ansi_c.h>
#include <stdbool.h>
//#include <progressbar.h>
#include <toolbox.h>
#include "bio_demo.h"
#include "serial.h"

#include "ppg_cb.h"
#include "timer_cb.h"
#include "mmd_comm.h"

#include "analysis_piezo.h"
#include "local_analysis_cb.h"






extern	  h_comm_handle_t h_comm_handle;

FILE * pLAfile;
int l_analysis_handle;
char LAfile_path[MAX_PATHNAME_LEN];

volatile    int     laStopFlag             =   1;
static CmtTSQCallbackID laPlotcallbackID = 0;
static      CmtThreadFunctionID laThreadFunctionID = NULL;

void LAStop(void);


void CVICALLBACK laPlotDataFromQueueCallback (CmtTSQHandle queueHandle, unsigned int event,
		int value, void *callbackData)
{
	new_tsq_t rtsq[1];
//	local_analysis_t ana_plot[1];
	analysis_result_t *ana_plot;
	float wave_data;
	double result;
	float filter_data;
	short i;
	switch (event)
	{
		case EVENT_TSQ_ITEMS_IN_QUEUE:

			CmtReadTSQData (queueHandle, rtsq, 1, TSQ_INFINITE_TIMEOUT, 0);



			i=0;
			ana_plot = rtsq[0].p_tsq;
	//		SetCtrlVal (l_analysis_handle, PANEL_LA_NUMERIC, ana_plot->count);
			while(i < SAMPLE_HZ)
			{

				wave_data = ana_plot->o_data[i];
				//result = ana_plot->analysis_result[i]	 ;
				result = ana_plot->variance	 ;
				//filter_data = ana_plot->hr_filted_data[i];
				filter_data = ana_plot->hr_enhanced_data[i];
				PlotStripChart (l_analysis_handle,PANEL_LA_CHART_DATA,&wave_data , 1, 0, 0, VAL_FLOAT);
				PlotStripChart (l_analysis_handle,PANEL_LA_CHART_RESULT,&filter_data , 1, 0, 0, VAL_FLOAT);
			//	PlotStripChart (l_analysis_handle,PANEL_LA_CHART_RESULT_2,&result , 1, 0, 0, VAL_DOUBLE);

				i++;
			}
			
							PlotStripChart (l_analysis_handle,PANEL_LA_CHART_RESULT_2,&result , 1, 0, 0, VAL_DOUBLE);

			free(ana_plot);
			break;
	}
}


int CVICALLBACK LA_QuitCb (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		{
			LAStop() ;
			if(pLAfile != NULL)
			{
				fclose(pLAfile);
				pLAfile = NULL;
			}

			if(laPlotcallbackID !=0)
			{
				CmtUninstallTSQCallback (h_comm_handle.queueHandle, laPlotcallbackID);
				laPlotcallbackID =0  ;
			}
			DiscardPanel (l_analysis_handle);
			break;
		}
	}
	return 0;
}


int CVICALLBACK LALoadFileCb (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		{
			if(pLAfile != NULL)
			{
				fclose(pLAfile);
				pLAfile = NULL;
			}

			if (FileSelectPopupEx ("", "*.csv", "", "Select File to load",VAL_OK_BUTTON, 0, 0, LAfile_path) != VAL_NO_FILE_SELECTED)
			{

				/* Open the file and write out the data */
				pLAfile = fopen (LAfile_path,"rb");
				if(pLAfile != NULL)
				{
					fseek(pLAfile,0,SEEK_SET);
					CmtFlushTSQ(h_comm_handle.queueHandle,TSQ_FLUSH_ALL ,NULL);
					/* Install a callback to read and plot the generated data. */
					if(laPlotcallbackID ==0)
						CmtInstallTSQCallback (h_comm_handle.queueHandle, EVENT_TSQ_ITEMS_IN_QUEUE, 1,laPlotDataFromQueueCallback, NULL,CmtGetCurrentThreadID(), &laPlotcallbackID);


				}
				else
				{
					MessagePopup ("Error:","Open bin file error!!");
					return 0 ;
				}


			}
		}
		break;
	}
	return 0;
}


/*------------------------------------------------------------------*/
/*  Function called to stop the Data Acquisition & cleanup          */
/*------------------------------------------------------------------*/
void LAStop(void)
{
	int laThreadFunctionStatus;

	if (!laStopFlag)
	{
		laStopFlag = 1;

		CmtGetThreadPoolFunctionAttribute (DEFAULT_THREAD_POOL_HANDLE,
										   laThreadFunctionID,
										   ATTR_TP_FUNCTION_EXECUTION_STATUS,
										   &laThreadFunctionStatus);
		if (laThreadFunctionStatus != kCmtThreadFunctionComplete)
			CmtWaitForThreadPoolFunctionCompletion (DEFAULT_THREAD_POOL_HANDLE,
													laThreadFunctionID, 1);
		CmtReleaseThreadPoolFunctionID (DEFAULT_THREAD_POOL_HANDLE,
										laThreadFunctionID);

	}

}
/*------------------------------------------------------------------*/
/*  Callback to stop/interrupt the data acquisition                 */
/*------------------------------------------------------------------*/
int CVICALLBACK LAStopCb (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			LAStop();
			/* Flush the thread-safe queue */
			CmtFlushTSQ (h_comm_handle.queueHandle, TSQ_FLUSH_ALL, NULL);
			//       SetCtrlVal (pnl, PNL_PLOTNUM, 0);
			//        UIRDimming(1);
			break;
	}
	return 0;
}

/*------------------------------------------------------------------*/
/*  PostDeferred Callback that displays the data acqusition status  */
/*------------------------------------------------------------------*/
void CVICALLBACK MessageCB (void *mesg)
{
	if (mesg != NULL)
	{
		SetCtrlVal (l_analysis_handle, PANEL_LA_LA_STRING, (char *)mesg);
		if (FindPattern ((char *)mesg, 0, strlen((char *)mesg), "Error", 0, 0) != -1)
		{
			/* Error occurred */
			SetCtrlVal (l_analysis_handle, PANEL_LA_LA_LED, 1);
			LAStopCb (l_analysis_handle, 0, EVENT_COMMIT, 0, 0, 0);
		}
		else if (FindPattern ((char *)mesg, 0, strlen((char *)mesg), "done", 0, 0) != -1)
			LAStopCb (l_analysis_handle, 0, EVENT_COMMIT, 0, 0, 0);
	}
}



float debug_buf[SAMPLE_HZ] ;

//h_comm_rdata_t la_data;
/*------------------------------------------------------------------*/
/*  Calls the Local Analysis functions.  */
/*------------------------------------------------------------------*/
int CVICALLBACK laThreadFunction (void *functionData)
{
	static float max_value = 0;
	static float min_value = 0xffffff;

	int     		mainThreadID,tdata,fret,median,cnt=0;
	char    		Mesg[600];
	short     		iStatus                     =   0;
	short     		iLoopCount                  =   0;
	float     	abs_value,lp_max,mapi,lp_data;
	analysis_result_t *pdBuffer               =   NULL;
	bool   new_rd;
	mainThreadID = CmtGetMainThreadID ();

	if ((pdBuffer = (analysis_result_t*)malloc(sizeof(analysis_result_t)))==NULL)
		return -1;

	while (!laStopFlag)
	{

		fret = fscanf(pLAfile,"%d\r\n",&tdata);
		if(fret > 0)
		{

			new_rd = analysis_piezo_all(tdata,pdBuffer) ;

			if(new_rd == true)
			{
				new_tsq_t ntsq;

				ntsq.p_tsq = (void*)pdBuffer;
				CmtWriteTSQData (h_comm_handle.queueHandle, &ntsq,1,TSQ_INFINITE_TIMEOUT, NULL);

				if ((pdBuffer = (analysis_result_t*)malloc(sizeof(analysis_result_t)))==NULL)

					break;

				Delay(0.5);
			}


		}
		else
		{
			Fmt (Mesg, "%s<Analysis file done");
			PostDeferredCallToThread (MessageCB, Mesg, mainThreadID);
			break;
		};
	}


	/* Free the daq buffer */
	if(pdBuffer != NULL)
		free(pdBuffer);

	return 0;
}


int CVICALLBACK LAStartCb (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		{

			if(pLAfile != NULL)
			{
				analysis_piezo_init()  ;
				SetInputMode (l_analysis_handle, PANEL_LA_LA_START, 0);
				laStopFlag = 0;
				CmtScheduleThreadPoolFunctionAdv (DEFAULT_THREAD_POOL_HANDLE, laThreadFunction, NULL,THREAD_PRIORITY_LOWEST, NULL, 0, NULL, 0, &laThreadFunctionID);

			}
			else
			{
				MessagePopup ("Error:","Open file error!!");

			}


		}

		break;

	}
	return 0;
}

