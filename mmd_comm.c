/*******************************************************************************************************
 *
 *
 *
 *
 *
 * *****************************************************************************************************/

#include <ansi_c.h>
#include <utility.h>
#include "serial.h"
#include "mmd_comm.h"



#define MAX_RDATA_LENGTH 48
#define MAX_RX_LENGTH 54


unsigned char MMDTSTRxPacket[MAX_RX_LENGTH] ;



unsigned char rstate,rlength;
unsigned short offset,rcrc,lcrc;



/*****************************************************************************************

*****************************************************************************************/
#define  RX_HEADER           0x00
#define  RX_CMD         0x01
#define  RX_LENGTH        0x02
#define  RX_DATA           0x03
#define  RX_CRC            0x04
#define  RX_END            0x05

#define  CMD_MASK            0xF0 
#define  CMD_HIGH			 0x80



void RecvInit(void)
{
	rstate = RX_HEADER;
	offset = 0;

}
unsigned char *  RecvFrame( unsigned char src,unsigned char *crc_error)
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
			if((CMD_MASK & MMDTSTRxPacket[offset - 1]) == CMD_HIGH)
			{
				rstate = RX_LENGTH;

			}
			else
			{
				RecvInit();
			}
			break;
			 case RX_LENGTH:
            rlength = MMDTSTRxPacket[offset-1];
            if(rlength <= MAX_RDATA_LENGTH)
            {
                rstate = RX_DATA;
                lcrc = 0;
            }
            else
            {
                RecvInit();
            }
            break;
		 case RX_DATA:
            if(offset >= (rlength+3))
            {
                rstate = RX_CRC;

            }
            lcrc += MMDTSTRxPacket[offset-1];
            break;
        case RX_CRC:
            if(offset == (rlength+4))
            {
                rcrc = MMDTSTRxPacket[offset-1];
                rcrc <<=8;
            }

            if(offset == (rlength+5))
            {
                rcrc |= MMDTSTRxPacket[offset-1];
                rstate = RX_END;

            }
            if(offset > (rlength+5) )
            {
                RecvInit();
            }
            break;
		case RX_END:
			if(MMDTSTRxPacket[offset - 1] == 0x0a)
			{
				(*crc_error) = (rcrc != lcrc);
				retvalue = MMDTSTRxPacket;
				RecvInit();
			}
			else
			{
				 if(offset >  (rlength+6))
                {
                    RecvInit();
                }

				

			}
			break;
	}



	return retvalue;
}

		
			
void sendCMD(int com_port,unsigned char cmd,unsigned char dtype0,unsigned char dtype)
{
	static  unsigned char Wrbuf[8];
	unsigned short tcrc = 0;
	if(com_port < 1)
		return;

	tcrc += dtype0;
	tcrc += dtype;
	Wrbuf[0] = START_DATA_HEADER;           // Start Header
	Wrbuf[1] = cmd;      //  command
	Wrbuf[2] = 2;      //  length
	Wrbuf[3] = dtype0;                           // Start command
	Wrbuf[4] = dtype;

	Wrbuf[5] = (unsigned char)(tcrc >> 8);
	Wrbuf[6] = (unsigned char)tcrc;
	Wrbuf[7] = '\n';
	SendData(com_port,Wrbuf, 8);              // Send command to firmware
}


unsigned char Rdbuf[8];
unsigned char *receiveSyncRspFrame(int com_port,int timeout,unsigned char *crc_error)
{
	unsigned char *ptrframe;
	int readbytes,newtime,oldtime = clock();
	unsigned char crc_ind; 
	   ( *crc_error) = 0;
	RecvInit() ;
	do
	{
		readbytes = ReceiveData(com_port,Rdbuf, 8);
		if(readbytes>0)
		{
			for (short i = 0; i < readbytes; i++)
			{
				ptrframe = RecvFrame(Rdbuf[i],&crc_ind);
				if( ptrframe)
				{
					( *crc_error) = crc_ind;
					return  ptrframe;
				}
			}
		}
		newtime = clock();
	}
	while ((newtime-oldtime) < timeout) ;

	return NULL;
}


unsigned char *sendSyncCMDWithRsp(int com_port,unsigned char cmd,unsigned char dtype0,unsigned char dtype,int timeout,unsigned char *result)
{
	unsigned char crc_err,*rspFrame;
	char message[64]= {0};

	*result = 1;
	sendCMD(com_port,cmd,dtype0,dtype);

	rspFrame = receiveSyncRspFrame(com_port,timeout,&crc_err) ;

	if(rspFrame == NULL)
	{
		*result = 0;
		sprintf (message, "CMD=%x respone timeout!!", cmd);
		MessagePopup ("Error:",message);

	}
	else
	{
		if(rspFrame[1] != cmd)
		{

			*result = 0;
			sprintf (message,"CMD=%x respone error!!", cmd);
			MessagePopup ("Error:",message);

		}
		
		if(crc_err)
		{

			*result = 0;
			sprintf (message,"CMD=%x respone crc error!!", cmd);
			MessagePopup ("Error:",message);

		}
	}

	return rspFrame;
}

unsigned char *sendSyncAPUpgradeWithRsp(int com_port,unsigned char *target,unsigned char *pdata,unsigned char pdlen,int timeout,unsigned char *result)
{
	unsigned char crc_err,*rspFrame;
	char message[64]= {0};	  
	unsigned short tcrc = 0;
	if(com_port < 1)
		return NULL;

	*result = 1; 
	for(unsigned char i=0;i<pdlen;i++)
	{
	tcrc += pdata[i];	
	}
	
	
	target[0] = START_DATA_HEADER;           // Start Header
	target[1] = FIRMWARE_UPGRADING_COMMAND;      //  command
	target[2] = MAX_TDATA_LENGTH;      //  length
	memcpy(target + 3,pdata,pdlen);

	target[pdlen+3] = (unsigned char)(tcrc >> 8);
	target[pdlen+4] = (unsigned char)tcrc;
	target[pdlen+5] = '\n';
	SendData(com_port,target, pdlen + MAX_TMISC_LENGTH);              // Send command to firmware

	rspFrame = receiveSyncRspFrame(com_port,timeout,&crc_err) ;

	if(rspFrame == NULL)
	{
		*result = 0;
		sprintf (message, "CMD=%x respone timeout!!", FIRMWARE_UPGRADING_COMMAND);
		MessagePopup ("Error:",message);

	}
	else
	{
		if(rspFrame[1] != FIRMWARE_UPGRADING_COMMAND)
		{

			*result = 0;
			sprintf (message,"CMD=%x respone error!!", FIRMWARE_UPGRADING_COMMAND);
			MessagePopup ("Error:",message);

		}
		
		if(crc_err)
		{

			*result = 0;
			sprintf (message,"CMD=%x respone crc error!!", FIRMWARE_UPGRADING_COMMAND);
			MessagePopup ("Error:",message);

		}
	}

	return rspFrame;
}
