//==============================================================================
//
// Title:		filter.c
// Purpose:		A short description of the implementation.
//
// Created on:	2019/1/27 at 13:36:56 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include "filter.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

/// HIFN  What does your function do?
/// HIPAR x/What inputs does your function expect?
/// HIRET What does your function return?

const double IIR2_05Hz_Table[6] = {
	1.000000000000000, -1.979267277585, 0.979480006562,    
  	0.0000531822443406, 0.0001063644886812, 0.0000531822443406     
};
const double IIR2_25Hz_Table[6] = {
	1,   -1.475480443593,   0.5869195080612,    //Y
  0.02785976611714,  0.05571953223427,  0.02785976611714,     	
};
const double IIR2_Stop50Hz_Table[6] = {                 					
	1.0000000000000,   -1.525271192437,   0.8816185923632,
	0.9408092961816,   -1.525271192437,   0.9408092961816,
};

struct Buffer
{
	double Hp[6];
	double Lp[6];
	double Bandstop[6];
	double Lp_1dot5Hz[6];
	double Lp_2Hz[6];
	int DriftBuf[100];
};
struct Buffer Lead_II_Buffer;

#define  chebylength 22


int Lead_II_Filter(int datum, const double *LpCoefs, const double *HpCoefs)
{
	int fdatum;
	static int IICnt = 0;
	
	fdatum = (int)IIR_HP_Filter(HpCoefs, Lead_II_Buffer.Hp, datum);							
	fdatum = (int)IIR_LP_Filter(LpCoefs, Lead_II_Buffer.Lp, fdatum);						
	fdatum = (int)IIR_Bandstop(IIR2_Stop50Hz_Table, Lead_II_Buffer.Bandstop, fdatum);
	IICnt++;
	
	return fdatum;
}

double IIR_HP_Filter(const double *pTable, double *pBuffer, int Xn)
{
	pBuffer[2] = pBuffer[1];
	pBuffer[1] = pBuffer[0];

	pBuffer[5] = pBuffer[4];
	pBuffer[4] = pBuffer[3];
	pBuffer[3] = (double)Xn;

	pBuffer[0] =
	(pBuffer[3] + pBuffer[5]) * pTable[3] +
	pBuffer[4] * pTable[4] -
	pBuffer[1] * pTable[1] -
	pBuffer[2] * pTable[2];

	return (double)Xn - pBuffer[0];
}

double IIR_LP_Filter(const double *pTable, double *pBuffer, int Xn)
{
	pBuffer[2] = pBuffer[1];
	pBuffer[1] = pBuffer[0];

	pBuffer[5] = pBuffer[4];
	pBuffer[4] = pBuffer[3];
	pBuffer[3] = (double)Xn;

	pBuffer[0] =
	(pBuffer[3] + pBuffer[5]) * pTable[3] +
	pBuffer[4] * pTable[4] -
	pBuffer[1] * pTable[1] -
	pBuffer[2] * pTable[2];

	return pBuffer[0];
}

void Shift2Left(int *array, int Length)
{
  static unsigned char i;
	for(i=0; i<(Length-1); i++)
        array[i] = array[i+1];
}


double IIR_Bandstop(const double *pTable, double *pBuffer, int Xn)
{
	pBuffer[2] = pBuffer[1];
	pBuffer[1] = pBuffer[0];

	pBuffer[5] = pBuffer[4];
	pBuffer[4] = pBuffer[3];
	pBuffer[3] = (double)Xn;

	pBuffer[0] =
		pBuffer[3] * pTable[3] +
		pBuffer[4] * pTable[4] +
		pBuffer[5] * pTable[5] -

		pBuffer[1] * pTable[1] -
		pBuffer[2] * pTable[2];

	return pBuffer[0];
}
 
