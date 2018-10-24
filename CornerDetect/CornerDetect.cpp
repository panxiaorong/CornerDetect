#include <stdio.h>
#include "CornerDetect.h"

// 评估图像亮度
void AW_AI_EvalImageBrightness(const unsigned char *pImageData, int width, int height, int *pBrightness, int *pScore, int *pGrade)
{
    assert(pImageData != NULL);

    int _Brightness = 0;
    if (pBrightness == NULL)
    {
        pBrightness = &_Brightness;
    }
    int _Score = 0;
    if (pScore == NULL)
    {
        pScore = &_Score;
    }
    int _Grade = 0;
    if (pGrade == NULL)
    {
        pGrade = &_Grade;
    }

    double sum = 0.0;
    int num_pixel = 0;   //去掉最大值和最小值之后的像素点的数量
    for (int i = 0; i< width * height; i++)
    {
        double val_pixel = (double)(pImageData[i]);
        //if (val_pixel > 40 && val_pixel < 250)
        {
            sum = sum + val_pixel;
            num_pixel++;
        }
    }

    double mean = 0;
    if (num_pixel != 0)
    {
        mean = sum / num_pixel;
    }
    if (num_pixel == 0)
    {
        mean = 0;
    }

    *pBrightness = (int)mean;
    //printf("%s(): Brightness = %6.2f\n", __func__, *pBrightness);

#define TOTAL_BRIGHTNESS        (255.0f)
#define UPPER_BRIGHTNESS        (235.0f)
#define LOWER_BRIGHTNESS        (100.0f)
#define BEST_BRIGHTNESS         (167.0f)

#define TOTAL_SCORE             (100.0f)
#define PASS_SCORE              (60.0f)

    double score = 0;
    double grade = 0;
    if (mean < LOWER_BRIGHTNESS)
    {
        score = PASS_SCORE - PASS_SCORE * (LOWER_BRIGHTNESS - mean) / (LOWER_BRIGHTNESS - 0);
        grade = -1;
    }
    else if (mean >= LOWER_BRIGHTNESS && mean <= UPPER_BRIGHTNESS)
    {
        if (mean < BEST_BRIGHTNESS)
        {
            score = TOTAL_SCORE - (TOTAL_SCORE - PASS_SCORE) * (BEST_BRIGHTNESS - mean) / (BEST_BRIGHTNESS - LOWER_BRIGHTNESS);
        }
        else
        {
            score = TOTAL_SCORE - (TOTAL_SCORE - PASS_SCORE) * (mean - BEST_BRIGHTNESS) / (UPPER_BRIGHTNESS - BEST_BRIGHTNESS);
        }
        grade = 0;
    }
    else if (mean > UPPER_BRIGHTNESS)
    {
        score = PASS_SCORE - PASS_SCORE * (mean - UPPER_BRIGHTNESS) / (TOTAL_BRIGHTNESS - UPPER_BRIGHTNESS);
        grade = 1;
    }
    *pScore = (int)score;
    *pGrade = (int)grade;
}
//-----------------------------------------------------------------------------

static void exchange(float &a, float &b)
{
	float tmp;
	tmp = b;
	b = a;
	a = tmp;
}

void Moravec(const unsigned char* pImageData, int width, int height, LP_CORNER_F pCorner, int win_size)
{
	assert(pImageData != NULL);

	if (win_size % 2 == 0)
	{
		win_size = win_size + 1;
	}

	int kernel_size = win_size / 2;
	float sum = 0;

	int pixel_x = pCorner->p.x;
	int pixel_y = pCorner->p.y;

	for (int row = pCorner->p.y - kernel_size; row <= pCorner->p.y + kernel_size; row++)
	{
		for (int col = pCorner->p.x - kernel_size; col <= pCorner->p.x + kernel_size; col++)
		{
			float tmp = fabs(pImageData[row * width + col] - pImageData[pixel_y * width + pixel_x]);
			sum = sum + tmp;
		}
	}
	sum = sum / (2 * kernel_size + 1) / (2 * kernel_size + 1);
	pCorner->m = sum;
}

