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
#include "bio_demo.h"
#include "serial.h"

#include "ppg_cb.h"
#include "timer_cb.h"
#include "mmd_comm.h"

extern	  h_comm_handle_t h_comm_handle;

extern unsigned char  funSelectionValue;
int PPG_handle;

static int RdAdcData,crc_err,dtype,err_comm;
static  unsigned char SteamCBdata[2],stream_seq[MAX_MMED_TYPES];
static  unsigned char plotting = 0;
static CmtTSQCallbackID ppgPlotcallbackID =0;

static FILE* csv_fd;
void openCsvFile(unsigned char file_num)
{
	char output_file[]="fl_ppg_xxx.csv";
	sprintf(output_file,"fl_ppg_%3d.csv",file_num); //设置文件名
	csv_fd = fopen(output_file,"w");	//“写”打开文件
	if(!csv_fd)
	{
		perror("fopen failed!");

	}

}

void writeCsvFile(int i)
{

	 fprintf(csv_fd, "%d\r\n", i);
}
void CVICALLBACK ppgPlotDataFromQueueCallback (CmtTSQHandle queueHandle, unsigned int event,
		int value, void *callbackData)
{
	h_comm_rdata_t rdata[1];

	double data_ld[1];

	switch (event)
	{
		case EVENT_TSQ_ITEMS_IN_QUEUE:

			CmtReadTSQData (queueHandle, rdata, 1, TSQ_INFINITE_TIMEOUT, 0);
			if(rdata->dframe[1] == DATA_STREAMING_COMMAND)
			{
				SetCtrlAttribute (PPG_handle, PANEL_PPG_TIMER, ATTR_ENABLED, 0);
			}
			else
			{
				if(rdata->dframe[0])
				{
					//	MessagePopup ("Error:","RecvSensor crc!!!!");
					crc_err++;
					//	sprintf (message, "Lost Frames: %d", err_comm);
					SetCtrlVal (PPG_handle, PANEL_PPG_CRC_ERROR_IND,crc_err);
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
								SetCtrlVal (PPG_handle, PANEL_PPG_LOST_IND,err_comm);
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

							if((funSelectionValue == PPG_SPO2)||(funSelectionValue == RESP_FUN) )
							{
								switch (dtype << 4)
								{

									case  SAMPLE_RESP:
									case  SAMPLE_PPG_R:
									//	Filter_Low(data_ld, data_ld[0],0) ;
										PlotStripChart (PPG_handle, PANEL_PPG_CHART_AC, data_ld, 1, 0, 0, VAL_DOUBLE);
									//	readSensor(data_ld[0]) ;
									//	AnalysisLoop();
										break;
									case  SAMPLE_IMP:
									case  SAMPLE_PPG_IR:
										PlotStripChart (PPG_handle, PANEL_PPG_CHART_DC, data_ld, 1, 0, 0, VAL_DOUBLE);
										break;

								}
							}
							else
							{
								switch (dtype << 4)
								{


									case  SAMPLE_PPG_G:
									case  SAMPLE_PPG_R:
									case  SAMPLE_PPG_IR:
										//		data_ld[0] =  RdAdcData - MedianFilterProcess(RdAdcData);
										//	Filter_Low(data_ld, data_ld[0],0) ;
										//	 data_ld[0] = map(data_ld[0], float I_Min, float I_Max, float O_Min, float O_Max)

										writeCsvFile(data_ld[0]);
										PlotStripChart (PPG_handle, PANEL_PPG_CHART_AC, data_ld, 1, 0, 0, VAL_DOUBLE);
										//	readSensor(data_ld[0]) ;
										//	AnalysisLoop();
										break;
									case  SAMPLE_PPG_GDC:
									case  SAMPLE_PPG_RDC:
									case  SAMPLE_PPG_IRDC:
										PlotStripChart (PPG_handle, PANEL_PPG_CHART_DC, data_ld, 1, 0, 0, VAL_DOUBLE);
										break;

								}
							}





						}
					}

				}

			}

			break;
	}
}

