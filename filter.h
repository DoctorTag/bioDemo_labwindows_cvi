//==============================================================================
//
// Title:		filter.h
// Purpose:		A short description of the interface.
//
// Created on:	2019/1/27 at 13:34:50 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __filter_H__
#define __filter_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions



#define BUFFER_X_LGTH   90
#define BUFFER_Y11_LGTH   1
#define BUFFER_Y12_LGTH   90
#define BUFFER_Y2_LGTH   1
#define DERIV_LENGTH 3
#define WINDOW_WIDTH 24		// 80ms
#define DD		4
#define ND		17
#define alpha 1.0
// 滤波器系数矩阵
extern const double IIR2_05Hz_Table[];
extern const double IIR2_1Hz_Table[];
extern const double IIR2_25Hz_Table[];
extern const double IIR2_Stop50Hz_Table[];
extern const double IIR2_20Hz_Table[];

// 导联滤波器
extern int Lead_I_Filter(int datum, double *LpCoefs, double *HpCoefs); 
extern int Lead_II_Filter(int datum, const double *LpCoefs, const double *HpCoefs);
extern int Lead_III_Filter(int datum, double *LpCoefs, double *HpCoefs);
extern int Resp_Filter(int datum);
// 带宽滤波器
extern double IIR_HP_Filter(const double *pTable, double *pBuffer, int Xn);
extern double IIR_LP_Filter(const double *pTable, double *pBuffer, int Xn);
extern double IIR_Bandstop(const double *pTable, double *pBuffer, int Xn);
// 去基线漂移滤波器
extern int Lead_I_DriftFilter(int datum);
extern int DNFilter(int datum, int Dstep, int Nlen, int *pBuffer, int InCnt);
extern int Lead_II_DriftFilter(int datum);
extern int Lead_III_DriftFilter(int datum);
// 去工频滤波器（运用卷积）
extern int NotchFlter(int datum, int *xBuffer);
// 差分
extern int deriv(int datum);
// 移动窗口
extern int mvwint(int datum);
// 移位函数
extern void Shift2Left(int *array, int Length);

extern int Cheby_Filter_Mon(int datum, int *xBuffer);




#ifdef __cplusplus
    }
#endif

#endif  /* ndef __filter_H__ */
