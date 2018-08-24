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



 int Init_ComPort (void); 

int SendData (unsigned char *data,int dlen);
    
int  ReceiveData (unsigned char *buf,int buflen);

int ShutDownCom (void);         

#ifdef __cplusplus
    }
#endif
