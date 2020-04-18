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
#define  PANEL_REG_READ                   2       /* control type: command, callback function: Reg_ReadCb */
#define  PANEL_REG_WRITE                  3       /* control type: command, callback function: Reg_WriteCb */
#define  PANEL_QUIT                       4       /* control type: command, callback function: Shutdown */
#define  PANEL_REG_INFO                   5       /* control type: numeric, callback function: (none) */
#define  PANEL_DECORATION_6               6       /* control type: deco, callback function: (none) */
#define  PANEL_REG_TABLE                  7       /* control type: table, callback function: RegTableCb */
#define  PANEL_DECORATION                 8       /* control type: deco, callback function: (none) */
#define  PANEL_TEXT_REG_MAP               9       /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXT_FUN_TEXT              10      /* control type: textMsg, callback function: (none) */
#define  PANEL_LISTBOX_FUNCTION           11      /* control type: listBox, callback function: (none) */
#define  PANEL_CMDBUTTON_TEST             12      /* control type: command, callback function: FunctionTestBeginCb */
#define  PANEL_BIOSENSOR_SWITCH           13      /* control type: binary, callback function: BioSensorOnCb */

#define  PANEL_CFG                        2
#define  PANEL_CFG_TCP_PORT               2       /* control type: numeric, callback function: (none) */
#define  PANEL_CFG_COM                    3       /* control type: numeric, callback function: (none) */
#define  PANEL_CFG_IP                     4       /* control type: string, callback function: (none) */
#define  PANEL_CFG_QUIT                   5       /* control type: command, callback function: cfgQuitCb */
#define  PANEL_CFG_OK                     6       /* control type: command, callback function: cfgOkCallback */
#define  PANEL_CFG_TCP_CHKBOX             7       /* control type: radioButton, callback function: (none) */
#define  PANEL_CFG_UART_CHKBOX            8       /* control type: radioButton, callback function: (none) */
#define  PANEL_CFG_DECORATION_2           9       /* control type: deco, callback function: (none) */
#define  PANEL_CFG_DECORATION             10      /* control type: deco, callback function: (none) */

#define  PANEL_ECGD                       3
#define  PANEL_ECGD_PLOT                  2       /* control type: textButton, callback function: ECG_D_StartCb */
#define  PANEL_ECGD_CHART_D               3       /* control type: strip, callback function: (none) */
#define  PANEL_ECGD_QUITECGD              4       /* control type: command, callback function: Quit_ECG_D_Cb */
#define  PANEL_ECGD_LOST_IND              5       /* control type: numeric, callback function: (none) */
#define  PANEL_ECGD_CRC_ERROR_IND         6       /* control type: numeric, callback function: (none) */
#define  PANEL_ECGD_TIMER                 7       /* control type: timer, callback function: ecgTimerCallback */

#define  PANEL_FWUP                       4
#define  PANEL_FWUP_FWUP_QUIT             2       /* control type: command, callback function: FWUP_QuitCb */
#define  PANEL_FWUP_LOAD_FILE             3       /* control type: command, callback function: LoadBinFileCb */
#define  PANEL_FWUP_PROGRESSBAR           4       /* control type: scale, callback function: (none) */
#define  PANEL_FWUP_TEXTMSG_2             5       /* control type: textMsg, callback function: (none) */
#define  PANEL_FWUP_FW_UPGRADE            6       /* control type: command, callback function: (none) */
#define  PANEL_FWUP_BIN_PATH              7       /* control type: textMsg, callback function: (none) */
#define  PANEL_FWUP_FW_SW                 8       /* control type: binary, callback function: FW_SWCb */

