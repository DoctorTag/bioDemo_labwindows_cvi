#include<iostream>
#include<iostream>
#include<fstream>
#include"Matrix_operation.h"
#include"mit_bih.h"
#include"sg_filt.h"

using namespace std;  

#define L 3000
static double B[11][11]={0};
double y[3000]={0};     //滤波之后的数据

int main()
{ 
  
  int i,j;
  sg_filt(&B[0][0]);       //得到滤波矩阵,B为11*11的滤波矩阵

  for(i=0;i<=M;i++)        //第0到M，一共M+1个点
  {
	for(j=0;j<N;j++)
    y[i]=y[i]+B[j][i]*mit[j];
  }

  for(i=M+1;i<=L-M-2;i++)  //第M+1到L-M-2，一共L-2M-2个点
  {
    for(j=0;j<N;j++)
    y[i]=y[i]+B[j][M]*mit[i-M+j-1];
  
  }
  for(i=0;i<=M;i++)       //第L-M-1到L-1个点，一共M+1个点
  {
    for(j=0;j<N;j++)
    y[L-M+i-1]=y[L-M+i-1]+B[j][M+i]*mit[L-N+j];
  }

 //y数组为滤波之后数据
  
//文件输出部分，用于检测
/*
  ofstream outfile;
  outfile.open("E:\myfile.txt"); //myfile.txt是存放数据的文件名
  if(outfile.is_open())
  {   
	  for(i=0;i<3000;i++)
	  {
	  outfile<<y[i]<<","<<endl;    //message是程序中处理的数据   
	  }
	  outfile.close(); }
  else
  {   
	  cout<<"不能打开文件!"<<endl;
  }
  */
 
}  

