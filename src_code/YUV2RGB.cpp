#include "define.h"
void YUV2RGB(int r[MaxBMPSizeX][MaxBMPSizeY], int g[MaxBMPSizeX][MaxBMPSizeY], int b[MaxBMPSizeX][MaxBMPSizeY], unsigned char *nY, unsigned char *U, unsigned char *V, int h, int w)
{
	float rr, gg, bb;
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
