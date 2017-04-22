#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES   
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include "bmp.h"
//===============================define region=============================
#define gray_scale 256
#define COMPARE(c) ( c<0 ? 0 : ( c>255 ? 255:c))
//=========================================================================
using namespace std;
//================rgb_array region===================
int R[MaxBMPSizeX][MaxBMPSizeY];
int G[MaxBMPSizeX][MaxBMPSizeY];
int B[MaxBMPSizeX][MaxBMPSizeY];

int r[MaxBMPSizeX][MaxBMPSizeY];
int g[MaxBMPSizeX][MaxBMPSizeY];
int b[MaxBMPSizeX][MaxBMPSizeY];

unsigned char *Y = NULL, *U = NULL, *V = NULL, *nY = NULL, *y = NULL, *u = NULL, *v = NULL;
char *inname = NULL, *outname = NULL;//bmpfilename 
//====================================================
FILE *fp = fopen("histogram.xls", "wb");
FILE *fp1 = fopen("probability.xls", "wb");
FILE *fp2 = fopen("accprobability.xls", "wb");
FILE *fp3 = fopen("equilzation.xls", "wb");
FILE *fp4 = fopen("newhistogram.xls", "wb");
FILE *fp5 = fopen("car.yuv", "wb");
//===========.function region==========
void RGB2YUV(int R[MaxBMPSizeX][MaxBMPSizeY], int G[MaxBMPSizeX][MaxBMPSizeY], int B[MaxBMPSizeX][MaxBMPSizeY], unsigned char* Y, unsigned char* U, unsigned char* V, int h, int w);
void YUV2RGB(int r[MaxBMPSizeX][MaxBMPSizeY], int g[MaxBMPSizeX][MaxBMPSizeY], int b[MaxBMPSizeX][MaxBMPSizeY], unsigned char *nY, unsigned char *U, unsigned char *V, int h, int w);
//======================================================
int main(int argc, char *argv[])
{
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
	system("dir \"./input_image/\"");
	inname = (char *)malloc(sizeof(char)* 100);
	outname = (char *)malloc(sizeof(char)* 100);
	puts("=====================================================");
	printf("Please input you want choose image:");
	scanf("%s",inname);
	string nametemp, file = "./input_image/", bmp(inname);
	nametemp=file + bmp;
	const char *bmpname = nametemp.c_str();
	open_bmp((char*)bmpname, R, G, B, width, height);
	Y = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
	nY = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
	U = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
	V = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
	y = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
	u = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
	v = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
	float total = 0.0;
	RGB2YUV(R, G, B, Y, U, V, height, width);
	//=============debug==================
	//for (int i = 0; i < height; i++)
	//{
	//	for (int j = 0; j < width; j++)
	//	{
	//		/*fprintf(fp, "%d\t", Y[j*width + (256-i)]);
	//		if (j == width - 1)
	//		fprintf(fp, "%d\n", Y[j*width + (256-i)]);*/
	//	}
	//}
	/*==============statistics================*/
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			static_y[Y[i*width + j]] += 1;
			total++;
		}
	}
	/*========================================*/

	/*============printf statistics===========*/
	for (int i = 0; i < gray_scale; i++)
		fprintf(fp, "%d\n", static_y[i]);
	/*========================================*/

	/*==============probability================*/
	for (int i = 0; i < gray_scale; i++)
	{
		prob_y[i] = ((float)static_y[i] / total);
	}
	/*========================================*/

	/*============printf probability===========*/
	for (int i = 0; i < gray_scale; i++)
		fprintf(fp1, "%f\n", prob_y[i]);
	/*========================================*/

	/*===========acculate probability============*/
	accprob_y[0] = prob_y[0];
	for (int i = 1; i < gray_scale; i++)
	{
		accprob_y[i] = (accprob_y[i - 1] + prob_y[i]);
	}
	/*==========================================*/

	/*============printf accprobability===========*/
	for (int i = 0; i < gray_scale; i++)
		fprintf(fp2, "%f\n", accprob_y[i]);
	/*===========================================*/

	//======decide min=====//
	int flag = 0, gray_min = 0, gray_max = 0;
	for (int i = 0; i < gray_scale; i++)
	{
		if (prob_y[i] != 0)//fetch probability small
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
	for (int i = 0; i < gray_scale; i++)
	{
		printf("acc=%f i=%d\n",accprob_y[i],(int)i);
		float prob_1 = accprob_y[i];
		if (prob_1 > 0.999999) //
		{
			gray_max = i;
			flag++;
		}
		if (flag == 1) break;
	}
	printf("prob_min=%f gray_min=%d gray_max=%d\n", prob_min, gray_min, gray_max);
	/*================ scalaing ================*/
	for (int i = 0; i < gray_scale; i++)
	{
		if (i >= gray_min && i <= gray_max)
		{
			equalization[i] = (int)((((accprob_y[i] - prob_min) / (1 - prob_min)) * 255) + 0.5);// +0.5 ¥|±Ë¤­¤J
			printf("%f\n", (((accprob_y[i] - prob_min) / (1 - prob_min)) * 255));
		}
		else
			equalization[i] = 0;
	}

	for (int i = 0; i < gray_scale; i++)
		fprintf(fp3, "%d\n", equalization[i]);
	/*==========================================*/

	//================ mapping ================//
	for (int i = 0; i < gray_scale; i++)
	{
		if (i >= gray_min && i <= gray_max)
			newhistogram[equalization[i]] += static_y[i];
	}

	for (int i = 0; i < gray_scale; i++)
		fprintf(fp4, "%d\n", newhistogram[i]);
	//======================================//

	//========== reset pixel value =========//
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			nY[i*width + j] = equalization[Y[i*width + j]];
		}
	}
	//======================================//

	YUV2RGB(r, g, b, nY, U, V, height, width);
	fwrite(Y, 1, sizeof(unsigned char)*width*height, fp5);
	fwrite(U, 1, sizeof(unsigned char)*width*height, fp5);
	fwrite(V, 1, sizeof(unsigned char)*width*height, fp5);
	printf("File new name:");
	scanf("%s",outname);
	string outbmp(outname), outfile = "./output_image/";
	nametemp = outfile + outbmp;
	const char *outbmpname = nametemp.c_str();
	save_bmp((char*)outbmpname, r, g, b);
	printf("Job Finished!\n");
	puts("=====================================================");
	close_bmp();
	system("PAUSE");
	free(static_y), free(prob_y), free(accprob_y), free(equalization), free(newhistogram), free(Y), free(U), free(V);
	fclose(fp), fclose(fp1), fclose(fp2), fclose(fp3), fclose(fp4);
	return 0;
}
/* bmp width-->Y height-->X*/
void RGB2YUV(int R[MaxBMPSizeX][MaxBMPSizeY], int G[MaxBMPSizeX][MaxBMPSizeY], int B[MaxBMPSizeX][MaxBMPSizeY], unsigned char *Y, unsigned char *U, unsigned char *V, int h, int w)
{
	float yy, uu, vv;
	printf("w=%d h=%d\n", w, h);
	for (int i = 0; i<h; i++)
	{
		for (int j = 0; j<w; j++)
		{
			yy = R[i][j] * 0.299 + G[i][j] * 0.537 + B[i][j] * 0.114;
			Y[i*w + j] = COMPARE(yy);
			uu = R[i][j] * -0.169 + G[i][j] * -0.331 + B[i][j] * 0.5 + 128;
			U[i*w + j] = COMPARE(uu);
			vv = R[i][j] * 0.5 + G[i][j] * -0.419 + B[i][j] * -0.081 + 128;
			V[i*w + j] = COMPARE(vv);
		}
	}

}
void YUV2RGB(int r[MaxBMPSizeX][MaxBMPSizeY], int g[MaxBMPSizeX][MaxBMPSizeY], int b[MaxBMPSizeX][MaxBMPSizeY], unsigned char *nY, unsigned char *U, unsigned char *V, int h, int w)
{
	float rr, gg, bb;
	printf("w=%d h=%d\n", w, h);
	for (int i = 0; i<h; i++)
	{
		for (int j = 0; j<w; j++)
		{
			rr = nY[i*w + j] + 1.13983 * (V[i*w + j] - 128);
			r[i][j] = COMPARE(rr);
			gg = nY[i*w + j] - 0.39465 * (U[i*w + j] - 128) - 0.58060 * (V[i*w + j] - 128);
			g[i][j] = COMPARE(gg);
			bb = nY[i*w + j] + 2.03211 * (U[i*w + j] - 128);
			b[i][j] = COMPARE(bb);
		}
	}
}