#define  PANEL_LA                         5
#define  PANEL_LA_START                   2       /* control type: textButton, callback function: LAStartCb */
#define  PANEL_LA_LA_QUIT                 3       /* control type: command, callback function: LA_QuitCb */
#define  PANEL_LA_CHART_RESULT            4       /* control type: strip, callback function: (none) */
#define  PANEL_LA_CHART_DATA              5       /* control type: strip, callback function: (none) */
#define  PANEL_LA_RESP_IND                6       /* control type: numeric, callback function: (none) */
#define  PANEL_LA_HR_IND                  7       /* control type: numeric, callback function: (none) */
#define  PANEL_LA_LA_LOAD_FILE            8       /* control type: command, callback function: LALoadFileCb */
#define  PANEL_LA_RUN_IND                 9       /* control type: string, callback function: (none) */
#define  PANEL_LA_BODY_STATUS             10      /* control type: string, callback function: (none) */
#define  PANEL_LA_LED_RUN                 11      /* control type: LED, callback function: (none) */
#define  PANEL_LA_LED_ANA                 12      /* control type: LED, callback function: (none) */
#define  PANEL_LA_RESULT_PLOT             13      /* control type: ring, callback function: (none) */

#define  PANEL_PPG                        6
#define  PANEL_PPG_PLOT                   2       /* control type: textButton, callback function: PPG_StartCb */
#define  PANEL_PPG_CHART_DC               3       /* control type: strip, callback function: (none) */
#define  PANEL_PPG_CHART_AC               4       /* control type: strip, callback function: (none) */
#define  PANEL_PPG_QUITPPG                5       /* control type: command, callback function: Quit_PPG_Cb */
#define  PANEL_PPG_CRC_ERROR_IND          6       /* control type: numeric, callback function: (none) */
#define  PANEL_PPG_LOST_IND               7       /* control type: numeric, callback function: (none) */
#define  PANEL_PPG_TIMER                  8       /* control type: timer, callback function: ppgTimerCallback */

#define  PANEL_PZ                         7
#define  PANEL_PZ_PLOTPIEZO               2       /* control type: textButton, callback function: Piezo_StartCb */
#define  PANEL_PZ_CHART_ANALYSIS          3       /* control type: strip, callback function: (none) */
#define  PANEL_PZ_CHART_RAW               4       /* control type: strip, callback function: (none) */
#define  PANEL_PZ_LOST_IND                5       /* control type: numeric, callback function: (none) */
#define  PANEL_PZ_RESP_IND                6       /* control type: numeric, callback function: (none) */
#define  PANEL_PZ_HR_IND                  7       /* control type: numeric, callback function: (none) */
#define  PANEL_PZ_CRC_ERROR_IND           8       /* control type: numeric, callback function: (none) */
#define  PANEL_PZ_QUITPIEZO               9       /* control type: command, callback function: Quit_Piezo_Cb */
#define  PANEL_PZ_TIMER                   10      /* control type: timer, callback function: piezoTimerCallback */
#define  PANEL_PZ_LOCAL_SAVE_CHK          11      /* control type: radioButton, callback function: (none) */
#define  PANEL_PZ_RESULT_PLOT             12      /* control type: ring, callback function: (none) */
#define  PANEL_PZ_BODY_STATUS             13      /* control type: string, callback function: (none) */
#define  PANEL_PZ_LED_BED                 14      /* control type: LED, callback function: (none) */
#define  PANEL_PZ_LED                     15      /* control type: LED, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  MENUBAR                          1
#define  MENUBAR_MENU_COM                 2       /* callback function: MenuConfigPortCb */
#define  MENUBAR_MENU_FM_UPGRADE          3       /* callback function: MenuFirmUpgradeCb */
#define  MENUBAR_MENU_LOCAL_ANALYSIS      4       /* callback function: MenuLocalAnalysisCb */


     /* Callback Prototypes: */

int  CVICALLBACK BioSensorOnCb(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cfgOkCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cfgQuitCb(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ECG_D_StartCb(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ecgTimerCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK FunctionTestBeginCb(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK FW_SWCb(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK FWUP_QuitCb(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LA_QuitCb(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LALoadFileCb(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LAStartCb(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LoadBinFileCb(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK MenuConfigPortCb(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MenuFirmUpgradeCb(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MenuLocalAnalysisCb(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK PanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Piezo_StartCb(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK piezoTimerCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PPG_StartCb(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ppgTimerCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Quit_ECG_D_Cb(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Quit_Piezo_Cb(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Quit_PPG_Cb(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Reg_ReadCb(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Reg_WriteCb(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RegTableCb(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Shutdown(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
