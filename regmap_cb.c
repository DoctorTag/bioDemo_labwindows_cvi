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
/* Include files                                                             */
/*---------------------------------------------------------------------------*/
#include <utility.h>
#include <cvirte.h>
#include <userint.h>
#include <ansi_c.h>
//#include <progressbar.h>
#include <toolbox.h>
#include "bio_demo.h"
//#include "serial.h"
#include "mmd_comm.h"
#include "regmap_cb.h"


#define REG_VALUE_COLUMN 5

Point focus;






int CVICALLBACK Reg_ReadAllCb (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	unsigned char result,*rspFrame;
	switch (event)
	{
		case EVENT_COMMIT:
		{
			for(unsigned char i=0; i<RegNum_MAX; i++)
			{
			//	rspFrame  =	sendSyncCMDWithRsp(comport,READ_REG_COMMAND,i,0,1000,&result)  ;
				if(result)
				{
					if(rspFrame)
					{
						char tmp_str[5] = {0};
						sprintf (tmp_str, "0x%x", rspFrame[4]);

						SetTableCellAttribute (panel, PANEL_REG_TABLE, MakePoint(REG_VALUE_COLUMN, i+1),ATTR_CTRL_VAL, tmp_str);
					}
				}
			}

		}
		break;
	}
	return 0;
}


int CVICALLBACK Reg_ReadCb (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	unsigned char result,*rspFrame;

	switch (event)
	{
		case EVENT_COMMIT:
		{
		//	rspFrame  =	sendSyncCMDWithRsp(comport,READ_REG_COMMAND,focus.y - 1,0,1000,&result)  ;
			if(result)
			{
				if(rspFrame)
				{
					char tmp_str[5] = {0};
					sprintf (tmp_str, "0x%x", rspFrame[4]);
					SetCtrlAttribute(panel, PANEL_REG_INFO,ATTR_CTRL_VAL,rspFrame[4]);
					SetTableCellAttribute (panel, PANEL_REG_TABLE, MakePoint(REG_VALUE_COLUMN, focus.y),ATTR_CTRL_VAL, tmp_str);
				}
			}
			/*
			else
			{
				if(rspFrame[4] != RSP_OK)
				{
					char message[64]= {0};
					sprintf (message,"READ_REG_COMMAND error= 0x%x !!", rspFrame[4]);
					MessagePopup ("Error :",message);
					return 0;
				}
			}
			*/

		}
		break;
	}
	return 0;
}


int CVICALLBACK Reg_WriteCb (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	unsigned char result,*rspFrame,wvalue;
	switch (event)
	{
		case EVENT_COMMIT:
		{
			if(focus.y > REG_ONLY_RD_LEN)
			{

				GetCtrlAttribute(panel, PANEL_REG_INFO,ATTR_CTRL_VAL,&wvalue);
				//	wvalue = 1;
			//	rspFrame  =	sendSyncCMDWithRsp(comport,WRITE_REG_COMMAND,focus.y-1,wvalue,1000,&result)  ;
				if(result)
				{
					if(rspFrame[4] == RSP_OK)
				{
					char tmp_str[5] = {0};
					sprintf (tmp_str, "0x%x", wvalue);
					
					SetTableCellAttribute (panel, PANEL_REG_TABLE, MakePoint(REG_VALUE_COLUMN, focus.y),ATTR_CTRL_VAL, tmp_str);
				}
				}
			}

		}
		break;
	}
	return 0;
}

