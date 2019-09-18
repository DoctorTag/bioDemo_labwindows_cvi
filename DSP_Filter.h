// DSP_Filter.h: interface for the DSP_Filter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DSP_FILTER_H__)
#define AFX_DSP_FILTER_H__

#include <stdbool.h>
#include "math.h"


void Filter_Low_init(float fk);


void Filter_Low(float *dest, float cursample, bool rst);




void Filter_Noht_Init(int fsmpl, float fc, float bw);


void Filter_Noht(float *dest, float cursample, bool rst);



void Filter_High_Init(int fsmpl, int f_kernel);



void Filter_High(float *dest, float cursample, bool rst);


void resetFilter(void);



#endif // !defined(AFX_DSP_FILTER_H__2649F000_8C49_11D7_8A9A_DF0DA3FB6A79__INCLUDED_)
