#ifndef MMD_COMM_H_
#define MMD_COMM_H_

#ifdef __cplusplus
    extern "C" {
#endif
		
#define PACK_SAMPLES 15


#define SAMPLE_PPG_GDC	 0x00
#define SAMPLE_PPG_RDC     0x10
#define SAMPLE_PPG_IRDC	 0x20
#define SAMPLE_IMP		        0x30
#define SAMPLE_GSR		        0x40

#define SAMPLE_PPG_G	 0x70
#define SAMPLE_PPG_R	 0x80
#define SAMPLE_PPG_IR     0x90
#define SAMPLE_RESP		        0xa0

#define SAMPLE_ECG		        0xb0
#define SAMPLE_WH		        0xc0

#define SAMPLE_GSENSOR		 0xd0

		

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
		
		
		

#define START_DATA_HEADER			0x02
#define WRITE_REG_COMMAND			0x91
#define READ_REG_COMMAND			0x92
#define DATA_STREAMING_COMMAND		0x93
#define DATA_STREAMING_PACKET		0x93
#define ACQUIRE_DATA_COMMAND		0x94
#define ACQUIRE_DATA_PACKET 		0x94
#define PROC_DATA_DOWNLOAD_COMMAND	0x95
#define DATA_DOWNLOAD_COMMAND		0x96
#define FIRMWARE_UPGRADE_COMMAND	0x97
#define START_RECORDING_COMMAND		0x98
#define FIRMWARE_VERSION_REQ		0x99
#define STATUS_INFO_REQ 			0x9A
#define FILTER_SELECT_COMMAND		0x9B
#define ERASE_MEMORY_COMMAND		0x9C
#define RESTART_COMMAND				0x9D
#define END_DATA_HEADER				0x03


		void RecvInit(void) ;
		unsigned char *  RecvFrame( unsigned char src);

#ifdef __cplusplus
    }
#endif

#endif /*MMDTST_MAIN_H_*/
