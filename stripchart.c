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
#include "stripchart.h"
#include "commcallback.h"
#include "mmd_comm.h"

volatile    int     LVSteamingFlag             =   0;
volatile  unsigned char SteamCBdata[2];

int ReadAdcData[32];

//static      CmtTSQHandle        tsqHdl;
//static      int                 status;
//static      CmtThreadFunctionID LVSteamThreadFunctionID;

static int hpanel;
void SteamData(unsigned char isbegin,unsigned char dtype) ;

int CVICALLBACK LVSteamThreadFunction(void *callbackData) ;
/*---------------------------------------------------------------------------*/
/* This is the application's entry-point.                                    */
/*---------------------------------------------------------------------------*/
int main (int argc, char *argv[])
{
	LVSteamingFlag             =   0;

	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;
	if ((hpanel = LoadPanel (0, "stripchart.uir", PANEL)) < 0)
		return -1;

	Init_ComPort ()  ;
	/* Display the panel and run the UI */
	DisplayPanel (hpanel);
	RunUserInterface ();

	/* Free resources and return */
	ShutDownCom ()  ;
	DiscardPanel (hpanel);
	CloseCVIRTE ();
	return 0;
}


/*---------------------------------------------------------------------------*/
/* Respond to the user's choice by enabling a timer which will plot data     */
/* continuously to a Stripchart.                                             */
/*---------------------------------------------------------------------------*/
int CVICALLBACK PlotData (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	int val, traces, i;

	if (event == EVENT_COMMIT)
	{
		GetCtrlVal(panel, control, &val);
		// SetCtrlAttribute (panel, PANEL_TIMER, ATTR_ENABLED, val);
		LVSteamingFlag = val;
		if(val ==1)
		{
	//	SteamCBdata[0] = (REG_FUN2_ECG_D | M_FUN_START|OBEY_MODE);
	//			SteamCBdata[1] = 0;

			SteamCBdata[0] = (M_FUN_START|OBEY_MODE);
		//	SteamCBdata[1] = REG_FUN1_PPG_R;
		//	SteamCBdata[1] = REG_FUN1_RESP;
			//	 SteamCBdata[1] = FUN1_COMBO_STD;
			 SteamCBdata[1] = REG_FUN1_ECG_A;

			PostDeferredCallToThread (LVSteamThreadFunction, (void*)SteamCBdata, CmtGetMainThreadID ());
		}
		else
		{

			SteamData(OBEY_MODE,SteamCBdata[1])   ;
		}


		GetCtrlAttribute(panel, PANEL_RESP_CHART, ATTR_NUM_TRACES, &traces);
		for (i=1; i<=traces; i++)
			SetTraceAttribute(panel, PANEL_RESP_CHART, i, ATTR_TRACE_LG_VISIBLE, 1);


		GetCtrlAttribute(panel, PANEL_ECG_CHART, ATTR_NUM_TRACES, &traces);
		for (i=1; i<=traces; i++)
			SetTraceAttribute(panel, PANEL_ECG_CHART, i, ATTR_TRACE_LG_VISIBLE, 1);


		GetCtrlAttribute(panel, PANEL_PPG1_CHART, ATTR_NUM_TRACES, &traces);
		for (i=1; i<=traces; i++)
			SetTraceAttribute(panel, PANEL_PPG1_CHART, i, ATTR_TRACE_LG_VISIBLE, 1);

		GetCtrlAttribute(panel, PANEL_PPG1_CHART_2, ATTR_NUM_TRACES, &traces);
		for (i=1; i<=traces; i++)
			SetTraceAttribute(panel, PANEL_PPG1_CHART_2, i, ATTR_TRACE_LG_VISIBLE, 1);


		GetCtrlAttribute(panel, PANEL_PPG1_CHART_3, ATTR_NUM_TRACES, &traces);
		for (i=1; i<=traces; i++)
			SetTraceAttribute(panel, PANEL_PPG1_CHART_3, i, ATTR_TRACE_LG_VISIBLE, 1);

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
		if(LVSteamingFlag == 1)
			SteamData(OBEY_MODE,SteamCBdata[1])   ;
		LVSteamingFlag=0;
		QuitUserInterface (0);
	}
	return 0;
}


/*---------------------------------------------------------------------------*/
/* Pause the stripchart traces                                               */
/*---------------------------------------------------------------------------*/
int CVICALLBACK PauseChart (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	int val;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(panel, control, &val);
			SetCtrlAttribute(panel, PANEL_PPG1_CHART , ATTR_STRIP_CHART_PAUSED, val);
			SetCtrlAttribute(panel, PANEL_PPG1_CHART_2 , ATTR_STRIP_CHART_PAUSED, val);
			SetCtrlAttribute(panel, PANEL_PPG1_CHART_3 , ATTR_STRIP_CHART_PAUSED, val);


			SetCtrlAttribute(panel, PANEL_RESP_CHART , ATTR_STRIP_CHART_PAUSED, val);

			SetCtrlAttribute(panel, PANEL_ECG_CHART , ATTR_STRIP_CHART_PAUSED, val);

			break;
	}
	return 0;
}



int CVICALLBACK PanelCB (int panel, int event, void *callbackData,
						 int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_CLOSE:
			if(LVSteamingFlag == 1)
				SteamData(OBEY_MODE,SteamCBdata[1])   ;
			LVSteamingFlag=0;
			QuitUserInterface (0);
			break;
	}
	return 0;
}

