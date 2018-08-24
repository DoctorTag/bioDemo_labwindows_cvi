/*------------------------------------------------------------------*/
/*                                                                  */
/*  FILE:       daqMT.c                                             */
/*                                                                  */
/*  PURPOSE:    This sample source illustrates how to use the       */
/*              multi-threading functions in the Utility library to */
/*              create a multi-threaded Data-Acquisition program.   */
/*                                                                  */
/*  NOTES:      You need to have CVI 5.5 or later.                  */
/*              You need to have a properly configured National     */
/*              Instrument's Data Acquisition board & software to   */
/*              acquire real data.                                  */
/*              If you do not have DAQ H/W you can run this         */
/*              sample using simulated data.                        */
/*				You must have the easyio DAQ library installed		*/
/*				to use this sample with real DAQ data. If you have	*/
/*				the easyio library installed, then change the		*/
/*				_HAVE_EASYIO_ flag to 1.							*/
/*                                                                  */
/*------------------------------------------------------------------*/

/*------------------------------------------------------------------*/
/*  Include Files                                                   */
/*------------------------------------------------------------------*/
#define _HAVE_EASYIO_ 0 /* indicates easyio library is installed */

/*------------------------------------------------------------------*/
/*  Include Files                                                   */
/*------------------------------------------------------------------*/
#include <cvirte.h>
#include <cvi2009compat.h>
#include <ansi_c.h>
#if _HAVE_EASYIO_
#include <easyio.h>
#endif
#include <formatio.h>
#include <analysis.h>
#include <utility.h>
#include <userint.h>
#include "daqMT.h"

/*------------------------------------------------------------------*/
/*  Constants                                                       */
/*------------------------------------------------------------------*/
#define     QUEUE_LENGTH                            2000
#define     ANALYSIS_LENGTH                         100
#define     SIMUL_INVALID_SAMPLERATE                -101
#define     SIMUL_ACQUISITION_NOT_CONFIGURED        -102
#define     SIMUL_OUT_OF_MEMORY_ERROR	            -103
#define     SIMUL_HALF_BUFFER_NOT_READY             -104
#define     SIMUL_BUFFER_OVERWRITTEN                -105
#define 	SIMUL_INVALID_BUFFER_ERROR				-106
#define     SIMUL_ITEMS_TO_SHIFT                    10

/*------------------------------------------------------------------*/
/*  Function Prototypes                                             */
/*------------------------------------------------------------------*/
int     CVICALLBACK DAQThreadFunction (void *functionData);
void    CVICALLBACK SingleThreadDAQFunc(void *callbackData);
void    CVICALLBACK AnalysisCB(CmtTSQHandle queueHandle, unsigned int event,
                                int value, void *callbackData);
void                DAQStop(void);
void                InitThreadSafeQueue(unsigned int sizeOfQueue);
void    CVICALLBACK MessageCB (void *callbackData);
void                UIRDimming(int flag);
short                 Simul_Clear(void);
short                 Simul_Start(unsigned long bufSize, double sampRate);
short                 Simul_Check(unsigned long *scanBackLog);
short                 Simul_Read(double halfBuf[], unsigned long *ptsTfr,
                                        short *stoppedFlag);

/*------------------------------------------------------------------*/
/*  File Static Variables                                           */
/*------------------------------------------------------------------*/
static      CmtTSQHandle        tsqHdl;
static      int                 status;
static      CmtThreadFunctionID daqThreadFunctionID;
static      int                 pnl;
static      char                daqMesg[600];
static      int                 helpHdl;
static      unsigned            iter;
static      double              timePrev;
static      double              timeCurr;
static      short               iDevice;
static      short               iChan;
static      unsigned int        ulCount;
static      short               iCyclesToRead;
static      double              dSampRate;
static		double				dSimulSampRate;
static      double              lastTime;
static      double              *pHalfBuffer, *pBuf;
static      unsigned int        ulSimulCount;

/*------------------------------------------------------------------*/
/*  Volatile Variables                                              */
/*------------------------------------------------------------------*/
volatile    int     singlethreadFlag        =   0;
volatile    int     fileFlag                =   0;
volatile    int     daqStopFlag             =   1;
volatile    int     dataSourceFlag          =   1;
volatile    short   simulStartFlag	        =   0;
volatile    short   simulHalfReadyFlag      =   0;

