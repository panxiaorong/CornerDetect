#include <iostream>
#include <math.h>
#include <vector>

#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>

using namespace cv;
using namespace std;

// TODO:建立一个新的结构体，POINT_F有公认的定义
typedef struct _POINT_F
{
	float x;
	float y;
	float m;
}POINT_F, *LP_POINT_F;

typedef struct _CORNER_F
{
	POINT_F p;
	float m;
}CORNER_F, *LP_CORNER_F;

// TODO:新建一个结构体，包含Brightness
void AW_AI_EvalImageQuality(const char *image, int width, int height, int *pBrightness)//, int *pTexture
{
	double sum = 0.0, sum_d = 0.0;
	const unsigned char *img = (const unsigned char*)image;
	int i_true = 0;
	for (int i = 0; i< width*height; i++)
	{
		double num = (double)(img[i]);
		if (num > 40 && num < 250)
		{
			sum = sum + num;
			i_true++;
		}
	}
	double mean;
	if (i_true != 0)
	{
		mean = sum / i_true;
	}
	if (i_true == 0)
	{
		mean = 0;
	}

	if (mean < 60)
	{
		*pBrightness = (int)(mean);
	}
	if (mean > 230)
	{
		*pBrightness = 60 - (int)((mean - 230)*(60 / 25));
	}
	if (mean >= 60 && mean <= 230)
	{
		*pBrightness = (int)(100 - fabs(mean - 120) * 0.23);
	}
}

void Moravec(const unsigned char* pImageData, int width, int height, LP_CORNER_F pCorner, int win_size)
{
	int r = win_size / 2;
	float sum = 0;

	int pixel_x = pCorner->p.x;
	int pixel_y = pCorner->p.y;
	for (int L = pCorner->p.y - r; L <= pCorner->p.y + r; L++)
	{
		for (int K = pCorner->p.x - r; K <= pCorner->p.x + r; K++)
		{
			float tmp = fabs(pImageData[L * width + K] - pImageData[pixel_y * width + pixel_x]);
			sum = sum + tmp;
		}
	}
	sum = sum / (2 * r + 1) / (2 * r + 1);
	pCorner->m = sum;
}

void Harris(const unsigned char* pImageData, int width, int height, LP_CORNER_F pCorner, int win_size)
{

}

float CornerDetect(const unsigned char* pImageData, int width, int height, LP_CORNER_F pCorner, int win_size, int method)
{
	if (pImageData == NULL)
	{
		return -1;
	}
	if (method == 0)
	{
		Moravec((unsigned char*)pImageData, width, height, pCorner, win_size);
		return pCorner->m;
	}
	if (method == 1)
	{
		Harris(pImageData, width, height, pCorner, win_size);
		return pCorner->m;
	}
}

// TODO:统一对外接口
void AW_AI_CornerDetect(const char* pImageData, int width, int height, LP_CORNER_F pCorners, int num, int win_size, int method = 0)
{
	if (pImageData == NULL)
	{
	}
	for (int i = 0; i < num; i++)
	{
		CornerDetect(pImageData, width, height, pCorners, win_size, method);
		pCorners++;
	}
}

static void exchange(float &a, float &b)
{
	float tmp;
	tmp = b;
	b = a;
	a = tmp;
}

static int gen_static_features(int img_width, int img_height, int x_count, int y_count, POINT_F features[])
{
	int x_interval = img_width / (x_count + 1);
	int y_interval = img_height / (y_count + 1);
	int x = 0;
	int y = 0;
	int i = 0;

	if ((x_interval == 0) || (y_interval == 0))
	{
		printf("%s(): Error: Image is too small, cannot get so many features!\n", __func__);
		return -1;
	}

	for (y = 0; y < y_count; y++)
	{
		for (x = 0; x < x_count; x++)
		{
			i = y * x_count + x;
			features[i].x = (float)((x + 1) * x_interval);
			features[i].y = (float)((y + 1) * y_interval);
			//printf("%s(): features[%d] = (%d, %d)\n", __func__, i, (int)(features[i].x), (int)(features[i].y));
		}
	}
	return 0;
}

