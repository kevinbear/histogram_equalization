#include "define.h"
void RGB2YUV(int R[MaxBMPSizeX][MaxBMPSizeY], int G[MaxBMPSizeX][MaxBMPSizeY], int B[MaxBMPSizeX][MaxBMPSizeY], unsigned char *Y, unsigned char *U, unsigned char *V, int h, int w)
{
	float yy, uu, vv;
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