int CVICALLBACK RegTableCb (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	char tmpstr[5];
	unsigned char tmpd;

	switch (event)
	{
		case EVENT_COMMIT:
			GetActiveTableCell(panel, control, &focus);

			if (focus.x == REG_VALUE_COLUMN)
			{
				//	SetTableCellAttribute (panel, control, MakePoint(REG_VALUE_COLUMN, focus.y),ATTR_CTRL_VAL, "88");

				//	SetTableCellVal (panel, PANEL_REG_TABLE, MakePoint(REG_VALUE_COLUMN, focus.y),0x88);
				//	TestChannel(panel, control, focus.y);
				switch(focus.y)
				{
					case (RegNum_Addl+1) :
						SetCtrlAttribute(panel, PANEL_REG_INFO,ATTR_LABEL_TEXT,"Reg:(0x00) ADC LOW");
						//	SetTableCellAttribute (panel, control, MakePoint(REG_VALUE_COLUMN, focus.y),ATTR_CTRL_VAL, "00");

						break;
					case (RegNum_Addm+1) :
						SetCtrlAttribute(panel, PANEL_REG_INFO,ATTR_LABEL_TEXT,"Reg:(0x01) ADC MIDDLE");
						//		SetTableCellAttribute (panel, control, MakePoint(REG_VALUE_COLUMN, focus.y),ATTR_CTRL_VAL, "01");
						break;
					case (RegNum_Addh+1) :
						SetCtrlAttribute(panel, PANEL_REG_INFO,ATTR_LABEL_TEXT,"Reg:(0x02) ADC HIGH");
						break;
					case (RegNum_Ad_Type_Cnt+1) :
						SetCtrlAttribute(panel, PANEL_REG_INFO,ATTR_LABEL_TEXT,"Reg:(0x03) AD Type&Seq");
						break;
					case (RegNum_Dev_Id+1) :
						SetCtrlAttribute(panel, PANEL_REG_INFO,ATTR_LABEL_TEXT,"Reg:(0x04) Device ID");
						break;
					case (RegNum_AP_VER+1) :
						SetCtrlAttribute(panel, PANEL_REG_INFO,ATTR_LABEL_TEXT,"Reg:(0x05) FM Version");
						break;
					case (RegNum_BL_VERSION+1) :
						SetCtrlAttribute(panel, PANEL_REG_INFO,ATTR_LABEL_TEXT,"Reg:(0x06) BL Version");
						break;
					case (RegNum_PStatus+1) :
						SetCtrlAttribute(panel, PANEL_REG_INFO,ATTR_LABEL_TEXT,"Reg:(0x07) Status");
						break;
					case (RegNum_Fun1+1) :
						SetCtrlAttribute(panel, PANEL_REG_INFO,ATTR_LABEL_TEXT,"Reg:(0x08) Function 1");
						break;
					case (RegNum_Fun2+1) :
						SetCtrlAttribute(panel, PANEL_REG_INFO,ATTR_LABEL_TEXT,"Reg:(0x09) Function 2");
						break;
					case (RegNum_RUN_Mode+1) :
						SetCtrlAttribute(panel, PANEL_REG_INFO,ATTR_LABEL_TEXT,"Reg:(0x0A) Run mode");
						break;   
						case (RegNum_PPG_PGA+1) :
						SetCtrlAttribute(panel, PANEL_REG_INFO,ATTR_LABEL_TEXT,"Reg:(0x0B) PPG PGA");
						break;
						case (RegNum_PPG_OP+1) :
						SetCtrlAttribute(panel, PANEL_REG_INFO,ATTR_LABEL_TEXT,"Reg:(0x0C) PPG LED POWER");
						break;
						case (RegNum_PPG_DCTH+1) :
						SetCtrlAttribute(panel, PANEL_REG_INFO,ATTR_LABEL_TEXT,"Reg:(0x0D) PPG DC Threshold");
						break;
						
						//	case (RegNum_Addl+1) :
						//		break;

				}
				GetTableCellAttribute (panel, control, MakePoint(REG_VALUE_COLUMN, focus.y),ATTR_CTRL_VAL, tmpstr);
				tmpd = strtol (tmpstr+2, NULL, 16);

				SetCtrlAttribute(panel, PANEL_REG_INFO,ATTR_CTRL_VAL,tmpd);
			}
			break;
	}
	return 0;
}
