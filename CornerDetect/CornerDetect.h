#ifndef __CORNER_DETECT_H__
#define __CORNER_DETECT_H__

#include <math.h>
#include <assert.h>

typedef struct _POINT_F
{
	float x;
	float y;
}POINT_F, *LP_POINT_F;

typedef struct _CORNER_F
{
	POINT_F p;
	float m;
}CORNER_F, *LP_CORNER_F;


// 评估图像质量
void AW_AI_EvalImageBrightness(const unsigned char *pImageData, int width, int height, int *pBrightness, int *pScore, int *pGrade);

// 角点检测
void AW_AI_CornerDetect(const unsigned char* pImageData, int width, int height, LP_CORNER_F pCorners, int num, int win_size = 7, int method = 0);

// 生成参考点
int AW_AI_GenPoints(int width, int height, int x_count, int y_count, POINT_F pPoints[]);

#endif
