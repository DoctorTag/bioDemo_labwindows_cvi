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
#include <analysis.h>
#include <utility.h>
#include <cvirte.h>
#include <userint.h>
#include <ansi_c.h>

#include <toolbox.h>
#include "bio_demo.h"
#include "serial.h"
#include "mmd_comm.h"
#include "ecg_d_cb.h"
#include "timer_cb.h"

#include "dsp_filter.h"     
#include "filter.h"
#include "medianfilter.h"

extern	  h_comm_handle_t h_comm_handle;

extern unsigned char  funSelectionValue;

int ECG_D_handle;

static int RdAdcData;
static  unsigned char SteamCBdata[2],stream_seq[MAX_MMED_TYPES],plotting;
static  int err_comm,crc_err;
static CmtTSQCallbackID ecgPlotcallbackID;

void CVICALLBACK ecgPlotDataFromQueueCallback (CmtTSQHandle queueHandle, unsigned int event,
		int value, void *callbackData)
{
	h_comm_rdata_t rdata[1];

	double data_ld[1];
	unsigned char dtype,ftype,dcnt;
	unsigned short dlen;
	switch (event)
	{
		case EVENT_TSQ_ITEMS_IN_QUEUE:

			CmtReadTSQData (queueHandle, rdata, 1, TSQ_INFINITE_TIMEOUT, 0);
			if(rdata->dframe[1] == DATA_STREAMING_COMMAND)
			{
				SetCtrlAttribute (ECG_D_handle, PANEL_ECGD_TIMER, ATTR_ENABLED, 0);
			}
			else
			{
				if(rdata->dframe[0])
				{
					//	MessagePopup ("Error:","RecvSensor crc!!!!");
					crc_err++;
					//	sprintf (message, "Lost Frames: %d", err_comm);
					SetCtrlVal (ECG_D_handle, PANEL_ECGD_CRC_ERROR_IND,crc_err);
				}
				else
				{
					ftype = (((rdata->dframe[1]& 0xf0 )-DATA_TYPE_DEFAULT) >> 4)+MASK_8BIT;
					dlen =   rdata->dframe[2];
					if(ftype == MASK_24BIT)
					dlen += 256;
					dtype = (rdata->dframe[1]& DATA_TYPE_MASK );
					if(dtype < MAX_MMED_TYPES)
					{
						if(rdata->dframe[3] > 0)
						{
							if(rdata->dframe[3] != (stream_seq[dtype] +1) )
							{
								err_comm++;
								//	sprintf (message, "Lost Frames: %d", err_comm);
								SetCtrlVal (ECG_D_handle, PANEL_ECGD_LOST_IND,err_comm);
							}

						}
						stream_seq[dtype] = rdata->dframe[3];
						//	MessagePopup ("Error:","RecvSensor frame lost!!!!");
						// LeadStaus = Rdbuf[4];                                   // Lead STATUS
					
						dcnt = (dlen*2)/ftype ;
						for (unsigned char LC = 0; LC < dcnt; LC++)                       // Decode received packet of 15 samples.
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
								//	unsigned char tmp_h  = rdata->dframe[3*LC + 6]  ;
									//if(tmp_h >=  0x80)
									//	tmp_h += 0x80;
									RdAdcData =  rdata->dframe[3*LC + 6];
									RdAdcData <<= 8;
									RdAdcData |= rdata->dframe[3*LC + 5];
									RdAdcData <<= 8;
									RdAdcData |= rdata->dframe[3*LC + 4];

								}

								break;
								
								default:
									
									while(1) ;
									break;
							}

							data_ld[0] =  RdAdcData;

							switch (dtype << 4)
							{


								case  SAMPLE_ECG:
							//		data_ld[0] =  RdAdcData - MedianFilterProcess(RdAdcData);
						//data_ld[0] = Lead_II_Filter(data_ld[0], IIR2_25Hz_Table, IIR2_05Hz_Table);
									//	data_ld[0] =  data_ld[0] - MedianFilterProcess(data_ld[0]);
										//	 data_ld[0] /=128;

									PlotStripChart (ECG_D_handle, PANEL_ECGD_CHART_D, data_ld, 1, 0, 0, VAL_DOUBLE);
									break;

							}





						}
					}

				}

			}
			  
			break;
	}
}


int CVICALLBACK ecgTimerCallback (int panel, int control, int event,
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



int CVICALLBACK Quit_ECG_D_Cb (int panel, int control, int event,
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

				CmtUninstallTSQCallback (h_comm_handle.queueHandle, ecgPlotcallbackID);



			}
			DiscardPanel (panel);
			break;
		}
	}
	return 0;
}

int CVICALLBACK ECG_D_StartCb (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	int val, traces, i;



	if (event == EVENT_COMMIT)
	{
		GetCtrlVal(panel, control, &val);

		if(val ==1)
		{
			err_comm = 0;
			crc_err=0;
			for (i=0; i<MAX_MMED_TYPES; i++)
				stream_seq[i] = 0;

			if(funSelectionValue == 3)
			{
			SteamCBdata[0] = (unsigned char)(REG_FUN2_ECG_DI >> 8);
				SteamCBdata[1] = (unsigned char)REG_FUN2_ECG_DI;
			//		SteamCBdata[0] = (unsigned char)(REG_FUN1_ECG_AII >> 8);
			//	SteamCBdata[1] = (unsigned char)REG_FUN1_ECG_AII ;

			}
			else
			{
				SteamCBdata[0] = (unsigned char)(REG_FUN1_ECG_A >> 8);
				SteamCBdata[1] = (unsigned char)REG_FUN1_ECG_A ;

			}
			MedianFilterInit(2048) ;
				h_comm_sendCMD(&h_comm_handle,DATA_STREAMING_COMMAND,SteamCBdata[0],SteamCBdata[1],0);  
			
		//	SetCtrlAttribute (ECG_D_handle, PANEL_ECGD_TIMER, ATTR_ENABLED, 1);
			plotting = 1;
			CmtFlushTSQ(h_comm_handle.queueHandle,TSQ_FLUSH_ALL ,NULL);
			/* Install a callback to read and plot the generated data. */
			CmtInstallTSQCallback (h_comm_handle.queueHandle, EVENT_TSQ_ITEMS_IN_QUEUE, 1, ecgPlotDataFromQueueCallback, NULL,CmtGetCurrentThreadID(), &ecgPlotcallbackID);


		}
		else
		{

			h_comm_sendCMD(&h_comm_handle,DATA_STREAMING_COMMAND,SteamCBdata[0]&(~M_FUN_START),SteamCBdata[1],0);
			plotting = 0;

			CmtUninstallTSQCallback (h_comm_handle.queueHandle, ecgPlotcallbackID);
		}

		GetCtrlAttribute(panel, PANEL_ECGD_CHART_D, ATTR_NUM_TRACES, &traces);
		for (i=1; i<=traces; i++)
			SetTraceAttribute(panel,PANEL_ECGD_CHART_D, i, ATTR_TRACE_LG_VISIBLE, 1);




	}
	return 0;
}