void Harris(const unsigned char* pImageData, int width, int height, LP_CORNER_F pCorner, int win_size)
{
	assert(pImageData != NULL);
	static int fx[] = { 1, 0, -1, 2, 0, -2, 1, 0, -1 };
	static int fy[] = { 1, 2, 1, 0, 0, 0, -1, -2, -1 };

	if (win_size % 2 == 0)
	{
		win_size = win_size + 1;
	}

	int kernel_size = win_size / 2;
	int pixel_x = pCorner->p.x;
	int pixel_y = pCorner->p.y;
	float sum_Ix2 = 0.0;
	float sum_Iy2 = 0.0;
	float sum_IxIy = 0.0;
	for (int row = pixel_y - kernel_size; row <= pixel_y + kernel_size; row++)
	{
		for (int col = pixel_x - kernel_size; col <= pixel_x + kernel_size; col++)
		{
			float Ix = 0.0;
			float Iy = 0.0;
			int m = 0;
			for (int sub_row = row - 1; sub_row <= row + 1; sub_row++)
			{
				for (int sub_col = col - 1; sub_col <= col + 1; sub_col++)
				{
					Ix = Ix + pImageData[sub_row * width + sub_col] * fx[m];
					Iy = Iy + pImageData[sub_row * width + sub_col] * fy[m];
					m++;
				}
			}
			sum_Ix2  += (Ix / 9) * (Ix / 9);
			sum_Iy2  += (Iy / 9) * (Iy / 9);
			sum_IxIy += (Ix / 9) * (Iy / 9);
		}
	}
	sum_Ix2  = sum_Ix2  / win_size / win_size;
	sum_Iy2  = sum_Iy2  / win_size / win_size;
	sum_IxIy = sum_IxIy / win_size / win_size;
	pCorner->m = sum_Ix2 * sum_Iy2 - sum_IxIy * sum_IxIy - 0.004 *(sum_Ix2 + sum_Iy2)*(sum_Ix2 + sum_Iy2);
}

void CornerDetect(const unsigned char* pImageData, int width, int height, LP_CORNER_F pCorner, int win_size, int method)
{
	if (pImageData == NULL)
	{
		return;
	}

	if (method == 0)
	{
		Moravec((unsigned char*)pImageData, width, height, pCorner, win_size);
		return;
	}
	else if (method == 1)
	{
		Harris(pImageData, width, height, pCorner, win_size);
		return;
	}
	else
	{
		assert(0);
	}
}

// 角点检测
void AW_AI_CornerDetect(const unsigned char* pImageData, int width, int height, LP_CORNER_F pCorners, int num, int win_size, int method)
{
	assert(pImageData != NULL);
	
	// 逐个计算角点强度
	for (int i = 0; i < num; i++)
	{
		CornerDetect(pImageData, width, height, &pCorners[i], win_size, method);
	}

	// 对角点按照强度降序排列
	for (int i = 0; i < num; i++)
	{
		for (int j = i+1; j < num; j++)
		{
			if (pCorners[i].m < pCorners[j].m)
			{
				exchange(pCorners[i].p.x, pCorners[j].p.x);
				exchange(pCorners[i].p.y, pCorners[j].p.y);
				exchange(pCorners[i].m,   pCorners[j].m);
			}
		}
	}
}

// 生成参考点
int AW_AI_GenPoints(int width, int height, int x_count, int y_count, POINT_F pPoints[])
{
	int x_interval = width / (x_count + 1);
	int y_interval = height / (y_count + 1);
	int x = 0;
	int y = 0;
	int i = 0;

	if ((x_interval == 0) || (y_interval == 0))
	{
		return -1;
	}

	for (y = 0; y < y_count; y++)
	{
		for (x = 0; x < x_count; x++)
		{
			i = y * x_count + x;
			pPoints[i].x = (float)((x + 1) * x_interval);
			pPoints[i].y = (float)((y + 1) * y_interval);
		}
	}
	return 0;
}
