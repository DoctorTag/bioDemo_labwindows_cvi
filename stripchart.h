/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1       /* callback function: PanelCB */
#define  PANEL_PLOT                       2       /* control type: textButton, callback function: PlotData */
#define  PANEL_PAUSE                      3       /* control type: textButton, callback function: PauseChart */
#define  PANEL_QUIT                       4       /* control type: command, callback function: Shutdown */
#define  PANEL_RESP_CHART                 5       /* control type: strip, callback function: (none) */
#define  PANEL_PPG1_CHART_3               6       /* control type: strip, callback function: (none) */
#define  PANEL_PPG1_CHART_2               7       /* control type: strip, callback function: (none) */
#define  PANEL_PPG1_CHART                 8       /* control type: strip, callback function: (none) */
#define  PANEL_ECG_CHART                  9       /* control type: strip, callback function: (none) */
#define  PANEL_Ind1                       10      /* control type: numeric, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK PanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PauseChart(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PlotData(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Shutdown(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
