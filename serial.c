/********************************************************************************************/
/********************************************************************************************/
/* File:    InstallCommCallback.c                                                           */
/* Purpose: This example illustrates the use of the RS232 Library's InstallComCallback      */
/*          function to respond to the LWRS_RXFLAG event. The user will be asked to enter   */
/*          an event character. Whenever this event character is detected on the COM port,  */
/*          a callback function will be executed.                                           */
/* Notes:   To run this example you need to do a Loopback Test, i.e. connect pin 2 to       */
/*          pin 3 on the serial port.                                                       */
/*          The example uses port 1 by default. To use another port, change the COM_PORT    */
/*          defined value.                                                                  */
/********************************************************************************************/
/********************************************************************************************/

/********************************************************************************************/
/* Include files                                                                            */
/********************************************************************************************/
#include <utility.h>
#include <rs232.h>
#include <ansi_c.h>
//#include <cvirte.h>
//#include <userint.h>
#include "serial.h"
#include <formatio.h>

/********************************************************************************************/
/* Constants                                                                                    */
/********************************************************************************************/
//#define COM_PORT    35
//int com_port;
/********************************************************************************************/
/* Globals                                                                                  */
/********************************************************************************************/


/********************************************************************************************/
/* Prototypes                                                                               */
/********************************************************************************************/
void CVICALLBACK Event_Char_Detect_Func (int portNo,int eventMask,void *callbackData);

/********************************************************************************************/
/* Application entry point.                                                                 */
/********************************************************************************************/
int Init_ComPort (int com_port)
{
	int RS232Error  ;
	DisableBreakOnLibraryErrors ();
	/*  Open and Configure Com port */
	RS232Error = OpenComConfig (com_port, "", 19200, 0, 8, 1, 512, 512);
	EnableBreakOnLibraryErrors ();

	if (RS232Error == 0)
	{ 
		/*  Turn off Hardware handshaking (loopback test will not function with it on) */
		SetCTSMode (com_port, LWRS_HWHANDSHAKE_OFF);

		/*  Make sure Serial buffers are empty */
		FlushInQ (com_port);
		FlushOutQ (com_port);
	}
 
	/*  Install a callback such that if the event character appears at the
		receive buffer, our function will be notified.  */
//  InstallComCallback (COM_PORT, LWRS_RXFLAG, 0, (int)gEventChar[0] , Event_Char_Detect_Func, 0);

	return RS232Error;
}

/********************************************************************************************/
/* SendData ():  Respond to the Send button to send characters out of the serial port.      */
/********************************************************************************************/
int SendData (int com_port,unsigned char *data,int dlen)
{
	if(com_port < 1)
		 return 0;
	FlushInQ (com_port);
	ComWrt (com_port, data, dlen);
	return dlen;
}

/********************************************************************************************/
/* Event_Char_Detect_Func ():  Fuction called when the event character is detected.         */
/********************************************************************************************/
void CVICALLBACK Event_Char_Detect_Func (int portNo,int eventMask,void *callbackData)
{
	return;
}



/********************************************************************************************/
/* ReceiveData ():  Read the data on the serial COM port.                                   */
/********************************************************************************************/
int  ReceiveData (int com_port,unsigned char *buf,int buflen)
{
	int     strLen;

	 if(com_port < 1)
		 return 0;
	/*  Read the characters from the port */
	strLen = GetInQLen (com_port);
	if(strLen > buflen)
		strLen = buflen;
	ComRd (com_port, buf, strLen);

	return strLen;
}

int ShutDownCom (int com_port)
{  
	return  CloseCom(com_port);
}