int CVICALLBACK ppgTimerCallback (int panel, int control, int event,
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



int CVICALLBACK Quit_PPG_Cb (int panel, int control, int event,
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

				CmtUninstallTSQCallback (h_comm_handle.queueHandle, ppgPlotcallbackID);



			}
			DiscardPanel (panel);
			if( csv_fd)
			fclose(csv_fd);
			break;
		}
	}
	return 0;
}

int CVICALLBACK PPG_StartCb (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	int val, traces, i;
	h_comm_rdata_t *rdata;


	if (event == EVENT_COMMIT)
	{
		GetCtrlVal(panel, control, &val);

		if(val ==1)
		{
			crc_err=0;
			err_comm =0;
			for (i=0; i<MAX_MMED_TYPES; i++)
				stream_seq[i] = 0;

			switch(funSelectionValue)
			{
				case PPG_G_FUN:
					SteamCBdata[0] = (unsigned char)(REG_FUN1_PPG_G >> 8);
					SteamCBdata[1] = (unsigned char)REG_FUN1_PPG_G;
					break;

				case PPG_R_FUN:
					SteamCBdata[0] = (unsigned char)(REG_FUN1_PPG_R >> 8);
					SteamCBdata[1] = (unsigned char)REG_FUN1_PPG_R;
					
					break;

				case PPG_IR_FUN:
					SteamCBdata[0] = (unsigned char)(REG_FUN1_PPG_IR >> 8);
					SteamCBdata[1] = (unsigned char)REG_FUN1_PPG_IR;
					break;

				case PPG_SPO2:
					SteamCBdata[0] = (unsigned char)(FUN1_COMBO_SPO2 >> 8);
					SteamCBdata[1] = (unsigned char)FUN1_COMBO_SPO2;
					break;

				case RESP_FUN:
					SteamCBdata[0] = (unsigned char)(REG_FUN1_RESP >> 8);
					SteamCBdata[1] = (unsigned char)REG_FUN1_RESP;
					break;

			}
			openCsvFile(1);  
		//	Filter_Low_init(0.1);
		//	initData() ;
		//	MedianFilterInit(2048) ;
			h_comm_sendCMD(&h_comm_handle,DATA_STREAMING_COMMAND,SteamCBdata[0],SteamCBdata[1],0);
			SetCtrlAttribute (PPG_handle, PANEL_PPG_TIMER, ATTR_ENABLED, 1);
			plotting = 1;
			CmtFlushTSQ(h_comm_handle.queueHandle,TSQ_FLUSH_ALL ,NULL);
			/* Install a callback to read and plot the generated data. */
			CmtInstallTSQCallback (h_comm_handle.queueHandle, EVENT_TSQ_ITEMS_IN_QUEUE, 1, ppgPlotDataFromQueueCallback, NULL,CmtGetCurrentThreadID(), &ppgPlotcallbackID);
		}
		else
		{
			h_comm_sendCMD(&h_comm_handle,DATA_STREAMING_COMMAND,SteamCBdata[0]&(~M_FUN_START),SteamCBdata[1],0);
			plotting = 0;

			CmtUninstallTSQCallback (h_comm_handle.queueHandle, ppgPlotcallbackID);

		}

		GetCtrlAttribute(panel, PANEL_PPG_CHART_DC, ATTR_NUM_TRACES, &traces);
		for (i=1; i<=traces; i++)
			SetTraceAttribute(panel,PANEL_PPG_CHART_DC, i, ATTR_TRACE_LG_VISIBLE, 1);

		GetCtrlAttribute(panel, PANEL_PPG_CHART_AC, ATTR_NUM_TRACES, &traces);
		for (i=1; i<=traces; i++)
			SetTraceAttribute(panel,PANEL_PPG_CHART_AC, i, ATTR_TRACE_LG_VISIBLE, 1);



	}
	return 0;
}





