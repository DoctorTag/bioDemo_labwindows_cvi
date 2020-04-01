#ifndef ANALYSIS_PIEZO_H__
#define ANALYSIS_PIEZO_H__


#include "stdbool.h"

 #define SAMPLE_HZ    80 

typedef enum
{
	OUT_OF_BED = 0,
	BODY_REPOSE,
	BODY_MOVE,
	BODY_MOVE_LIGHT,
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
	float o_data[SAMPLE_HZ] ;
	float hr_filted_data[SAMPLE_HZ] ;
	float hr_enhanced_data[SAMPLE_HZ] ; 
	float resp_data[SAMPLE_HZ];
	
	
	Body_status cur_body_status;
	Sleeping_posture sleep_post;
	Sleep_status  cur_sleep_status;
	unsigned short hr;
	unsigned char  resp;
	
	uint32_t moved_Intensity;
	 /* only test */
	
		double analysis_result[SAMPLE_HZ] ;


} analysis_result_t;


//float analysis_piezo(int pie_data) ;

void analysis_piezo_init(void);

bool analysis_piezo_all(int pie_data,analysis_result_t * a_result) ;

#endif
