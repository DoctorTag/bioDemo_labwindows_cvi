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

#include "bio_common.h"
#include "bio_demo.h"
#include "serial.h"

//#include "ppg_cb.h"
#include "timer_cb.h"
#include "mmd_comm.h"

#include "analysis_piezo.h"


extern	  h_comm_handle_t h_comm_handle;

FILE * pLAfile;
int l_analysis_handle;
char LAfile_path[MAX_PATHNAME_LEN];

volatile    int     laStopFlag             =   1;
static CmtTSQCallbackID laPlotcallbackID = 0;
static      CmtThreadFunctionID laThreadFunctionID = NULL;


static volatile int resultPlot =  0; 
static void LAStop(void);

void CVICALLBACK laPlotDataFromQueueCallback (CmtTSQHandle queueHandle, unsigned int event,
		int value, void *callbackData)
{
	static	double enhance_peaks[2];
		analysis_result_t *ana_plot;  


	new_tsq_t rtsq[1];

//	float wave_data;
//	double result;
//	float filter_data;
	short i;
	switch (event)
	{
		case EVENT_TSQ_ITEMS_IN_QUEUE:

			CmtReadTSQData (queueHandle, rtsq, 1, TSQ_INFINITE_TIMEOUT, 0);

			ana_plot = rtsq[0].p_tsq;

			if(ana_plot->resp_ok == true)
				SetCtrlVal (l_analysis_handle, PANEL_LA_RESP_IND,ana_plot->resp);
			i=0;
			while(i < SAMPLE_HZ)
			{
				PlotStripChart (l_analysis_handle,PANEL_LA_CHART_DATA,ana_plot->hr_raw_dbuf+i , 1, 0, 0, VAL_INTEGER);
				i++;
			}

			if(resultPlot == 2)
			{
				enhance_peaks[0] = ana_plot->moved_Intensity;
				enhance_peaks[1] = ana_plot->moved_Intensity;
				//	SetTraceAttribute(l_analysis_handle,PANEL_LA_CHART_RESULT, 2, ATTR_TRACE_VISIBLE , 0);
				PlotStripChart (l_analysis_handle, PANEL_LA_CHART_RESULT, enhance_peaks, 2, 0, 1, VAL_DOUBLE);
			}

			if(resultPlot == 0)
			{
				i=0;
				while(i < SAMPLE_HZ)
				{
					enhance_peaks[0] = ana_plot->hr_filted_dbuf[i];
					enhance_peaks[1] = ana_plot->hr_filted_dbuf[i];
					PlotStripChart (l_analysis_handle, PANEL_LA_CHART_RESULT,enhance_peaks ,2, 0, 1, VAL_DOUBLE);
					i++;
				}
			}

			if(resultPlot == 3)
			{
				if(ana_plot->resp_ok == true)
				{

					i=0;
					while(i < ANA_BUF_LEN)
					{
						unsigned char di = 0;
						enhance_peaks[0] = ana_plot->resp_dbuf[i];
						while(di < ana_plot->resp_ppoints)
						{
							if(ana_plot->resp_peak_locbuf[di] == i)
							{
								enhance_peaks[1] = ana_plot->resp_dbuf[i];
								break;
							}
							di++;
						}
						PlotStripChart (l_analysis_handle, PANEL_LA_CHART_RESULT, enhance_peaks, 2, 0, 0,VAL_DOUBLE);
						i++;
					}


				}
			}

			if(resultPlot == 1)
			{
				i=0;
				while(i < SAMPLE_HZ)
				{	 
					unsigned char di = 0;
					enhance_peaks[0] = ana_plot->hr_enhanced_dbuf[i];
					while(di < ana_plot->hr_ppoints)
					{
						if(ana_plot->hr_peak_locbuf[di] == i)
						{
							enhance_peaks[1] = ana_plot->hr_enhanced_dbuf[i];
							break;
						}
						di++;
					}
					//	SetTraceAttribute(l_analysis_handle,PANEL_LA_CHART_RESULT, 2, ATTR_TRACE_VISIBLE , 0);
					//PlotStripChart (l_analysis_handle, PANEL_LA_CHART_RESULT, ana_plot->hr_enhanced_data+i, 1, 0, 0, VAL_FLOAT);

					PlotStripChart (l_analysis_handle, PANEL_LA_CHART_RESULT, enhance_peaks, 2, 0, 0, VAL_DOUBLE);
					i++;
				}


			}


			switch(ana_plot->cur_body_status)
			{
				case OUT_OF_BED:
					SetCtrlVal (l_analysis_handle, PANEL_LA_BODY_STATUS,"Out of bed");
					break;

				case BODY_REPOSE:
					SetCtrlVal (l_analysis_handle, PANEL_LA_BODY_STATUS,"Repose");
					break;

				case BODY_MOVE:
					SetCtrlVal (l_analysis_handle, PANEL_LA_BODY_STATUS,"Moving");
					break;
				case BODY_UNKNOW:
					SetCtrlVal (l_analysis_handle, PANEL_LA_BODY_STATUS,"Unknow");
					break;
			}

			if(ana_plot->hr_ok == true)
			{
				SetCtrlVal (l_analysis_handle, PANEL_LA_LED_ANA,1);
				//	SetCtrlVal (l_analysis_handle, PANEL_LA_RESP_IND,ana_plot->resp);
				SetCtrlVal (l_analysis_handle, PANEL_LA_HR_IND,ana_plot->hr);

			}
			else
			{
				SetCtrlVal (l_analysis_handle, PANEL_LA_LED_ANA,0);
				//		SetCtrlVal (l_analysis_handle, PANEL_LA_RESP_IND,0);
				SetCtrlVal (l_analysis_handle, PANEL_LA_HR_IND,0);

			}

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
static void LAStop(void)
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
		SetCtrlVal (l_analysis_handle, PANEL_LA_RUN_IND, (char *)mesg);
		if (FindPattern ((char *)mesg, 0, strlen((char *)mesg), "Error", 0, 0) != -1)
		{
			/* Error occurred */
			SetCtrlVal (l_analysis_handle, PANEL_LA_LED_RUN, 1);
			LAStopCb (l_analysis_handle, 0, EVENT_COMMIT, 0, 0, 0);
		}
		else if (FindPattern ((char *)mesg, 0, strlen((char *)mesg), "done", 0, 0) != -1)
			LAStopCb (l_analysis_handle, 0, EVENT_COMMIT, 0, 0, 0);
	}
}



