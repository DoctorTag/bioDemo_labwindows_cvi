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

#define  PANEL_FWUP                       2
#define  PANEL_FWUP_FWUP_QUIT             2       /* control type: command, callback function: FWUP_QuitCb */
#define  PANEL_FWUP_LOAD_FILE             3       /* control type: command, callback function: LoadBinFileCb */
#define  PANEL_FWUP_PROGRESSBAR           4       /* control type: scale, callback function: (none) */
#define  PANEL_FWUP_TEXTMSG_2             5       /* control type: textMsg, callback function: (none) */
#define  PANEL_FWUP_FW_UPGRADE            6       /* control type: command, callback function: FWUpgradeCb */
#define  PANEL_FWUP_BIN_PATH              7       /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  MENUBAR                          1
#define  MENUBAR_MENU1                    2
#define  MENUBAR_MENU1_ITEM1              3       /* callback function: ConfigMenuCallback */
#define  MENUBAR_MENU1_ITEM2              4       /* callback function: ConfigMenuCallback */


     /* Callback Prototypes: */

void CVICALLBACK ConfigMenuCallback(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK FWUP_QuitCb(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK FWUpgradeCb(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LoadBinFileCb(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PauseChart(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PlotData(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Shutdown(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