/*------------------------------------------------------------------*/
/*  Program Entry-point                                             */
/*------------------------------------------------------------------*/
int main (int argc, char *argv[])
{
    /* Initialize the CVI-RTE */
    if (InitCVIRTE (0, argv, 0) == 0)
        return -1;

    /* UI stuff */
    if ((pnl = LoadPanel (0, "daqMT.uir", PNL)) < 0)
        return -1;
    SetInputMode (pnl, PNL_STOP, 0);
    SetAxisScalingMode (pnl, PNL_GRAPH, VAL_XAXIS, VAL_MANUAL, 0, 99);
    SetAxisScalingMode (pnl, PNL_GRAPH, VAL_LEFT_YAXIS, VAL_MANUAL, -5.0, 5.0);
    SetGraphCursor (pnl, PNL_GRAPH, 1, 10, -4.0);
    SetGraphCursor (pnl, PNL_GRAPH, 2, 90, 4.0);
    SetGraphCursor (pnl, PNL_GRAPH, 3, 43, 0.5);
    SetActiveGraphCursor (pnl, PNL_GRAPH, 3);
    SetInputMode (pnl, PNL_DEVICE, _HAVE_EASYIO_);
    SetInputMode (pnl, PNL_CHANNEL, _HAVE_EASYIO_);
    SetInputMode (pnl, PNL_SOURCE, _HAVE_EASYIO_);
    DisplayPanel (pnl);

    /* Setup the thread safe queue */
    InitThreadSafeQueue(QUEUE_LENGTH);

    /* Start the UI Event handling loop */
    RunUserInterface ();
	
    DiscardPanel (pnl);
    if (pBuf != NULL)
    	free(pBuf);

    /* Destroy Thread-safe Queue */
    CmtDiscardTSQ (tsqHdl);

    return 0;
}

/*------------------------------------------------------------------*/
/*  Called by PostDeferredCallback in Single threaded case          */
/*------------------------------------------------------------------*/
void CVICALLBACK SingleThreadDAQFunc(void *callbackData)
{
    /* Call the DAQ function in the main (UI) thread */
    DAQThreadFunction(callbackData);
}

/*------------------------------------------------------------------*/
/*  Callback to start Data Acquisition                              */
/*------------------------------------------------------------------*/
int CVICALLBACK StartDAQCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    int             priority;
#if defined (_NI_mswin_) && _NI_mswin_
    int             thread_priority[6]  =   {
                                                THREAD_PRIORITY_TIME_CRITICAL,
                                                THREAD_PRIORITY_HIGHEST,
                                                THREAD_PRIORITY_ABOVE_NORMAL,
                                                THREAD_PRIORITY_NORMAL,
                                                THREAD_PRIORITY_BELOW_NORMAL,
                                                THREAD_PRIORITY_LOWEST
                                            };
#endif

    switch (event)
    {
        case EVENT_COMMIT:
            /* Get DAQ parameters from User interface */
            GetCtrlVal (pnl, PNL_DEVICE, &iDevice);
            GetCtrlVal (pnl, PNL_CHANNEL, &iChan);
            GetCtrlVal (pnl, PNL_SAMPRATE, &dSampRate);
            GetCtrlVal (pnl, PNL_BUFLEN, &ulCount);
            GetCtrlVal (pnl, PNL_LOOP, &iCyclesToRead);
            GetCtrlVal (pnl, PNL_THREAD, (int *)&singlethreadFlag);
            GetCtrlVal (pnl, PNL_SOURCE, (int *)&dataSourceFlag);
            GetCtrlVal (pnl, PNL_FILEFLAG, (int *)&fileFlag);
            GetCtrlVal (pnl, PNL_PRIORITY, &priority);

            /* Unset the Stop flag, setup chart*/
            daqStopFlag = 0;
            iter = 0;
            ClearStripChart (pnl, PNL_CHART);
            SetCtrlAttribute (pnl, PNL_CHART, ATTR_POINTS_PER_SCREEN, iCyclesToRead);
            SetCtrlVal (pnl, PNL_LED, 0);
            UIRDimming(0);

            /* Single-thread or Multi-thread execution */
            if (singlethreadFlag)
            {
                /* Post a call to the DAQ function to run on the main thread */
                PostDeferredCallToThread (SingleThreadDAQFunc, NULL, CmtGetMainThreadID ());
            }
            else
            {
                /* Start the Data Acquisition in a different thread */
                status = CmtScheduleThreadPoolFunctionAdv (
                            DEFAULT_THREAD_POOL_HANDLE, DAQThreadFunction, NULL,
#if defined (_NI_mswin_) && _NI_mswin_
                            thread_priority[priority], NULL, 0, NULL, 0,
#elif defined (_NI_linux_) && _NI_linux_
                            DEFAULT_THREAD_PRIORITY, NULL, 0, NULL, 0,
#endif
                            &daqThreadFunctionID);
            }
            timePrev = Timer();
            break;
    }
    return 0;
}

