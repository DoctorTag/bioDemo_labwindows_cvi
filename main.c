#include<iostream>
#include<iostream>
#include<fstream>
#include"Matrix_operation.h"
#include"mit_bih.h"
#include"sg_filt.h"

using namespace std;  

#define L 3000
static double B[11][11]={0};
double y[3000]={0};     //�˲�֮�������

int main()
{ 
  
  int i,j;
  sg_filt(&B[0][0]);       //�õ��˲�����,BΪ11*11���˲�����

  for(i=0;i<=M;i++)        //��0��M��һ��M+1����
  {
	for(j=0;j<N;j++)
    y[i]=y[i]+B[j][i]*mit[j];
  }

  for(i=M+1;i<=L-M-2;i++)  //��M+1��L-M-2��һ��L-2M-2����
  {
    for(j=0;j<N;j++)
    y[i]=y[i]+B[j][M]*mit[i-M+j-1];
  
  }
  for(i=0;i<=M;i++)       //��L-M-1��L-1���㣬һ��M+1����
  {
    for(j=0;j<N;j++)
    y[L-M+i-1]=y[L-M+i-1]+B[j][M+i]*mit[L-N+j];
  }

 //y����Ϊ�˲�֮������
  
//�ļ�������֣����ڼ��
/*
  ofstream outfile;
  outfile.open("E:\myfile.txt"); //myfile.txt�Ǵ�����ݵ��ļ���
  if(outfile.is_open())
  {   
	  for(i=0;i<3000;i++)
	  {
	  outfile<<y[i]<<","<<endl;    //message�ǳ����д��������   
	  }
	  outfile.close(); }
  else
  {   
	  cout<<"���ܴ��ļ�!"<<endl;
  }
  */
 
}  

