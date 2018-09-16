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

#ifndef _FWUP_CB_H_
#define _FWUP_CB_H_


#ifdef __cplusplus
    extern "C" {
#endif
		
		typedef enum tBIO_SENSOR_STATE {
	
	BIO_NORMAL =0,
	BIO_BOOTL,
	BIO_LOSE	
}BIO_SENSOR_STATE;

int CVICALLBACK LoadBinFile (int panel, int control, int event, void *callbackData, int eventData1, int eventData2) ;


#ifdef __cplusplus
    }
#endif


#endif
