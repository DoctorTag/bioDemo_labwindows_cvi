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

#define  HPANEL                           1
#define  HPANEL_TEXTBOX                   2       /* control type: textBox, callback function: (none) */
#define  HPANEL_CHELP                     3       /* control type: command, callback function: CloseHelpCB */

#define  PNL                              2
#define  PNL_GRAPH                        2       /* control type: graph, callback function: (none) */
#define  PNL_SAMPRATE                     3       /* control type: numeric, callback function: (none) */
#define  PNL_CHANNEL                      4       /* control type: numeric, callback function: (none) */
#define  PNL_BUFLEN                       5       /* control type: numeric, callback function: (none) */
#define  PNL_LOOP                         6       /* control type: numeric, callback function: (none) */
#define  PNL_DEVICE                       7       /* control type: numeric, callback function: (none) */
#define  PNL_SOURCE                       8       /* control type: binary, callback function: (none) */
#define  PNL_THREAD                       9       /* control type: binary, callback function: (none) */
#define  PNL_FILEFLAG                     10      /* control type: binary, callback function: (none) */
#define  PNL_PRIORITY                     11      /* control type: ring, callback function: (none) */
#define  PNL_ZOOM                         12      /* control type: command, callback function: ZoomCB */
#define  PNL_RESTORE                      13      /* control type: command, callback function: RestoreGraph */
#define  PNL_MESG                         14      /* control type: string, callback function: (none) */
#define  PNL_LED                          15      /* control type: LED, callback function: (none) */
#define  PNL_QUIT                         16      /* control type: command, callback function: QuitCB */
#define  PNL_HELP                         17      /* control type: command, callback function: HelpCB */
#define  PNL_STOP                         18      /* control type: command, callback function: StopDAQCB */
#define  PNL_START                        19      /* control type: command, callback function: StartDAQCB */
#define  PNL_PLOTNUM                      20      /* control type: numeric, callback function: (none) */
#define  PNL_DECORATION                   21      /* control type: deco, callback function: (none) */
#define  PNL_CHART                        22      /* control type: strip, callback function: (none) */
#define  PNL_TEXTMSG_2                    23      /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK CloseHelpCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK HelpCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QuitCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RestoreGraph(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK StartDAQCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK StopDAQCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ZoomCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
