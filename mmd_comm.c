/*---------------------------------------------------------------------------

BSD 3-Clause License

Copyright (c) 2018, feelkit
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


---------------------------------------------------------------------------*/

#include "asynctmr.h"
#include <tcpsupp.h>
#include <ansi_c.h>
#include <utility.h>
#include "serial.h"
#include "timer_cb.h"
#include "mmd_comm.h"




/*****************************************************************************************

*****************************************************************************************/
#define  RX_HEADER           0x00
#define  RX_CMD         0x01
#define  RX_LENGTH        0x02
#define  RX_DATA           0x03
#define  RX_CRC            0x04
//#define  RX_END            0x05



void RecvInit(h_comm_protocol_t *pc)
{
	pc->rx_state = RX_HEADER;
	pc->offset = 0;

}

unsigned char RecvStreamFrame(h_comm_protocol_t *pc, unsigned char src,unsigned char *crc_error)
{
	unsigned char ret = 0;

	pc->rframe.dframe[ pc->offset] = src ;
	pc->offset ++;


	switch(pc->rx_state)
	{
		case RX_HEADER:
			if(pc->rframe.dframe[pc->offset - 1] == START_DATA_HEADER)
			{
				pc->rx_state = RX_CMD;
				pc->rlength = 0;

			}
			else if(pc->rframe.dframe[pc->offset - 1] == (START_DATA_HEADER+1)  )
			{
				pc->rx_state = RX_CMD;
				pc->rlength = 256;

			}
			else
			{
				RecvInit(pc);
			}

			break;
		case RX_CMD:
			pc->rx_state = RX_LENGTH;
			break;
		case RX_LENGTH:
			pc->rlength |= (pc->rframe.dframe[pc->offset-1]);
			if(pc->rlength <= MAX_RDATA_LENGTH)
			{
				pc->rx_state = RX_DATA;
				pc->lcrc = 0;
			}
			else
			{
				RecvInit(pc);
			}
			break;
		case RX_DATA:
			if(pc->offset >= (pc->rlength+3))
			{
				pc->rx_state = RX_CRC;

			}
			pc->lcrc ^= pc->rframe.dframe[pc->offset-1];
			break;
		case RX_CRC:
			if(pc->offset == (pc->rlength+4))
			{
				pc->rcrc = pc->rframe.dframe[pc->offset-1];

				(*crc_error) = ((pc->rcrc) != (pc->lcrc));
				ret = 1;
				RecvInit(pc);
			}


			if(pc->offset > (pc->rlength+4) )
			{
				RecvInit(pc);
			}
			break;

	}

	return ret;
}


static int CVICALLBACK TCPCallback (unsigned int handle, int xType,
									int errCode, void *callbackData)
{
	unsigned char result,ret;
	unsigned char Rdbuf[1024];
	h_comm_handle_t *ptr_recv;
	int readbytes;

	ptr_recv = (h_comm_handle_t *)callbackData;
	switch (xType)
	{
		case TCP_DISCONNECT:
			/* Server disconnected. Notify user. */
			MessagePopup ("tcp service", "tcp Server disconnected!");
			ptr_recv->tcp_ok = 0;
			break;
		case TCP_DATAREADY:

			DisableBreakOnLibraryErrors ();
			while (1)
			{
				if ((readbytes = ClientTCPRead (handle, Rdbuf,1024, 10)) < 0)
					break;
				for (short i = 0; i < readbytes; i++)
				{
					ret = RecvStreamFrame(&ptr_recv->tcp_pc, Rdbuf[i],&result);
					if( ret)
					{
						ptr_recv->tcp_pc.rframe.ctype  = TCP_CH;
						ptr_recv->tcp_pc.rframe.dframe[0] = result;


						CmtWriteTSQData (ptr_recv->queueHandle, (void*)&ptr_recv->tcp_pc.rframe,1,0, NULL);

					}


				}
			}
			EnableBreakOnLibraryErrors ();

			ptr_recv->tcp_rdata_sig++;
			break;
	}

	return 0;
}

int CVICALLBACK serial_comm_recv_Thread(void *callbackData)
{
	h_comm_handle_t *ptr_recv;
	unsigned char result,ret;
	unsigned char Rdbuf[1024];
	int readbytes;

	ptr_recv = (h_comm_handle_t *)callbackData;

	RecvInit(&ptr_recv->ser_pc) ;

	while( ptr_recv->s_receiving)
	{
		if(ptr_recv->com_port > 0)
		{
			readbytes = ReceiveData(ptr_recv->com_port,Rdbuf, 1024);

			if(readbytes>0)
			{
				for (short i = 0; i < readbytes; i++)
				{
					ret = RecvStreamFrame(&ptr_recv->ser_pc, Rdbuf[i],&result);
					if( ret)
					{
						//	void *tmp_rx = malloc(sizeof(h_comm_rdata_t));
						ptr_recv->ser_pc.rframe.ctype  = SERIAL_CH;
						ptr_recv->ser_pc.rframe.dframe[0] = result;

						//			memcpy(tmp_rx, (void *)&(ptr_recv->ser_pc.rframe),sizeof(h_comm_rdata_t));

						CmtWriteTSQData (ptr_recv->queueHandle, &(ptr_recv->ser_pc.rframe),1,TSQ_INFINITE_TIMEOUT, NULL);

					}



				}
			}
		}
		else
			ptr_recv->s_receiving = 0;


	}

	if(ptr_recv->com_ok > 0)
		ShutDownCom (ptr_recv->com_port)  ;


	return 0;
}

