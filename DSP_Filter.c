// DSP_Filter.cpp: implementation of the DSP_Filter class.
//
//////////////////////////////////////////////////////////////////////


#include "DSP_Filter.h"



#define FILTER_H_M  200
#define PI  3.141592

#define BASELINE_H  500
#define BASELINE_GHR  40


float H[200];

float RFilter_Low_A0;
float RFilter_Low_B1;
float RFilter_Low_B2;
float RFilter_Low_B3;
float RFilter_Low_B4;

float Filter_Noht_A0;
float Filter_Noht_A1 ;
float Filter_Noht_A2 ;
float Filter_Noht_B1 ;
float Filter_Noht_B2 ;

void Filter_Low_init(float fk)
{
//Calculate filter coefficients recursive form
    float x = fk;
  //  float ruslt ;
    RFilter_Low_A0 = pow((1 - x), 4);
    //ruslt = (1 - x);
   // pow(&ruslt, 1, &ruslt);
    //pow(&ruslt, 1, &RFilter_Low_A0);
    RFilter_Low_B1 = 4 * x;
    RFilter_Low_B2 = -6 * x * x;
    RFilter_Low_B3 = 4 * x * x * x;
    RFilter_Low_B4 = -x * x * x * x;
}

void Filter_Low(float *dest, float cursample, bool rst)
{

    static float y_b1 = 0;
    static float y_b2 = 0;
    static float y_b3 = 0;
    static float y_b4 = 0;
    if(rst)
    {
        y_b1 = 0;
        y_b2 = 0;
        y_b3 = 0;
        y_b4 = 0;
        return;
    }

    *dest = (cursample * RFilter_Low_A0 + y_b1 * RFilter_Low_B1 + y_b2 * RFilter_Low_B2
             + y_b3 * RFilter_Low_B3 + y_b4 * RFilter_Low_B4) * 1.005;
    y_b4 = y_b3;
    y_b3 = y_b2;
    y_b2 = y_b1;
    y_b1 = *dest;


}


void Filter_Noht_Init(int fsmpl, float fc, float bw)
{

    //Calculate noht koefizients
    float K, R, fcut;
    fcut = fc / (float)fsmpl;
    R = 1 - 3 * bw;
    K = (1 - 2 * R * cos(2 * PI * fcut) + R * R) / (2 - 2 * cos(2 * PI * fcut));

    Filter_Noht_A0 = K;
    Filter_Noht_A1 = -2 * K * cos(2 * PI * fcut);
    Filter_Noht_A2 = K;
    Filter_Noht_B1 = 2 * R * cos(2 * PI * fcut);
    Filter_Noht_B2 = -R * R;

}






void Filter_Noht(float *dest, float cursample, bool rst)
{

    static float y_b1 = 0;
    static float y_b2 = 0;
    static float x_a1 = 0;
    static float x_a2 = 0;
    static unsigned char cnt = 0;
    if(rst)
    {
        y_b1 = 0;
        y_b2 = 0;
        x_a1 = 0;
        x_a2 = 0;
        cnt = 0;
        return;
    }

    if(cnt < 2)
    {
        *dest = 0;
//*dest =cursample;
        x_a2 = x_a1;
        x_a1 = cursample;
        cnt++;
    }
    else
    {
        *dest = (cursample * Filter_Noht_A0 + x_a1 * Filter_Noht_A1 + x_a2 * Filter_Noht_A2
                 + y_b1 * Filter_Noht_B1 + y_b2 * Filter_Noht_B2);
        x_a2 = x_a1;
        x_a1 = cursample;
        y_b2 = y_b1;
        y_b1 = *dest;


    }


}


void Filter_High_Init(int fsmpl, int f_kernel)
{
    float sum = 0;
    int M = f_kernel,i;
    float k = 500 / fsmpl;
	 float FC = 0.00134 * k;
  //  float FC = 0.00134 * k;
    //Filter > 0.67Hz
    for(i = 0; i < M; i++)
    {
        if((i - M / 2) == 0)
            H[i] = 2 * PI * FC;
        if(0 > (i - M / 2) | 0 < (i - M / 2))
            H[i] = sin(2 * PI * FC * (i - M / 2)) / (i - M / 2);
        H[i] = H[i] * (0.54 - 0.46 * cos(2 * PI * i / M));
    }

    for(i = 0; i < M; i++)
    {
        sum = sum + H[i];
    }
    for(i = 0; i < M; i++)
    {
        H[i] = H[i] / sum;
    }
    for(i = 0; i < M; i++)
    {
        H[i] = -H[i];
    }
    H[M / 2] = H[M / 2] + 1;


}


void Filter_High(float *dest, float cursample, bool rst)
{
    static float xbuf[FILTER_H_M + 1] = {0};
    float ydest = 0;
    static unsigned char cnt = 0;
    int i;
    if(rst)
    {
        for( i = 0; i < FILTER_H_M + 1; i++)
        {
            xbuf[i] = 0;
        }
        cnt = 0;
        return;
    }
    if(cnt < FILTER_H_M / 2)
    {
        xbuf[FILTER_H_M / 2 + cnt] = cursample;
        cnt++;
        *dest = 0;
    }
    else
    {
        xbuf[FILTER_H_M] = cursample;
        for(int i = 0; i < FILTER_H_M; i++)
        {
            //*(Y + j) = *(Y + j) + *(X + j - i) * H[i];

            ydest = ydest + xbuf[i + 1] * H[FILTER_H_M - 1 - i];

            xbuf[i] = xbuf[i + 1];

        }
        *dest = ydest;
    }


}

int  NormalizeData(int cursample, bool rst)
{
    static int BASEL_H[BASELINE_H] = {0};
    static long  temp = 0;
    static int cnt = 0;
    int i;

    if(rst)
    {
        for( i = 0; i < BASELINE_H; i++)
        {
            BASEL_H[i] = 0;
        }
        cnt = 0;
        temp = 0;
        return 0;
    }

    BASEL_H[cnt] = cursample;
    if(cnt < (BASELINE_H - 1))
        cnt++;
    else
        cnt = 0;

    temp += cursample;
    temp -= BASEL_H[cnt];
    return (cursample - (int)(temp / BASELINE_H));

}

int  NormalizeGhr(int cursample, bool rst)
{
    static int BASEL_GHR[BASELINE_GHR] = {0};
    static long  temp = 0;
    static int cnt = 0;
    int i;

    if(rst)
    {
        for( i = 0; i < BASELINE_GHR; i++)
        {
            BASEL_GHR[i] = 0;
        }
        cnt = 0;
        temp = 0;
        return 0;
    }

    BASEL_GHR[cnt] = cursample;
    if(cnt < (BASELINE_GHR - 1))
        cnt++;
    else
        cnt = 0;

    temp += cursample;
    temp -= BASEL_GHR[cnt];
    return (cursample - (int)(temp / BASELINE_GHR));

}

void resetFilter(void)
{
    Filter_Noht(0, 0, 1);
    NormalizeData(0, 1);
	NormalizeGhr(0, 1);
    Filter_Low(0, 0, 1);
    Filter_High(0, 0, 1);
}
