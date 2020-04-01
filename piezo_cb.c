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
#include "asynctmr.h"
#include <analysis.h>
#include <utility.h>
#include <cvirte.h>
#include <userint.h>
#include <ansi_c.h>
//#include <progressbar.h>
#include <toolbox.h>

#include "bio_common.h"

#include "bio_demo.h"
#include "serial.h"

//#include "piezo_cb.h"
#include "timer_cb.h"
#include "mmd_comm.h"

#include "analysis_piezo.h"


extern	  h_comm_handle_t h_comm_handle;

//xtern unsigned char  funSelectionValue;
int PIEZO_handle;

static int RdAdcData,crc_err,dtype,err_comm;
static  unsigned char SteamCBdata[2],stream_seq[MAX_MMED_TYPES];
static  unsigned char plotting = 0;
static CmtTSQCallbackID piezoPlotcallbackID =0;

static FILE* piezo_csv_fd = NULL;

static	analysis_result_t ana_result;


volatile int fileSaveFlag =  0;
volatile int resultPlot =  0;


void CVICALLBACK piezoPlotDataFromQueueCallback (CmtTSQHandle queueHandle, unsigned int event,
		int value, void *callbackData)
{
	h_comm_rdata_t *rdata;
	new_tsq_t rtsq[1];
	double data_ld[1];
	bool ana_ok;
	short i;
	switch (event)
	{
		case EVENT_TSQ_ITEMS_IN_QUEUE:

			CmtReadTSQData (queueHandle, rtsq, 1, TSQ_INFINITE_TIMEOUT, 0);
			rdata = rtsq[0].p_tsq;
			if(rdata->dframe[1] == DATA_STREAMING_COMMAND)
			{
				SetCtrlAttribute (PIEZO_handle, PANEL_PZ_TIMER, ATTR_ENABLED, 0);
			}
			else
			{
				if(rdata->dframe[0])
				{
					//	MessagePopup ("Error:","RecvSensor crc!!!!");
					crc_err++;
					//	sprintf (message, "Lost Frames: %d", err_comm);
					SetCtrlVal (PIEZO_handle, PANEL_PZ_CRC_ERROR_IND,crc_err);
				}
				else
				{

					dtype = (rdata->dframe[1]& DATA_TYPE_MASK );
					if(dtype < MAX_MMED_TYPES)
					{
						if(rdata->dframe[3] > 0)
						{
							if(rdata->dframe[3] != (stream_seq[dtype] +1) )
							{
								err_comm++;
								//	sprintf (message, "Lost Frames: %d", err_comm);
								SetCtrlVal (PIEZO_handle, PANEL_PZ_LOST_IND,err_comm);
							}

						}
						stream_seq[dtype] = rdata->dframe[3];

						for (short LC = 0; LC < PACK_SAMPLES; LC++)                       // Decode received packet of 15 samples.
						{
							switch(rdata->dframe[1]& FRAME_TYPE_MASK )
							{
								case DATA_TYPE_12BIT:
									if(LC%2)
									{
										RdAdcData = rdata->dframe[3*(LC-1)/2 + 5];
										RdAdcData &= 0xf0;
										RdAdcData = (RdAdcData << 4);
										RdAdcData |= rdata->dframe[3*(LC-1)/2 + 6];

									}
									else
									{

										RdAdcData = rdata->dframe[3*LC/2 + 5];
										RdAdcData &= 0x0f;
										RdAdcData = (RdAdcData << 8);
										RdAdcData |= rdata->dframe[3*LC/2 + 4];

									}
									break;

								case DATA_TYPE_24BIT:

								{
									RdAdcData = rdata->dframe[3*LC + 6];
									RdAdcData <<= 8;
									RdAdcData |= rdata->dframe[3*LC + 5];
									RdAdcData <<= 8;
									RdAdcData |= rdata->dframe[3*LC + 4];

								}

								break;
							}

							data_ld[0] =  RdAdcData;



							if( (dtype << 4) == SAMPLE_PIEZO)
							{
								if(fileSaveFlag == 1)
									writeDataToCsvFile(piezo_csv_fd, RdAdcData);
								ana_ok = analysis_piezo_all(RdAdcData,&ana_result) ;

								if(ana_ok == true)
								{

									if(resultPlot == 2)
									{
										double moved_result = ana_result.moved_Intensity;
										PlotStripChart (PIEZO_handle, PANEL_PZ_CHART_ANALYSIS, &moved_result, 1, 0, 0, VAL_DOUBLE);
									}
									else
									{
										i=0;
										while(i < SAMPLE_HZ)
										{
											if(resultPlot == 0)
												PlotStripChart (PIEZO_handle, PANEL_PZ_CHART_ANALYSIS, ana_result.hr_filted_data+i, 1, 0, 0, VAL_FLOAT);
											else
												PlotStripChart (PIEZO_handle, PANEL_PZ_CHART_ANALYSIS, ana_result.hr_enhanced_data+i, 1, 0, 0, VAL_FLOAT);
											i++;
										}
									}
								}
								PlotStripChart (PIEZO_handle, PANEL_PZ_CHART_RAW, data_ld, 1, 0, 0, VAL_DOUBLE);


							}

						}
					}

				}

			}
			free(rdata);
			break;
	}
}