void SteamData(unsigned char isbegin,unsigned char dtype)
{
	static  unsigned char Wrbuf[10];

	Wrbuf[0] = START_DATA_HEADER;           // Start Header
	Wrbuf[1] = DATA_STREAMING_COMMAND;      // ECG data streaming command
	Wrbuf[2] = isbegin;                           // Start command
	Wrbuf[3] = dtype;

	Wrbuf[4] = END_DATA_HEADER;
	Wrbuf[5] = END_DATA_HEADER;
	Wrbuf[6] = '\n';
	SendData(Wrbuf, 7);              // Send command to firmware
}


int CVICALLBACK LVSteamThreadFunction(void *callbackData)
{
	int traces,i;
	unsigned int opacity;
	static unsigned int oldOpacity = 255;
//	double data_ppg[3];
	double data_ld[1];

	unsigned char *ptrframe;
	unsigned char Rdbuf[64];
	unsigned char iLoopCnt = 0;
	//unsigned char Wrbuf[10];
	short ADC_Data_ptr;
	//short filter_tmp;
	double fout;
	int readbytes;

	unsigned char *pcldata = (unsigned char *)callbackData ;
	/*
	Wrbuf[0] = START_DATA_HEADER;           // Start Header
	Wrbuf[1] = DATA_STREAMING_COMMAND;      // ECG data streaming command
	Wrbuf[2] = 1;                           // Start command
	//   Wrbuf[3] = ADC_FUN_ECG;
	Wrbuf[3] = *((unsigned char *)callbackData);

	Wrbuf[4] = END_DATA_HEADER;
	Wrbuf[5] = END_DATA_HEADER;
	Wrbuf[6] = '\n';
	*/
	SteamData(*pcldata,*(pcldata+1))   ;
	readbytes = ReceiveData(Rdbuf, 7);   // Read buffer
	while (readbytes > 0)
	{
		readbytes = ReceiveData(Rdbuf, 7);
	}

	//SendData(Wrbuf, 7);              // Send command to firmware
	// Stream_Live_Data = TRUE;                // Set busy flag
	// initFilter();
	//resetFilter();
	RecvInit() ;
	while( LVSteamingFlag)
	{

		memset(Rdbuf,0,63);                 // Clear receive buffer
		readbytes =ReceiveData(Rdbuf, 63);          // Read packet of 14 samples

		if(readbytes>0)
		{
			for (short i = 0; i < readbytes; i++)
			{
				ptrframe = RecvFrame(Rdbuf[i]);
				if( ptrframe)
				{
					ADC_Data_ptr = 0;                               // Set pointer
					ReadAdcData[ADC_Data_ptr++] = ptrframe[2];                 // Heart Rate
					ReadAdcData[ADC_Data_ptr++] = ptrframe[3];                 // Respiration Rate

					ReadAdcData[ADC_Data_ptr++] = ptrframe[4];                 // Lead STATUS
					// LeadStaus = Rdbuf[4];                                   // Lead STATUS
					for (short LC = 0; LC < PACK_SAMPLES; LC++)                       // Decode received packet of 15 samples.
					{
						ReadAdcData[LC+ADC_Data_ptr] =  ptrframe[LC*3 + 7];
						//  if (ReadAdcData[LC+ADC_Data_ptr] > 127)
						//      ReadAdcData[LC+ADC_Data_ptr]=ReadAdcData[LC+ADC_Data_ptr] -256;
						ReadAdcData[LC+ADC_Data_ptr] = ReadAdcData[LC+ADC_Data_ptr] << 8;
						ReadAdcData[LC+ADC_Data_ptr] |=  ptrframe[LC * 3+ 6];              // Channel 0 ( Resp or Lead I)

						// if(LC%2 == 1)
						{
							data_ld[0] =  ReadAdcData[LC+ADC_Data_ptr];

							/* Note how we plot three points at once, one for each trace */
							switch (ptrframe[LC*3 + 5])
							{
										case  SAMPLE_IMP:
									//case  SAMPLE_PPG_R:
								case  SAMPLE_ECG:
									//	case  SAMPLE_ECG_HF:
									//	case  SAMPLE_PPG_GDC:
									//	data_ld[0] =  ReadAdcData[LC+ADC_Data_ptr];
									PlotStripChart (hpanel, PANEL_ECG_CHART, data_ld, 1, 0, 0, VAL_DOUBLE);
									break;

								case  SAMPLE_PPG_G:
									//data_ppg[2]  =  ReadAdcData[LC+ADC_Data_ptr];
									PlotStripChart (hpanel, PANEL_PPG1_CHART, data_ld, 1, 0, 0, VAL_DOUBLE);
									break;
								case  SAMPLE_PPG_IR:
									PlotStripChart (hpanel, PANEL_PPG1_CHART_2, data_ld, 1, 0, 0, VAL_DOUBLE);
									break;
								case  SAMPLE_PPG_R:
									PlotStripChart (hpanel, PANEL_PPG1_CHART_3, data_ld, 1, 0, 0, VAL_DOUBLE);
									break;
									//		case  SAMPLE_PPG_IR:
								case  SAMPLE_RESP:
									//	data_ld[0] =  ReadAdcData[LC+ADC_Data_ptr];
									PlotStripChart (hpanel, PANEL_RESP_CHART, data_ld, 1, 0, 0, VAL_DOUBLE);
									break;

							}



						}

					}
				}
			}

		}
		ProcessSystemEvents();
	}

  
	return 0;
}//DWORD WINAPI ThreadFunction(LPVOID iValue)
