
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



#ifndef MMD_COMM_H_
#define MMD_COMM_H_

#ifdef __cplusplus
extern "C" {
#endif



#define MAX_SENSOR_ADTYPES  16
#define LOCAL_AN_ECG    MAX_SENSOR_ADTYPES

#define MAX_MMED_TYPES  (LOCAL_AN_ECG+1)
#define LOCAL_CMD_RDATA  (MAX_MMED_TYPES)

	#define  FRAME_TYPE_MASK            0x70
	#define  DATA_TYPE_MASK            0x0F


#define PACK_SAMPLES 100
//	#define PACK_SAMPLES 1


#define MAX_RDATA_LENGTH (3*PACK_SAMPLES+1)

//#define MAX_RDATA_LENGTH ((3*PACK_SAMPLES)/2+1)

#define SAMPLE_PPG_GDC	 0x00
#define SAMPLE_PPG_RDC   0x10
#define SAMPLE_PPG_IRDC	 0x20
#define SAMPLE_IMP		 0x30
#define SAMPLE_GSR		 0x40

#define SAMPLE_PPG_G	 0x70
#define SAMPLE_PPG_R	 0x80
#define SAMPLE_PPG_IR    0x90
#define SAMPLE_RESP		 0xa0

#define SAMPLE_ECG		 0xb0
#define SAMPLE_WH		 0xc0

#define SAMPLE_GSENSOR	 0xd0



#define REG_FUN1_ECG_AII          0x0001
#define REG_FUN1_ECG_V1           0x0002
#define REG_FUN1_ECG_A            0x0004

#define REG_FUN1_PPG_IR           0x0008
#define REG_FUN1_PPG_R            0x0010
#define REG_FUN1_PPG_G            0x0020
#define REG_FUN1_IMP              0x0040
#define REG_FUN1_RESP             0x0080

#define REG_FUN2_ECG_DI           0x0100
#define REG_FUN2_WH               0x0200
#define REG_FUN2_GSR              0x0400
#define REG_FUN2_EMG              0x0800
#define REG_FUN2_TEMP             0x1000

#define FUN1_COMBO_SPO2            (REG_FUN1_PPG_R|REG_FUN1_PPG_IR)

#define FUN1_COMBO_STD            (REG_FUN1_PPG_R|REG_FUN1_PPG_IR|REG_FUN1_RESP|REG_FUN2_GSR|REG_FUN1_PPG_G)
#define FUN1_COMBO_HR_RESP        (REG_FUN1_PPG_G|REG_FUN1_RESP)


#define M_FUN_START        0x10

#define HALT_MODE 	    0xe000
#define OBEY_MODE       0xc000
#define WATCH_MODE      0xa000
#define CAL_MODE        0x8000
#define FWUP_MODE       0x6000


#define  MASK_8BIT     2
#define  MASK_12BIT   3
#define  MASK_16BIT   4
#define  MASK_24BIT   6

#define START_DATA_HEADER			0x55
#define WRITE_REG_COMMAND			0x11
#define READ_REG_COMMAND			0x12
#define DATA_STREAMING_COMMAND		0x13
#define DATA_STREAMING_PACKET		0x13
#define ACQUIRE_DATA_COMMAND		0x14
#define ACQUIRE_DATA_PACKET 		0x14
#define DEVICE_ID_REQ              	0x15
#define DATA_DOWNLOAD_COMMAND		0x16
//#define FIRMWARE_UPGRADE_CMD	    0x17
#define FIRMWARE_UPGRADING_COMMAND	0x18
#define FIRMWARE_VERSION_REQ		0x19
#define STATUS_INFO_REQ 			0x1A
#define FILTER_SELECT_COMMAND		0x1B
#define ERASE_MEMORY_COMMAND		0x1C
#define RESTART_COMMAND				0x1D
#define START_RECORDING_COMMAND		0x1E



#define DATA_TYPE_DEFAULT                0x20
#define DATA_TYPE_8BIT	                     DATA_TYPE_DEFAULT	
#define DATA_TYPE_12BIT	              (DATA_TYPE_DEFAULT+((MASK_12BIT-MASK_8BIT)<<4))
#define DATA_TYPE_16BIT	               (DATA_TYPE_DEFAULT+((MASK_16BIT-MASK_8BIT)<<4))
#define DATA_TYPE_24BIT	               (DATA_TYPE_DEFAULT+((MASK_24BIT-MASK_8BIT)<<4))

#define DATA_TYPE_MASK		          0x0F

#define AP_MASK				0x80

#define NORMAL_RST				    0x01
#define FWUPGRADE_RST				0x02

#define FWUP_PWD        0x58
#define RESET_PWD        0x68


#define BL_CMD_REBOOT    0X60
#define BL_CMD_UPGRADE_REQ    0X61
#define BL_CMD_ERASEAPP    0X62
#define BL_CMD_JMPAPP    0X63
#define BL_CMD_APRDY    0X64
#define BL_CMD_PROGRAM    0X6f

#define BL_CMD_OTA_DSEG0    0X80
#define BL_CMD_OTA_DSEG1    0X81
#define BL_CMD_OTA_DSEG2    0X82
#define BL_CMD_OTA_DSEG3    0X83
#define BL_CMD_OTA_DSEG4    0X84
#define BL_CMD_OTA_DSEG5    0X85
#define BL_CMD_OTA_DSEG6    0X86
#define BL_CMD_OTA_DSEG7    0X87

#define BL_CMD_OTA_REQ    0X88


#define RSP_OK                 (0x01)  /*! Command processed OK    */
#define RSP_PARA_ERROR     (0x02)  /*! Invalid parameters      */
#define RSP_CMD_ERROR  (0x03)  /*! Invalid command         */

#define RSP_SENSOR_ERROR  (0x04)
#define RSP_PROGRAM_ERROR  (0x05)
#define RSP_CRC_ERROR  (0x06)
#define RSP_VERIFY_ERROR  (0x07)
#define RSP_UNKNOW                 (0x08)



#define MAX_TDATA_LENGTH 128

#define MAX_TMISC_LENGTH 8

#define MAX_RX_LENGTH (MAX_RDATA_LENGTH+5)  /*1 Start+1CMD+1DLEN+MAX_TDATA+2CRC*/

#define PPG_G_FUN   (0)
#define PPG_R_FUN   (1)
#define PPG_IR_FUN  (2)
#define PPG_SPO2    (9)

#define RESP_FUN    (5)

#define SERIAL_CH   (1)
#define TCP_CH   (2)


	typedef struct
	{
		unsigned char ctype;
		unsigned char dframe[MAX_RX_LENGTH];
		 	void *tmptest;  
	} h_comm_rdata_t;

	typedef struct
	{
		unsigned char rx_state;
		unsigned short offset;
		unsigned char lcrc;
		unsigned char rcrc;
		unsigned short rlength;
		//unsigned char rx_frame[MAX_RX_LENGTH];
		h_comm_rdata_t rframe;
	

	} h_comm_protocol_t;

	typedef void   (*mmed_recv_handler_t)(unsigned char *ptrframe,unsigned char crc_result) ;




	typedef struct
	{
		unsigned char s_receiving;
		unsigned char com_port;
		unsigned char com_ok;
		h_comm_protocol_t ser_pc;

		unsigned char t_receiving;
		unsigned tcp_rdata_sig ;
		unsigned char tcp_ok;
		unsigned tcp_handle;
		unsigned tcp_port;
		char serverip[16];
		h_comm_protocol_t tcp_pc;
		CmtTSQHandle queueHandle;
	} h_comm_handle_t;



	void RecvInit(h_comm_protocol_t *pc)  ;
	unsigned char RecvStreamFrame(h_comm_protocol_t *pc, unsigned char src,unsigned char *crc_error) ;
	h_comm_rdata_t *sendSyncCMDWithRsp(h_comm_handle_t *handle,unsigned char cmd,unsigned char dtype0,unsigned char dtype,int timeout)  ;
//	h_comm_rdata_t *sendFWUpgradeWithRsp(h_comm_handle_t *handle,unsigned char up_cmd,unsigned char *target,unsigned short offset,unsigned char *pdata,unsigned char pdlen,int timeout);
//	int h_comm_sendCMD(h_comm_handle_t *handle,unsigned char cmd,unsigned char dtype0,unsigned char dtype) ;
	int h_comm_sendCMD(h_comm_handle_t *handle,unsigned char cmd,unsigned char reg2,unsigned char reg1,unsigned char cmd_par) ;

	int h_comm_sendFWUpgrade(h_comm_handle_t *handle,unsigned char up_cmd,unsigned char *target,unsigned short offset,unsigned char *pdata,unsigned char pdlen) ;
	h_comm_rdata_t *waitSyncRspFrame(CmtTSQHandle queueHandle,int timeout) ;

	void h_comm_connectTcpServer(h_comm_handle_t *handle)  ;
	int CVICALLBACK serial_comm_recv_Thread(void *callbackData);


	int CVICALLBACK tcp_comm_recv_Thread(void *callbackData);

#ifdef __cplusplus
}
#endif

#endif /*MMDTST_MAIN_H_*/