int CVICALLBACK piezoTimerCallback (int panel, int control, int event,
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



int CVICALLBACK Quit_Piezo_Cb (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		{

			if(plotting)
			{
				h_comm_sendCMD(&h_comm_handle,DATA_STREAMING_COMMAND,SteamCBdata[0]&(~M_FUN_START),SteamCBdata[1],0)   ;

				plotting = 0;

				CmtUninstallTSQCallback (h_comm_handle.queueHandle, piezoPlotcallbackID);

			}
			DiscardPanel (panel);
			if( piezo_csv_fd)
				fclose(piezo_csv_fd);
			break;
		}
	}
	return 0;
}

int CVICALLBACK Piezo_StartCb (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	int val, traces, i;
//	h_comm_rdata_t *rdata;


	if (event == EVENT_COMMIT)
	{
		GetCtrlVal(panel, control, &val);

		if(val ==1)
		{
			GetCtrlVal (panel, PANEL_PZ_CHECKBOX, (int *)&fileSaveFlag);
			GetCtrlVal (panel, PANEL_PZ_RING, (int *)&resultPlot);

			crc_err=0;
			err_comm =0;
			for (i=0; i<MAX_MMED_TYPES; i++)
				stream_seq[i] = 0;


			SteamCBdata[0] = (unsigned char)(REG_FUN2_PIEZO >> 8);
			SteamCBdata[1] = (unsigned char)REG_FUN2_PIEZO;

			analysis_piezo_init()  ;
			if(fileSaveFlag == 1)
				piezo_csv_fd = openCsvFileForWrite("piezo");
			h_comm_sendCMD(&h_comm_handle,DATA_STREAMING_COMMAND,SteamCBdata[0],SteamCBdata[1],0);
			SetCtrlAttribute (PIEZO_handle, PANEL_PPG_TIMER, ATTR_ENABLED, 1);
			plotting = 1;
			CmtFlushTSQ(h_comm_handle.queueHandle,TSQ_FLUSH_ALL ,NULL);
			/* Install a callback to read and plot the generated data. */
			CmtInstallTSQCallback (h_comm_handle.queueHandle, EVENT_TSQ_ITEMS_IN_QUEUE, 1, piezoPlotDataFromQueueCallback, NULL,CmtGetCurrentThreadID(), &piezoPlotcallbackID);
		}
		else
		{
			h_comm_sendCMD(&h_comm_handle,DATA_STREAMING_COMMAND,SteamCBdata[0]&(~M_FUN_START),SteamCBdata[1],0);
			plotting = 0;

			CmtUninstallTSQCallback (h_comm_handle.queueHandle, piezoPlotcallbackID);

		}

		GetCtrlAttribute(panel, PANEL_PZ_CHART_ANALYSIS, ATTR_NUM_TRACES, &traces);
		for (i=1; i<=traces; i++)
			SetTraceAttribute(panel,PANEL_PZ_CHART_ANALYSIS, i, ATTR_TRACE_LG_VISIBLE, 1);

		GetCtrlAttribute(panel, PANEL_PZ_CHART_RAW, ATTR_NUM_TRACES, &traces);
		for (i=1; i<=traces; i++)
			SetTraceAttribute(panel,PANEL_PZ_CHART_RAW, i, ATTR_TRACE_LG_VISIBLE, 1);



	}
	return 0;
}





