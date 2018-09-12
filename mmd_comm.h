#ifndef MMD_COMM_H_
#define MMD_COMM_H_

#ifdef __cplusplus
extern "C" {
#endif

#define PACK_SAMPLES 15


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



#define REG_FUN1_ECG_A             0x01
#define REG_FUN1_PPG_R             0x02
#define REG_FUN1_PPG_G             0x04
#define REG_FUN1_PPG_IR            0x08
//#define REG_FUN1_PPG_DARK          0x10
#define REG_FUN1_IMP               0x20
#define REG_FUN1_RESP              0x40
#define REG_FUN1_GSR               0x80

#define REG_FUN2_ECG_D             0x01
#define REG_FUN2_WH                0x02

#define FUN1_COMBO_STD                (REG_FUN1_PPG_R|REG_FUN1_PPG_IR|REG_FUN1_RESP|REG_FUN1_GSR|REG_FUN1_PPG_G)

#define FUN1_COMBO_HR_RESP        (REG_FUN1_PPG_G|REG_FUN1_RESP)


#define M_FUN_START        0x10

#define HALT_MODE 	    0xe0
#define OBEY_MODE       0xc0
#define WATCH_MODE      0xa0
#define CAL_MODE        0x80
#define FWUP_MODE       0x60



#define START_DATA_HEADER			0x55
#define WRITE_REG_COMMAND			0x81
#define READ_REG_COMMAND			0x82
#define DATA_STREAMING_COMMAND		0x83
#define DATA_STREAMING_PACKET		0x83
#define ACQUIRE_DATA_COMMAND		0x84
#define ACQUIRE_DATA_PACKET 		0x84
#define DEVICE_ID_REQ              	0x85
#define DATA_DOWNLOAD_COMMAND		0x86
#define FIRMWARE_UPGRADE_REQ    	0x87
#define FIRMWARE_UPGRADING_COMMAND	0x88
#define FIRMWARE_VERSION_REQ		0x89
#define STATUS_INFO_REQ 			0x8A
#define FILTER_SELECT_COMMAND		0x8B
#define ERASE_MEMORY_COMMAND		0x8C
#define RESTART_COMMAND				0x8D
#define START_RECORDING_COMMAND		0x8E


#define END_DATA_HEADER				0x0A


#define AP_MASK				0x80

#define NORMAL_RST				    0x01
#define FWUPGRADE_RST				0x02
#define FWUPGRADE_PWD				0x58
	
	
	 #define MAX_TDATA_LENGTH 128 
	
	   #define MAX_TMISC_LENGTH 6
	void RecvInit(void) ;
	unsigned char *  RecvFrame( unsigned char src,unsigned char *crc_error);
//	void SteamData(int com_port,unsigned char isbegin,unsigned char dtype);
	void sendCMD(int com_port,unsigned char cmd,unsigned char dtype0,unsigned char dtype) ;
	unsigned char *receiveSyncRspFrame(int com_port,int timeout,unsigned char *crc_error);
	unsigned char *sendSyncCMDWithRsp(int com_port,unsigned char cmd,unsigned char dtype0,unsigned char dtype,int timeout,unsigned char *result);
	   unsigned char *sendSyncAPUpgradeWithRsp(int com_port,unsigned char *target,unsigned char *pdata,unsigned char pdlen,int timeout,unsigned char *result) ;
	
#ifdef __cplusplus
}
#endif

#endif /*MMDTST_MAIN_H_*/