//float debug_buf[SAMPLE_HZ] ;

//h_comm_rdata_t la_data;
/*------------------------------------------------------------------*/
/*  Calls the Local Analysis functions.  */
/*------------------------------------------------------------------*/
int CVICALLBACK laThreadFunction (void *functionData)
{

	int     		mainThreadID,tdata,fret;
	char    		Mesg[600];

	analysis_result_t *pdBuffer               =   NULL;
	bool   new_rd;
	mainThreadID = CmtGetMainThreadID ();

	if ((pdBuffer = (analysis_result_t*)malloc(sizeof(analysis_result_t)))==NULL)
		return -1;
	pdBuffer->resp_ok = false;
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
				pdBuffer->resp_ok = false;

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
	int val;
	switch (event)
	{
		case EVENT_COMMIT:
		{
			GetCtrlVal(panel, control, &val);
			if(val == 1)
			{


				if(pLAfile != NULL)
				{
					GetCtrlVal (panel, PANEL_LA_RESULT_PLOT, (int *)&resultPlot);

					//	if(resultPlot == 1)
					//		SetTraceAttribute(l_analysis_handle,PANEL_LA_CHART_RESULT, 2, ATTR_TRACE_VISIBLE , 1);
					//	else
					//		SetTraceAttribute(l_analysis_handle,PANEL_LA_CHART_RESULT, 2, ATTR_TRACE_VISIBLE , 0);


					analysis_piezo_init(0xF30000)  ;
					//SetInputMode (l_analysis_handle, PANEL_LA_LA_START, 0);
					laStopFlag = 0;
					CmtScheduleThreadPoolFunctionAdv (DEFAULT_THREAD_POOL_HANDLE, laThreadFunction, NULL,THREAD_PRIORITY_LOWEST, NULL, 0, NULL, 0, &laThreadFunctionID);

				}
				else
				{
					MessagePopup ("Error:","Open file error!!");

				}
			}
			else
			{
				LAStop();
				/* Flush the thread-safe queue */
				CmtFlushTSQ (h_comm_handle.queueHandle, TSQ_FLUSH_ALL, NULL);

			}

		}

		break;

	}
	return 0;
}

