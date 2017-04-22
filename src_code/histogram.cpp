#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES   
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include "define.h"
using namespace std;
//================rgb_array region===================//
int R[MaxBMPSizeX][MaxBMPSizeY];
int G[MaxBMPSizeX][MaxBMPSizeY];
int B[MaxBMPSizeX][MaxBMPSizeY];

int r[MaxBMPSizeX][MaxBMPSizeY];
int g[MaxBMPSizeX][MaxBMPSizeY];
int b[MaxBMPSizeX][MaxBMPSizeY];
//================YUV region=========================//
unsigned char *Y = NULL, *U = NULL, *V = NULL, *nY = NULL, *y = NULL, *u = NULL, *v = NULL;
char *inname = NULL, *outname = NULL;//bmpfilename 
//====================================================//
//===========write data for debug============//
FILE *fp = fopen("histogram.xls", "wb");
FILE *fp1 = fopen("probability.xls", "wb");
FILE *fp2 = fopen("accprobability.xls", "wb");
FILE *fp3 = fopen("equilzation.xls", "wb");
FILE *fp4 = fopen("newhistogram.xls", "wb");
//===========================================//
int main(int argc, char *argv[])
{
	//=======variable declare and malloc regino========//
	int *static_y = (int *)malloc(sizeof(int)* 256);
	float *prob_y = (float *)malloc(sizeof(float)* 256);
	float *accprob_y = (float *)malloc(sizeof(float)* 256);
	int *equalization = (int *)malloc(sizeof(int)* 256);
	int *newhistogram = (int *)malloc(sizeof(int)* 256);
	memset(static_y, 0, sizeof(int)* 256);
	memset(prob_y, 0, sizeof(float)* 256);
	memset(accprob_y, 0, sizeof(float)* 256);
	memset(equalization, 0, sizeof(int)* 256);
	memset(newhistogram, 0, sizeof(int)* 256);
	int width, height;
	float prob_min = 0.0;
	system("dir \"./input_image/\"");//list all bmp file 
	inname = (char *)malloc(sizeof(char)* 100);
	outname = (char *)malloc(sizeof(char)* 100);
	//================================================//
	//==========request input file name======//
	puts("=====================================================");
	printf("Please input you want choose image:");
	scanf("%s",inname);
	string nametemp, file = "./input_image/", bmp(inname);
	nametemp=file + bmp;
	const char *bmpname = nametemp.c_str();
	open_bmp((char*)bmpname, R, G, B, width, height);
	//======================================//
	Y = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
	nY = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
	U = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
	V = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
	float total = 0.0;
	RGB2YUV(R, G, B, Y, U, V, height, width);//call conver function 方便統計
	//============open debug flag=============//
	int debug_flag = 1; //flag change 1 cna print the excel to see the every setp condition
	/*==============statistics================*/
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			static_y[Y[i*width + j]] += 1;
			total++;
		}
	}
	if (debug_flag==1)//printf statistics for bedug
		for (int i = 0; i < gray_scale; i++) 
			fprintf(fp, "%d\n", static_y[i]);
	/*=========================================*/
	/*==============probability================*/
	for (int i = 0; i < gray_scale; i++)
		prob_y[i] = ((float)static_y[i] / total);

	if (debug_flag == 1)//printf probability for bedug
		for (int i = 0; i < gray_scale; i++)
			fprintf(fp1, "%f\n", prob_y[i]);
	/*========================================*/
	/*===========acculate probability============*/
	accprob_y[0] = prob_y[0];
	for (int i = 1; i < gray_scale; i++)
	{
		accprob_y[i] = (accprob_y[i - 1] + prob_y[i]);
	}
	if (debug_flag == 1)//printf accprobability for bedug
		for (int i = 0; i < gray_scale; i++)
			fprintf(fp2, "%f\n", accprob_y[i]);
	/*==========================================*/
	//======decide min&max =====//
	int flag = 0, gray_min = 0, gray_max = 0;
	for (int i = 0; i < gray_scale; i++)//fetch smallest probability  and smallest gray_scale value
	{
		if (prob_y[i] != 0)
		{
			prob_min = prob_y[i];
			gray_min = i;
			flag++;
		}
		if (flag == 1)
		{
			flag = 0;
			break;
		}
	}
	for (int i = 0; i < gray_scale; i++)//fetch largest gray_scale value
	{
		//printf("acc=%f i=%d\n",accprob_y[i],(int)i);
		float prob_1 = accprob_y[i];
		if (prob_1 > 0.999999) //使用==1會有問題目前改成>0.999999
		{
			gray_max = i;
			flag++;
		}
		if (flag == 1) break;
	}
	//printf("prob_min=%f gray_min=%d gray_max=%d\n", prob_min, gray_min, gray_max);
	/*================ scalaing ================*/
	for (int i = 0; i < gray_scale; i++)
	{
		if (i >= gray_min && i <= gray_max)
			equalization[i] = (int)((((accprob_y[i] - prob_min) / (1 - prob_min)) * 255) + 0.5);// +0.5 四捨五入
		else
			equalization[i] = 0;
	}
	if (debug_flag == 1)//printf scalaing for bedug
		for (int i = 0; i < gray_scale; i++)
			fprintf(fp3, "%d\n", equalization[i]);
	/*==========================================*/
	//================ mapping ================//
	for (int i = 0; i < gray_scale; i++)
	{
		if (i >= gray_min && i <= gray_max)
			newhistogram[equalization[i]] += static_y[i];
	}
	if (debug_flag == 1)//printf mapping for bedug
		for (int i = 0; i < gray_scale; i++)
			fprintf(fp4, "%d\n", newhistogram[i]);
	//======================================//
	//========== reset pixel value =========//
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			nY[i*width + j] = equalization[Y[i*width + j]];
	//======================================//
	YUV2RGB(r, g, b, nY, U, V, height, width);//call conver function 轉回rgb給bmp.cpp存檔
	//==========request input file name======//
	printf("File new name:");
	scanf("%s",outname);
	string outbmp(outname), outfile = "./output_image/";
	nametemp = outfile + outbmp;
	const char *outbmpname = nametemp.c_str();
	save_bmp((char*)outbmpname, r, g, b);
	//======================================//
	printf("Job Finished!\n");
	puts("=====================================================");
	close_bmp();
	system("PAUSE");
	//=============relase memory============//
	free(static_y), free(prob_y), free(accprob_y), free(equalization), free(newhistogram), free(Y), free(U), free(V),free(nY),free(inname),free(outname);
	fclose(fp), fclose(fp1), fclose(fp2), fclose(fp3), fclose(fp4);
	//======================================//
	return 0;
}
