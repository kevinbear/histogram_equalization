//===============================define region=============================
#define _CRT_SECURE_NO_WARNINGS
#define MaxBMPSizeX	1024
#define MaxBMPSizeY	1024
#define gray_scale 256
#define COMPARE(c) ( c<0 ? 0 : ( c>255 ? 255:c))
//=========================================================================

//===========================bmp cpp function regin=================================
void open_bmp(char filename[], int bmp_r[MaxBMPSizeX][MaxBMPSizeY], int bmp_g[MaxBMPSizeX][MaxBMPSizeY], int bmp_b[MaxBMPSizeX][MaxBMPSizeY], int& width, int& height);
void save_bmp(char filename[], int bmp_r[MaxBMPSizeX][MaxBMPSizeY], int bmp_g[MaxBMPSizeX][MaxBMPSizeY], int bmp_b[MaxBMPSizeX][MaxBMPSizeY]);
void close_bmp();
//==================================================================================
//===========================color convert function regin===========================
void RGB2YUV(int R[MaxBMPSizeX][MaxBMPSizeY], int G[MaxBMPSizeX][MaxBMPSizeY], int B[MaxBMPSizeX][MaxBMPSizeY], unsigned char* Y, unsigned char* U, unsigned char* V, int h, int w);
void YUV2RGB(int r[MaxBMPSizeX][MaxBMPSizeY], int g[MaxBMPSizeX][MaxBMPSizeY], int b[MaxBMPSizeX][MaxBMPSizeY], unsigned char *nY, unsigned char *U, unsigned char *V, int h, int w);
//==================================================================================