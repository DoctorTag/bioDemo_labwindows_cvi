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

#ifndef _REGMAP_CB_H_
#define _REGMAP_CB_H_


#ifdef __cplusplus
extern "C" {
#endif

	#define RegNum_Addl                        0x00
#define RegNum_Addm                      0x01
#define RegNum_Addh                       0x02
#define RegNum_Ad_Type_Cnt          0x03
#define RegNum_Dev_Id                    0x04
#define RegNum_PStatus                   0x05
#define RegNum_AP_VER                   0x06
#define RegNum_BL_VERSION	          0x07

#define RegNum_Fun1                        0x08
#define RegNum_Fun2                        0x09
#define RegNum_RUN_Mode               0x0a
#define BL_REG_DATA                      0x0A

#define RegNum_PPG_PGA                 0x0b
#define RegNum_PPG_OP                    0x0c
#define RegNum_PPG_DCTH                0x0d



#define RegNum_MAX   14
	
	#define REG_ONLY_RD_LEN  8


	int CVICALLBACK PanelRegMapCB (int panel, int event, void *callbackData,int eventData1, int eventData2) ;


	int CVICALLBACK RegMap_QuitCb (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)  ;


	int CVICALLBACK RegTableCb (int panel, int control, int event, void *callbackData, int eventData1, int eventData2) ;




#ifdef __cplusplus
}
#endif


#endif