int CVICALLBACK serial_comm_recv_Thread_tmp(void *callbackData)
{
	h_comm_handle_t *ptr_recv;
	unsigned char result,ret;
	unsigned char Rdbuf[200];
	int readbytes;

	ptr_recv = (h_comm_handle_t *)callbackData;

	RecvInit(&ptr_recv->ser_pc) ;

	while( ptr_recv->s_receiving)
	{
		if(ptr_recv->com_port > 0)
		{
			readbytes = ReceiveData(ptr_recv->com_port,Rdbuf, 200);

			if(readbytes>0)
			{
				for (short i = 0; i < readbytes; i++)
				{
					ret = RecvStreamFrame(&ptr_recv->ser_pc, Rdbuf[i],&result);
					if( ret)
					{
						ptr_recv->ser_pc.rframe.ctype  = SERIAL_CH;
						ptr_recv->ser_pc.rframe.dframe[0] = result;


						CmtWriteTSQData (ptr_recv->queueHandle, (void*)&ptr_recv->ser_pc.rframe,1,TSQ_INFINITE_TIMEOUT, NULL);

					}



				}
			}
		}
		else
			ptr_recv->s_receiving = 0;


	}

	if(ptr_recv->com_ok > 0)
		ShutDownCom (ptr_recv->com_port)  ;


	return 0;
}

void h_comm_connectTcpServer(h_comm_handle_t *handle)
{
	RecvInit(&handle->tcp_pc) ;
	DisableBreakOnLibraryErrors ();
	if (ConnectToTCPServer (&(handle->tcp_handle), handle->tcp_port, handle->serverip,TCPCallback, handle, 5000) < 0)
		MessagePopup("TCP Client", "Connection to server failed !");
	else
	{
		handle->tcp_ok = 1;
		//ptr_recv->receiving = 1;
	}
	EnableBreakOnLibraryErrors ();

}


int h_comm_sendCMD1(h_comm_handle_t *handle,unsigned char cmd,unsigned char dtype0,unsigned char dtype)
{
	unsigned char Wrbuf[8];

	unsigned char tcrc = 0;
	if((handle->com_ok < 1)&&(handle->tcp_ok < 1))
	{
		MessagePopup ("Error:","Nothing communication ");
		return 0;
	}

	tcrc ^= dtype0;
	tcrc ^= dtype;
	tcrc ^= cmd;
	Wrbuf[0] = START_DATA_HEADER;           // Start Header
	Wrbuf[1] = 3;      //  length

	Wrbuf[2] = cmd;      //  command
	Wrbuf[3] = dtype0;                           // Start command
	Wrbuf[4] = dtype;

//	Wrbuf[5] = (unsigned char)(tcrc >> 8);
	Wrbuf[5] = tcrc;
	Wrbuf[6] = '\n';

	if(handle->tcp_ok > 0)
	{

		return ClientTCPWrite (handle->tcp_handle, Wrbuf,7, 1000) ;
	}

	if(handle->com_ok > 0)
	{
		return SendData(handle->com_port,Wrbuf, 7);
	}
	return 0;
}



int h_comm_sendCMD(h_comm_handle_t *handle,unsigned char cmd,unsigned char reg2,unsigned char reg1,unsigned char cmd_par)
{
	unsigned char Wrbuf[8];

	unsigned char tcrc = 0;
	if((handle->com_ok < 1)&&(handle->tcp_ok < 1))
	{
		MessagePopup ("Error:","Nothing communication ");
		return 0;
	}

	tcrc ^= reg2;
	tcrc ^= reg1;
	tcrc ^= cmd_par;   
	Wrbuf[0] = START_DATA_HEADER;           // Start Header

	Wrbuf[1] = cmd;      //  command
	Wrbuf[2] =3;      //  length

	Wrbuf[3] = reg2;                           // Start command
	Wrbuf[4] = reg1;

	Wrbuf[5] = cmd_par;
	Wrbuf[6] = tcrc;
	Wrbuf[7] = '\n';

	if(handle->tcp_ok > 0)
	{

		return ClientTCPWrite (handle->tcp_handle, Wrbuf,8, 1000) ;
	}

	if(handle->com_ok > 0)
	{
		return SendData(handle->com_port,Wrbuf, 8);
	}
	return 0;
}

int h_comm_sendFWUpgrade(h_comm_handle_t *handle,unsigned char up_cmd,unsigned char *target,unsigned short offset,unsigned char *pdata,unsigned char pdlen)
{

	unsigned char tcrc = 0;
	if((handle->com_ok < 1)&&(handle->tcp_ok < 1))
	{
		MessagePopup ("Error:","Nothing communication ");
		return 0;
	}

	for(unsigned char i=0; i<pdlen; i++)
	{
		tcrc ^= pdata[i];
	}

	target[0] = START_DATA_HEADER;           // Start Header
	target[1] = FIRMWARE_UPGRADING_COMMAND;      //  command
	target[2] = (pdlen+3);      //  length
	target[3] = up_cmd ;
	target[4] = (unsigned char)(offset>>8);      //  offset high
	target[5] = (unsigned char)offset;      //  offset low
	memcpy(target + 6,pdata,pdlen);

	tcrc ^= target[3];
	tcrc ^= target[4];
	tcrc ^= target[5];
	target[pdlen+6] = tcrc;
//	target[pdlen+7] = (unsigned char)tcrc;
	target[pdlen+7] = '\n';

	if(handle->tcp_ok >0)
		return ClientTCPWrite (handle->tcp_handle, target, pdlen + MAX_TMISC_LENGTH, 1000) ;
	else  if(handle->com_ok > 0)
		return SendData(handle->com_port,target, pdlen + MAX_TMISC_LENGTH);

	return 0;
}