/*------------------------------------------------------------------*/
/*  ThreadSafe Queue's Callback that Plots the Data on the Graph    */
/*------------------------------------------------------------------*/
void CVICALLBACK AnalysisCB (CmtTSQHandle queueHandle, unsigned int event,
                           int value, void *callbackData)
{
    double          data[ANALYSIS_LENGTH];

    switch (event)
    {
        case EVENT_TSQ_ITEMS_IN_QUEUE:
            /* Read data from the thread safe queue and plot on the graph */
            while (value >= ANALYSIS_LENGTH)
            {
                timeCurr = Timer();
                CmtReadTSQData (tsqHdl, data, ANALYSIS_LENGTH, TSQ_INFINITE_TIMEOUT, 0);
                /* Plot the data. Add any Signal-processing/Analysis stuff here */
                DeleteGraphPlot (pnl, PNL_GRAPH, -1, VAL_DELAYED_DRAW);
                PlotY (pnl, PNL_GRAPH, data, ANALYSIS_LENGTH, VAL_DOUBLE, VAL_THIN_LINE,
                           VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
                value -= ANALYSIS_LENGTH;
                SetCtrlVal (pnl, PNL_PLOTNUM, iter++);
                PlotStripChartPoint (pnl, PNL_CHART, timeCurr-timePrev);
                timePrev = timeCurr;
            }
            break;
    }
}

/*------------------------------------------------------------------*/
/*  Callback to stop/interrupt the data acquisition                 */
/*------------------------------------------------------------------*/
int CVICALLBACK StopDAQCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    switch (event)
    {
        case EVENT_COMMIT:
            /* Stop the Data Acquisition if it is going on */
            DAQStop();
            /* Flush the thread-safe queue */
            CmtFlushTSQ (tsqHdl, TSQ_FLUSH_ALL, NULL);
            SetCtrlVal (pnl, PNL_PLOTNUM, 0);
            UIRDimming(1);
            break;
    }
    return 0;
}

/*------------------------------------------------------------------*/
/*  Callback that quits the program                                 */
/*------------------------------------------------------------------*/
int CVICALLBACK QuitCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    switch (event)
    {
        case EVENT_COMMIT:
            /* Stop the Data Acquisition if it is going on */
            DAQStop();
            /* Raise flag to return from RunUserInterface */
            QuitUserInterface (0);
            break;
    }
    return 0;
}

/*------------------------------------------------------------------*/
/*  Function called to stop the Data Acquisition & cleanup          */
/*------------------------------------------------------------------*/
void DAQStop(void)
{
    int daqThreadFunctionStatus;

    /* Check if the Stop flag is already up */
    if (!daqStopFlag)
    {
        /* Raise the Stop flag */
        daqStopFlag = 1;
        /* In multi-threaded case wait for the DAQ Thread Function to complete
           and then release its ID */
        if (!singlethreadFlag)
        {
            CmtGetThreadPoolFunctionAttribute (DEFAULT_THREAD_POOL_HANDLE,
                                               daqThreadFunctionID,
                                               ATTR_TP_FUNCTION_EXECUTION_STATUS,
                                               &daqThreadFunctionStatus);
            if (daqThreadFunctionStatus != kCmtThreadFunctionComplete)
                CmtWaitForThreadPoolFunctionCompletion (DEFAULT_THREAD_POOL_HANDLE,
                                                        daqThreadFunctionID, 1);
            CmtReleaseThreadPoolFunctionID (DEFAULT_THREAD_POOL_HANDLE,
                                                daqThreadFunctionID);
            }
        }
}

