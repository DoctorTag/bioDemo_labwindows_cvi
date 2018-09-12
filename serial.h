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



 int Init_ComPort (int com_port); 

int SendData (int com_port,unsigned char *data,int dlen);
    
int  ReceiveData (int com_port,unsigned char *buf,int buflen);

int ShutDownCom (int com_port);         

#ifdef __cplusplus
    }
#endif
