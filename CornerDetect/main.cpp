#include <iostream>
#include <math.h>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include "CornerDetect.h"

using namespace cv;
using namespace std;

int main() 
{
	VideoCapture cap("..\\data\\record2_256x256_3.avi");  //flow_x5_y5(1).avi    record2_256x256_3.avi
	if (!cap.isOpened())
	{
		cout << " ”∆µ∂¡»° ß∞‹£°";
		return -1;
	}
	cap.set(cv::CAP_PROP_POS_FRAMES, 0); //Ã¯÷°
    int index = 0;

	while(1)
	{
        printf("\n");

		Mat frame, frame_gray;
		cap >> frame;
		if (frame.empty())
		{
            break;
		}
		cvtColor(frame, frame_gray, CV_RGB2GRAY);
        index++;

        //¡¡∂»∆¿π¿
        if(1)
        {
            int brightness = 0;
            int score = 0;
            int grade = 0;
            AW_AI_EvalImageBrightness((const unsigned char*)(frame_gray.data), frame_gray.cols, frame_gray.rows, &brightness, &score, &grade);

            printf("%s(): index = %d, brightness = %d, score = %d, grade = %d\n", __func__, index, brightness, score, grade);

            if (grade < 0)
            {
                printf("%s(): Image is too dark!\n", __func__);
            }
            else if (grade > 0)
            {
                printf("%s(): Image is too bright!\n", __func__);
            }

            char text[20];
            sprintf(text, "score = %d", score);
            Scalar color;
            if (score > 60)
            {
                color = Scalar(255, 0, 0);
            }
            else
            {
                color = Scalar(0, 0, 255);
            }
            putText(frame, text, Point(25, 20), cv::FONT_HERSHEY_PLAIN, 1, color, 1, 6);
            imshow("Brightness", frame);
        }
        
		//Ω«µ„ºÏ≤‚
        if(0)
        {
            POINT_F point[49];
            AW_AI_GenPoints(frame.cols, frame.rows, 7, 7, point);
            LP_CORNER_F corners = (LP_CORNER_F)malloc(49 * sizeof(CORNER_F));
            for (int i = 0; i < 49; i++)
            {
                corners[i].p = point[i];
            }

            AW_AI_CornerDetect((const unsigned char*)frame_gray.data, frame_gray.cols, frame_gray.rows, corners, 49, 19, 1);
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
                    circle(frame, tmp, 1, cv::Scalar(0, 255, 0), 2, 8, 0);
                    putText(frame, text, Point(tmp.x, tmp.y + 20), cv::FONT_HERSHEY_PLAIN, 0.85, Scalar(0, 255, 0), 1, 5);
                }
            }

            imshow("Corners", frame);
            free(corners);
        }

        waitKey(0);
	}

	cap.release();
	return 0;
}