/*------------------------------------------------------------------*/
/*  Function to initialize the Thread Safe Queue                    */
/*------------------------------------------------------------------*/
void InitThreadSafeQueue(unsigned int sizeOfQueue)
{
    /* Create Thread-safe Queue */
    CmtNewTSQ (sizeOfQueue, sizeof(double), OPT_TSQ_DYNAMIC_SIZE, &tsqHdl);

    /* AnalysisCB will be called when ANALYSIS_LENGTH items are in the Queue */
    CmtInstallTSQCallback (tsqHdl, EVENT_TSQ_ITEMS_IN_QUEUE, ANALYSIS_LENGTH,
                           AnalysisCB, NULL, CmtGetCurrentThreadID(),
                           NULL);
}

/*------------------------------------------------------------------*/
/*  Calls the DAQ functions. Thread function in multi-thread case.  */
/*------------------------------------------------------------------*/
int CVICALLBACK DAQThreadFunction (void *functionData)
{
    int     		mainThreadID;
    int     		stat;
    int     		fileHdl = -1;
    char    		filePath[600];
    short     		iStatus                     =   0;
    short     		iLoopCount                  =   0;
    double     		*pdVoltBuffer               =   NULL;
    short     		iDAQstopped                 =   0;
    unsigned long	ulPtsTfr                    =   0;
    unsigned long 	scanBkLog;

    /* Gets the main thread's ID to post messages to it */
    mainThreadID = CmtGetMainThreadID ();

    /* Allocating memory for the DAQ buffer */
    if ((pdVoltBuffer = (double*)malloc(ulCount*sizeof(double)))==NULL)
        return -1;

    /* Opening file when data-logging is enabled */
    if (fileFlag)
    {
        stat = FileSelectPopupEx ("", "*.dat", "",
                                "Open a binary data file", VAL_OK_BUTTON,
                                0, 0, filePath);
        if (stat > 0)
        {
            fileHdl = OpenFile (filePath, VAL_WRITE_ONLY, VAL_OPEN_AS_IS, VAL_BINARY);
            if (fileHdl == -1)
                return -1;
        }
        else
        {
            fileFlag = 0;
            return -1;
        }
    }

#if _HAVE_EASYIO_
    if (dataSourceFlag) /* Data from Data Acquisition Hardware */
    {
        /* Format the Channel String from channel number */
        Fmt(chanStr, "%s<%d", iChan);
        /* Acquire data indefinitely into circular buffer from a single channel */
		iStatus = AIStartAcquisition (iDevice, chanStr,  ulCount, dSampRate, 0.0,
									  0.0, &dActScanRate, &taskId);
        if (iStatus)
        {
            Fmt (daqMesg, "%s<DAQ Error: %d[b2]", iStatus);
            PostDeferredCallToThread (MessageCB, daqMesg, mainThreadID);
        }
    }
    else /* Simulated Data */
#endif
    {
        iStatus = Simul_Start(ulCount, dSampRate);
        if (iStatus)
        {
            Fmt (daqMesg, "%s<Simulation Error: %d[b2]", iStatus);
            PostDeferredCallToThread (MessageCB, daqMesg, mainThreadID);
        }
    }

    /* Main loop that reads data from the double buffer */
    while ((iLoopCount < iCyclesToRead) && (iStatus == 0) && (!daqStopFlag))
    {
        /* Checks the backlog in the buffer */
#if _HAVE_EASYIO_
        if (dataSourceFlag)
			iStatus = AICheckAcquisition (taskId, &scanBkLog);
        else
#endif
            iStatus = Simul_Check (&scanBkLog);

        if ((scanBkLog >= ulCount/2) && (iStatus == 0))
        {
            /* Transfers data from the double-buffer */
#if _HAVE_EASYIO_
            if (dataSourceFlag) {
            	ulPtsTfr = ulCount/2;
				iStatus = AIReadAcquisition (taskId, ulPtsTfr, CONSECUTIVE_MODE,
											 &scanBkLog, GROUP_BY_SCAN, pdVoltBuffer);
			}
            else
#endif
                iStatus = Simul_Read(pdVoltBuffer, &ulPtsTfr, &iDAQstopped);

            /* Write to binary file if data-logging is enabled */
            if(fileFlag)
                WriteFile (fileHdl, (char *)pdVoltBuffer, sizeof(pdVoltBuffer[0])*ulPtsTfr);

            /* Increment loop count */
            iLoopCount++;
            Fmt (daqMesg, "%s<# Half Buffers: %d[b2]", iLoopCount);
            PostDeferredCallToThread (MessageCB, daqMesg, mainThreadID);

            /* Write upto ANALYSIS_LENGTH data points to the Thread Safe Queue
               for analysis by the UI thread (in the AnalysisCB function) */
            CmtWriteTSQData (tsqHdl, pdVoltBuffer,
                            ulPtsTfr > ANALYSIS_LENGTH ? ANALYSIS_LENGTH : ulPtsTfr,
                            TSQ_INFINITE_TIMEOUT, NULL);
            if (!dataSourceFlag) ProcessSystemEvents();
        }
        /* In the Single-threaded case, Process Events: for analysis, timer callback, etc */
        if (singlethreadFlag)
            ProcessSystemEvents();
    }

    if (iStatus)
    {
        Fmt (daqMesg, "%s<Error:  %d[b2]", iStatus);
        PostDeferredCallToThread (MessageCB, daqMesg, mainThreadID);
    }
    else
    {
        Fmt (daqMesg, "%s<Acquisition done");
        PostDeferredCallToThread (MessageCB, daqMesg, mainThreadID);
    }

    /* Stop and Clear the acquisition */
#if _HAVE_EASYIO_
    if (dataSourceFlag)
		AIClearAcquisition (taskId);
    else
#endif
        Simul_Clear();

    /* Close binary file */
    if (fileFlag)
        CloseFile (fileHdl);

    /* Free the daq buffer */
    free(pdVoltBuffer);

    return 0;
}

