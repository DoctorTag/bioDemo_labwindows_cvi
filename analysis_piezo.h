#ifndef ANALYSIS_PIEZO_H__
#define ANALYSIS_PIEZO_H__

#include "stdbool.h"


#define SAMPLE_HZ    80      /*Don't change the value */

#define ANA_BUF_SECS  (9)

#define ANA_BUF_LEN  (SAMPLE_HZ*ANA_BUF_SECS)

typedef enum
{
	OUT_OF_BED = 0,
	BODY_REPOSE,
	BODY_MOVE,
	BODY_UNKNOW
} Body_status;


typedef enum
{
	DEEP_SLEEP = 0,
	LIGHT_SLEEP,
	WAKE_CALM
} Sleep_status;

typedef enum
{
	POSTURE_SUPINE = 0,
	POSTURE_LATERALT

} Sleeping_posture;


typedef struct
{
	Body_status cur_body_status;
	Sleeping_posture sleep_post;
	Sleep_status  cur_sleep_status;
	short hr;
	char  resp;
	bool hr_ok;
	bool resp_ok; 

	/* only test */
	uint32_t moved_Intensity;
	short hr_peak_points[SAMPLE_HZ] ;
	unsigned char ppoints;
	/* data length is SAMPLE_HZ */
//	int *o_data;
//	float *hr_filted_data;
//	float *hr_enhanced_data;
//	float *resp_data;
	int raw_data[SAMPLE_HZ] ;
	float hr_filted_data[SAMPLE_HZ] ;
	float hr_enhanced_data[SAMPLE_HZ] ;
	float resp_data[ANA_BUF_LEN] ;
	short resp_peak_points[SAMPLE_HZ] ;
	unsigned char resp_ppoints;

} analysis_result_t;


//float analysis_piezo(int pie_data) ;

void analysis_piezo_init(uint32_t out_of_bed_threshold,uint32_t raw_peak_threshold);

bool analysis_piezo_all(int pie_data,analysis_result_t * a_result) ;

#endif
