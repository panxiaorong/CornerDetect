#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <math.h>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <vector>

using namespace cv;
using namespace std;

void AW_AI_EvalImageQuality(const char* image, int width, int height, int *pBrightness)//, int *pTexture
{
	double sum = 0.0, sum_d = 0.0;
	const unsigned char* img = (const unsigned char*)image;
	int i_true = 0;
	for (int i = 0; i< width*height; i++)
	{
		double num = (double)(img[i]);
		if (num > 40)
		{
			sum = sum + num;
			i_true++;
		}
	}
	double mean, stddev;
	if (i_true != 0)
	{
		mean = sum / i_true;
	}
	if (i_true == 0)
	{
		mean = 0;
		stddev = 0;
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

int main()
{
	Mat frame;
	VideoCapture cap("flow_x5_y5(1).avi");//flow_x5_y5(1).avi    record2_256x256_3.avi
	if (!cap.isOpened())
	{
		cout << "ÊÓÆµ¶ÁÈ¡Ê§°Ü£¡";
		return -1;
	}
	while (1)
	{
		Mat frame, frame_gray;
		cap >> frame;
		if (frame.empty())
		{
			break;
		}
		cvtColor(frame, frame_gray, CV_RGB2GRAY);
		int pB;
		AW_AI_EvalImageQuality((const char*)(frame_gray.data), frame_gray.cols, frame_gray.rows, &pB);
		char text[20];
		sprintf(text, "bright quality=%d", pB);
		putText(frame, text, Point(25, 25), cv::FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 255), 2, 6);
		namedWindow("src", 0);
		resizeWindow("src", 640, 480);
		imshow("src", frame);
		waitKey(30);
	}
	cap.release();

	return 0;
}