/*------------------------------------------------------------------*/
/*  Callback that restores the graph to original zoom-size          */
/*------------------------------------------------------------------*/
int CVICALLBACK RestoreGraph (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    switch (event)
    {
        case EVENT_COMMIT:
            SetAxisScalingMode (pnl, PNL_GRAPH, VAL_XAXIS, VAL_MANUAL, 0, 99);
            SetAxisScalingMode (pnl, PNL_GRAPH, VAL_LEFT_YAXIS, VAL_MANUAL, -5.0, 5.0);
            break;
    }
    return 0;
}

void swap(double *a, double *b)
{
    double temp;

    temp = *a;
    *a = *b;
    *b = temp;
}

/*------------------------------------------------------------------*/
/*  Callback that Zooms into the graph                              */
/*------------------------------------------------------------------*/
int CVICALLBACK ZoomCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    double xMin, xMax, yMin, yMax;

    switch (event)
    {
        case EVENT_COMMIT:
            GetGraphCursor (pnl, PNL_GRAPH, 1, &xMin, &yMin);
            GetGraphCursor (pnl, PNL_GRAPH, 2, &xMax, &yMax);
            if (xMin > xMax)    swap(&xMin, &xMax);
            if (yMin > yMax)    swap(&yMin, &yMax);
            if (xMin == xMax) xMax += 0.001;
            if (yMin == yMax) yMax += 0.001;
            SetAxisScalingMode (pnl, PNL_GRAPH, VAL_XAXIS, VAL_MANUAL, xMin, xMax);
            SetAxisScalingMode (pnl, PNL_GRAPH, VAL_LEFT_YAXIS, VAL_MANUAL, yMin, yMax);
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
        SetCtrlVal (pnl, PNL_MESG, (char *)mesg);
        if (FindPattern ((char *)mesg, 0, strlen((char *)mesg), "Error", 0, 0) != -1)
        {   /* Error occurred */
            SetCtrlVal (pnl, PNL_LED, 1);
            StopDAQCB (pnl, 0, EVENT_COMMIT, 0, 0, 0);
        }
        else if (FindPattern ((char *)mesg, 0, strlen((char *)mesg), "done", 0, 0) != -1)
            StopDAQCB (pnl, 0, EVENT_COMMIT, 0, 0, 0);
    }
}

/*------------------------------------------------------------------*/
/*  Callback that displays Help for the program                     */
/*------------------------------------------------------------------*/
int CVICALLBACK HelpCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    switch (event)
    {
        case EVENT_COMMIT:
            if ((helpHdl = LoadPanel (0, "daqMT.uir", HPANEL)) < 0)
            {
                MessagePopup ("Error", "Help could not be launched.");
                return 0;
            }
            DisplayPanel (helpHdl);
            break;
    }
    return 0;
}

/*------------------------------------------------------------------*/
/*  Callback that discards the Help Window                          */
/*------------------------------------------------------------------*/
int CVICALLBACK CloseHelpCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    switch (event)
    {
        case EVENT_COMMIT:
            DiscardPanel (helpHdl);
            break;
    }
    return 0;
}

