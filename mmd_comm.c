/*******************************************************************************************************
 *
 *
 *
 *
 *
 * *****************************************************************************************************/

//#include "bsp.h"
//#include "DSP_Filter.h"
#include "mmd_comm.h"



#define MAX_STR_LENGTH 64


unsigned char MMDTSTRxPacket[64] ;



unsigned char rstate;
unsigned short offset;



/*****************************************************************************************

*****************************************************************************************/
#define  RX_HEADER           0x00
#define  RX_CMD         0x01
//#define  RX_CODE        0x02
#define  RX_DATA           0x03
#define  RX_END            0x04

void RecvInit(void)
{
    rstate = RX_HEADER;
    offset = 0;

}
unsigned char *  RecvFrame( unsigned char src)
{
    	unsigned char *retvalue = (void*)0;
	MMDTSTRxPacket[ offset] = src ;
    offset ++;


    switch(rstate)
    {
        case RX_HEADER:
            if(MMDTSTRxPacket[offset - 1] == START_DATA_HEADER)
            {
                rstate = RX_CMD;

            }
            else
            {
                RecvInit();
            }

            break;
        case RX_CMD:
            if((0x90 & MMDTSTRxPacket[offset - 1]) == 0x90)
            {
                rstate = RX_DATA;

            }
            else
            {
                RecvInit();
            }
            break;
        case RX_DATA:
            if(MMDTSTRxPacket[offset - 1] == END_DATA_HEADER)
            {
                rstate = RX_END;

            }
            if(offset >= MAX_STR_LENGTH )
            {
                RecvInit();
            }
            break;
        case RX_END:
            if(MMDTSTRxPacket[offset - 1] == 0x0a)
            {
  //              MMDTST_Recoder_state.command = MMDTSTRxPacket[1];
                //MMDTST_Recoder_state.command = readBytes;
      //          Decode_Recieved_Command();
	  retvalue = MMDTSTRxPacket; 
                RecvInit();
            }
            else
            {
                if(offset >= MAX_STR_LENGTH)
                {
                    RecvInit();
                }
                else if(MMDTSTRxPacket[offset - 1] != END_DATA_HEADER)
                    rstate = RX_DATA;

            }
            break;
    }



      return retvalue;
}




