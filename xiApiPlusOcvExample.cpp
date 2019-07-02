// xiApiPlusOcvExample.cpp : program opens first camera, captures and displays 40 images

#include <stdio.h>
#include "xiApiPlusOcv.hpp"

using namespace cv;
using namespace std;
int main(int argc, char* argv[])
{
	try
	{
		// Sample for XIMEA OpenCV
		xiAPIplusCameraOcv cam;

		// Retrieving a handle to the camera device
		printf("Opening first camera...\n");
		cam.OpenFirst();
		
		// Set exposure
		cam.SetExposureTime(1000); //10000 us = 10 ms
		// Note: The default parameters of each camera might be different in different API versions
		
		printf("Starting acquisition...\n");
		cam.StartAcquisition();
		
		printf("First pixel value \n");
		XI_IMG_FORMAT format = cam.GetImageDataFormat();
		while (1) {
			Mat cv_mat_image = cam.GetNextImageOcvMat();
			if (format == XI_RAW16 || format == XI_MONO16)
				normalize(cv_mat_image, cv_mat_image, 0, 65536, NORM_MINMAX, -1, Mat());// 0 - 65536, 16 bit unsigned integer range

			Mat dst;
			cv::threshold(cv_mat_image, dst, 0, 30, cv::THRESH_BINARY | cv::THRESH_OTSU);
			cv::threshold(cv_mat_image, dst, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

			vector<vector<cv::Point>> contours;

			cv::findContours(dst, contours, cv::RETR_LIST ,cv::CHAIN_APPROX_NONE);
			int y = dst.rows;
			int x;

			for (int i = 0; i < contours.size(); i++) {
				for (int j = 0; j < contours[i].size(); j++) {
					cv::Point p = contours[i][j];
					if (p.y < y) {
						y = p.y;
						x = p.x;
					}
				}
			}
			cv::cvtColor(dst, dst, cv::COLOR_GRAY2RGB);
			cv::circle(dst, cv::Point(x, y), 10, cv::Scalar(0, 200, 0), 4, 8);


			
			cv::imshow("Image from camera", dst);


			cvWaitKey(1);
			printf("\t%d\n", cv_mat_image.at<unsigned char>(0, 0));
		}
		
		cam.StopAcquisition();
		cam.Close();
		printf("Done\n");
		
		cvWaitKey(500);
	}
	catch(xiAPIplus_Exception& exp)
	{
		printf("Error:\n");
		exp.PrintError();
#ifdef WIN32
		Sleep(2000);
#endif
		cvWaitKey(2000);
		return -1;
	}
	return 0;
}