/*------------------------------------------------------------------*/
/*  Function that does the UIR Dimming                              */
/*------------------------------------------------------------------*/
void UIRDimming(int flag)
{
    SetInputMode (pnl, PNL_SAMPRATE, flag);
    SetInputMode (pnl, PNL_BUFLEN, flag);
    SetInputMode (pnl, PNL_LOOP, flag);
    SetInputMode (pnl, PNL_THREAD, flag);
#if _HAVE_EASYIO_
    SetInputMode (pnl, PNL_DEVICE, flag);
    SetInputMode (pnl, PNL_CHANNEL, flag);
    SetInputMode (pnl, PNL_SOURCE, flag);
#endif
    SetInputMode (pnl, PNL_FILEFLAG, flag);
    SetInputMode (pnl, PNL_PRIORITY, flag);
    SetInputMode (pnl, PNL_START, flag);
    SetInputMode (pnl, PNL_STOP, !flag);
}

/*------------------------------------------------------------------*/
/*  Simulation Function that clears the continuous acquisition 		*/
/*------------------------------------------------------------------*/
short Simul_Clear(void)
{
    simulStartFlag = 0;
    pHalfBuffer = NULL;
    return 0;
}

/*------------------------------------------------------------------*/
/*  Simulation Function that starts the acquisition                 */
/*------------------------------------------------------------------*/
short Simul_Start(unsigned long bufSize, double sampRate)
{
    int i;

    if (pBuf != NULL)
    	free(pBuf);

    if (simulStartFlag)
        return SIMUL_ACQUISITION_NOT_CONFIGURED;
    else
    {
        if (sampRate <= 0.0)
        	return SIMUL_INVALID_SAMPLERATE;
    	else
    		dSimulSampRate = sampRate;
    	simulHalfReadyFlag = 0;
    	ulSimulCount = bufSize;
        pBuf = (double *) malloc(sizeof(double) * bufSize);
        if (pBuf != NULL)
        {
            simulStartFlag = 1;
            for (i=0; i<bufSize; ++i)
                pBuf[i] = (double) (rand() * 5.0/(double)RAND_MAX);
            pHalfBuffer = pBuf;
        }
        else
            return SIMUL_OUT_OF_MEMORY_ERROR;
        lastTime = Timer();
        return 0;
    }
}

/*------------------------------------------------------------------*/
/*  Simulation Function that returns the scan back log	            */
/*------------------------------------------------------------------*/
short Simul_Check(unsigned long *scanBkLog)
{
    double          currTime;
    unsigned long   numSamples;

    currTime = Timer();
    numSamples = (currTime - lastTime) * dSimulSampRate;
    *scanBkLog = numSamples;
    if (numSamples > ulSimulCount){
        return SIMUL_BUFFER_OVERWRITTEN;
    } else if (numSamples < ulSimulCount/2){
        simulHalfReadyFlag = 0;
    } else {
    	simulHalfReadyFlag = 1;
    }
    return 0;
}

/*------------------------------------------------------------------*/
/*  Simulation Function that transfers the half buffer              */
/*------------------------------------------------------------------*/
short Simul_Read(double halfBuf[], unsigned long *ptsTfr, short *stoppedFlag)
{
    double shiftBuf[SIMUL_ITEMS_TO_SHIFT];

    if (simulHalfReadyFlag == 0)  {
        return SIMUL_HALF_BUFFER_NOT_READY;
    }

    if (pHalfBuffer != NULL)
    {
        memcpy (halfBuf, pHalfBuffer, sizeof(double) * ulSimulCount/2);
        /* Shift the data so that the plot changes noticeably */
        memcpy (shiftBuf, pHalfBuffer + (ulSimulCount - SIMUL_ITEMS_TO_SHIFT),
                sizeof (shiftBuf));
        memmove (pHalfBuffer + SIMUL_ITEMS_TO_SHIFT, pHalfBuffer,
                 (ulSimulCount * sizeof (pHalfBuffer[0])) - sizeof (shiftBuf));
        memcpy (pHalfBuffer, shiftBuf, sizeof (shiftBuf));
    }
    else
        return SIMUL_INVALID_BUFFER_ERROR;
    simulHalfReadyFlag = 0;
    *stoppedFlag = 0;
    *ptsTfr = ulCount/2;
    lastTime = Timer();
    return 0;
}