void harris(unsigned char *img, int width, int height, POINT_F features[], int num_features, int win_size)
{
	int r = win_size / 2;
	for (int i = 0; i < num_features; i++)
	{
		int pixel_x = features[i].x;
		int pixel_y = features[i].y;
		float sum_Ix2 = 0.0;
		float sum_Iy2 = 0.0;
		float sum_IxIy = 0.0;
		for (int w = pixel_y - r; w < pixel_y + r; w++)
		{
			for (int h = pixel_x - r; h < pixel_x + r; h++)
			{
				float Ix = 0.0;
		        float Iy = 0.0;
    	        int fx[] = { 1, 0, -1, 2, 0, -2, 1, 0, -1 };
		        int fy[] = { 1, 2, 1, 0, 0, 0, -1, -2, -1 };
		        int m = 0;
		        for (int L = w - 1; L < w + 1; L++)
	        	{
			        for (int K = h - 1; K < h + 1; K++)
			        {
			           	Ix = Ix + img[L * width + K] * fx[m];
				        Iy = Iy + img[L * width + K] * fy[m];
				        m++;
			         }
		         	 m++;
	        	}
		        float Ixy = (Ix / 9) * (Iy / 9);
		        float Ix2 = (Ix / 9) * (Ix / 9);
	        	float Iy2 = (Iy / 9) * (Iy / 9);
				sum_Ix2 = sum_Ix2 + Ix2;
				sum_Iy2 = sum_Iy2 + Iy2;
				sum_IxIy = sum_IxIy + Ixy;
			}
		}
		sum_Ix2 = sum_Ix2 / win_size / win_size;
		sum_Iy2 = sum_Iy2 / win_size / win_size;
		sum_IxIy = sum_IxIy / win_size / win_size;
		float M = sum_Ix2 * sum_Iy2 - sum_IxIy * sum_IxIy - 0.004 *(sum_Ix2 + sum_Iy2)*(sum_Ix2 + sum_Iy2);
	}
}

void sort_poit(LP_CORNER_F corners,int num)
{
	for (int i = 0; i < num; i++)
	{
		for (int j = i; j < num; j++)
		{
			if (corners[i].m < corners[j].m)
			{
				exchange(corners[i].p.x, corners[j].p.x);
				exchange(corners[i].p.y, corners[j].p.y);
				exchange(corners[i].m, corners[j].m);
			}
		}
	}
}

int main()
{
	Mat frame;
	VideoCapture cap("record2_256x256_3.avi");//flow_x5_y5(1).avi    record2_256x256_3.avi
	if (!cap.isOpened())
	{
		cout << "视频读取失败！";
		return -1;
	}

	cap.set(cv::CAP_PROP_POS_FRAMES, 1000); //跳帧

	while(1)
	{
		Mat frame,frame_gray;
		cap >> frame;
		if (frame.empty())
		{
            break;
		}
		//frame = imread("lena.png");
		cvtColor(frame, frame_gray, CV_RGB2GRAY);

		//brightness
		int pB;
		AW_AI_EvalImageQuality((const char*)(frame_gray.data), frame_gray.cols, frame_gray.rows, &pB);
		char text[20];
		sprintf(text, "bright quality=%d", pB);
		putText(frame, text, Point(25, 25), cv::FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 255), 2, 6);

		//corner detect
		POINT_F point[49];
		gen_static_features(frame.cols, frame.rows, 7, 7, point);
		LP_CORNER_F corners = (LP_CORNER_F)malloc(49*sizeof(CORNER_F));
		for (int i = 0; i < 49; i++)
		{
			corners[i].p = point[i];
		}
		AW_AI_CornerDetect((const char*)frame_gray.data, frame_gray.cols, frame_gray.rows, corners, 49, 7);
		sort_poit(corners, 49);
		for (int i = 0; i < 49; i++)
		{
			Point tmp;
			tmp.x = corners[i].p.x;
			tmp.y = corners[i].p.y;

			char text[20];
			sprintf(text, "%1.1f", corners[i].m);
			if (i < 10)
			{
				circle(frame, tmp, 1, cv::Scalar(0, 0, 255), 2, 8, 0);
				putText(frame, text, Point(tmp.x, tmp.y + 20), cv::FONT_HERSHEY_PLAIN, 0.85, Scalar(0, 0, 255), 1, 5);
			}
			else
			{
				circle(frame, tmp, 1, cv::Scalar(0,255, 0 ), 2, 8, 0);
                putText(frame, text, Point(tmp.x, tmp.y + 20), cv::FONT_HERSHEY_PLAIN, 0.85, Scalar(0, 255, 0), 1, 5);
			}		
		}
		imshow("src_point", frame);
		waitKey(0);
	    free(corners);
	}
	cap.release();
	return 0;
} 