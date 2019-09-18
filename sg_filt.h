#ifndef _sg_filt_h_
#define _sg_filt_h_

//#include<iostream>

void sg_filt(double B[]);    //µÃµ½ÂË²¨¾ØÕó


#define L 100 
#define N 11
#define D 3
#define M 5


double * First_filter_buf1(double * IIR_Filter_Data);
double *  Else_filter_buf1(double * IIR_Filter_Data);


#endif
